#ifndef MIMICRY_LOADSTATE_H
#define MIMICRY_LOADSTATE_H

#include "MyHttpTask.h"
#include "MyLimit.h"

enum MyLoadState_e {
    Low,
    Middle,
    Height,
    Heavy,
};

/// <summary>
/// 负载管理
/// </summary>
class MyLoadController_c {
protected:

    std::shared_ptr<mimicry::MyTimer_c> timer = nullptr;

public:

    bool enableAutoFlushState = false;
    /// 当前负载状态
    MyLoadState_e state = MyLoadState_e::Low;

    static int loadStateToInt(MyLoadState_e state) {
        return state;
    }

    /* * 当前负载状态是否 <= [in_state]
     *
     */
    bool isAllow(const MyLoadState_e in_state) {
        return (state <= in_state);
    }

    template<typename T>
    T& switchValue(
        T&                  yValue,
        T&                  nValue,
        const MyLoadState_e in_state = MyLoadState_e::Middle
    ) {
        if (isAllow(in_state)) {
            return yValue;
        } else {
            return nValue;
        }
    }

    template<typename T = void>
    T switchDo(
        const std::function<T()>& yFun,
        const std::function<T()>& nFun,
        const MyLoadState_e       in_state = MyLoadState_e::Middle
    ) {
        if (isAllow(in_state)) {
            return yFun();
        } else {
            return nFun();
        }
    }

    void initAutoFlushState();

    void setState(MyLoadState_e in_state) {
        state = in_state;
    }

    void defRefuseConnect(MyHttpTask_c* in_http_task);
};

#endif