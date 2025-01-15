#include "MyHttpTask.h"
#include "workflow/HttpMessage.h"
#include "workflow/StringUtil.h"
#include <regex>
#include <string_view>


#include "GlobalUtil.h"

#ifdef _WIN32
#include <wsipv6ok.h>
#endif // _WIN32
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>

#endif // !_WIN32

#if _MSC_VER
#define strcasecmp _stricmp // strcasecmp 找不到标识符
#endif

#define HTTP_KEEPALIVE_DEFAULT (60 * 1000)
#define HTTP_KEEPALIVE_MAX     (300 * 1000)

using std::multimap;
using std::string;
using std::string_view;

MyHttpProcess_t Nullptr_MyHttpProcess = nullptr;

const char *mimicry::HttpBodyType_e::Form_data             = "multipart/form-data",
           *mimicry::HttpBodyType_e::X_www_form_urlencoded = "application/x-www-form-urlencoded",
           *mimicry::HttpBodyType_e::Json                  = "application/json",
           *mimicry::HttpBodyType_e::Xml                   = "application/xml",
           *mimicry::HttpBodyType_e::Text                  = "text/plain",
           *mimicry::HttpBodyType_e::Binary                = "application/octet-stream";

std::multimap<std::string, mimicry::HTTP_formData_s>
    mimicry::read_form_data(const std::string& in_bound, const std::string& in_data) {
    std::multimap<std::string, mimicry::HTTP_formData_s> remap;
    string_view                                          view{in_data};
    std::regex                                           reg_name{
        "\\r\\nContent-Disposition:\\s*form-data;\\s*name=\"(.*)\"(?:\\s*;\\s*filename=\"(.*)\"(?:\\s*;\\s*filename\\*=(.*)''(.*))?\\r\\nContent-Type:\\s*(.*))?\\r\\n\\r\\n"
    };
    string                   find_bound = "--" + in_bound;
    mimicry::HTTP_formData_s data;
    size_t                   pos = 0, pos_2 = 0;
    if ((pos = view.find(find_bound)) != string::npos) {
        pos += find_bound.size();
        for (; (pos_2 = view.find(find_bound, pos)) != string::npos;) {
            string_view block = view.substr(pos, pos_2 - pos); // 截取两个bound之间的内容
            std::match_results<string_view::const_iterator> block_match_view;
            if (std::regex_search(
                    block.begin(),
                    block.end(),
                    block_match_view,
                    reg_name
                )) { // 匹配 name 和可能存在的 filename
                data.fileName              = block_match_view.str(2);
                data.fileName_CharEncoding = block_match_view.str(3);
                data.fileName_             = block_match_view.str(4);
                data.fileType              = block_match_view.str(5);
                data.isFile                = data.fileName.empty() == false;
                const char* str_p          = &(*(block_match_view[0].second));
                size_t      len            = block.end() - 2 - block_match_view[0].second;
                // const string_view var_view = string_view{ block_match_view[0].second, block.end()
                // - 2 };
                data.value = string(str_p, len);
                remap.insert({block_match_view.str(1), data});
            }
            pos = pos_2 + find_bound.size();
        }
    }
    return remap;
}

const std::string& MyHttpRequest_c::get_path() {
    if (this->path.empty() || this->enable_cache_path == false) {
        const char *uri = this->get_request_uri(), *p = uri;
        while (*p != '\0' && *p != '?')
            p++;
        string path(uri, p - uri), repath;
        repath.resize(path.size(), '\0');
        for (size_t i = 0, j = 0, len = path.size(); i < len;) {
            repath[j] = path[i];
            if (path[i] == '/') {
                if (i + 1 < len && path[i + 1] == '/') { // //
                    i += 1;
                    continue;
                } else if (i + 2 < len && path[i + 1] == '.' && path[i + 2] == '/') { // /./
                    i += 2;
                    continue;
                } else if (i + 3 < len && path[i + 1] == '.' && path[i + 2] == '.' && path[i + 3] == '/') { // /../
                    i += 3;
                    for (--j; j > 0; --j) // 查找repath中上一个 / 的位置
                        if (repath[j] == '/')
                            break;
                    if (j < 0)
                        j = 0;
                    continue;
                }
            }
            ++i;
            ++j;
        }
        this->path = std::move(mimicry::url_decode(repath));
    }
    return this->path;
}

protocol::HttpHeaderCursor& MyHttpRequest_c::get_header(bool doFlush) {
    if (this->header == nullptr)
        this->header = new protocol::HttpHeaderCursor{this};
    else if (doFlush || enable_cache_header == false) { // 不启用缓存则重新生成
        delete (this->header);
        this->header = new protocol::HttpHeaderCursor{this};
    }
    this->header->rewind();
    return *(this->header);
}

void MyHttpRequest_c::clear_cache_header() {
    if (header) {
        delete header;
        header = nullptr;
    }
}

bool MyHttpRequest_c::find_header(const std::string& in_key, std::string& re_value) {
    return this->get_header().find(in_key, re_value);
}

std::string MyHttpRequest_c::get_parsed_body() {
    const void* body = nullptr; // 获取请求头中的数据
    size_t      size = 0;
    if (protocol::HttpRequest::get_parsed_body(&body, &size)) {
        return string((const char*)(body), size);
    } else {
        return "";
    }
}

const std::string& MyHttpRequest_c::get_router_path() {
    return this->router_path;
}

void MyHttpRequest_c::set_router_path(const std::string& in_router_path) {
    this->router_path = in_router_path;
}

std::map<std::string, std::string> MyHttpRequest_c::get_cookie() {
    std::string cookieStr;
    if (this->find_header("Cookie", cookieStr)) {
        return mimicry::Cookie_c::from_cookie(cookieStr);
    } else {
        return {};
    }
}

void MyHttpRequest_c::set_cookie(const mimicry::Cookie_c& in_cookie) {
    this->add_header_pair("Cookie", in_cookie.to_string_const().c_str());
}

std::string MyHttpRequest_c::get_argStr() {
    string type, body;
    this->find_header("Content-Type", type);
    if (type.find(mimicry::HttpBodyType_e::X_www_form_urlencoded) != string::npos)
        body = std::move(this->get_parsed_body());
    auto&& rearg = mimicry::get_url_arg(this->get_request_uri());
    if (rearg.empty() == false) {
        if (body.empty()) {
            return rearg;
        } else {
            body += '&';
            body += rearg;
        }
    }
    return body;
}

std::multimap<std::string, std::string> MyHttpRequest_c::get_arg() {
    string&&                 argStr = this->get_argStr();
    multimap<string, string> reMap;
    if (argStr != "") {
        auto&& strList = mimicry::strSplit(argStr, '&'); // 按&分割为键值对数组
        for (auto it = strList.begin(), endIt = strList.end(); it != endIt; ++it) {
            auto&& key_value = mimicry::strSplit(*it, '='); // 按等号分割为key-value
            switch (key_value.size()) {
            case 1:
                { // 仅有key
                    reMap.insert(
                        {mimicry::url_decode(mimicry::removeBetweenSpace(key_value[0])), ""}
                    );
                }
                break;
            case 2:
                { // 有key和value
                    reMap.insert(
                        {mimicry::url_decode(mimicry::removeBetweenSpace(key_value[0])),
                         mimicry::url_decode(mimicry::removeBetweenSpace(key_value[1]))}
                    );
                }
                break;
            }
        }
    }
    return reMap;
}

std::multimap<std::string, mimicry::HTTP_formData_s> MyHttpRequest_c::get_form_data() {
    std::smatch bound_m;
    string      type, body;
    this->find_header("Content-Type", type);
    if (std::regex_search(
            type,
            bound_m,
            std::regex(
                std::string(mimicry::HttpBodyType_e::Form_data) + "\\s*;\\s*boundary\\s*=\\s*(.+)"
            )
        )) {
        string&& bound     = bound_m.str(1);
        auto&&   parseBody = this->get_parsed_body();
        return mimicry::read_form_data(bound, parseBody);
    } else {
        return std::multimap<std::string, mimicry::HTTP_formData_s>{};
    }
}

bool MyHttpRequest_c::has_keep_alive_header() {
    return http_parser_keep_alive(this->parser);
}

bool MyHttpRequest_c::has_content_length_header() {
    string str;
    return this->find_header("Content-Length", str);
}

bool MyHttpRequest_c::has_connection_header() {
    string str;
    return this->find_header("Connection", str);
}

// Resp
protocol::HttpHeaderCursor& MyHttpResponse_c::get_header(bool doFlush) {
    if (this->header == nullptr)
        this->header = new protocol::HttpHeaderCursor{this};
    else if (doFlush || enable_cache_header == false) { // 不启用缓存则重新生成
        delete (this->header);
        this->header = new protocol::HttpHeaderCursor{this};
    }
    return *(this->header);
}

void MyHttpResponse_c::clear_cache_header() {
    if (header) {
        delete header;
        header = nullptr;
    }
}

bool MyHttpResponse_c::find_header(const std::string& in_key, std::string& re_value) {
    return this->get_header().find(in_key, re_value);
}

void MyHttpResponse_c::set_cookie(const mimicry::Cookie_c& in_cookie) {
    this->add_header_pair("Set-Cookie", in_cookie.to_string_const());
}

void MyHttpResponse_c::out_err(int in_code, const std::string& in_tip) noexcept {
    auto base = MyRespBaseEntity_c<int>::error(in_code, in_tip);
    this->out_entity(*base);
    delete base;
}

bool MyHttpResponse_c::has_keep_alive_header() {
    return http_parser_keep_alive(this->parser);
}

bool MyHttpResponse_c::has_content_length_header() {
    string str;
    return this->find_header("Content-Length", str);
}

bool MyHttpResponse_c::has_connection_header() {
    string str;
    return this->find_header("Connection", str);
}

// ServerTask
MyHttpServerTask_c::MyHttpServerTask_c(CommService* service, MyHttpProcess_t& process) noexcept :
    WFServerTask<MyHttpRequest_c, MyHttpResponse_c>(service, WFGlobal::get_scheduler(), process),
    req_is_alive_(false),
    req_has_keep_alive_header_(false) {
    WFServerTask<MyHttpRequest_c, MyHttpResponse_c>::set_callback([this](MyHttpTask_c* task) {
        this->run_callback();
    });
}

void MyHttpServerTask_c::handle(int state, int error) {
    if (state == WFT_STATE_TOREPLY) {
        if (false == this->req.has_keep_alive_header()) {
            // 默认 keep alive
            req_is_alive_ = true;
        } else {
            req_is_alive_ = this->req.is_keep_alive();
        }
        if (req_is_alive_ && this->req.has_keep_alive_header()) {
            protocol::HttpHeaderCursor  req_cursor{&this->req};
            protocol::HttpMessageHeader header;
            header.name                = "Keep-Alive";
            header.name_len            = strlen("Keep-Alive");
            req_has_keep_alive_header_ = req_cursor.find(&header);
            if (req_has_keep_alive_header_) {
                req_keep_alive_.assign((const char*)header.value, header.value_len);
            }
        }
    }

    this->WFServerTask::handle(state, error);
}

CommMessageOut* MyHttpServerTask_c::message_out() {
    auto resp = this->get_resp();
    resp->clear_cache_header();
    protocol::HttpMessageHeader header;

    if (!resp->get_http_version()) {
        resp->set_http_version("HTTP/1.1");
    }

    const char* status_code_str = resp->get_status_code();
    if (!status_code_str || !resp->get_reason_phrase()) {
        int status_code;
        if (status_code_str) {
            status_code = atoi(status_code_str);
        } else {
            status_code = HttpStatusOK;
        }

        protocol::HttpUtil::set_response_status(resp, status_code);
    }

    if (!resp->is_chunked() && !resp->has_content_length_header()) {
        std::string&& size_str = std::to_string(resp->get_output_body_size());
        header.name            = "Content-Length";
        header.name_len        = strlen("Content-Length");
        header.value           = size_str.c_str();
        header.value_len       = size_str.size();
        resp->add_header(&header);
    }

    bool is_alive;
    if (resp->has_connection_header()) {
        is_alive = resp->is_keep_alive();
    } else {
        is_alive = req_is_alive_;
    }

    if (!is_alive) {
        this->keep_alive_timeo = 0;
    } else {
        // req---Connection: Keep-Alive
        // req---Keep-Alive: timeout=5,max=100

        if (req_has_keep_alive_header_) {
            int                      flag   = 0;
            std::vector<std::string> params = StringUtil::split(req_keep_alive_, ',');

            for (const auto& kv : params) {
                std::vector<std::string> arr = StringUtil::split(kv, '=');
                if (arr.size() < 2)
                    arr.emplace_back("0");

                std::string key = StringUtil::strip(arr[0]);
                std::string val = StringUtil::strip(arr[1]);
                if (!(flag & 1) && strcasecmp(key.c_str(), "timeout") == 0) {
                    flag |= 1;
                    // keep_alive_timeo = 5000ms when Keep-Alive: timeout=5
                    this->keep_alive_timeo = 1000 * atoi(val.c_str());
                    if (flag == 3)
                        break;
                } else if (!(flag & 2) && strcasecmp(key.c_str(), "max") == 0) {
                    flag |= 2;
                    if (this->get_seq() >= atoi(val.c_str())) {
                        this->keep_alive_timeo = 0;
                        break;
                    }

                    if (flag == 3)
                        break;
                }
            }
        }

        if (((unsigned int)this->keep_alive_timeo) > HTTP_KEEPALIVE_MAX) {
            this->keep_alive_timeo = HTTP_KEEPALIVE_MAX;
        }
        // if (this->keep_alive_timeo < 0 || this->keep_alive_timeo > HTTP_KEEPALIVE_MAX)
    }

    if (!resp->has_connection_header()) {
        header.name     = "Connection";
        header.name_len = 10;
        if (this->keep_alive_timeo == 0) {
            header.value     = "close";
            header.value_len = 5;
        } else {
            header.value     = "Keep-Alive";
            header.value_len = 10;
        }

        resp->add_header(&header);
    }
    resp->clear_cache_header();
    return this->WFServerTask::message_out();
}

MyHttpServerTask_c* MyHttpServerTask_c::task_cast(MyHttpTask_c* in_task) {
    return dynamic_cast<MyHttpServerTask_c*>(in_task);
}

bool MyHttpServerTask_c::get_remoteAddr(sockaddr_in& in_addr) {
    socklen_t len = sizeof(in_addr);
    return (this->get_peer_addr((sockaddr*)(&in_addr), &len) == 0);
}

bool MyHttpServerTask_c::get_xffAddrs(std::vector<std::string>& in_str) {
    std::string addrsStr;
    if (this->get_req()->find_header("X-Forwarded-For", addrsStr)) {
        mimicry::strEliminate(addrsStr, ' ');
        in_str = std::move(mimicry::strSplit(addrsStr, ','));
        return true;
    }
    return false;
}

std::string MyHttpServerTask_c::get_addr() {
    std::vector<std::string> addrs;
    if (get_xffAddrs(addrs) && addrs.empty() == false) {
        return addrs[0];
    }
    sockaddr_in addr_in{};
    if (true == this->get_remoteAddr(addr_in)) {
        return std::string(inet_ntoa(addr_in.sin_addr));
    }
    return std::string();
}

void MyHttpServerTask_c::push_callback(const MyHttpProcess_t& in_callback) {
    callbackArr.push_back(in_callback);
}

void MyHttpServerTask_c::run_callback() {
    for (auto it = callbackArr.begin(); it != callbackArr.end();) {
        (*it)(this);
        callbackArr.erase(it);
        it = callbackArr.begin();
    }
}

void MyHttpServerTask_c::clear_callback() {
    callbackArr.clear();
}

std::string mimicry::get_url_oriain(const char* in_url) {
    for (const char* lindex = in_url; *lindex != '\0'; ++lindex) { // 寻找 //
        if (*lindex == '/' && *(lindex + 1) == '/') {
            lindex             += 2;
            const char* rindex  = lindex;
            for (; *rindex != '\0'; ++rindex) { // 寻找 / 或 结束\0
                if (*rindex == '/')
                    break;
            }
            return std::string{in_url, rindex};
        }
    }
    return std::string{};
}

std::string mimicry::get_url_oriain(const std::string& in_url) {
    return mimicry::get_url_oriain(in_url.c_str());
}

std::string mimicry::get_url_host(const char* in_url) {
    for (; *in_url != '\0'; ++in_url) { // 寻找 //
        if (*in_url == '/' && *(in_url + 1) == '/') {
            // 由于 *in_url != '\0' 即in_url非最后一个字符
            // 因此 in_url + 1不会越界
            in_url             += 2;
            const char* rindex  = in_url;
            for (; *rindex != '\0'; ++rindex) { // 寻找 / 或 结束\0
                if (*rindex == '/')
                    break;
            }
            return std::string{in_url, rindex};
        }
    }
    return std::string{};
}

std::string mimicry::get_url_host(const std::string& in_url) {
    return mimicry::get_url_host(in_url.c_str());
}

std::string mimicry::get_url_uri(const char* in_url) {
    if (*in_url == '/')
        return std::string{in_url};
    else {
        for (; *in_url != '\0'; ++in_url) { // 寻找 //
            if (*in_url == '/' && *(in_url + 1) == '/') {
                in_url             += 2;
                const char* rindex  = in_url;
                for (; *rindex != '\0'; ++rindex) { // 寻找 ? 或 结束 \0
                    if (*rindex == '?')
                        break;
                }
                return std::string{in_url, rindex};
            }
        }
        return std::string{};
    }
}

std::string mimicry::get_url_uri(const std::string& in_url) {
    return mimicry::get_url_uri(in_url.c_str());
}

std::string mimicry::get_url_path(const char* in_url) {
    auto        uri_str = mimicry::get_url_uri(in_url);
    const char *uri = uri_str.c_str(), *p = uri;
    while (*p != '\0' && *p != '?')
        p++;
    string path(uri, p - uri), repath;
    repath.resize(path.size(), '\0');
    for (size_t i = 0, j = 0, len = path.size(); i < len;) {
        repath[j] = path[i];
        if (path[i] == '/') {
            if (i + 1 < len && path[i + 1] == '/') { // //
                i += 1;
                continue;
            } else if (i + 2 < len && path[i + 1] == '.' && path[i + 2] == '/') { // /./
                i += 2;
                continue;
            } else if (i + 3 < len && path[i + 1] == '.' && path[i + 2] == '.' && path[i + 3] == '/') { // /../
                i += 3;
                for (--j; j > 0; --j) // 查找repath中上一个 / 的位置
                    if (repath[j] == '/')
                        break;
                if (j < 0)
                    j = 0;
                continue;
            }
        }
        ++i;
        ++j;
    }
    return mimicry::url_decode(repath);
}

std::string mimicry::get_url_path(const std::string& in_url) {
    return mimicry::get_url_path(in_url.c_str());
}

std::string mimicry::get_url_arg(const char* in_url) {
    const char *uri = in_url, *start = uri; // 获取uri链接中数据
    for (; *start != '\0' && *start != '?'; ++start)
        ;
    if (*start != '\0')
        ++start;
    return std::string{start};
}

std::string mimicry::get_url_arg(const std::string& in_url) {
    return mimicry::get_url_arg(in_url.c_str());
}
