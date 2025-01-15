#ifndef MIMICRY_LIMIT_H
#define MIMICRY_LIMIT_H

#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "MyFactory.h"
#include "MyLog.h"
#include "TgNullable.h"

enum MyHitLimitType_e {
    Allow,  // 允许
    Refuse, // 拒绝
};

// 限制访问次数
template<typename T = long long>
class MyHitLimit_c {
protected:

    /// 限制方式
    MyHitLimitType_e limit_type = MyHitLimitType_e::Refuse;
    /// * 限制周期内最大访问次数
    /// * 仅当 [limit_type] = [MyHitLimitType_e::Refuse]，且 [limitSize] > 0
    /// 时有效
    /// * 当计数大于该限制时，将自动将目标添加到限制中
    int limitSize = -1;
    /// * 触发限制时默认的限制时长
    time_t limitAge = 0;

    /// 记录限制的值和限制时长
    TgMap_c<T, time_t> hitLimit;
    /// 记录一段时间内，某值访问的次数
    TgMap_c<T, int> hitNum;

    /// hit的管理周期；单位：s秒；
    time_t                              hitInterval = -1;
    std::shared_ptr<mimicry::MyTimer_c> timer_task  = nullptr;
    // 返回是否清理 [hitNum]
    std::function<bool(MyHitLimit_c<T>* in_limit)> onHitIntervalControl
        = nullptr;

    void onHitInterval() {
        if (true == this->onHitIntervalControl(this)) {
            this->hitNum.clear();
        }
    }

public:

    MyHitLimit_c(
        MyHitLimitType_e in_type,
        int              in_limitSize = -1,
        time_t           in_limitAge  = 0
    ) :
        limit_type(in_type),
        limitSize(in_limitSize),
        limitAge(in_limitAge) {}

    void set_limitSize(int in_limitSize, bool clear_limitMap) {
        this->limitSize = in_limitSize;
        if (clear_limitMap) {
            this->hitLimit.clear();
        }
    }

    /// 设置限制方式
    void set_limitType(MyHitLimitType_e in_type, bool clear_limitMap) {
        this->limit_type = in_type;
        if (clear_limitMap) {
            this->hitLimit.clear();
        }
    }

    /// 重设限制值
    void reSet_hitLimit(std::map<T, time_t>&& in_map) {
        this->hitLimit = std::forward<std::map<T, time_t>>(in_map);
    }

    /// <summary>清空限制集</summary>
    void clear_hitLimit() {
        this->hitLimit.clear();
    }

    /// 添加限制
    /// *	[in_time]: 指定限制的截止时间点 (单位：s 秒)
    /// *	当 [in_time] < 0，则不限制时长
    /// *	当 [in_time] = 0 时，使用默认的 [limitAge] 和 当前时间
    /// 计算截止时间点 *	当 [in_time] >
    /// 0，则只在指定时间点[in_time]内有效
    void add_hitLimit(T in_ip, time_t in_time = 0) {
        if (0 == in_time) {
            this->hitLimit[in_ip].setValue(std::time(nullptr) + limitAge);
        } else {
            this->hitLimit[in_ip].setValue(in_time);
        }
    }

    /// 添加限制
    /// *	[in_time]: 指定限制的时长 (单位：s 秒)
    /// *	当 [in_age] < 0 时，视为不限制时长
    /// *	当 [in_age] = 0 时，使用默认的 [limitAge]
    /// *	当 [in_age] > 0 时，计算截止时间点
    void add_hitLimit_byAge(T in_ip, time_t in_age = 0) {
        auto in_time = in_age;
        // 当指定 [in_age] > 0 时，计算截止时间点
        if (in_time > 0) {
            in_time = std::time(nullptr) + in_time;
        } else if (0 == in_time) {
            in_time = std::time(nullptr) + limitAge;
        }
        this->hitLimit[in_ip].setValue(in_time);
    }

    /// 移除一个限制
    void remove_hitLimit(T in_ip) {
        this->hitLimit.remove(in_ip);
    }

    /// 添加访问记录
    int add_hit(T in_ip) {
        auto& it  = hitNum[in_ip];
        auto  num = it.getValue(0) + 1;
        it.setValue(num);
        return num;
    }

    /// 添加访问记录
    /// * 如果指定 [autoLimit] = true，则检查是否在限制值[limitSize]内
    bool add_hit(T in_ip, bool autoLimit) {
        auto num = add_hit(in_ip);
        if (true == autoLimit) {
            if (false == isAllow(in_ip)) {
                return false;
            }
            if (MyHitLimitType_e::Refuse == limit_type && limitSize > 0
                && num >= limitSize) {
                return false;
            }
        }
        return true;
    }

    TgMap_c<T, int>& get_hit() {
        return this->hitNum;
    }

    int get_hit(T in_ip) {
        return hitNum[in_ip].getValue(0);
    }

    /// 设置IpHit的周期管理事件执行函数
    /// *	如果 [in_ipHitInterval] 小于等于 0，则 取消管理函数
    void set_onHitIntervalControl(
        time_t in_ipHitInterval,
        const std::function<bool(MyHitLimit_c<T>* in_limit)>&
            in_onIpHitIntervalControl
    ) {
        // 取消之前的定时器
        if (nullptr != this->timer_task) {
            this->timer_task->cancle();
            this->timer_task = nullptr;
        }
        this->hitInterval          = in_ipHitInterval;
        this->onHitIntervalControl = in_onIpHitIntervalControl;
        if (hitInterval > 0) {
            // 创建新定时器
            this->timer_task = mimicry::MyTimer_c::cLoop(
                std::chrono::seconds(this->hitInterval),
                [this]() {
                    this->onHitInterval();
                }
            );
        }
    }

    /// <summary>检查是否在 [in_time] 时间点前放行 [in_ip]</summary>
    /// <param name="in_ip">待检查的Ip</param>
    /// <param name="in_time">待检查时间点(单位：s秒)；如果 小于等于 0,
    /// 则取调用时的时间点进行判断</param> <returns>是否放行访问</returns>
    bool isAllow(T in_ip, time_t in_time = -1) {
        auto& it = this->hitLimit[in_ip];
        switch (limit_type) {
        case MyHitLimitType_e::Allow:
            {
                // 未找到，且为允许类型，直接返回false
                if (it.isNull()) {
                    return false;
                }
                // 有找到
                auto limitTime = it.getValue();
                if (limitTime <= 0) {
                    // 不限制时间
                    return true;
                }
                if (in_time <= 0) {
                    // 取当前时间点进行判断
                    time_t now_time = time(nullptr);
                    // 当前时间小于等于允许ip的时长时放行
                    return (now_time <= limitTime);
                } else {
                    return (in_time <= limitTime);
                }
            }
            break;
        case MyHitLimitType_e::Refuse:
            {
                // 未找到，且为拒绝类型，直接返回true
                if (it.isNull()) {
                    return true;
                }
                auto limitTime = it.getValue();
                // 有找到
                if (limitTime <= 0) {
                    // 不限制时间
                    return false;
                }
                if (in_time <= 0) {
                    time_t now_time = time(nullptr);
                    // 当前时间大于等于拒绝ip的时长时放行
                    return (now_time > limitTime);
                } else {
                    return (in_time > limitTime);
                }
            }
            break;
        default:
            return false;
            break;
        }
    }

    virtual ~MyHitLimit_c() {
        if (nullptr != timer_task) {
            timer_task->cancle();
        }
        timer_task = nullptr;
    }

    /// 预设 周期检查拒绝
    static bool defRefuseOnHitIntervalControl(MyHitLimit_c<T>* in_limit) {
        auto& hitNum = in_limit->get_hit();
        for (auto it = hitNum.begin(); it != hitNum.end(); ++it) {
            auto value = it->second.getValue(0);
            if (value >= in_limit->limitSize) {
                // 如果访问超速，禁止访问
                MyLog_c::getInstance()->cfinfo(fmt::format(
                    "Refuse IP {} seconds: {}",
                    std::to_string(in_limit->limitAge),
                    std::to_string(it->first) + " | " + std::to_string(value)
                ));
                in_limit->add_hitLimit_byAge(it->first);
            }
        }
        return true;
    }
};

/// 包含多个限制
template<typename T = long long>
class MyHitListLimit_c {
protected:

    std::vector<MyHitLimit_c<T>> hitlist;

public:

    MyHitLimit_c<T>& newItem(
        MyHitLimitType_e in_type,
        int              in_limitSize = -1,
        time_t           in_limitAge  = 0
    ) {
        hitlist.push_back(MyHitLimit_c<T>(in_type, in_limitSize, in_limitAge));
        return hitlist.back();
    }
};

#endif // !MIMICRY_MYLIMIT_H
