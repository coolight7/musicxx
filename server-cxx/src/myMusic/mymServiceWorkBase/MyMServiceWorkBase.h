#ifndef MYMUSIC_SRCINFOBASE_H
#define MYMUSIC_SRCINFOBASE_H

#include "myMusic/user/User.h"

#include <string>
#include <initializer_list>
#include "myServer/MyHttpServer.h"

#define _MyArgItem_d(entity, argName, isFormat, enable, necessary) \
	MyArgItem_c{entity.argName, #argName, isFormat, enable, necessary}

#define _MyArgItem_notNecessary_d(entity, argName, isFormat) \
	_MyArgItem_d(entity, argName, isFormat, true, false)

#define _MyArgItem_necessary_d(entity, argName, isFormat) \
	_MyArgItem_d(entity, argName, isFormat, true, true)

// 请求参数验证解析
template<typename T = int>
class MyArgItem_c {
protected:
	bool enable = false,				//是否使用
		necessary = false;				//是否必要
	const std::function<bool()> 
		fun_enable = nullptr, 
		fun_necessary = nullptr;

public:
	T& data;						//数据
	const std::string& argName;		//参数名称
	const std::function<bool(const std::string&)> format_fun = nullptr;	//格式检查函数

	bool isEnable() {
		if (fun_enable) {
			return fun_enable();
		}
		else {
			return enable;
		}
	}
	bool isNecessary() {
		if (fun_necessary) {
			return fun_necessary();
		}
		else {
			return necessary;
		}
	}

	MyArgItem_c(
		T& in_data,
		const std::string& in_argName,
		const std::function<bool(const std::string&)>& in_format_fun, 
		const std::function<bool()>& in_fun_able,
		const std::function<bool()>& in_fun_necessary
	): fun_enable(in_fun_able),
		fun_necessary(in_fun_necessary),
		data(in_data),
		argName(in_argName),
		format_fun(in_format_fun){}
	MyArgItem_c(
		T& in_data,
		const std::string& in_argName,
		const std::function<bool(const std::string&)>& in_format_fun,
		bool in_able,
		const std::function<bool()>& in_fun_necessary
	):enable(in_able), 
		fun_necessary(in_fun_necessary),
		data(in_data),
		argName(in_argName),
		format_fun(in_format_fun) {}
	MyArgItem_c(
		T& in_data,
		const std::string& in_argName,
		const std::function<bool(const std::string&)>& in_format_fun,
		const std::function<bool()>& in_fun_able,
		bool in_necessary
	):necessary(in_necessary),
		fun_enable(in_fun_able),
		data(in_data),
		argName(in_argName),
		format_fun(in_format_fun) {}
	MyArgItem_c(
		T& in_data, 
		const std::string& in_argName,
		const std::function<bool(const std::string&)>& in_format_fun = nullptr,
		bool in_able = true,
		bool in_necessary = false
	):enable(in_able),
		necessary(in_necessary),
		data(in_data),
		argName(in_argName),
		format_fun(in_format_fun) {}


	
	template<typename T1, typename... Args>
	static bool analyse(
		MyHttpTask_c* in_http_task,
		std::multimap<std::string, std::string>& in_map,
		MyArgItem_c<T1>&& in_item
	) {
		if (in_item.isEnable()) {
			auto it = in_map.find(in_item.argName);
			if (in_item.isNecessary() && it == in_map.end()) {
				// 不存在该必要参数
#if _isDebug_d
				in_http_task->get_resp()->out_err(
					mimicry::ResponeState_e::ValueHiatus, 
					in_item.argName
				);
#else 
				in_http_task->get_resp()->out_err(
					mimicry::ResponeState_e::ValueHiatus
				);
#endif
				return false;
			}
			if (it != in_map.end()) {						
				// 存在参数
				if (in_item.format_fun && in_item.format_fun(it->second) == false) {
					// 有格式检查函数且检查失败
#if _isDebug_d
					in_http_task->get_resp()->out_err(
						mimicry::ResponeState_e::ValueError, 
						in_item.argName
					);
#else 
					in_http_task->get_resp()->out_err(
						mimicry::ResponeState_e::ValueError
					);
#endif
					return false;
				}
				else {
					// TODO: 仅支持 int | string | bool
					if constexpr (std::is_same_v<T1, std::string>) {
						in_item.data = std::move(it->second);
					} else if constexpr(std::is_same_v<T1, int>
						|| std::is_same_v<T1, unsigned>
						|| std::is_same_v<T1, long>
						|| std::is_same_v<T1, long long>
						|| std::is_same_v<T1, unsigned long>
						|| std::is_same_v<T1, unsigned long long>
						) {
						in_item.data = atoi(it->second.c_str());
					} else if constexpr (std::is_same_v<T1, bool>) {
						if (it->second == "true") {
							in_item.data = true;
						}
						else {
							in_item.data = false;
						}
					} else {
						assert(false);
					}
				}
			}
		}
		return true;
	}
	template<typename T1, typename... Args>
	static bool analyse(
		MyHttpTask_c* in_http_task,
		std::multimap<std::string, std::string>& in_map,
		MyArgItem_c<T1>&& in_item,
		MyArgItem_c<Args>&&... in_args
	) {
		return MyArgItem_c<>::analyse(in_http_task, in_map, std::forward<MyArgItem_c<T1>>(in_item))
			&& MyArgItem_c<>::analyse(in_http_task, in_map, std::forward<MyArgItem_c<Args>>(in_args)...);
	}
	/* 解析请求参数
		* re_jwt：将会检查http中是否携带jwt，并验证其有效性
		* in_http_task：http task
		* in_fun_checkJWT：
			* 可选，传入nullptr则不执行
			* 在检查jwt后执行用户自定义的jwt检查，返回false时为失败，终止后续解析
		* in_args：希望接收的参数
	*/
	template<typename... Args>
	static bool analyse(
		UserJWT_c& re_jwt,
		MyHttpTask_c* in_http_task,
		const std::function<bool(UserJWT_c&)>& in_fun_checkJWT,
		MyArgItem_c<Args>&&... in_args
	) {
		if (User_c::assert_jwt_user(in_http_task, re_jwt) == false) {
			return false;
		}
		if (nullptr != in_fun_checkJWT && in_fun_checkJWT(re_jwt) == false) {
			in_http_task->get_resp()->out_err(mimicry::ResponeState_e::Permission);
			return false;
		}
		auto&& args = in_http_task->get_req()->get_arg();
		//如果解析成功
		return MyArgItem_c<>::analyse(in_http_task, args, std::forward<MyArgItem_c<Args>>(in_args)...);
	}
	template<typename... Args>
	static bool analyse(
		MyHttpTask_c* in_http_task,
		MyArgItem_c<Args>&&... in_args
	) {
		auto&& args = in_http_task->get_req()->get_arg();
		//如果解析成功
		return MyArgItem_c<>::analyse(in_http_task, args, std::forward<MyArgItem_c<Args>>(in_args)...);
	}
	

	template<typename... Args>
	static MyArgItem_c<Args...> builder() {
	
	}
};

class MyMServiceWorkBase_c {
protected:
	MyHttpServer_c* server;
	std::string path_prefix;			// "[/project_name]/api/playlist"

	size_t limit_get_list_ids = 20, 
		limit_get_like_list = 100;

	std::string argName_id;

public:
	static void insert_fun(MySqlTask_c* task, MyHttpTask_c* in_http_task) {
		if (task->is_success()) {
			auto&& insert_id = task->get_resp()->get_last_insert_id();
			in_http_task->get_resp()->out_success(&insert_id);
		} else {
			in_http_task->get_resp()->out_err(mimicry::ResponeState_e::Exception);
		}
	}
	//如果song内容没有变化，则修改行数为0，因此部分情况若判断修改的行数会导致请求返回错误
	static void update_fun(
		MySqlTask_c* task,
		MyHttpTask_c* in_http_task, 
		bool in_check_affected,
		int err_code
	) {
		if (task->is_success() && 
			(false == in_check_affected 
				|| task->get_resp()->get_affected_rows() > 0)) {
			in_http_task->get_resp()->out_success();
		} else {
			in_http_task->get_resp()->out_err(err_code);
		}
	}

	MyMServiceWorkBase_c(
		MyHttpServer_c* in_server, 
		const std::string& in_path_prefix, 
		const std::string& in_argName_id
	) :server(in_server), 
		path_prefix(in_path_prefix),
		argName_id(in_argName_id){}
	/* 验证id格式
	*/
	virtual bool isFormat_id(const std::string& in_id) = 0;
	//断言请求中包含uid和id参数
	virtual bool assert_uid_id(MyHttpTask_c* in_http_task, int& re_uid, int& re_id);

	virtual ~MyMServiceWorkBase_c() {}
};

#endif // !MYMUSIC_SRCINFOBASE_H