#include "MyFactory.h"
#include <memory>

mimicry::MyTimer_c::MyTimer_c(const std::chrono::seconds& in_seconds, const std::function<void()>& in_fun, bool in_isLoop) {
    seconds = in_seconds;
    isLoop = in_isLoop;
    fun = in_fun;
    _createTimer();
}

/// 创建一个循环
/// * 注意
std::shared_ptr<mimicry::MyTimer_c> mimicry::MyTimer_c::cLoop(const std::chrono::seconds& in_seconds, const std::function<void()>& in_fun) {
    return std::make_shared<mimicry::MyTimer_c>(in_seconds, in_fun, true);
}

/// 取消定时器的执行
void mimicry::MyTimer_c::cancle() {
    isDispose = true;
}

void mimicry::MyTimer_c::_createTimer() {
    timer = WFTaskFactory::create_timer_task(
        seconds.count(), 0, 
        [this](WFTimerTask* in_timer) {
            this->_onClockDo(in_timer);
        });
    timer->set_callback([this](WFTimerTask* in_timer) {
        this->_onClockDo(in_timer);
    });
    timer->start();
}

// 计时位置到来时执行
void mimicry::MyTimer_c::_onClockDo(WFTimerTask* in_timer) {
    if(false == isDispose) {
        fun();
        // 由于fun内可能会修改 [isDispose]，因此这里再判断一次
        if(false == isDispose && isLoop) {
            _createTimer();
        }
    }
}