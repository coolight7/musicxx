#ifndef MIMICRY_COOKIES_H
#define MIMICRY_COOKIES_H

#include <map>
#include <string>

#include "GlobalUtil.h"

namespace mimicry {

    class Cookie_c {
    protected:

        std::string key, value;

        bool   m_is_secure = false, m_is_HttpOnly = false;
        time_t max_age = -1, expires = -1;

        std::string path;   // 路径
        std::string domain; // 域名

        std::string* toStr = nullptr; // 缓存序列化的string

        void clear_toStr();

    public:

        Cookie_c() {}

        Cookie_c(const Cookie_c& in_cookie) {
            if (in_cookie.toStr != nullptr) {
                this->toStr = new std::string{*(in_cookie.toStr)};
            }
        }

        static std::map<std::string, std::string>
            from_cookie(const std::string& in_cookie_str);
        // from_set-cookie

        bool set(const std::string& in_key, const std::string& in_value);
        const std::string& get_key() const;
        const std::string& get_value() const;

        Cookie_c& set_Expires(time_t in_time);
        time_t    get_Expires() const;
        Cookie_c& set_MaxAge(time_t in_age);
        time_t    get_MaxAge() const;

        Cookie_c&          set_Domain(const std::string& in_Domain);
        const std::string& get_Domain() const;
        Cookie_c&          set_Path(const std::string& in_path);
        const std::string& get_Path() const;

        Cookie_c& set_Secure(bool in_bool);
        bool      is_Secure() const;
        Cookie_c& set_HttpOnly(bool in_bool);
        bool      is_HttpOnly() const;

        const std::string& to_string(bool isMust = false);
        std::string        to_string_const() const;

        operator std::string() {
            return to_string();
        }

        ~Cookie_c() {
            clear_toStr();
        }
    };

}; // namespace mimicry

#endif