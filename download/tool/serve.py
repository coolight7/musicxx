#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""拟声下载页的本地预览服务（Python 版，只用标准库）。

做两件事：
1. 把上一级目录（download/）当成静态站点提供，直接预览 index.html 的效果；
2. 把 /api 开头的请求转发到接口服务，页面里的相对路径接口不用改就能取到真实数据。

用法：
    python serve.py                            # 默认 8080 端口，转发到线上接口
    python serve.py 9000                       # 指定端口
    python serve.py 9000 http://127.0.0.1:7777 # 转发到自建服务
    python serve.py 9000 mock                  # 断网时用 tool/mock/ 里的快照数据预览
按 Ctrl+C 停止。
"""

import http.server
import json
import os
import socket
import socketserver
import sys
import urllib.error
import urllib.parse
import urllib.request

HERE = os.path.dirname(os.path.abspath(__file__))
# 静态站点根目录就是工具目录的上一级（download/）
ROOT = os.path.abspath(os.path.join(HERE, os.pardir))
MOCK_DIR = os.path.join(HERE, "mock")
UPSTREAM = "https://api.music.bool.run"
DEFAULT_PORT = 8080
# 只转发这几个头的目的是避开 gzip、keep-alive 这类需要额外处理的头
FORWARD_HEADERS = ("Accept", "Content-Type", "Cookie", "Referer", "Origin", "User-Agent")

port = DEFAULT_PORT
use_mock = False
upstream = UPSTREAM


class PreviewHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=ROOT, **kwargs)

    # ---------- 静态文件：关掉缓存，改完代码刷新就能看到 ----------
    def end_headers(self):
        self.send_header("Cache-Control", "no-store, max-age=0")
        super().end_headers()

    def guess_type(self, path):
        mime = super().guess_type(path)
        if mime.startswith("text/") and "charset" not in mime:
            mime += "; charset=utf-8"
        return mime

    # ---------- 接口 ----------
    def do_GET(self):
        if self.path.startswith("/api/"):
            self.handle_api()
            return
        super().do_GET()

    def do_POST(self):
        if self.path.startswith("/api/"):
            self.handle_api()
            return
        self.send_error(405, "Only GET/POST is supported")

    def handle_api(self):
        body = None
        length = self.headers.get("Content-Length")
        if length:
            body = self.rfile.read(int(length))
        if use_mock:
            self.answer(200, mock_response(self.path))
            return
        headers = {}
        for key in FORWARD_HEADERS:
            if key in self.headers:
                headers[key] = self.headers[key]
        request = urllib.request.Request(
            upstream + self.path, data=body, headers=headers, method=self.command
        )
        try:
            with urllib.request.urlopen(request, timeout=60) as resp:
                data = resp.read()
                status = resp.status
        except urllib.error.HTTPError as err:
            data = err.read()
            status = err.code
        except Exception as err:  # 断网、超时、DNS 出错等
            print("[api] fail %s %s -> %s" % (self.command, self.path, err))
            self.send_error(502, "upstream request failed: %s (try: serve.py %d mock)" % (err, port))
            return
        self.answer(status, data)

    def answer(self, status, data):
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        if self.command != "HEAD":
            self.wfile.write(data)

    def log_message(self, fmt, *args):
        print("[http] %s" % (fmt % args))


def to_int(value, default):
    try:
        return int(value)
    except Exception:
        return default


def mock_response(path):
    """从 tool/mock/ 里取快照：下载列表 + 各平台的版本列表 / 更新日志。"""
    query = {}
    if "?" in path:
        for pair in path.split("?", 1)[1].split("&"):
            if "=" in pair:
                key, value = pair.split("=", 1)
                query[urllib.parse.unquote(key)] = urllib.parse.unquote(value)
    sliced = False
    if path.startswith("/api/procedure/get/download/list"):
        name = "download-list.json"
    elif path.startswith("/api/procedure/get/version/list"):
        name = "version_list_%s_%s.json" % (query.get("type", ""), query.get("branch", "Main"))
        sliced = True  # mock 也按 offset/limit 切片，方便预览分页
    elif path.startswith("/api/procedure/get/versionLog"):
        name = "versionLog_%s_%s.json" % (query.get("type", ""), query.get("branch", ""))
    else:
        return json.dumps({"code": 5000, "tip": "mock 数据里没有这个接口: " + path}).encode("utf-8")
    file_path = os.path.join(MOCK_DIR, name)
    if not os.path.isfile(file_path):
        print("[api] mock missing: %s" % name)
        return json.dumps({"code": 5000, "tip": "mock 数据里没有: " + name}).encode("utf-8")
    with open(file_path, "rb") as fp:
        body = fp.read()
    if sliced:
        resp = json.loads(body.decode("utf-8"))
        offset = to_int(query.get("offset"), 0)
        limit = to_int(query.get("limit"), 20)
        resp["data"] = (resp.get("data") or [])[offset:offset + limit]
        body = json.dumps(resp, ensure_ascii=False).encode("utf-8")
    return body


def lan_ips():
    """尽量列出可用的局域网地址，方便用手机/平板打开预览。"""
    ips = []
    # 方式一：按路由选出的本机地址（大多数情况下就是 Wi-Fi 网卡地址）
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            sock.connect(("8.8.8.8", 80))
            ips.append(sock.getsockname()[0])
        finally:
            sock.close()
    except Exception:
        pass
    # 方式二：主机名解析出来的地址，作为补充
    try:
        for info in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            ip = info[4][0]
            if ip not in ips and not ip.startswith("127."):
                ips.append(ip)
    except Exception:
        pass
    return ips


def main():
    global port, use_mock, upstream
    for arg in sys.argv[1:]:
        arg = arg.strip()
        if not arg:
            continue
        if arg.isdigit():
            port = int(arg)
        elif arg in ("mock", "--mock"):
            use_mock = True
        elif arg.startswith("http://") or arg.startswith("https://"):
            # 转发到自建服务，例如 python serve.py 8080 http://127.0.0.1:7777
            upstream = arg.rstrip("/")
        else:
            print("unknown argument: %s" % arg)
            print(__doc__)
            return 1

    socketserver.TCPServer.allow_reuse_address = True
    # 多线程，避免浏览器并发请求时互相等
    with socketserver.ThreadingTCPServer(("0.0.0.0", port), PreviewHandler) as httpd:
        # flush=True：输出被重定向到文件时也能马上看到提示
        print("serve dir : %s" % ROOT, flush=True)
        print("api       : %s" % ("mock data in %s" % MOCK_DIR if use_mock else upstream), flush=True)
        print("preview   : http://127.0.0.1:%d/index.html" % port, flush=True)
        for ip in lan_ips():
            print("in LAN    : http://%s:%d/index.html  (phone/tablet preview)" % (ip, port), flush=True)
        print("stop      : Ctrl+C", flush=True)
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nbye", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
