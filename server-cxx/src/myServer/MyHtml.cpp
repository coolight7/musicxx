#include "MyHtml.h"

#include "MyHttpFile.h"

using std::map;
using std::string;

std::map<int, MyHtml_c::DefaultHtmlValue_c> MyHtml_c::defaultHtml;

void MyHtml_c::defaultHTML_set_html(int in_code, const string& in_html) {
    defaultHtml[in_code] = MyHtml_c::DefaultHtmlValue_c(
        MyHtml_c::DefaultHtmlValue_c::DefaultHtmlType_e::Html,
        in_html
    );
}

void MyHtml_c::defaultHTML_set_file(int in_code, const string& in_path) {
    defaultHtml[in_code] = MyHtml_c::DefaultHtmlValue_c(
        MyHtml_c::DefaultHtmlValue_c::DefaultHtmlType_e::File,
        in_path
    );
}

bool MyHtml_c::defaultHTML_out(MyHttpTask_c* task, int in_codeInt, const string& in_codeStr) {
    auto resp = task->get_resp();
    auto it   = defaultHtml.find(in_codeInt);
    if (it == defaultHtml.end()) {
        return false;
    }
    resp->clear_output_body();
    if (it->second.type == MyHtml_c::DefaultHtmlValue_c::DefaultHtmlType_e::Html) {
        resp->append_output_body(it->second.str);
    } else {
        MyHttpFile_c::out_file(task, it->second.str, false, false);
    }
    if (in_codeStr.empty())
        resp->set_status_code(std::to_string(in_codeInt));
    else
        resp->set_status_code(in_codeStr);
    return true;
}

MyHttpProcess_t MyHtml_c::defaultHTML_fun(int in_codeInt) {
    return [=](MyHttpTask_c* task) {
        defaultHTML_out(task, in_codeInt);
    };
}