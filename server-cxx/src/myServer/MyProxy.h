#include <string>

#include "MyHttpTask.h"
#include "MyLog.h"

class MyProxy_c {

public:

    MyProxy_c() {}

    static WFHttpTask* http(
        MyHttpTask_c*          in_req_task,
        const std::string&     in_toPath,
        bool                   enable_autoInfo,
        const http_callback_t& req_call,
        const http_callback_t& resp_call
    );
};