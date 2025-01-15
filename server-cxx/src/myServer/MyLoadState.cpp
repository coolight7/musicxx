#include "MyLoadState.h"
#include "MyLog.h"
#include "workflow/WFHttpServer.h"

#ifdef _WIN32
#include <IPHlpApi.h>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "IPHlpApi.lib")
#else
#include <stdio.h>
#include <sys/sysinfo.h>
#endif

typedef struct _CPU_PACKED {
    char         name[16];
    unsigned int user;   // 用户模式
    unsigned int nice;   // 低优先级的用户模式
    unsigned int system; // 内核模式
    unsigned int idle;   // 空闲处理器时间
} CPU_OCCUPY;

/*收发数据包结构体*/
typedef struct _RTX_BYTES {
    long int       tx_bytes;
    long int       rx_bytes;
    struct timeval rtx_time;
} RTX_BYTES;

#ifndef _WIN32
// linux
void getCurrentDownloadRates(long int* save_rate) {}
#endif

void MyLoadController_c::initAutoFlushState() {}

void MyLoadController_c::defRefuseConnect(MyHttpTask_c* in_http_task) {
    this->switchDo<void>(
        [in_http_task]() {
            in_http_task->get_resp()->out_err(mimicry::ResponeState_e::Forbidden
            );
            // 关闭连接
            in_http_task->get_resp()->add_header_pair("Connection", "close");
        },
        [in_http_task]() {
            // 直接打断
            in_http_task->noreply();
        }
    );
}