#ifndef MYHTML_H
#define MYHTML_H

#include <map>
#include <string>


#include "MyHttpTask.h"

class MyHtml_c {
protected:
public:

    class DefaultHtmlValue_c { // 默认html的map的value类型
    public:

        enum DefaultHtmlType_e {
            Html,
            File
        };

        int         type = DefaultHtmlType_e::Html;
        std::string str;

        DefaultHtmlValue_c() {}

        DefaultHtmlValue_c(int in_type, const std::string& in_str) {
            type = in_type;
            str  = in_str;
        }
    };

    static std::map<int, DefaultHtmlValue_c> defaultHtml; // http状态码默认html页面
    /* 添加http状态码对应的默认html页面
     * \param in_code http状态码
     * \param in_html html内容字符串
     */
    static void defaultHTML_set_html(int in_code, const std::string& in_html);
    /* 添加http状态码对应的默认html页面
     * \param in_code http状态码
     * \param in_path html文件路径
     */
    static void defaultHTML_set_file(int in_code, const std::string& in_path);
    /* 将指定http状态码对应的默认html内容输出到task返回
     * \param task Http请求的task
     * \param in_codeInt int类型http状态码
     * \param in_codeStr string类型http状态码，当指定该值时则int型不使用并使用该值
     */
    static bool
        defaultHTML_out(MyHttpTask_c* task, int in_codeInt, const std::string& in_codeStr = "");
    static MyHttpProcess_t defaultHTML_fun(int in_codeInt);
};

#endif // !MYHTML_H
