#include "MyHttpServer.h"

#include "nlohmann/json.hpp"

#include <ctime>
#include <fstream>
#include <stdlib.h>


#include "MyHtml.h"
#include "MyHttpFile.h"
#include "MyHttpMethod.h"
#include "MyHttpState.h"


#include "workflow/HttpUtil.h"

using nlohmann::json;
using std::map;
using std::string;

std::function<void(MyHttpTask_c*, MyHttpServer_c*)> MyHttpServer_c::serverFun
    = [](MyHttpTask_c* myTask, MyHttpServer_c* server) {
          if (server->onRequestStart && server->onRequestStart(myTask) == false) {
              // [server] 的 [onRequestStart] 事件存在，且 [onRequestStart] 返回了
              // false，阻断继续执行
              return;
          }
          auto        request = myTask->get_req();
          std::string reHost  = "";
          // Http1.1要求，必须存在 Host
          if (false == request->find_header("Host", reHost)) {
              MyHtml_c::defaultHTML_out(myTask, MyHttpState_e::BadRequest);
              return;
          }
	else if (false == server->header_limit.isFormat_host(reHost)
		|| false == server->header_limit.isFormat_referer(request)
		|| false == server->header_limit.isFormat_origin(request)) {
              // 检查部分请求头字段是否符合要求，不符合则直接返回 403
              MyHtml_c::defaultHTML_out(myTask, MyHttpState_e::Forbidden);
              return;
          }
          auto response                 = myTask->get_resp();
          request->enable_cache_header  = true;
          request->enable_cache_path    = true;
          response->enable_cache_header = false;
          const string& path            = request->get_path();
          string        method_str;
          if (request->get_method(method_str) == false) {
              server->log.cwarn(string(request->get_request_uri()) + " get_method error");
          }

          string re_path;
          auto&  fun = server->router.get(path, MyHttpMethod_c::strToInt(method_str), re_path);
          request->set_router_path(re_path);
          response->add_header_pair("Server", "MimicryHttpServer");
          request->clear_cache_header();
          response->clear_cache_header();
          if (fun) { // 判断是否存在路由位置
              fun(myTask);
          } else if (re_path.empty()) // not found
              MyHtml_c::defaultHTML_out(myTask, MyHttpState_e::NotFound);
          else
              MyHtml_c::defaultHTML_out(myTask, MyHttpState_e::MethodNotAllowed);
      };
//-------  MyHttp  -------

// 自带的一些html
#define HTML_bottom "<br><span>MimicryHttpServer</span>"
#define HTML_first \
    "<html><body style='display:flex;flex-direction: column;justify-content: center;align-items: center;'>"
#define HTML_last       "</body></html>"
#define DefaultHTML(x)  HTML_first x HTML_bottom HTML_last
#define DefaultHTML_200 DefaultHTML("<h1>Hello</h1>")
#define DefaultHTML_400 DefaultHTML("<h1>400 Bad Request</h1>")
#define DefaultHTML_401 DefaultHTML("<h1>401 Permission</h1>")
#define DefaultHTML_403 DefaultHTML("<h1>403 Forbidden</h1>")
#define DefaultHTML_404 DefaultHTML("<h1>404 NotFound</h1>")
#define DefaultHTML_405 DefaultHTML("<h1>405 MethodNotAllowed</h1>")
#define DefaultHTML_500 DefaultHTML("<h1>500 Internal server error</h1>")
#define DefaultHTML_502 DefaultHTML("<h1>502 BadGateway</h1>")
#define DefaultHTML_503 DefaultHTML("<h1>503 Service Unavailable</h1>")

// MyHttpServerLimit_c -------------
bool MyHttpHeaderLimit_c::isFormat_referer(const std::string& in_str) {
    if (nullptr == limit_referer) {
        return true;
    }
    return std::regex_match(in_str, *limit_referer.get());
}

bool MyHttpHeaderLimit_c::isFormat_referer(MyHttpRequest_c* in_req) {
    if (nullptr == limit_referer) {
        return true;
    }
    std::string reStr;
    in_req->find_header("Referer", reStr);
    return this->isFormat_referer(reStr);
}

bool MyHttpHeaderLimit_c::isFormat_origin(const std::string& in_str) {
    if (nullptr == limit_origin) {
        return true;
    }
    return std::regex_match(in_str, *limit_origin.get());
}

bool MyHttpHeaderLimit_c::isFormat_origin(MyHttpRequest_c* in_req) {
    if (nullptr == limit_origin) {
        return true;
    }
    std::string reStr;
    in_req->find_header("Origin", reStr);
    return this->isFormat_origin(reStr);
}

bool MyHttpHeaderLimit_c::isFormat_host(const std::string& in_str) {
    if (nullptr == limit_host) {
        return true;
    }
    return std::regex_match(in_str, *limit_host.get());
}

bool MyHttpHeaderLimit_c::isFormat_host(MyHttpRequest_c* in_req) {
    if (nullptr == limit_host) {
        return true;
    }
    std::string reStr;
    in_req->find_header("Host", reStr);
    return this->isFormat_host(reStr);
}

// MyHttpServer ---------------------
MyHttpServer_c::MyHttpServer_c(const WFServerParams* in_params) noexcept :
    WFServer<MyHttpRequest_c, MyHttpResponse_c>(in_params, nullptr),
    log("[MyHttpServer] ") {
    WFServer<MyHttpRequest_c, MyHttpResponse_c>::process
        = std::bind(serverFun, std::placeholders::_1, this);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::OK, DefaultHTML_200);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::Unauthorized, DefaultHTML_401);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::Forbidden, DefaultHTML_403);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::NotFound, DefaultHTML_404);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::MethodNotAllowed, DefaultHTML_405);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::InternalServerError, DefaultHTML_500);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::BadGateway, DefaultHTML_502);
    MyHtml_c::defaultHTML_set_html(MyHttpState_e::ServiceUnavailable, DefaultHTML_503);
    // 默认主页欢迎页面
    this->router.add("/*", MyHttpMethod_e::Get, MyHtml_c::defaultHTML_fun(MyHttpState_e::OK));
}

CommSession* MyHttpServer_c::new_session(long long seq, CommConnection* conn) {
    MyHttpTask_c* task
        = new MyHttpServerTask_c(this, WFServer<MyHttpRequest_c, MyHttpResponse_c>::process);
    task->set_keep_alive(this->params.keep_alive_timeout);
    task->set_receive_timeout(this->params.receive_timeout);
    task->get_req()->set_size_limit(this->params.request_size_limit);
    return task;
}

void MyHttpServer_c::set_onRequestStart(std::function<bool(MyHttpTask_c*)>&& in_onRequestStart) {
    this->onRequestStart = std::forward<std::function<bool(MyHttpTask_c*)>>(in_onRequestStart);
}

bool MyHttpServer_c::set_config(
    const string&                          in_confPath,
    std::function<bool(nlohmann::json&)>&& in_succCallback
) noexcept {
    if (in_confPath.empty()) {
        log.cinfo("Reading config | no config file");
        return true;
    }
    log.cline();
    log.cinfo("Reading config | file: " + in_confPath);
    std::ifstream in_confFile;
    in_confFile.open(in_confPath);
    if (in_confFile.is_open()) {
        string conf_dir; // 配置文件所在目录
        for (auto it = in_confPath.end() - 1; it != in_confPath.begin(); --it) {
            if (*it == '/' || *it == '\\') {
                conf_dir = string(in_confPath.begin(), it + 1);
                break;
            }
        }
        json conf_json;
        try {
            in_confFile >> conf_json;
        } catch (...) {
            in_confFile.close();
            log.cerror("config file read to json faild, Please check it! path: " + in_confPath);
            return false;
        }
        in_confFile.close();
        // 读取json成功
        if (conf_json["server"].is_null() == false) {
            auto& conf_json_server = conf_json["server"];
            if (conf_json_server["port"].is_number_integer()) {
                this->config.server_port = conf_json_server["port"].get<int>();
                log.cinfo("Set | server - port: " + std::to_string(this->config.server_port));
            }
            if (conf_json_server["home"].is_string()) {
                this->config.server_home = conf_json_server["home"].get<std::string>();
                log.cinfo("Set | server - home: " + this->config.server_home);
            }
            if (conf_json_server["mysql"].is_null() == false) {
                auto& conf_json_mysql = conf_json_server["mysql"];
                if (conf_json_mysql["url"].is_string()) {
                    this->config.server_mysql_url = conf_json_mysql["url"].get<std::string>();
                    log.cinfo("Set | server - mysql - url: " + this->config.server_mysql_url);
                }
                if (conf_json_mysql["dbname"].is_string()) {
                    this->config.server_mysql_dbname = conf_json_mysql["dbname"].get<std::string>();
                    log.cinfo("Set | server - mysql - dbname: " + this->config.server_mysql_dbname);
                }
            }
        }
        // 设置html状态码（200/404...）的默认html页面
        if (conf_json["src"]["html"]["default"].is_null() == false) {
            json& defHtml = conf_json["src"]["html"]["default"];
            for (auto& item : defHtml.items()) {
                string key_str
                    = (item.value().get<std::map<std::string, std::string>>()).begin()->first;
                int    key_i     = atoi(key_str.c_str());
                string file_path = string(*(item.value().begin()));
                if (false == file_path.empty()) {
                    if (file_path[0] == '.') {
                        file_path = conf_dir + file_path;
                    }
                    log.cinfo(
                        "Set | src - html - default: code:" + key_str + "; file:" + file_path
                    );
                    MyHtml_c::defaultHTML_set_file(key_i, file_path);
                }
            }
        }
        bool rebool = true;
        if (in_succCallback) {
            rebool = in_succCallback(conf_json);
        }
        log.cinfo("Config complete");
        log.cline();
        return rebool;
    } else {
        log.cerror("config file open faild or not found, path: " + in_confPath);
    }
    return false;
}

void MyHttpServer_c::set_header_limit(const MyHttpHeaderLimit_c& in_limit) noexcept {
    this->header_limit = in_limit;
}

int MyHttpServer_c::start(int in_port) {
    int reNum = WFServer<MyHttpRequest_c, MyHttpResponse_c>::start(in_port);
    log.cinfo("run home: " + config.server_home);
    log.cinfo("listen on port: " + std::to_string(in_port));
    if (reNum == 0)
        log.cinfo("start success");
    else
        log.cerror("start error");
    return reNum;
}

void MyHttpServer_c::stop() {
    WFServer<MyHttpRequest_c, MyHttpResponse_c>::stop();
    log.cinfo("stop");
}
