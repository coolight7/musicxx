var listId = "list";
var downlistId = "downlist";
var popupId = "mimicry_popup";
var adaptBodyId = "adaptBody";

var netRespData = [];

function loadData() {
    var listWidget = document.getElementById(listId);
    if (null != listWidget) {
        var innerHtml = "";
        for (var i = 0; i < netRespData.length; ++i) {
            var item = netRespData[i];
            var systemName = systemTypeToViewName(item["type"]);
            var branchName = branchTypeToViewName(item["branch"]);
            if (item["branch"] == "SPA4") {
                continue;
            }
            var depict = getDepict(item["type"], item["branch"]);
            var li =
                '<li class="cmusic_playlist_li_notransform" style="margin-top: 10px;margin-bottom: 10px;">'
                + '    <div class="mimicry_row" style="margin-bottom: 10px;justify-content: space-between;align-items: center;">'
                + '        <div class="mimicry_row" style="align-items: center;">'
                + '            <img class="cmusic_icon" src="images/{{item_icon}}" />'
                + '            <div class="mimicry_row" style="align-items: center;">'
                + '                <span class="cmusic_textMain">{{systemName}} {{branchName}}</span>'
                + '            </div>'
                + '        </div>'
                + '        <button class="mimicry_button" onClick="download({{index}})">下载</button>'
                + '    </div>'
                + '    <div class="mimicry_row">'
                + '        <div style="display: inline-block; white-space: nowrap; background-color: #66ccff; border-radius: 5px; padding-left: 5px; padding-right: 5px; margin-bottom:10px; align-items: center; justify-content: center;">'
                + '            <span class="cmusic_textCross" style="color: #fff;">{{item_version_str}}</span>'
                + '        </div>'
                + '    </div>'
                + '    <span class="cmusic_textCross" style="white-space: pre-line;">{{depict}}</span>'
                + '</li>';
            li = li.replace("{{item_icon}}", systemTypeToIconFile(item["type"]));
            li = li.replace("{{systemName}}", systemName);
            li = li.replace("{{branchName}}", branchName);
            li = li.replace("{{index}}", i);
            li = li.replace("{{item_version_str}}", item['version_str']);
            li = li.replace("{{depict}}", depict);
            innerHtml += li;
        }
        listWidget.innerHTML = innerHtml;
        return true;
    }
    return false;
}

function download(index) {
    var item = netRespData[index];
    var list = JSON.parse(item["downlist"]);
    if (list.length == 0) {
        window.open(item["link"], "_blank");
    } else if (list.length == 1) {
        window.open(list[0]["link"], "_blank");
    } else {
        var innerHtml = "";
        for (var i = 0; i < list.length; ++i) {
            var item = list[i];
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
            li = li.replace("{{item_name}}", item["name"]);
            li = li.replace("{{item_button_class}}", item["name"]);
            li = li.replace("{{item_class}}", (item["name"] == "arm64") ? "mimicry_button" : 'mimicry_button mimicry_button_normal');
            li = li.replace("{{item_link}}", item["link"]);
            li = li.replace("{{item_depict}}", item["depict"]);
            innerHtml += li;
        }
        var popupWidget = document.getElementById(downlistId);
        popupWidget.innerHTML = innerHtml;
        popupOpen();
    }
}

function downloadLink(link) {
    if (link && link.length > 0) {
        window.open(link, "_blank");
    }
}

function popupOpen() {
    var popupWidget = document.getElementById(popupId);
    if (null != popupWidget) {
        popupWidget.className = "mimicry_popup";
    }
}

function popupClose() {
    var popupWidget = document.getElementById(popupId);
    if (null != popupWidget) {
        popupWidget.className = "musicxx_hide";
    }
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

window.onresize = autoAdaptLayout;

window.onload = function () {
    // 自适应横竖屏
    autoAdaptLayout();
    // 请求下载列表
    xhr = new XMLHttpRequest();
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
    var url = "/api/procedure/get/download/list";
    if (window.location.host.indexOf("127.0.0.1") >= 0) {
        // 本地测试
        // url = "https://api.music.mimicry.cool" + url;
        url = "http://127.0.0.1:1000" + url;
    }
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
            return "泛支持版";
        case "Main":
            return "";
    }
}

function getDepict(system, branch) {
    switch (system) {
        case "android":
            if (branch == "Main") {
                return '• 系统要求：Android 5.0 或以上';
            } else if (branch == "SPA4") {
                return '• 系统要求：Android 4.1 或以上;\n'
                    + '• 为兼容安卓4.x，舍弃了很多功能和性能，且已经很久未更新，不建议使用该分支。';
            }
        case 'ios':
            return '• 系统要求：arm64 IOS 12.1 或以上\n'
                + '• 目前仅开放 未签名安装包，需要自行搜索如何安装\n';
        case "windows":
            if (branch == "Main") {
                return '• 系统要求：x64 Windows 10 或以上';
            }
        case "macos":
            return '• 系统要求：arm64/x64 Macos11 或以上';
        case "linux":
            return '• 系统要求：x64\n'
                + '• 需要安装依赖包，建议查看<a class="cmusic_textLink" href="https://blog.mimicry.cool/help/list/selectBranch.html#linux">安装帮助</a>';
    }
    return ""
}
