#!/bin/bash
# 拟声下载页本地预览（macOS / Linux）：优先用 python3，其次用 node
# 用法：./preview.sh            默认 8080 端口
#      ./preview.sh 9000       指定端口
#      ./preview.sh 9000 mock  用 tool/mock 里的快照数据（断网时用）
cd "$(dirname "$0")" || exit 1

if command -v python3 >/dev/null 2>&1; then
    exec python3 serve.py "$@"
fi

if command -v node >/dev/null 2>&1; then
    exec node serve.js "$@"
fi

echo "[ERROR] python3 or node is required."
exit 1
