#include "Cookie.h"

#include <regex>
#include <stdlib.h>

using std::map;
using std::string;

void mimicry::Cookie_c::clear_toStr() {
    if (this->toStr != nullptr) {
        delete toStr;
        toStr = nullptr;
    }
}

std::map<std::string, std::string>
    mimicry::Cookie_c::from_cookie(const std::string& in_cookie_str) {
    std::map<std::string, std::string> arr;
    std::regex                         reg{"(\\S+)=([^;\\s]*)"};
    std::sregex_iterator pos{in_cookie_str.begin(), in_cookie_str.end(), reg},
        end;
    for (; pos != end; ++pos)
        arr[pos->str(1)] = std::move(pos->str(2));
    return arr;
}

bool mimicry::Cookie_c::set(const string& in_key, const string& in_value) {
    this->key   = in_key;
    this->value = in_value;
    this->clear_toStr();
    return true;
}

const std::string& mimicry::Cookie_c::get_key() const {
    return this->key;
}

const std::string& mimicry::Cookie_c::get_value() const {
    return this->value;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_Expires(time_t in_time) {
    this->expires = in_time;
    this->clear_toStr();
    return *this;
}

time_t mimicry::Cookie_c::get_Expires() const {
    return this->expires;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_MaxAge(time_t in_age) {
    this->max_age = in_age;
    this->clear_toStr();
    return *this;
}

time_t mimicry::Cookie_c::get_MaxAge() const {
    return this->max_age;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_Domain(const string& in_domain) {
    this->domain = in_domain;
    this->clear_toStr();
    return *this;
}

const std::string& mimicry::Cookie_c::get_Domain() const {
    return this->domain;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_Path(const string& in_path) {
    this->path = in_path;
    this->clear_toStr();
    return *this;
}

const string& mimicry::Cookie_c::get_Path() const {
    return this->path;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_Secure(bool in_bool) {
    this->m_is_secure = in_bool;
    this->clear_toStr();
    return *this;
}

bool mimicry::Cookie_c::is_Secure() const {
    return this->m_is_secure;
}

mimicry::Cookie_c& mimicry::Cookie_c::set_HttpOnly(bool in_bool) {
    this->m_is_HttpOnly = in_bool;
    this->clear_toStr();
    return *this;
}

bool mimicry::Cookie_c::is_HttpOnly() const {
    return this->m_is_HttpOnly;
}

const std::string& mimicry::Cookie_c::to_string(bool isMust) {
    if (toStr == nullptr) {
        toStr = new std::string{std::move(this->to_string_const())};
    }
    return *toStr;
}

std::string mimicry::Cookie_c::to_string_const() const {
    if (toStr == nullptr) {
        std::string reStr;
        if (this->key.empty() == false) {
            reStr.reserve(200); // 假设每个键值对共有20个字符长度
            reStr += this->key;
            if (this->value.empty() == false) {
                reStr += '=';
                reStr += this->value;
            }
            reStr += ';';
            if (this->path.empty() == false) {
                reStr += "Path=";
                reStr += this->path;
                reStr += ";";
            }
            if (this->domain.empty() == false) {
                reStr += "Domain=";
                reStr += this->domain;
                reStr += ";";
            }
            reStr += "Max-Age=";
            reStr += std::to_string(this->max_age);
            reStr += ';';
            if (this->expires > 0) {
                reStr += "Expires=";
                reStr += std::to_string(this->expires);
                reStr += ';';
            }
            if (this->m_is_HttpOnly)
                reStr += "HttpOnly;";
            if (this->m_is_secure)
                reStr += "Secure;";
        }
        return reStr;
    } else {
        return *toStr;
    }
}
