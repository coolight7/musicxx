const loadingWidget = document.getElementById("loading");
const bindSuccessWidget = document.getElementById("bind_success");
const bindFaildWidget = document.getElementById("bind_faild");

function getQueryString(name) {
    let reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
    let r = window.location.search.substr(1).match(reg);
    if (r != null) {
        return unescape(r[2]);
    };
    return null;
}

window.onload = () => {
    const jwtToken = getQueryString("state"); // state
    const authCode = getQueryString("code");
    console.log(jwtToken + " - " + authCode);
    xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            loadingWidget.style = "display:none";
            if (xhr.status >= 200 && xhr.status < 300) {
                try {
                    const code = JSON.parse(xhr.responseText)["code"]
                    console.log(code)
                    if (code == 2000) {
                        bindSuccessWidget.style = "";
                        return;
                    }
                } catch (e) {
                    console.log(e)
                }
            }
            bindFaildWidget.style = "";
        }
    };
    xhr.open(
        "POST", 
        "https://api.music.mimicry.cool/api/user/baiduPan/refreshAccessToken?authCode=" + authCode + "&state="+jwtToken, 
        true,
    );
    xhr.send(null);
}
