#include "GlobalUtil.h"

#include <algorithm>
#include <assert.h>
#include <ctype.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <string_view>
#include <time.h>

// #include "unicode/ucnv.h"
// #include "unicode/utypes.h"
// #include "unicode/ucsdet.h"

#include "openssl/md5.h"
#include "openssl/sha.h"

#ifdef _WIN32
#include <WS2tcpip.h>
#include <direct.h>
#include <io.h>

#endif // _WIN32
#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#endif // !_WIN32

using std::fstream;
using std::regex;
using std::regex_match;
using std::string;
using std::string_view;
using std::vector;

using std::map;

vector<string> mimicry::strSplit(const string& in_str, char in_char) {
    vector<std::string> re_strlist;
    std::istringstream  iss{in_str};                  // 输入流
    for (string item; getline(iss, item, in_char);) { // 以split为分隔符
        re_strlist.push_back(item);
    }
    return re_strlist;
}

void mimicry::strEliminate(string& in_str, char in_char) {
    in_str.erase(
        std::remove(in_str.begin(), in_str.end(), in_char),
        in_str.end()
    );
}

bool mimicry::has_string(
    const std::string&                        in_str,
    const std::initializer_list<const char*>& in_strs
) {
    for (auto it = in_strs.begin(), endit = in_strs.end(); it != endit; ++it) {
        if (in_str.find(*it) != string::npos) {
			return false;
		}
    }
    return true;
}

bool mimicry::has_char(
    const std::string&                 in_str,
    const std::initializer_list<char>& in_chars
) {
    const char *endch = in_chars.end(), *ch;
    for (auto it = in_str.begin(), endit = in_str.end(); it != endit; ++it) {
        for (ch = in_chars.begin(); ch != endch; ++ch) {
            if (*it == *ch) {
				return false;
			}
        }
    }
    return true;
}

std::string& mimicry::removeBetweenSpace(std::string& in_str) {
    auto it = in_str.end();
    if (in_str.size() > 0) {
        do {
            --it;
            if (*it == ' ')
                in_str.erase(it);
            else
                break;
        } while (it != in_str.begin());
    }
    for (auto it = in_str.begin(); it != in_str.end() && *it == ' '; ++it) {
		in_str.erase(it);
	}
    return in_str;
}

std::string mimicry::toHttpTimeStr(time_t in_time) {
    char      nowTimeStr[64] = {};
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &in_time);
#endif // _WIN32
#ifndef _WIN32
    localtime_r(&in_time, &tm);
#endif // !_WIN32
    strftime(nowTimeStr, sizeof(nowTimeStr), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    return string{nowTimeStr};
}

std::string& mimicry::escape(
    std::string&                       in_str,
    const std::initializer_list<char>& in_chars
) {
    if (in_str.empty()) {
		return in_str;
	}
    const char *endch = in_chars.end(), *ch;
    for (auto it = in_str.begin(), endit = in_str.end();;) {
        for (ch = in_chars.begin(); ch != endch; ++ch) {
            if (*it == *ch) {
                it = in_str.insert(it, '\\');
                ++it;
                endit = in_str.end();
                break;
            }
        }
        if (it == endit || ++it == endit) {
			break;
		}
    }
    return in_str;
}

std::string mimicry::escape(
    const std::string&                 in_str,
    const std::initializer_list<char>& integers
) {
    std::string str = in_str;
    return mimicry::escape(str, integers);
}

std::string mimicry::getMD5(const std::string& in_str) {
    return mimicry::getMD5(in_str.c_str(), in_str.size());
}

std::string mimicry::getMD5(const char* in_str, size_t len) {
    char          result[34]            = {};
    unsigned char md[MD5_DIGEST_LENGTH] = {};
    MD5((const unsigned char*)in_str, len, md);
    std::stringstream ss;
    ss.fill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss.width(2);
        ss << std::hex << int(md[i]);
    }
    ss.read(result, 32);
    return result;
}

std::string mimicry::getSHA1(const std::string& in_str) {
    return mimicry::getSHA1(in_str.c_str(), in_str.size());
}

std::string mimicry::getSHA1(const char* in_str, size_t len) {
    char          result[42] = {};
    unsigned char md[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)in_str, len, md);
    std::stringstream ss;
    ss.fill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss.width(2);
        ss << std::hex << int(md[i]);
    }
    ss.read(result, 40);
    return result;
}

std::string mimicry::getSHA512(const std::string& in_str) {
    return mimicry::getSHA512(in_str.c_str(), in_str.size());
}

std::string mimicry::getSHA512(const char* in_str, size_t len) {
    char          result[130] = {};
    unsigned char md[SHA512_DIGEST_LENGTH];
    SHA512((unsigned char*)in_str, len, md);
    std::stringstream ss;
    ss.fill('0');
    for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        ss.width(2);
        ss << std::hex << int(md[i]);
    }
    ss.read(result, 128);
    return result;
}

size_t mimicry::getUtf8Len(const char* in_str, size_t in_strLen) {
    size_t length = 0;
    for (size_t i = 0, len = 0; i < in_strLen; i += len) {
        unsigned char byte = in_str[i];
        if (byte >= 0xFC) // lenght 6
            len = 6;
        else if (byte >= 0xF8)
            len = 5;
        else if (byte >= 0xF0)
            len = 4;
        else if (byte >= 0xE0)
            len = 3;
        else if (byte >= 0xC0)
            len = 2;
        else
            len = 1;
        length++;
    }
    return length;
}

size_t mimicry::getUtf8Len(const std::string in_str) {
    return mimicry::getUtf8Len(in_str.c_str(), in_str.size());
}

bool mimicry::isFormat_limitChar(
    const char*                        in_str,
    size_t                             in_len,
    const std::initializer_list<char>& in_chars,
    size_t                             in_minLen,
    size_t                             in_maxLen
) {
    assert(in_minLen <= in_maxLen);
    size_t length = 0;
    for (size_t i = 0, len = 0; i < in_len; i += len) {
        unsigned char byte = in_str[i];
        if (byte >= 0xFC) // lenght 6
            len = 6;
        else if (byte >= 0xF8)
            len = 5;
        else if (byte >= 0xF0)
            len = 4;
        else if (byte >= 0xE0)
            len = 3;
        else if (byte >= 0xC0)
            len = 2;
        else {
            // 该字符为ASCii
            len = 1;
            // 检查该char是否在禁用列表中
            for (auto it = in_chars.begin(); it != in_chars.end(); ++it) {
                if (in_str[i] == *it) {
                    return false;
                }
            }
        }
        // 增加字符数量
        length++;
        // 判断是否超过
        if (length > in_maxLen) {
            return false;
        }
    }
    return (length >= in_minLen);
}

bool mimicry::isFormat_limitChar(
    const std::string&                 in_str,
    const std::initializer_list<char>& in_chars,
    size_t                             in_minLen,
    size_t                             in_maxLen
) {
    return isFormat_limitChar(
        in_str.c_str(),
        in_str.size(),
        in_chars,
        in_minLen,
        in_maxLen
    );
}

std::string mimicry::url_decode(const std::string& str) {
    auto fun_fromHex = [](unsigned char x) -> unsigned char {
        unsigned char y;
        if (x >= 'A' && x <= 'Z')
            y = x - 'A' + 10;
        else if (x >= 'a' && x <= 'z')
            y = x - 'a' + 10;
        else if (x >= '0' && x <= '9')
            y = x - '0';
        else
            y = 0;
        return y;
    };
    string re_str{};
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') {
            re_str += ' ';
        } else if (i + 2 < length && str[i] == '%') {
            unsigned char high  = fun_fromHex((unsigned char)str[++i]);
            unsigned char low   = fun_fromHex((unsigned char)str[++i]);
            re_str             += high * 16 + low;
        } else
            re_str += str[i];
    }
    return re_str;
}

std::string mimicry::url_encode(const std::string& str) {
    auto fun_toHex = [](unsigned char x) -> unsigned char {
        return x > 9 ? x + 55 : x + 48;
    };
    string re_str;
    size_t len = str.length();
    for (size_t i = 0; i < len; i++) {
        if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_')
            || (str[i] == '.') || (str[i] == '~')) {
            re_str += str[i];
        } else if (str[i] == ' ') {
            re_str += "+";
        } else {
            re_str += '%';
            re_str += fun_toHex((unsigned char)str[i] >> 4);
            re_str += fun_toHex((unsigned char)str[i] % 16);
        }
    }
    return re_str;
}

bool mimicry::isFormat_int(const std::string& in_str) {
    regex reg{"[+-]?\\d+"};
    return regex_match(in_str, reg);
}

bool mimicry::isFormat_unsignedInt(const std::string& in_str) {
    regex reg{"\\d+"};
    return regex_match(in_str, reg);
}

bool mimicry::isFormat_bool(const std::string& in_str) {
    return (in_str == "true" || in_str == "false");
}

bool mimicry::isFormat_MD5(const std::string& in_str) {
    if (in_str.size() != MD5_DIGEST_LENGTH * 2) {
		return false;
	}
    regex reg_passwd{"[0-9a-zA-Z]*"};
    return regex_match(in_str, reg_passwd);
}

bool mimicry::isFormat_SHA1(const std::string& in_str) {
    if (in_str.size() != SHA_DIGEST_LENGTH * 2) {
		return false;
	}
    regex reg_passwd{"[0-9a-zA-Z]*"};
    return regex_match(in_str, reg_passwd);
}

bool mimicry::isFormat_SHA512(const std::string& in_str) {
    if (in_str.size() != SHA512_DIGEST_LENGTH * 2) {
		return false;
	}
    regex reg_passwd{"[0-9a-zA-Z]*"};
    return regex_match(in_str, reg_passwd);
}

bool mimicry::isFormat_ipv4(const std::string& in_ipv4) {
    regex reg_passwd{
        "(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}"
    };
    return regex_match(in_ipv4, reg_passwd);
}

bool mimicry::isFormat_email(const std::string& in_str) {
    // 匹配任意字符开头，包含@，然后接任意字符，包含.然后接任意字符
    regex reg_passwd{"^.+\\@.+\\..+$"};
    return regex_match(in_str, reg_passwd);
}

unsigned int mimicry::ipv4_strToNum(const std::string& in_ipstr) {
    struct in_addr addr {};

    struct in6_addr ipv6_inaddr {};

    if (inet_pton(AF_INET, in_ipstr.c_str(), (void*)&addr) > 0) {
        // 转换成功
#ifdef _WIN32
        return (unsigned int)(addr.s_addr);
#else
        return addr.s_addr;
#endif // _WIN32
    }
    return 0;
}

bool mimicry::isExist(
    const std::multimap<std::string, std::string>& in_map,
    const std::initializer_list<
        std::multimap<std::string, std::string>::const_iterator>& in_its
) {
    auto map_endit = in_map.end();
    for (auto it = in_its.begin(), endit = in_its.end(); it != endit; ++it) {
        if (*it == map_endit) {
            return false;
        }
    }
    return true;
}

bool mimicry::isExist_dir(const char* in_path) {
#ifdef _WIN32
    return (_access(in_path, 0) == 0);
#endif // _WIN32
#ifndef _WIN32
    return (access(in_path, 0) == 0);
#endif // !_WIN32
}

bool mimicry::createDir(const std::string& in_path) {
    char* view   = new char[in_path.size() + 1]{};
    bool  rebool = true;
    for (size_t i = 0, len = in_path.size(); i < len; ++i) {
        view[i] = in_path[i];
        if (view[i] == '\\' || view[i] == '/' || i == len - 1) {
            if (mimicry::isExist_dir(view) == false) {
#ifdef _WIN32
                if (_mkdir(view) != 0)
#endif // _WIN32
#ifndef _WIN32
                    if (mkdir(view, 0) != 0)
#endif
                    {
                        rebool = false;
                        break;
                    }
            }
        }
    }
    delete[] view;
    return rebool;
}

bool mimicry::createFile(const std::string& in_path) {
    if (in_path.empty()) {
        return false;
    }
    std::ofstream file{in_path};
    if (file.is_open() == false) {
        auto it = in_path.end();
        do {
            --it;
            if (*it == '\\' || *it == '/')
                break;
        } while (it != in_path.begin());
        if (mimicry::createDir(string(in_path.begin(), it)) == false) {
            return false;
        }
        file.close();
        file.open(in_path);
    }
    return file.is_open();
}

/*
bool mimicry::get_str_encoding(const std::string& in_str, size_t in_use_len,
std::string& re_encoding) { in_use_len = (in_use_len <= in_str.size()) ?
in_use_len : in_str.size(); UCharsetDetector* csd; const UCharsetMatch** csm;
        int32_t matchCount = 0;
        UErrorCode status = U_ZERO_ERROR;
        csd = ucsdet_open(&status);
        bool rebool = false;
        if (status == U_ZERO_ERROR) {
                ucsdet_setText(csd, in_str.c_str(), in_use_len, &status);
                if (status == U_ZERO_ERROR) {
                        csm = ucsdet_detectAll(csd, &matchCount, &status);
                        if (status == U_ZERO_ERROR && matchCount > 0) {
                                re_encoding = ucsdet_getName(csm[0], &status);
//分配了内存， 需要释放 if (status == U_ZERO_ERROR){ rebool = true;
                                }
                        }
                }
        }
        ucsdet_close(csd);
        return rebool;
}
int mimicry::to_encoding(const std::string& in_str,
        size_t in_str_len,
        std::string& re_str,
        const std::string& from_encoding,
        const std::string& to_encoding) {
        UErrorCode error = U_ZERO_ERROR;
        in_str_len = (in_str_len <= in_str.size()) ? in_str_len : in_str.size();
        int32_t new_len = in_str_len * 2 + 1;
        char* re_str_p = new char[new_len];
        ucnv_convert(to_encoding.c_str(), from_encoding.c_str(), re_str_p,
new_len, in_str.c_str(), in_str_len, &error); re_str = re_str_p;
        delete[]re_str_p;
        return error;
}
bool mimicry::to_encoding_utf8(const std::string& in_str,
        size_t in_str_len,
        std::string& re_str) {
        string from_encoding;
        size_t test_len = in_str_len;
        if (test_len > 1000)
                test_len = 1000;
        if (mimicry::get_str_encoding(in_str, in_str_len, from_encoding)) {
                if (mimicry::to_encoding(in_str, in_str_len, re_str,
from_encoding, "UTF-8") == 0) { return true;
                }
        }
        return false;
}
*/
std::wstring mimicry::to_wstring(const std::string str) { // string转wstring
    size_t len = str.size() * 2;                          // 预留字节数
    setlocale(LC_CTYPE, "");       // 必须调用此函数
    wchar_t* p = new wchar_t[len]; // 申请一段内存存放转换后的字符串
    mbstowcs(p, str.c_str(), len); // 转换
    std::wstring str1(p);
    delete[] p; // 释放申请的内存
    return str1;
}

std::string mimicry::to_string(const std::wstring str) { // wstring转string
    size_t len = str.size() * 4;
    setlocale(LC_CTYPE, "");
    char* p = new char[len];
    wcstombs(p, str.c_str(), len);
    std::string str1(p);
    delete[] p;
    return str1;
}