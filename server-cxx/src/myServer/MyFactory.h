#ifndef MIMICRY_FACTORY_H
#define MIMICRY_FACTORY_H

#include <chrono>
#include "workflow/WFHttpServer.h"

namespace mimicry {

    /// 定时器
    class MyTimer_c {
    protected:
        /// 是否为循环计时器
        bool isLoop = false;
        /// 是否已经销毁
        bool isDispose = false;
        WFTimerTask* timer = nullptr;
        std::function<void()> fun = nullptr;
        std::chrono::seconds seconds = std::chrono::seconds::zero();

        void _createTimer();

        // 计时位置到来时执行
        void _onClockDo(WFTimerTask* in_timer);
    public:
        MyTimer_c(const std::chrono::seconds& in_seconds, const std::function<void()>& in_fun, bool in_isLoop = false);

        static std::shared_ptr<mimicry::MyTimer_c> cLoop(const std::chrono::seconds& in_seconds, const std::function<void()>& in_fun);

        /// 取消定时器的执行
        void cancle();

        ~MyTimer_c() {
            cancle();
        }
    };


}

#endif