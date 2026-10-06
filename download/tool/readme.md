# download/tool —— 下载页本地预览服务

用来在本地预览 `download/index.html`（下载页）的效果：它把 `download/` 当静态站点提供，
并把页面里的 `/api/...` 请求转发到接口服务，所以页面代码一行都不用改。

## 快速开始

| 系统 | 怎么做 |
| --- | --- |
| Windows | 双击 `preview.bat`（依次尝试 node、python、py） |
| macOS / Linux | `./preview.sh` |
| 手动启动 | `python serve.py` 或 `node serve.js` |

启动后打开控制台输出的地址，默认是 <http://127.0.0.1:8080/index.html>。
按 `Ctrl+C` 停止。

## 参数

```bash
python serve.py                             # 默认 8080 端口，接口走线上
python serve.py 9000                        # 换端口
python serve.py 9000 http://127.0.0.1:7777  # 接口转发到自建服务
python serve.py 9000 mock                   # 断网时用 tool/mock/ 里的快照数据
node serve.js 9000 mock                     # Node 版同样支持
```

Windows 上给 bat 传参：`preview.bat 9000 mock`、`preview.bat 9000 http://127.0.0.1:7777`。

## 说明

- **手机 / 平板预览**：启动后会打印局域网地址（如 `http://192.168.x.x:8080/index.html`），
  手机连同一个 Wi-Fi 就能打开；Windows 首次会弹防火墙询问，选允许。
- **接口**：`/api/...` 默认转发到 `https://api.music.bool.run`，需要联网。要换成自建服务，
  按上面的方式传地址，或改 `serve.py` / `serve.js` 里的 `UPSTREAM`。
- **mock 数据**（`tool/mock/`）：
  - `download-list.json`：下载列表接口的真实返回快照；
  - `version_list_<type>_<branch>.json`：版本列表接口（`/get/version/list`）的快照，
    每个平台 45 个版本（其中更老的那些是占位版本，只为看「加载更多」的分页效果）；
    mock 服务会按 `offset` / `limit` 切片，跟真实服务端一样一页 20 条。
  - `versionLog_*.json`：更新日志接口的快照，用来预览「服务端还是老版本」时的兜底路径。
  - 快照只用于看排版和交互，数据会过期；**旧版本的安装包链接是 `example.com` 占位地址**，
    点了不会真的下载，最新版本用的仍是 `download-list.json` 里的真实链接。
- **不缓存**：服务会给所有响应加 `Cache-Control: no-store`，改完 `index.html` / `run.js`
  直接刷新就能看到效果。
- **端口被占用**：换一个端口，例如 `preview.bat 9000`。
- **页面报“下载列表加载失败”**：说明接口没通（断网、被墙、请求头校验等），
  可以先用 `mock` 模式看排版。

## 注意

这个目录就在站点根目录（`download/`）下，部署整站时会被一起上传，脚本内容公开可见。
如果不希望公开，部署前删掉本目录，或在 web 服务器上禁止访问 `/tool/`。
