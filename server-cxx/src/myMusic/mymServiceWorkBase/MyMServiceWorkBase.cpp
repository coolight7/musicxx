#include "myMusic/user/User.h"

#include <fstream>
#include <regex>

#include "MyMServiceWorkBase.h"
#include "myMusic/lyric/LyricDB.h"

using std::string;
using std::regex;
using std::regex_match;

bool MyMServiceWorkBase_c::assert_uid_id(MyHttpTask_c* in_http_task, int& re_uid, int& re_id)
{
	UserJWT_c ujwt;
	if (User_c::assert_jwt_user(in_http_task, ujwt)) {
		auto&& args = in_http_task->get_req()->get_arg();
		auto id_it = args.find(this->argName_id);
		if (id_it == args.end()) {
			in_http_task->get_resp()->out_err(mimicry::ResponeState_e::ValueHiatus);
			return false;
		}
		if (this->isFormat_id(id_it->second) == false) {
			in_http_task->get_resp()->out_err(mimicry::ResponeState_e::ValueError);
			return false;
		}
		re_id = atoi(id_it->second.c_str());
		re_uid = ujwt.aud;
		return true;
	}
	return false;
}
