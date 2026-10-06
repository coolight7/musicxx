var listId = "list";
var downlistId = "downlist";
var popupId = "mimicry_popup";
var popupVersionId = "popupVersion";
var logPopupId = "mimicry_log_popup";
var logPopupVersionId = "logPopupVersion";
var logPopupTextId = "logPopupText";
var versionPopupId = "mimicry_version_popup";
var versionPopupTitleId = "versionPopupTitle";
var versionPopupListId = "versionPopupList";
var versionPopupMoreId = "versionPopupMore";
var versionMoreBtnId = "versionMoreBtn";
var adaptBodyId = "adaptBody";

// 版本列表一页 20 个版本
var VERSION_PAGE_SIZE = 20;

var netRespData = [];

// 每个平台（type + branch）的版本列表状态：
// records 已取回的记录（版本倒序），showCount 当前展示多少条，
// serverPaged 服务端是否支持分页（不支持就一次取全后在本地分页）
var versionStore = {};
// 当前打开的版本弹窗属于哪个平台，-1 表示没打开
var versionPopupIndex = -1;

function loadData() {
    var listWidget = document.getElementById(listId);
    if (null == listWidget) {
        return false;
    }
    var innerHtml = "";
    for (var i = 0; i < netRespData.length; ++i) {
        var item = netRespData[i];
        var systemName = systemTypeToViewName(item["type"]);
        var branchName = branchTypeToViewName(item["branch"]);
        var depict = getDepict(item["type"], item["branch"]);

        innerHtml += ''
            + '<li class="cmusic_playlist_li_notransform" style="margin-top: 10px;margin-bottom: 10px;">'
            + '    <div class="mimicry_row" style="margin-bottom: 10px;justify-content: space-between;align-items: center;">'
            + '        <div class="mimicry_row" style="align-items: center;">'
            + '            <img class="cmusic_icon" src="images/' + systemTypeToIconFile(item["type"]) + '" />'
            + '            <div class="mimicry_row" style="align-items: center;">'
            + '                <span class="cmusic_textMain">' + systemName + ' ' + branchName + '</span>'
            + '            </div>'
            + '        </div>'
            + '        <button class="mimicry_button" onClick="download(' + i + ')">下载</button>'
            + '    </div>'
            + '    <div class="mimicry_row" style="margin-bottom: 10px;justify-content: space-between;align-items: center;">'
            + '        <span class="cmusic_version_chip" onClick="openVersionPopup(' + i + ')">'
            + '            <span class="cmusic_version_text" id="versiontext_' + i + '">' + escapeHtml(item["version_str"]) + '</span>'
            + '            <span class="cmusic_version_arrow"></span>'
            + '        </span>'
            + '        <button class="mimicry_button mimicry_button_normal" onClick="showUpdateLog(' + i + ')">更新日志</button>'
            + '    </div>'
            + '    <span class="cmusic_textCross" style="white-space: pre-line;">' + depict + '</span>'
            + '</li>';
    }
    listWidget.innerHTML = innerHtml;
    return true;
}

// 点『下载』：下载当前选中的那个版本
function download(index) {
    var item = getDownloadRecord(index);
    var list = null;
    try {
        list = JSON.parse(item["downlist"]);
    } catch (e) {
        list = null;
    }
    if (null == list) {
        list = [];
    }
    if (0 == list.length) {
        var link = (null == item["link"]) ? "" : "" + item["link"];
        if (link.length > 0) {
            window.open(link, "_blank");
            return;
        }
        showNoLink(item);
        return;
    }
    if (1 == list.length) {
        window.open(list[0]["link"], "_blank");
        return;
    }
    var innerHtml = "";
    for (var i = 0; i < list.length; ++i) {
        var downItem = list[i];
        var li = '<li class="cmusic_playlist_li_notransform" style="padding-top: 20px;padding-bottom: 20px;">'
            + '    <div class="mimicry_row" style="justify-content: space-between;">'
            + '        <span class="cmusic_textMain">{{item_name}}</span>'
            + '        <button '
            + '            class="{{item_class}}"'
            + '            onClick="downloadLink(\'{{item_link}}\')"'
            + '            >下载</button>'
            + '    </div>'
            + '    <span class="cmusic_textCross">{{item_depict}}</span>'
            + '</li>';
        li = li.replace("{{item_name}}", downItem["name"]);
        li = li.replace("{{item_class}}", (downItem["name"] == "arm64") ? "mimicry_button" : 'mimicry_button mimicry_button_normal');
        li = li.replace("{{item_link}}", downItem["link"]);
        li = li.replace("{{item_depict}}", downItem["depict"]);
        innerHtml += li;
    }
    var popupWidget = document.getElementById(downlistId);
    popupWidget.innerHTML = innerHtml;
    // 弹窗里标出要下载的是哪个版本
    setElementHtml(popupVersionId, escapeHtml(item["version_str"]));
    popupOpenById(popupId);
}

// 该版本没有安装包链接时在弹窗里说明，避免用户以为下载坏了
function showNoLink(inItem) {
    var popupWidget = document.getElementById(downlistId);
    if (null == popupWidget) {
        return;
    }
    popupWidget.innerHTML = ''
        + '<li class="cmusic_playlist_li_notransform" style="padding-top: 20px;padding-bottom: 20px;">'
        + '    <span class="cmusic_textCross">『该版本没有可用的安装包链接，请重新选择版本』</span>'
        + '</li>';
    setElementHtml(popupVersionId, escapeHtml(inItem["version_str"]));
    popupOpenById(popupId);
}

// 点『更新日志』：弹窗显示当前选中版本的更新日志
function showUpdateLog(index) {
    var item = getVersionRecord(index);
    setElementHtml(logPopupVersionId, versionTitleHtml(index, item));
    var logStr = (null == item["log"]) ? "" : "" + item["log"];
    if (logStr.length <= 0) {
        setElementHtml(logPopupTextId, '<span class="cmusic_textCross">该版本没有填写更新日志</span>');
    } else {
        setElementHtml(logPopupTextId, versionLogToHtml(logStr));
    }
    popupOpenById(logPopupId);
}

function downloadLink(link) {
    if (link && link.length > 0) {
        window.open(link, "_blank");
    }
}

function popupOpenById(inId) {
    setElementClass(inId, "mimicry_popup");
}

function popupClose() {
    setElementClass(popupId, "musicxx_hide");
    setElementClass(logPopupId, "musicxx_hide");
    setElementClass(versionPopupId, "musicxx_hide");
}

function removeBobbom() {
    return false;
}

function autoAdaptLayout() {
    var bodyWidget = document.getElementById(adaptBodyId);
    if (window.innerHeight > window.innerWidth) {
        // 竖屏
        bodyWidget.className = "mimicry_column";
    } else {
        // 横屏
        bodyWidget.className = "mimicry_row";
    }
}

/* ==================== 版本列表与更新日志 ==================== */

// 文本转义后再写进页面，避免日志里的 < > & 破坏页面
function escapeHtml(inText) {
    var text = (null == inText) ? "" : "" + inText;
    text = text.replace(/&/g, "&amp;");
    text = text.replace(/</g, "&lt;");
    text = text.replace(/>/g, "&gt;");
    return text;
}

// 更新日志里只有少量 markdown 写法，这里只处理 `高亮` 和行首的 ## 标题
function versionLogToHtml(inText) {
    var html = escapeHtml(inText);
    html = html.replace(/`([^`\r\n]+)`/g, '<span class="cmusic_verlog_code">$1</span>');
    html = html.replace(/(^|[\r\n])[ \t]*#{1,6}[ \t]+([^\r\n]+)/g, '$1<span class="cmusic_verlog_head">$2</span>');
    return html;
}

// 更新日志里记录的是秒级时间戳，这里按本地时间显示到日期
function formatDateStr(inTimestamp) {
    var stamp = parseInt(inTimestamp, 10);
    if (isNaN(stamp) || stamp <= 0) {
        return "";
    }
    var date = new Date(stamp * 1000);
    var month = date.getMonth() + 1;
    var day = date.getDate();
    return date.getFullYear() + "-" + (month < 10 ? "0" + month : "" + month) + "-" + (day < 10 ? "0" + day : "" + day);
}

function versionCacheKey(inItem) {
    return inItem["type"] + "_" + inItem["branch"];
}

// 是不是下载列表里的那个最新版本
// 版本列表里的记录不带安装包链接，最新版本统一用下载列表里的那条
function isLatestVersion(index, inItem) {
    var item = netRespData[index];
    return parseInt(inItem["version"], 10) == parseInt(item["version"], 10);
}

// 取某个平台的版本列表状态
function getVersionStore(index) {
    var key = versionCacheKey(netRespData[index]);
    var store = versionStore[key];
    if (null == store) {
        store = {
            "records": [],      // 已经取回的记录，版本倒序
            "showCount": 0,     // 列表里现在展示多少条
            "serverPaged": true, // 服务端支持分页时，取一页显示一页
            "noMore": false,    // 没有下一页了
            "loading": false,   // 正在请求
            "failed": false,    // 上一次请求失败
            "selected": parseInt(netRespData[index]["version"], 10) // 当前选中的版本，默认最新
        };
        versionStore[key] = store;
    }
    return store;
}

// 当前选中的版本记录，还没取到这份数据时返回 null
function getSelectedRecord(index) {
    var store = getVersionStore(index);
    for (var i = 0; i < store.records.length; ++i) {
        if (store.records[i]["version"] == store.selected) {
            return store.records[i];
        }
    }
    return null;
}

// 看日志用的记录：取不到时退回下载列表里的最新版本
function getVersionRecord(index) {
    var record = getSelectedRecord(index);
    if (null == record) {
        return netRespData[index];
    }
    return record;
}

// 下载用的记录：最新版本用下载列表里的那条（带安装包链接）
function getDownloadRecord(index) {
    var record = getVersionRecord(index);
    if (true == isLatestVersion(index, record)) {
        return netRespData[index];
    }
    return record;
}

function setElementHtml(inId, inHtml) {
    var widget = document.getElementById(inId);
    if (null != widget) {
        widget.innerHTML = inHtml;
    }
}

function setElementClass(inId, inClassName) {
    var widget = document.getElementById(inId);
    if (null != widget) {
        widget.className = inClassName;
    }
}

// 解析接口返回的 {"code":..,"data":..}，取不到数据时返回 null
function parseRespData(inText) {
    var respJson = null;
    if ("undefined" != typeof JSON && null != JSON.parse) {
        try {
            respJson = JSON.parse(inText);
        } catch (e) {
            respJson = null;
        }
    }
    if (null == respJson) {
        try {
            respJson = (new Function("return " + inText))();
        } catch (e) {
            respJson = null;
        }
    }
    if (null == respJson || null == respJson["data"] || null == respJson["data"].length) {
        return null;
    }
    return respJson["data"];
}

function requestJsonList(inUrl, inCallback) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (4 != xhr.readyState) {
            return;
        }
        var list = null;
        if (xhr.status >= 200 && xhr.status < 300) {
            list = parseRespData(xhr.responseText);
        }
        inCallback(list);
    };
    xhr.open("GET", inUrl, true);
    xhr.setRequestHeader("Cache-control", "no-store, max-age=0");
    xhr.send(null);
}

// 点版本号：打开版本弹窗，第一次打开时才去取第一页版本
function openVersionPopup(index) {
    var item = netRespData[index];
    var store = getVersionStore(index);
    versionPopupIndex = index;
    setElementHtml(
        versionPopupTitleId,
        escapeHtml(systemTypeToViewName(item["type"]) + " " + branchTypeToViewName(item["branch"]))
    );
    popupOpenById(versionPopupId);
    if (0 == store.records.length) {
        setElementHtml(
            versionPopupListId,
            '<li class="cmusic_playlist_li_notransform" style="padding-top: 12px;padding-bottom: 12px;">'
            + '<span class="cmusic_textCross">正在加载版本列表...</span></li>'
        );
        loadMoreVersions();
    } else {
        renderVersionPopup(index);
    }
}

// 取下一页版本（弹窗底部的『加载更多』也走这里）
function loadMoreVersions() {
    var index = versionPopupIndex;
    if (index < 0) {
        return;
    }
    var store = getVersionStore(index);
    if (true == store.loading) {
        return;
    }
    // 本地还有没展示出来的，直接往下翻
    if (store.showCount < store.records.length) {
        store.showCount = Math.min(store.showCount + VERSION_PAGE_SIZE, store.records.length);
        renderVersionPopup(index);
        return;
    }
    if (true == store.noMore) {
        return;
    }
    loadVersionPage(index);
}

// 一页一页地从服务端取版本记录
function loadVersionPage(index) {
    var item = netRespData[index];
    var store = getVersionStore(index);
    store.loading = true;
    store.failed = false;
    renderVersionPopup(index);
    if (true == store.serverPaged) {
        requestJsonList(
            "/api/procedure/get/version/list?application=Musicxx"
            + "&type=" + encodeURIComponent(item["type"])
            + "&branch=" + encodeURIComponent(item["branch"])
            + "&offset=" + store.records.length
            + "&limit=" + VERSION_PAGE_SIZE,
            function (list) {
                if (null != list) {
                    store.loading = false;
                    appendVersionRecords(store, list);
                    store.showCount = store.records.length; // 服务端分页：取回多少就展示多少
                    store.noMore = (list.length < VERSION_PAGE_SIZE);
                    renderVersionPopup(index);
                    return;
                }
                // 服务端还没有分页接口（老服务端）：退回更新日志接口，一次取全后在本地分页
                store.serverPaged = false;
                loadVersionPage(index);
            }
        );
        return;
    }
    requestJsonList(
        "/api/procedure/get/versionLog?from=0&application=Musicxx"
        + "&type=" + encodeURIComponent(item["type"])
        + "&branch=" + encodeURIComponent(item["branch"]),
        function (list) {
            store.loading = false;
            if (null == list) {
                store.failed = true;
                renderVersionPopup(index);
                return;
            }
            appendVersionRecords(store, list);
            store.showCount = Math.min(VERSION_PAGE_SIZE, store.records.length);
            store.noMore = true;
            renderVersionPopup(index);
        }
    );
}

function appendVersionRecords(store, list) {
    for (var i = 0; i < list.length; ++i) {
        store.records[store.records.length] = list[i];
    }
    // 版本号倒序，最新的排在最前面
    store.records.sort(function (left, right) {
        return right["version"] - left["version"];
    });
    // 同一版本号有多条记录时只留一条
    var unique = [];
    for (var j = 0; j < store.records.length; ++j) {
        if (j > 0 && store.records[j]["version"] == unique[unique.length - 1]["version"]) {
            continue;
        }
        unique[unique.length] = store.records[j];
    }
    store.records = unique;
}

// 画版本弹窗：列表 + 底部按钮（加载更多 / 正在加载 / 重试）
function renderVersionPopup(index) {
    var store = getVersionStore(index);
    var count = Math.min(store.showCount, store.records.length);
    var innerHtml = "";
    if (0 == count) {
        innerHtml = '<li class="cmusic_playlist_li_notransform" style="padding-top: 12px;padding-bottom: 12px;">'
            + '<span class="cmusic_textCross">没有取到版本列表</span></li>';
    }
    for (var i = 0; i < count; ++i) {
        var record = store.records[i];
        var isSelected = (record["version"] == store.selected);
        innerHtml += '<li class="cmusic_playlist_li_notransform cmusic_version_item"'
            + ' onClick="chooseVersion(' + index + ', ' + record["version"] + ')"'
            + ' style="padding-top: 12px;padding-bottom: 12px;">'
            + '    <div class="mimicry_row" style="justify-content: space-between;align-items: center;">'
            + '        <span class="cmusic_textMain"' + (isSelected ? ' style="color: #66ccff;"' : '') + '>'
            + escapeHtml(record["version_str"])
            + (true == isLatestVersion(index, record) ? '（最新）' : '')
            + '</span>'
            + '        <span class="cmusic_textCross">' + escapeHtml(formatDateStr(record["created_time"]))
            + (isSelected ? ' · 已选择' : '') + '</span>'
            + '    </div>'
            + '</li>';
    }
    if (true == store.failed) {
        innerHtml += '<li class="cmusic_playlist_li_notransform" style="padding-top: 12px;padding-bottom: 12px;">'
            + '<span class="cmusic_textCross" style="color: #F00;">『版本列表加载失败』</span></li>';
    }
    setElementHtml(versionPopupListId, innerHtml);

    var moreBtn = document.getElementById(versionMoreBtnId);
    if (true == store.loading) {
        setElementClass(versionPopupMoreId, "mimicry_row");
        if (null != moreBtn) {
            moreBtn.innerHTML = "正在加载...";
            moreBtn.disabled = true;
        }
        return;
    }
    if (null != moreBtn) {
        moreBtn.innerHTML = (true == store.failed) ? "重试" : "加载更多";
        moreBtn.disabled = false;
    }
    var hasMore = (store.showCount < store.records.length) || (false == store.noMore);
    setElementClass(versionPopupMoreId, hasMore ? "mimicry_row" : "musicxx_hide");
}

// 选中某个版本：更新版本号徽章并把弹窗关掉
function chooseVersion(index, version) {
    var store = getVersionStore(index);
    store.selected = version;
    setElementHtml("versiontext_" + index, escapeHtml(versionText(index, version)));
    popupClose();
}

function versionText(index, version) {
    var store = getVersionStore(index);
    for (var i = 0; i < store.records.length; ++i) {
        if (store.records[i]["version"] == version) {
            return "" + store.records[i]["version_str"];
        }
    }
    var item = netRespData[index];
    if (item["version"] == version) {
        return "" + item["version_str"];
    }
    return "" + version;
}

// 更新日志弹窗的标题：版本号、名称、日期
function versionTitleHtml(index, inItem) {
    var html = escapeHtml(inItem["version_str"]);
    var nameStr = (null == inItem["name"]) ? "" : "" + inItem["name"];
    if (nameStr.length > 0 && nameStr.indexOf("" + inItem["version_str"]) < 0) {
        html += " · " + escapeHtml(nameStr);
    }
    var dateStr = formatDateStr(inItem["created_time"]);
    if (dateStr.length > 0) {
        html += " · " + dateStr;
    }
    if (true == isLatestVersion(index, inItem)) {
        html += ' · <span style="color: #66ccff;">当前最新版本</span>';
    }
    return html;
}

/* =========================================================== */

window.onresize = autoAdaptLayout;

window.onload = function () {
    // 自适应横竖屏
    autoAdaptLayout();
    // 请求下载列表
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            var rebool = (function () {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        netRespData = JSON.parse(xhr.responseText)["data"]
                    } catch (e) {
                        console.log(e);
                        netRespData = ((new Function("return " + xhr.responseText))())["data"];
                    }
                    function getNum(item) {
                        var branchNum = 0;
                        switch (item.branch) {
                            case "Main": {
                                branchNum = 10;
                            } break;
                            case "SPA4": {
                                branchNum = 9;
                            } break;
                            case "SPEXAMINE": {
                                branchNum = 8;
                            } break;
                            case "SPEXAMINELIMIT": {
                                branchNum = 7;
                            } break;
                            case "SPA5": {
                                branchNum = 6;
                            } break;
                        }
                        var systemNum = 0;
                        switch (item.type) {
                            case "android": {
                                systemNum = 10;
                            } break;
                            case "ios": {
                                systemNum = 9;
                            } break;
                            case "windows": {
                                systemNum = 8;
                            } break;
                            case "macos": {
                                systemNum = 7;
                            } break;
                            case "linux": {
                                systemNum = 6;
                            } break;
                        }
                        return systemNum + branchNum * 10;
                    }

                    netRespData.sort(function (left, right) {
                        return (getNum(right) - getNum(left));
                    });
                    return loadData();
                } else {
                    console.log("request faild")
                }
                return false;
            })();
            if (true === rebool) {
                // 加载成功
            } else {
                // 加载失败
                var listWidget = document.getElementById(listId);
                if (null != listWidget) {
                    listWidget.innerHTML =
                        '<li class="cmusic_playlist_li_notransform" style="display: flex;justify-content: center;align-items: center;">'
                        + '    <span class="cmusic_textMain" style="color: #F00">『下载列表加载失败』</span>'
                        + '</li>';
                }
            }
        }
    };
    var url = "/api/procedure/get/download/list?application=Musicxx";
    // if (window.location.host.indexOf("127.0.0.1") >= 0) {
    //     // 本地测试
    //     url = "https://api.music.bool.run" + url;
    //     // url = "http://127.0.0.1:1000" + url;
    // }
    xhr.open("GET", url, true);
    xhr.setRequestHeader("Cache-control", "no-store, max-age=0");
    xhr.send(null);
};

function systemTypeToViewName(type) {
    switch (type) {
        case "android":
            return "安卓";
        case 'ios':
            return "IPhone";
        case "windows":
            return "windows";
        case "macos":
            return "macos";
        case "linux":
            return "linux";
    }
    return "";
}

function systemTypeToIconFile(type) {
    switch (type) {
        case "android":
            return "android.png";
        case "ios":
            return "apple.png";
        case "windows":
            return "windows.png";
        case "macos":
            return "apple.png";
        case "linux":
            return "linux.png";
    }
    return "gravatar.jpg";
}

function branchTypeToViewName(type) {
    switch (type) {
        case "SPA4":
            return "安卓4支持版";
        case "SPA5":
            return "安卓5支持版";
        case "Main":
            return "";
    }
}

function getDepict(system, branch) {
    switch (system) {
        case "android":
            if (branch == "Main") {
                return '• 系统要求：Android 7.0 或以上';
            } else if (branch == "SPA5") {
                return '• 系统要求：Android 5.0 或以上;\n'
                    + '• 为兼容安卓5.x，舍弃了很多功能和性能，不建议使用该分支。';
            } else if (branch == "SPA4") {
                return '• 系统要求：Android 4.1 或以上;\n'
                    + '• 为兼容安卓4.x，舍弃了很多功能和性能，且已经很久未更新，不建议使用该分支。';
            }
        case 'ios':
            return '• 系统要求：arm64 IOS 13.0 或以上\n'
                + '• 目前仅开放 未签名安装包，需要自行搜索如何安装\n';
        case "windows":
            if (branch == "Main") {
                return '• 系统要求：x64 Windows 10 或以上';
            }
        case "macos":
            return '• 系统要求：arm64/x64 Macos11 或以上';
        case "linux":
            return '• 系统要求：x64\n'
                + '• 可能需要安装依赖包，建议查看<a class="cmusic_textLink" href="https://blog.music.bool.run/help/list/selectBranch.html#linux">安装帮助</a>';
    }
    return ""
}
