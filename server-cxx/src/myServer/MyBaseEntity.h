#ifndef MYBASEENTITY_H
#define MYBASEENTITY_H

#include <string>
#include "MyHttpState.h"
#include "nlohmann/json.hpp"

template<typename T = int>
class MyRespBaseEntity_c {
public:
	T*			data;
	int			code;
	std::string tip;

	MyRespBaseEntity_c(T* in_data, int in_code, const std::string& in_tip = "")
		:data(in_data), code(in_code), tip(in_tip) {}
	MyRespBaseEntity_c(int in_code, const std::string& in_tip = "") 
		:data(nullptr), code(in_code), tip(in_tip) {}

	bool isSuccess() {
		return (code == mimicry::ResponeState_e::OK || code == mimicry::ResponeState_e::OKCustom);
	}

	template<typename T1 = int>
	static MyRespBaseEntity_c<T1>* success(T1* in_data, int in_code = mimicry::ResponeState_e::OK) {
		return new MyRespBaseEntity_c<T1>(in_data, in_code);
	}
	template<typename T1 = int>
	static MyRespBaseEntity_c<T1>* error(int in_code, const std::string& in_tip) {
		return new MyRespBaseEntity_c<T1>(in_code, in_tip);
	}

	template<typename T1 = int>
	static bool fromWFTask(WFHttpTask* in_task, MyRespBaseEntity_c<T1>& reEntity) {
		if (in_task->get_state() == WFT_STATE_SUCCESS) {
			// task 正常
			std::string reStatucode;
			auto resp = in_task->get_resp();
			// 请求成功
			if (resp->get_status_code(reStatucode) && reStatucode.starts_with("2")) {
				const void* body;
				size_t len;
				if (resp->get_parsed_body(&body, &len)) {
					nlohmann::json j = nlohmann::json::parse(
						std::string((const char*)body),
						nullptr,
						false);
					from_json(j, reEntity);
					return true;
				}
			}
		}
		return false;
	}
};

template<typename T>
inline void from_json(const nlohmann::json& j, MyRespBaseEntity_c<T>& base) {
	try {
		// j[name] 不存在时将抛出异常
		auto& reCode = j["code"];
		if (reCode.is_number_integer()) {
			base.code = reCode.get<int>();
		}
		auto& reTip = j["tip"];
		if (reTip.is_string()) {
			base.tip = reTip.get<std::string>();
		}
		auto& redata = j["data"];
		if (base.data != nullptr && redata.is_null() == false) {
			*base.data = redata.get<T>();
		}
	}
	catch (...) {}
}
template<typename T>
inline void to_json(nlohmann::json& j, const MyRespBaseEntity_c<T>& base) {
	if (base.data != nullptr) {
		j = nlohmann::json{
			{"data", *(base.data)},
			{"code", base.code},
			{"tip",  base.tip}
		};
	}
	else {
		j = nlohmann::json{
			{"code", base.code},
			{"tip",  base.tip}
		};
	}
}
#endif // !MYBASEENTITY_H