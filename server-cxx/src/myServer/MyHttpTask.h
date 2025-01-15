#ifndef MYHTTPTASK_H
#define MYHTTPTASK_H

#include <functional>
#include <vector>

#include "workflow/HttpUtil.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/Workflow.h"


#include "nlohmann/json.hpp"

#include "Cookie.h"
#include "MyBaseEntity.h"
#include "MyHttpMethod.h"
#include "MyLog.h"

namespace mimicry {
    class HttpBodyType_e {
    public:

        static const char *  Form_data, *X_www_form_urlencoded, *Json, *Xml, *Text, *Binary;
        static constexpr int Form_data_int = 1, X_www_form_urlencoded_int = 2, Json_int = 4,
                             Xml_int = 8, Text_int = 16, Binary_int = 32;
    };

    struct HTTP_formData_s {
        bool isFile = false; // 是否为文件，否的话fileName和fileType是无效的
        // 注意：fileName可能会包含./
        // ../等路径信息，此时可能会有一些危险的操作，建议截取最后一个/之后的文件名或自行命名
        std::string fileName;
        std::string fileName_CharEncoding;
        // 有时可能文件名会乱码，因此请求中会有指定编码的文件名，对应字段 fileName*
        std::string fileName_;
        std::string fileType;
        std::string value;
    };

    std::multimap<std::string, mimicry::HTTP_formData_s>
        read_form_data(const std::string& in_bound, const std::string& in_data);

    std::string get_url_oriain(const char* in_url);
    std::string get_url_oriain(const std::string& in_url);
    std::string get_url_host(const char* in_url);
    std::string get_url_host(const std::string& in_url);
    std::string get_url_uri(const char* in_url);
    std::string get_url_uri(const std::string& in_url);
    std::string get_url_path(const char* in_url);
    std::string get_url_path(const std::string& in_url);
    std::string get_url_arg(const char* in_url);
    std::string get_url_arg(const std::string& in_url);

}; // namespace mimicry

// Req
class MyHttpRequest_c : public protocol::HttpRequest {
protected:

    std::string                 path;
    std::string                 router_path;
    protocol::HttpHeaderCursor* header = nullptr;

public:

    // 是否启用缓存请求头 / 请求路径
    bool enable_cache_header = false, enable_cache_path = true;
    /*获取请求路径
     * 去除../并解析到对应的位置
     * 去除连续 / 和 ./
     * urldecode
     */
    const std::string&          get_path();
    protocol::HttpHeaderCursor& get_header(bool doFlush = false);
    void                        clear_cache_header();
    bool                        find_header(const std::string& in_key, std::string& re_value);
    // 获取请求体
    std::string get_parsed_body();
    // 将请求体按form-data解析并返回解析结果
    std::multimap<std::string, mimicry::HTTP_formData_s> get_form_data();
    // 获取该请求对应的路由路径
    const std::string& get_router_path();
    // 设置该请求对应的路由路径
    void set_router_path(const std::string& in_router_path);
    // 获取cookie
    std::map<std::string, std::string> get_cookie();
    // 设置cookie
    void set_cookie(const mimicry::Cookie_c& in_cookie);
    // 获取请求携带的参数组合字符串（包含uri参数、x-www-form-urlencoded参数）
    std::string get_argStr();
    // 获取
    std::multimap<std::string, std::string> get_arg();

    bool has_keep_alive_header();
    bool has_content_length_header();
    bool has_connection_header();

    ~MyHttpRequest_c() {
        this->clear_cache_header();
    }
};

// Resp
class MyHttpResponse_c : public protocol::HttpResponse {
protected:

    protocol::HttpHeaderCursor* header = nullptr;

public:

    bool enable_cache_header = false;

    MyHttpResponse_c() {}

    MyHttpResponse_c(protocol::HttpResponse&& in_resp) {
        *this = std::move(in_resp);
    }

    MyHttpResponse_c& operator=(protocol::HttpResponse&& in_resp) {
        this->clear_cache_header();
        this->enable_cache_header        = false;
        *((protocol::HttpResponse*)this) = std::move(in_resp);
        return *this;
    }

    protocol::HttpHeaderCursor& get_header(bool doFlush = false);
    void                        clear_cache_header();
    bool                        find_header(const std::string& in_key, std::string& re_value);
    void                        set_cookie(const mimicry::Cookie_c& in_cookie);

    // 向请求返回一个由entity转换的json
    template<typename T>
    void out_entity(const T& in_entity) noexcept {
        try {
            nlohmann::json myjson = in_entity;
            // to_json(myjson, in_entity);
            this->append_output_body(myjson.dump());
        } catch (...) {
            try {
                auto entity
                    = MyRespBaseEntity_c<int>::error(mimicry::ResponeState_e::Exception, "");
                nlohmann::json rejson = *entity;
                // to_json(rejson, *entity);
                this->append_output_body(rejson.dump());
                delete entity;
            } catch (...) {
                this->append_output_body("{\"code\":5000,\"tip\":\"\"}");
            }
        }
    }

    template<typename T>
    void out_entity(int in_code, T* in_data, const std::string& in_tip) noexcept {
        auto base = MyRespBaseEntity_c<T>{in_data, in_code, in_tip};
        this->out_entity<MyRespBaseEntity_c<T>>(base);
    }

    // 向请求返回成功，并将data转json后返回
    template<typename T = int>
    void out_success(T* in_data = nullptr) noexcept {
        auto base = MyRespBaseEntity_c<T>::success(in_data);
        this->out_entity(*base);
        delete base;
    }

    // 向请求返回失败，并指定错误码和提示
    void out_err(int in_code, const std::string& in_tip = "") noexcept;

    bool has_keep_alive_header();
    bool has_content_length_header();
    bool has_connection_header();

    ~MyHttpResponse_c() {
        this->clear_cache_header();
    }
};

// MyHttpTask_c
using MyHttpTask_c = WFNetworkTask<MyHttpRequest_c, MyHttpResponse_c>;

using MyHttpProcess_t = std::function<void(MyHttpTask_c*)>;
extern MyHttpProcess_t Nullptr_MyHttpProcess;

// ServerTask
class MyHttpServerTask_c : public WFServerTask<MyHttpRequest_c, MyHttpResponse_c> {
protected:

    std::vector<MyHttpProcess_t> callbackArr;

    bool        req_is_alive_;
    bool        req_has_keep_alive_header_;
    std::string req_keep_alive_;

    void handle(int state, int error) override;

    CommMessageOut* message_out() override;

public:

    MyHttpServerTask_c(const MyHttpServerTask_c&) = delete;
    MyHttpServerTask_c(CommService* service, MyHttpProcess_t& process) noexcept;
    /* 将父类指针 MyHttpTask_c* 转为子类 MyHttpServerTask_c*
     */
    static MyHttpServerTask_c* task_cast(MyHttpTask_c* in_task);

    /* 获取上一级请求的ip信息
     * 即若请求从客户端出发，经过了代理才被我们接收，则这个函数将返回代理的ip信息
     */
    bool get_remoteAddr(sockaddr_in& in_addr);
    /* 尝试从http请求中获取客户端的真实ip信息
     * 由于请求经过Nginx等代理后可能会在http请求头中留下原本客户端的真实ip
     * 这个函数将尝试获取在Http请求头中的可能存在的真实ip信息
     * X-Forwarded-For: IP0, IP1, IP2, ... (IPN = Remote_Addr)
     */
    bool get_xffAddrs(std::vector<std::string>& in_str);
    /*尝试获取真实客户端ip
     * 即优先获取xffAddrs
     */
    std::string get_addr();

    void push_callback(const MyHttpProcess_t& in_callback);
    void run_callback();
    void clear_callback();
};

#endif // !MYHTTPTASK_H