#include "GlobalUtil.h"
#include <string>

namespace mimicry {
    class SqlString_c : public std::string {
    protected:

        bool auto_s_quotation = true;

    public:

        SqlString_c() {}

        SqlString_c(size_t size) {
            this->reserve(size);
        }

        SqlString_c(const char* in_char) :
            std::string(in_char) {}

        SqlString_c(const std::string& in_str) :
            std::string(in_str) {}

        SqlString_c(const SqlString_c& in_sql) :
            std::string(in_sql),
            auto_s_quotation(in_sql.auto_s_quotation) {}

        // 在 / 和 % 添加字符串时是否启用自动在字符两端添加单引号
        // 当 % 替换为null时不会添加引号，默认启用
        void set_auto_s_quotation(bool in_auto) {
            this->auto_s_quotation = in_auto;
        }

        /// * / 除法
        /// * 转义特殊字符，并添加 引号
        static std::string doDivided(const std::string& in_str, bool in_auto_s_quotation = true) {
            std::string reStr;
            if (in_auto_s_quotation) {
                reStr += '\'';
                reStr += mimicry::escape(in_str, {'\\', '\'', '"', '`'});
                reStr += '\'';
            } else
                reStr += mimicry::escape(in_str, {'\\', '\'', '"', '`'});
            return reStr;
        }

        /// * % 求余
        /// * 转义特殊字符，并添加 引号
        /// * 如果目标为空字符串，则替代为 null
        static std::string doRemainde(const std::string& in_str, bool in_auto_s_quotation = true) {
            std::string reStr;
            if (in_str.empty())
                reStr = "null";
            else {
                std::string str = in_str;
                if (in_auto_s_quotation) {
                    reStr += '\'';
                    reStr += mimicry::escape(str, {'\\', '\'', '"', '`'});
                    reStr += '\'';
                } else
                    reStr += mimicry::escape(str, {'\\', '\'', '"', '`'});
            }
            return reStr;
        }

        SqlString_c& operator+(const std::string&) = delete;

        // 自动转义字符串后append附加进来
        // 转义目标字符：斜杠\\ 单引号' 双引号"
        // 默认在添加的字符两端添加单引号
        SqlString_c& operator/(const std::string& in_str) {
            if (auto_s_quotation) {
                *this += '\'';
                *this += mimicry::escape(in_str, {'\\', '\'', '"', '`'});
                *this += '\'';
            } else
                *this += mimicry::escape(in_str, {'\\', '\'', '"', '`'});
            return *this;
        }

        SqlString_c& operator/(const int in_t) {
            *this += '\'';
            *this += std::to_string(in_t);
            *this += '\'';
            return *this;
        }

        SqlString_c& operator/(const bool in_bool) {
            if (true == in_bool) {
                *this += "true";
            } else {
                *this += "false";
            }
            return *this;
        }

        // 自动转义字符串后append附加进来
        // 如果目标为空字符串则替换为null
        // 转义目标字符：斜杠\\ 单引号' 双引号"
        // 默认在添加的字符两端添加单引号，当 % 替换为null时不会添加引号
        SqlString_c& operator%(const std::string& in_str) {
            if (in_str.empty())
                *this += "null";
            else {
                std::string str = in_str;
                if (auto_s_quotation) {
                    *this += '\'';
                    *this += mimicry::escape(str, {'\\', '\'', '"', '`'});
                    *this += '\'';
                } else
                    *this += mimicry::escape(str, {'\\', '\'', '"', '`'});
            }
            return *this;
        }

        // 如果 in_t < 0 则替换为null
        // 默认在添加的字符两端添加单引号，当 % 替换为null时不会添加引号
        SqlString_c& operator%(const int in_t) {
            if (in_t < 0)
                *this += "null";
            else {
                *this += '\'';
                *this += std::to_string(in_t);
                *this += '\'';
            }
            return *this;
        }

        // 直接append附加字符串
        SqlString_c& operator*(const std::string& in_str) {
            *this += in_str;
            return *this;
        }

        SqlString_c& operator*(const char in_char) {
            *this += in_char;
            return *this;
        }

        SqlString_c& operator*(const char* in_char) {
            *this += in_char;
            return *this;
        }

        // int double float
        template<typename T>
        SqlString_c& operator*(const T& in_t) {
            *this += std::to_string(in_t);
            return *this;
        }

        SqlString_c& operator=(const std::string& in_str) {
            *((std::string*)this) = in_str;
            return *this;
        }

        SqlString_c& operator=(const char* in_str) {
            *((std::string*)this) = in_str;
            return *this;
        }
    };
}; // namespace mimicry
