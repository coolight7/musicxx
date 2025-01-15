#ifndef MYSQLTASK_H
#define MYSQLTASK_H

#include <cstring>
#include <initializer_list>

#include "workflow/MySQLResult.h"
#include "workflow/WFTaskFactory.h"

#include "MyHttpTask.h"
#include "SqlString.h"

class MySqlTask_c;

using MySqlCallback_t = std::function<void(MySqlTask_c*)>;

class MySqlItemType_e {
public:

    static constexpr int Int       = 1;
    static constexpr int Double    = 2;
    static constexpr int Ulonglong = 4;
    static constexpr int Str       = 8;
    static constexpr int Datetime  = 16;
    static constexpr int Time      = 32;
    static constexpr int Null      = 64;
};

template<typename T>
class MySqlItem_c {
public:

    T&  data;
    int type;

    MySqlItem_c(T& in_data, int in_type, bool allowNull = false) :
        data(in_data),
        type(in_type) {
        if (allowNull) {
            type |= MySqlItemType_e::Null;
        }
    }
};

template<typename T>
static MySqlItem_c<T> mysqlItemAutoBuilder(T& in_data, bool allowNull = false) {
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned int>) {
        return MySqlItem_c(in_data, MySqlItemType_e::Int, allowNull);
    } else if constexpr (std::is_same_v<T, double>) {
        return MySqlItem_c(in_data, MySqlItemType_e::Double, allowNull);
    } else if constexpr (std::is_same_v<T, unsigned long long> || std::is_same_v<T, size_t> || std::is_same_v<T, time_t>) {
        return MySqlItem_c(in_data, MySqlItemType_e::Ulonglong, allowNull);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return MySqlItem_c(in_data, MySqlItemType_e::Str, allowNull);
    } else {
        assert(false);
    }
    return MySqlItem_c(in_data, MySqlItemType_e::Str, allowNull);
}

class MySqlTask_c {
protected:

    WFMySQLTask*         wfTask;
    mimicry::SqlString_c sqlComm;
    MySqlCallback_t      _callback;

    MySqlTask_c(
        WFMySQLTask*           in_task,
        const MySqlCallback_t& in_callback
    ) noexcept :
        wfTask(in_task),
        sqlComm(wfTask->get_req()->get_query()),
        _callback(in_callback) {
        wfTask->set_callback([this](WFMySQLTask* task) {
            this->callback();
            delete (this);
        });
    }

public:

    MySqlTask_c(const MySqlTask_c&) = delete;

    void callback() {
        if (this->_callback) {
            this->_callback(this);
        }
    }

    void set_callback(const MySqlCallback_t& in_callback) {
        _callback = in_callback;
    }

    auto get_req() {
        return this->wfTask->get_req();
    }

    protocol::MySQLResponse* get_resp() {
        return wfTask->get_resp();
    }

    const mimicry::SqlString_c& getSqlComm() const {
        return this->sqlComm;
    }

    bool is_success() {
        return (
            this->wfTask->get_state() == WFT_STATE_SUCCESS
            && this->get_resp()->get_packet_type() != MYSQL_PACKET_ERROR
        );
    }

    int get_state() const {
        return this->wfTask->get_state();
    }

    int get_error() {
        return this->wfTask->get_error();
    }

    // 获取第一个sql执行后返回的数据
    std::vector<std::vector<protocol::MySQLCell>> get_firstSql_result() {
        std::vector<std::vector<protocol::MySQLCell>> rows;
        if (this->is_success()) { // sql成功
            auto&& cursor = this->getResultCursor();
            if (cursor.get_cursor_status()
                == MYSQL_STATUS_GET_RESULT) { // 有结果返回
                cursor.fetch_all(rows);
            }
        }
        return rows;
    }

    protocol::MySQLResultCursor getResultCursor() {
        return protocol::MySQLResultCursor{this->get_resp()};
    }

    void start() {
        wfTask->start();
    }

    void run_push(SubTask* task) {
        assert(nullptr != task);
#if _isRelease_d
        // release
        if (nullptr != this) {
            series_of(task)->push_back(this->toSubTask());
        }
#else
        // debug
        series_of(task)->push_back(this->toSubTask());
#endif
    }

    SubTask* toSubTask() {
        return this->wfTask;
    }

    static MySqlTask_c*
        getMyTask(WFMySQLTask* in_task, const MySqlCallback_t& in_callback) {
        assert(in_task != nullptr);
        return (new MySqlTask_c{in_task, in_callback});
    }

    /*判断in_arr中元素类型是否正确
     */
    static bool is_type(
        const std::vector<protocol::MySQLCell>& in_arr,
        const std::initializer_list<int>        in_types
    ) {
        if (in_arr.size() < in_types.size())
            return false;
        auto t_it = in_types.begin(), t_endit = in_types.end();
        auto c_it = in_arr.begin();
        for (; t_it != t_endit; ++c_it, ++t_it) {
            if (*t_it & MySqlItemType_e::Null) {
                if (c_it->is_null())
                    continue;
                else if (*t_it == MySqlItemType_e::Null)
                    return false;
            }

            if (*t_it & MySqlItemType_e::Int) {
                if (c_it->is_int() == false) {
                    return false;
                }
            } else if (*t_it & MySqlItemType_e::Double) {
                if (c_it->is_double() == false && c_it->is_float() == false) {
                    return false;
                }
            } else if (*t_it & MySqlItemType_e::Ulonglong) {
                if (c_it->is_ulonglong() == false) {
                    return false;
                }
            } else if (*t_it & MySqlItemType_e::Str) {
                if (c_it->is_string() == false) {
                    return false;
                }
            } else if (*t_it & MySqlItemType_e::Datetime) {
                if (c_it->is_datetime() == false) {
                    return false;
                }
            } else if (*t_it & MySqlItemType_e::Time) {
                if (c_it->is_time() == false && c_it->is_date() == false) {
                    return false;
                }
            }
        }
        return true;
    }

    template<typename T>
    static bool to_type(
        std::vector<protocol::MySQLCell>::const_iterator in_it,
        MySqlItem_c<T>&&                                 in_item
    ) {
        int type = in_item.type;
        if constexpr (std::is_same_v<T, unsigned long long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned long> || std::is_same_v<T, long>) {
            if (type & MySqlItemType_e::Ulonglong && in_it->is_ulonglong()) {
                in_item.data = in_it->as_ulonglong();
                return true;
            }
        }
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned long long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned long> || std::is_same_v<T, long>) {
            if (type & MySqlItemType_e::Int && in_it->is_int()) {
                in_item.data = in_it->as_int();
                return true;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (type & MySqlItemType_e::Int && in_it->is_int()) {
                int rebool = in_it->as_int();
                if (rebool == 0) {
                    in_item.data = false;
                } else {
                    in_item.data = true;
                }
                return true;
            }
        } else if constexpr ((std::is_same_v<T, double>
                              || std::is_same_v<T, float>)) {
            if (type & MySqlItemType_e::Double) {
                if (in_it->is_double()) {
                    in_item.data = in_it->as_double();
                    return true;
                } else if (in_it->is_float()) {
                    in_item.data = in_it->as_float();
                    return true;
                }
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            // TODO: 类型 text 在这里 is_string() => false
            if (type & MySqlItemType_e::Str && in_it->is_string()) {
                in_item.data = in_it->as_string();
                return true;
            } else if (type & MySqlItemType_e::Datetime && in_it->is_datetime()) {
                in_item.data = in_it->as_datetime();
                return true;
            } else if (type & MySqlItemType_e::Time) {
                if (in_it->is_time()) {
                    in_item.data = in_it->as_time();
                    return true;
                } else if (in_it->is_date()) {
                    in_item.data = in_it->as_date();
                    return true;
                }
            }
        } else {
            assert(false);
        }
        if (type & MySqlItemType_e::Null) {
            if (in_it->is_null())
                return true;
        }
        return false;
    }

    template<typename T, typename... Args>
    static bool to_type(
        std::vector<protocol::MySQLCell>::const_iterator in_it,
        MySqlItem_c<T>&&                                 in_item,
        MySqlItem_c<Args>&&... in_args
    ) {
        return to_type(in_it, std::forward<MySqlItem_c<T>>(in_item))
               && to_type(
                   in_it + 1,
                   std::forward<MySqlItem_c<Args>>(in_args)...
               );
    }

    /* 将protocol::MySQLCell结果数组转换类型并赋值出来
     * 注意类型对应：
     * 接收变量 ==> 指定判断类型
     * long long / unsigned long long / size_t / time_t ==> Ulonglong
     * int / unsigned	==> Int
     * double / float	==> Double
     * string			==> Str | DataTime | Time
     */
    template<typename... Args>
    static bool to_type(
        std::vector<protocol::MySQLCell>::const_iterator in_begin,
        std::vector<protocol::MySQLCell>::const_iterator in_end,
        MySqlItem_c<Args>&&... in_args
    ) {
        if (in_end - in_begin == sizeof...(in_args)) {
            return to_type(
                in_begin,
                std::forward<MySqlItem_c<Args>>(in_args)...
            );
        }
        return false;
    }
};

#endif // !MYSQLTASK_H