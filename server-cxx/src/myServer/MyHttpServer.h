#ifndef MYHTTPSERVER_H
#define MYHTTPSERVER_H

#include <functional>
#include <memory>
#include <regex>
#include <string>


#include "nlohmann/json.hpp"
#include "workflow/WFHttpServer.h"


#include "MyHttpTask.h"
#include "MyLog.h"
#include "MyRouter.h"


// 配置信息
class MyHttpServerConfig_c {
public:

    int         server_port         = 7777;
    std::string server_home         = MyHttpServer_HOME;
    std::string server_mysql_url    = "mysql://root:@127.0.0.1:3306/";
    std::string server_mysql_dbname = "mymusic";

    MyHttpServerConfig_c() {}
};

// 访问限制
class MyHttpHeaderLimit_c {
protected:

    std::shared_ptr<std::regex> limit_referer = nullptr;
    std::shared_ptr<std::regex> limit_origin  = nullptr;
    std::shared_ptr<std::regex> limit_host    = nullptr;

public:

    MyHttpHeaderLimit_c() {}

    // 设置对应请求头限制的正则表达式
    std::shared_ptr<std::regex> set_limit_referer(std::shared_ptr<std::regex>& in_limit_referer) {
        auto ptr      = std::move(limit_referer);
        limit_referer = in_limit_referer;
        return ptr;
    }

    std::shared_ptr<std::regex> set_limit_origin(std::shared_ptr<std::regex>& in_limit_origin) {
        auto ptr     = std::move(limit_origin);
        limit_origin = in_limit_origin;
        return ptr;
    }

    std::shared_ptr<std::regex> set_limit_host(std::shared_ptr<std::regex>& in_limit_host) {
        auto ptr   = std::move(limit_host);
        limit_host = in_limit_host;
        return ptr;
    }

    /// 检查 [in_str] 是否符合 [limit_referer] 的要求
    /// *	当 [limit_referer] 为 nullptr 时，返回 true
    /// *	否则进行正则表达式匹配
    bool isFormat_referer(const std::string& in_str);
    bool isFormat_referer(MyHttpRequest_c* in_req);
    bool isFormat_origin(const std::string& in_str);
    bool isFormat_origin(MyHttpRequest_c* in_req);
    bool isFormat_host(const std::string& in_str);
    bool isFormat_host(MyHttpRequest_c* in_req);

    MyHttpHeaderLimit_c& operator=(const MyHttpHeaderLimit_c& in_limit) {
        this->limit_referer = in_limit.limit_referer;
        this->limit_origin  = in_limit.limit_origin;
        this->limit_host    = in_limit.limit_host;
        return *this;
    }

    virtual ~MyHttpHeaderLimit_c() {}
};

class MyHttpServer_c : public WFServer<MyHttpRequest_c, MyHttpResponse_c> {
protected:

    // 请求处理
    static std::function<void(MyHttpTask_c*, MyHttpServer_c*)> serverFun;
    std::function<bool(MyHttpTask_c*)>                         onRequestStart = nullptr;
    // 自定义日志
    MyLog_c log;

    CommSession* new_session(long long seq, CommConnection* conn) override;

public:

    MyHttpServerConfig_c config       = MyHttpServerConfig_c(); // 配置信息
    MyHttpHeaderLimit_c  header_limit = MyHttpHeaderLimit_c();  // 访问限制信息
    MyRouter_c           router       = MyRouter_c();           // 路由

    MyHttpServer_c(const WFServerParams* in_params = &SERVER_PARAMS_DEFAULT) noexcept;
    MyHttpServer_c(const MyHttpServer_c&) = delete;

    /*启动服务
     * \param in_port Http服务监听端口号
     */
    int start(int in_port);
    // 停止服务
    void stop();
    /// <summary>
    /// 设置Http请求的启动事件，当有Http请求到来时，先执行该函数
    /// </summary>
    /// <param
    /// name="in_onRequestStart">事件执行函数，如果该函数返回false，将阻断默认的路由/对应函数等后续执行</param>
    void set_onRequestStart(std::function<bool(MyHttpTask_c*)>&& in_onRequestStart);
    /* 读取配置文件
     * \param in_confPath 配置文件所在路径
     */
    bool set_config(
        const std::string&                     in_confPath,
        std::function<bool(nlohmann::json&)>&& in_succCallback = nullptr
    ) noexcept;
    void set_header_limit(const MyHttpHeaderLimit_c& in_limit) noexcept;

    ~MyHttpServer_c() override {
        this->stop();
    }
};

#endif // !MYHTTPSERVER_H