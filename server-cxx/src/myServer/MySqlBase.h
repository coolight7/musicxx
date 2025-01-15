#ifndef MYSQLBASE_H
#define MYSQLBASE_H

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include "spdlog/spdlog.h"
#include "workflow/WFMySQLConnection.h"
#include "workflow/WFTaskFactory.h"

#include "GlobalUtil.h"
#include "MyLog.h"
#include "MySqlTask.h"

class MySqlBase_c {
protected:

    bool        isClose = false;
    std::string url;

public:

    MySqlBase_c(const std::string& in_url) :
        url(in_url) {}

    MySqlTask_c* get(const std::string& in_query, const MySqlCallback_t& in_callback) {
        if (isClose) {
            return nullptr;
        }

        auto task = WFTaskFactory::create_mysql_task(this->url, 0, nullptr);
        task->get_req()->set_query(in_query);
        auto mytask = MySqlTask_c::getMyTask(task, nullptr);
        mytask->set_callback(in_callback);
        return mytask;
        // return conn.create_query_task(query, std::bind(fun,
        // std::placeholders::_1, in_callback, before_callback));
        /*
        auto fun = [callback = in_callback, in_query](WFMySQLTask* in_task) {
                spdlog::info(in_query);
                auto mytask = MySqlTask_c::getMyTask(in_task);
                callback(mytask);
        };
        return conn.create_query_task(in_query, fun);	//不可用
        */
    }

    // 启动一个sql任务
    MySqlTask_c* dosql(const std::string& in_sql, const MySqlCallback_t& callback) {
        return this->get(in_sql, callback);
    }

    MySqlTask_c* doSqlSaveErrLog(
        const std::string&     in_sql,
        const MySqlCallback_t& callback,
        MyLog_c*               in_log,
        const char*            fileName,
        const char*            funName,
        int                    lineNum
    ) {
        MySqlTask_c* mytask = nullptr;
        if (in_log) {
            auto fun = [in_log,
                        fileName,
                        funName,
                        lineNum](MySqlTask_c* mytask, const MySqlCallback_t& callback) {
                if (mytask->is_success() == false) {
                    auto&       sqlComm = mytask->getSqlComm();
                    std::string err;
                    err.reserve(sizeof(funName) + 24 + sqlComm.size());
                    err = fmt::format("{}:({}:{}) : ", fileName, funName, lineNum);
                    if (mytask->get_state() != WFT_STATE_SUCCESS) {
                        err += "task error | ";
                    } else if (mytask->get_resp()->get_packet_type() == MYSQL_PACKET_ERROR) {
                        err += "respone error | ";
                    }
                    err += sqlComm;
                    in_log->cferror(err);
                }
                if (callback) {
                    callback(mytask);
                }
            };
            mytask = this->get(in_sql, std::bind(fun, std::placeholders::_1, callback));
        } else {
            mytask = this->get(in_sql, callback);
        }
        return mytask;
    }

#define _doSqlSaveErrlog_d(sql, fun, log) \
    doSqlSaveErrLog(sql, fun, log, __FILE__, __FUNCTION__, __LINE__)

    MySqlTask_c* doSqlSaveAllLog(
        const std::string&     in_sql,
        const MySqlCallback_t& callback,
        MyLog_c*               in_log,
        const char*            fileName,
        const char*            funName,
        int                    lineNum
    ) {
        MySqlTask_c* mytask = nullptr;
        if (in_log) {
            auto fun = [in_log,
                        fileName,
                        funName,
                        lineNum](MySqlTask_c* mytask, const MySqlCallback_t& callback) {
                auto& sqlComm = mytask->getSqlComm();
                if (mytask->is_success() == false) {
                    std::string err;
                    err.reserve(sizeof(funName) + 24 + sqlComm.size());
                    err = fmt::format("{}:({}:{}) : ", fileName, funName, lineNum);
                    if (mytask->get_state() != WFT_STATE_SUCCESS) {
                        err += " : task error | ";
                    } else if (mytask->get_resp()->get_packet_type() == MYSQL_PACKET_ERROR) {
                        err += " : respone error | ";
                    }
                    err += sqlComm;
                    in_log->cferror(err);
                } else {
                    std::string succ;
                    succ.reserve(sizeof(funName) + 24 + sqlComm.size());
                    succ  = funName;
                    succ += " : success | ";
                    succ += sqlComm;
                    in_log->cfinfo(succ);
                }
                if (callback) {
                    callback(mytask);
                }
            };
            mytask = this->get(in_sql, std::bind(fun, std::placeholders::_1, callback));
        } else {
            mytask = this->get(in_sql, callback);
        }
        return mytask;
    }

#define _dosqlSaveAllLog_d(sql, fun, log) \
    doSqlSaveAllLog(sql, fun, log, __FILE__, __FUNCTION__, __LINE__)

    void close(const MySqlCallback_t& callback = [](auto task) {}) {
        if (isClose) {
            return;
        }
        isClose = true;
        // conn.create_disconnect_task([](auto task){})->start();
    }

    // 转义目标字符：斜杠\\ 单引号' 双引号"
    static std::string&
        escape(std::string& in_str, std::initializer_list<char> integers = {'\\', '\'', '"'}) {
        return mimicry::escape(in_str, integers);
    }

    static std::string escape(
        const std::string&          in_str,
        std::initializer_list<char> integers = {'\\', '\'', '"'}
    ) {
        std::string str{in_str};
        return MySqlBase_c::escape(str, integers);
    }

    ~MySqlBase_c() {
        close();
    }
};

#endif // !MYSQLBASE_H