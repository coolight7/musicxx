#!/usr/bin/env node
/**
 * 拟声下载页的本地预览服务（Node 版，无第三方依赖）。
 *
 * 做两件事：
 * 1. 把上一级目录（download/）当成静态站点提供，直接预览 index.html 的效果；
 * 2. 把 /api 开头的请求转发到接口服务，页面里的相对路径接口不用改就能取到真实数据。
 *
 * 用法：
 *     node serve.js                            // 默认 8080 端口，转发到线上接口
 *     node serve.js 9000                       // 指定端口
 *     node serve.js 9000 http://127.0.0.1:7777 // 转发到自建服务
 *     node serve.js 9000 mock                  // 断网时用 tool/mock/ 里的快照数据预览
 * 按 Ctrl+C 停止。
 */
'use strict';

const fs = require('fs');
const http = require('http');
const https = require('https');
const os = require('os');
const path = require('path');
const url = require('url');

const HERE = __dirname;
const ROOT = path.resolve(HERE, '..'); // 静态站点根目录（download/）
const MOCK_DIR = path.join(HERE, 'mock');
const UPSTREAM = 'https://api.music.bool.run';
const DEFAULT_PORT = 8080;
// 只转发这几个头的目的是避开 gzip、keep-alive 这类需要额外处理的头
const FORWARD_HEADERS = ['accept', 'content-type', 'cookie', 'referer', 'origin', 'user-agent'];

const MIME = {
    '.html': 'text/html; charset=utf-8',
    '.htm': 'text/html; charset=utf-8',
    '.js': 'application/javascript; charset=utf-8',
    '.css': 'text/css; charset=utf-8',
    '.json': 'application/json; charset=utf-8',
    '.txt': 'text/plain; charset=utf-8',
    '.md': 'text/markdown; charset=utf-8',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.jpeg': 'image/jpeg',
    '.gif': 'image/gif',
    '.webp': 'image/webp',
    '.svg': 'image/svg+xml',
    '.ico': 'image/x-icon'
};

function answer(res, status, type, body) {
    res.writeHead(status, {
        'Content-Type': type,
        'Content-Length': Buffer.byteLength(body),
        // 关掉缓存，改完代码刷新就能看到
        'Cache-Control': 'no-store, max-age=0'
    });
    res.end(body);
}

function serveStatic(req, res) {
    const pathname = decodeURIComponent(url.parse(req.url).pathname || '/');
    let filePath = path.resolve(ROOT, '.' + pathname);
    if (filePath !== ROOT && filePath.indexOf(ROOT + path.sep) !== 0) {
        answer(res, 403, 'text/plain; charset=utf-8', '403 forbidden');
        console.log('[http] 403 ' + pathname);
        return;
    }
    fs.stat(filePath, function (err, stat) {
        if (!err && stat.isDirectory()) {
            filePath = path.join(filePath, 'index.html');
        }
        fs.readFile(filePath, function (err2, data) {
            if (err2) {
                answer(res, 404, 'text/plain; charset=utf-8', '404 not found: ' + pathname);
                console.log('[http] 404 ' + pathname);
                return;
            }
            answer(res, 200, MIME[path.extname(filePath).toLowerCase()] || 'application/octet-stream', data);
            console.log('[http] 200 ' + pathname);
        });
    });
}

function mockResponse(reqUrl) {
    const parsed = url.parse(reqUrl, true);
    const query = parsed.query;
    let name = '';
    let sliced = false;
    if (reqUrl.indexOf('/api/procedure/get/download/list') === 0) {
        name = 'download-list.json';
    } else if (reqUrl.indexOf('/api/procedure/get/version/list') === 0) {
        name = 'version_list_' + (query.type || '') + '_' + (query.branch || 'Main') + '.json';
        sliced = true; // mock 也按 offset/limit 切片，方便预览分页
    } else if (reqUrl.indexOf('/api/procedure/get/versionLog') === 0) {
        name = 'versionLog_' + (query.type || '') + '_' + (query.branch || '') + '.json';
    } else {
        return JSON.stringify({ code: 5000, tip: 'mock 数据里没有这个接口: ' + reqUrl });
    }
    try {
        const body = fs.readFileSync(path.join(MOCK_DIR, name), 'utf8');
        if (!sliced) {
            return body;
        }
        const resp = JSON.parse(body);
        const offset = parseInt(query.offset, 10) || 0;
        const limit = parseInt(query.limit, 10) || 20;
        resp.data = (resp.data || []).slice(offset, offset + limit);
        return JSON.stringify(resp);
    } catch (e) {
        console.log('[api] mock missing: ' + name);
        return JSON.stringify({ code: 5000, tip: 'mock 数据里没有: ' + name });
    }
}

function proxy(req, res) {
    const headers = {};
    FORWARD_HEADERS.forEach(function (key) {
        if (req.headers[key]) {
            headers[key] = req.headers[key];
        }
    });
    const target = url.parse(upstream);
    const client = target.protocol === 'https:' ? https : http;
    const upstreamReq = client.request(
        { hostname: target.hostname, port: target.port, path: req.url, method: req.method, headers: headers },
        function (up) {
            res.writeHead(up.statusCode, {
                'Content-Type': up.headers['content-type'] || 'application/json; charset=utf-8',
                'Cache-Control': 'no-store, max-age=0'
            });
            up.pipe(res);
            console.log('[api] ' + up.statusCode + ' ' + req.url);
        }
    );
    upstreamReq.on('error', function (err) {
        console.log('[api] fail ' + req.url + ' -> ' + err.message);
        answer(
            res,
            502,
            'text/plain; charset=utf-8',
            '502 upstream request failed: ' + err.message + ' (try: node serve.js ' + port + ' mock)'
        );
    });
    req.pipe(upstreamReq);
}

function lanIps() {
    const ips = [];
    const interfaces = os.networkInterfaces();
    Object.keys(interfaces).forEach(function (name) {
        (interfaces[name] || []).forEach(function (info) {
            if (info.family === 'IPv4' && !info.internal && ips.indexOf(info.address) < 0) {
                ips.push(info.address);
            }
        });
    });
    return ips;
}

let port = DEFAULT_PORT;
let useMock = false;
let upstream = UPSTREAM;
process.argv.slice(2).forEach(function (arg) {
    if (/^\d+$/.test(arg)) {
        port = parseInt(arg, 10);
    } else if (arg === 'mock' || arg === '--mock') {
        useMock = true;
    } else if (arg.indexOf('http://') === 0 || arg.indexOf('https://') === 0) {
        // 转发到自建服务，例如 node serve.js 8080 http://127.0.0.1:7777
        upstream = arg.replace(/\/+$/, '');
    } else {
        console.log('unknown argument: ' + arg);
    }
});

http.createServer(function (req, res) {
    if (req.url.indexOf('/api/') === 0) {
        if (useMock) {
            answer(res, 200, 'application/json; charset=utf-8', mockResponse(req.url));
            console.log('[api] mock ' + req.url);
            return;
        }
        proxy(req, res);
        return;
    }
    serveStatic(req, res);
}).listen(port, '0.0.0.0', function () {
    console.log('serve dir : ' + ROOT);
    console.log('api       : ' + (useMock ? 'mock data in ' + MOCK_DIR : UPSTREAM));
    console.log('preview   : http://127.0.0.1:' + port + '/index.html');
    lanIps().forEach(function (ip) {
        console.log('in LAN    : http://' + ip + ':' + port + '/index.html  (phone/tablet preview)');
    });
    console.log('stop      : Ctrl+C');
});
