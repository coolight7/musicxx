#include "MyProxy.h"
#include "MyHtml.h"

using std::string;

WFHttpTask* MyProxy_c::http(
    MyHttpTask_c*          in_task,
    const std::string&     in_toPath,
    bool                   enable_autoInfo,
    const http_callback_t& req_call,
    const http_callback_t& resp_call
) {
    auto*       req        = in_task->get_req();
    SeriesWork* series     = series_of(in_task);
    WFHttpTask* proxy_task = WFTaskFactory::create_http_task(
        in_toPath,
        0,
        0,
        [in_task,
         resp_call](WFHttpTask* in_proxy_task) { // in_proxy_task = proxy_task
            if (in_proxy_task->get_state() == WFT_STATE_SUCCESS) {
                const void* body;
                size_t      len;
                auto        resp       = in_task->get_resp();
                auto        proxy_resp = in_proxy_task->get_resp();
                proxy_resp->get_parsed_body(&body, &len);
                proxy_resp->append_output_body_nocopy(body, len);
                if (nullptr != resp_call) {
                    resp_call(in_proxy_task);
                }
                *resp = std::move(*proxy_resp); // 将代理的响应移动给原task的请求
            } else {                            // 404
                MyHtml_c::defaultHTML_out(in_task, MyHttpState_e::NotFound);
            }
        }
    );
    const void* body = nullptr;
    size_t      len  = 0;
    req->set_request_uri(proxy_task->get_req()->get_request_uri());
    req->protocol::HttpRequest::get_parsed_body(&body, &len);
    req->append_output_body_nocopy(body, len);
    *proxy_task->get_req() = std::move(*req); // 将原task的请求移动给代理task的请求
    if (enable_autoInfo) {
        auto   proxy_req  = proxy_task->get_req();
        auto&& url_oriain = mimicry::get_url_oriain(in_toPath);
        proxy_req->set_header_pair("Referer", url_oriain + "/");
        proxy_req->set_header_pair("Origin", url_oriain);
        proxy_req->set_header_pair("Host", mimicry::get_url_host(in_toPath));
        proxy_task->get_resp()->set_size_limit(200 * 1024 * 1024);
    }
    if (nullptr != req_call) {
        req_call(proxy_task);
    }
    *series << proxy_task;
    return proxy_task;
}
