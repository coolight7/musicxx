#ifndef MIMICRY_GLOBALUTIL_H
#define MIMICRY_GLOBALUTIL_H

/*
//                      __     __
//                     0__0o0o0__0
//                      o8888888o
//                     88"  *  "88
//                     (| /\ /\ |)
//                      0\  _  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//               天依保佑         永无BUG
*/

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

namespace mimicry {
    // 将in_str按in_char分割为多部分
    std::vector<std::string> strSplit(const std::string& in_str, char in_char);
    // 在 in_str 中删除 in_char 字符
    void strEliminate(std::string& in_str, char in_char);
    // 将in_str中的in_chars字符用反斜杠\\转义
    std::string& escape(std::string& in_str, const std::initializer_list<char>& in_chars);
    std::string  escape(const std::string& in_str, const std::initializer_list<char>& integers);

    bool has_string(const std::string& in_str, const std::initializer_list<const char*>& in_strs);
    bool has_char(const std::string& in_str, const std::initializer_list<char>& in_chars);
    // 移除字符串两端的空格
    std::string& removeBetweenSpace(std::string& in_str);
    /*将时间戳转换为Http时间格式
     * \param in_time 时间戳，单位：秒
     */
    std::string toHttpTimeStr(time_t in_time);
    std::string getMD5(const std::string& in_str);
    std::string getMD5(const char* in_str, size_t len);
    std::string getSHA1(const std::string& in_str);
    std::string getSHA1(const char* in_str, size_t len);
    std::string getSHA512(const std::string& in_str);
    std::string getSHA512(const char* in_str, size_t len);
    // url编解码
    std::string url_decode(const std::string& str);
    std::string url_encode(const std::string& str);
    // 无符号值，即只包含数字
    bool isFormat_int(const std::string& in_str);
    bool isFormat_unsignedInt(const std::string& in_str);
    bool isFormat_bool(const std::string& in_str);
    bool isFormat_MD5(const std::string& in_str);
    // char 类型的 SHA1 值检查： [0-9a-zA-Z]{40}
    bool         isFormat_SHA1(const std::string& in_str);
    bool         isFormat_SHA512(const std::string& in_str);
    bool         isFormat_ipv4(const std::string& in_ipv4);
    bool         isFormat_email(const std::string& in_str);
    unsigned int ipv4_strToNum(const std::string& in_ipstr);
    /// <summary>
    ///  获取utf-8字符串的字符个数
    /// </summary>
    /// <param name="in_str"></param>
    /// <param name="in_strLen"></param>
    /// <returns></returns>
    size_t getUtf8Len(const char* in_str, size_t in_strLen);
    size_t getUtf8Len(const std::string in_str);
    bool   isFormat_limitChar(
          const char*                        in_str,
          size_t                             in_len,
          const std::initializer_list<char>& in_chars,
          size_t                             in_minLen = 0,
          size_t                             in_maxLen = size_t(-1)
      );
    bool isFormat_limitChar(
        const std::string&                 in_str,
        const std::initializer_list<char>& in_chars,
        size_t                             in_minLen = 0,
        size_t                             in_maxLen = size_t(-1)
    );
    // bool ip_numTostr(in_ip);

    // 判断 in_its 是否都 != in_map.end()
    bool isExist(
        const std::multimap<std::string, std::string>& in_map,
        const std::initializer_list<std::multimap<std::string, std::string>::const_iterator>& in_its
    );
    // 是否存在目录路径
    bool isExist_dir(const char* in_path);
    /*创建目录路径
     * 如果存在则不操作且返回true
     * 如果不存在则创建，并返回是否创建成功
     */
    bool createDir(const std::string& in_path);
    /*创建文件
     * 如果文件存在则返回true
     * 如果不存在则创建，并返回是否创建成功
     */
    bool createFile(const std::string& in_path);
    /*判断字符串编码
     * \param in_str 待判断的字符串
     * \param in_use_len 取in_str的长度，长度越长耗时越长，但准确性会更高
     * \param re_encoding 返回可能性最高的编码格式名称
     *
     * \return 是否判断成功
     */
    /*
    bool get_str_encoding(const std::string& in_str, size_t in_use_len,
    std::string& re_encoding); int to_encoding(const std::string& in_str, size_t
    in_str_len, std::string& re_str, const std::string& from_encoding, const
    std::string& to_encoding); bool to_encoding_utf8(const std::string& in_str,
            size_t in_str_len,
            std::string& re_str);
    */
    std::wstring to_wstring(const std::string str);
    std::string  to_string(const std::wstring str);

    /// ## 运行的系统平台信息
    class MyPlatform_c {
#ifdef NDEBUG
        // release
#define _isDebug_d   false
#define _isRelease_d true
        static constexpr bool isDebug    = false;
        static constexpr bool isNotDebug = true;
#else
        // debug
#define _isDebug_d   true
#define _isRelease_d false
        static constexpr bool isDebug    = true;
        static constexpr bool isNotDebug = false;
#endif // NDEBUG

#ifdef _WIN32
#define _isWindows_d true
        static constexpr bool isWindows = true;
        static constexpr bool isLinux   = false;
#else
#define _isLinux_d true
        static constexpr bool isWindows = false;
        static constexpr bool isLinux   = true;
#endif // _WIN32
    };
} // namespace mimicry

#endif // !MIMICRY_GLOBALUTIL_H