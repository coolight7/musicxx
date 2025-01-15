#include "MyHttpFile.h"

#include <fstream>
#include <regex>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32

#endif // _WIN32
#ifndef _WIN32

#endif // linux

#include "workflow/WFTaskFactory.h"

#include "MyHttpFile.h"
#include "MyLog.h"

using std::ifstream;
using std::ofstream;
using std::string;

string mimicry::getFileWETag(const mimicry::myStat& in_fstat) {
    return "W/\""
           + mimicry::getSHA1(std::to_string(in_fstat.st_size) + std::to_string(in_fstat.mtime))
           + '"';
}

std::string mimicry::getFileMIME(const std::string& in_filePath) {
    if (in_filePath.ends_with(".html") || in_filePath.ends_with(".htm")) {
        return "text/html";
    } else if (in_filePath.ends_with(".css")) {
        return "text/css";
    } else if (in_filePath.ends_with(".js")) {
        return "application/javascript";
    } else {
        return "";
    }
}

bool MyHttpFile_c::out_file(
    MyHttpTask_c* task,
    const string& path,
    bool          enable_range,
    bool          enable_ETag
) {
    mimicry::myStat fstat;
    if (mimicry::getFileStat(path, fstat) == false)
        return false;
    auto   resp    = task->get_resp();
    auto   req     = task->get_req();
    bool   doRange = false;
    string range, fETag;
    if (enable_ETag) { // 是否启用ETag
        resp->add_header_pair("Cache-Control", "no-cache");
        resp->add_header_pair("Vary", "User-Agent");
        string eTag;
        fETag = mimicry::getFileWETag(fstat);
        if (req->find_header("If-None-Match", eTag)) { // 查找请求中是否有ETag
            if (fETag == eTag) {                       // ETag未变化
                resp->add_header_pair("Date", mimicry::toHttpTimeStr(time(NULL)).c_str());
                resp->add_header_pair("ETag", fETag);
                resp->add_header_pair(
                    "Expires",
                    mimicry::toHttpTimeStr(time(NULL) + size_t(60 * 60 * 24 * 365)).c_str()
                );
                resp->set_status_code("304");
                return true;
            }
        }
    }
    if (enable_range) { // 是否使用分段
        resp->add_header_pair("Accept-Ranges", "bytes");
        range.reserve(32);
        doRange = req->find_header("Range", range);
    } else {
        resp->add_header_pair("Accept-Ranges", "none");
    }
    ifstream ifile;
    ifile.open(path, std::fstream::binary);
    if (ifile.is_open()) {
        ifile.seekg(0, std::ios::end);
        const size_t flength = size_t(ifile.tellg()); // 计算文件大小
        size_t       fbegin = 0, fend = 0;            // 读取请求的range需要的范围
        if (doRange) {                                // 启用分段下载
            std::smatch mlist;
            doRange = regex_search(
                range,
                mlist,
                std::regex("\\s*([\\d]*)\\s*-\\s*([\\d]*)")
            ); // 正则表达式匹配出请求指定的范围
            if (doRange) {
                auto &&lstr = mlist.str(1), rstr = mlist.str(2);
                if (lstr.empty()) {   // 设定左范围
                    fbegin = 0;       // 省略左范围
                    if (rstr.empty()) // 设定右范围
                        fend = flength;
                    else { // - end ，返回结尾的end个字节
                        fbegin = flength - atoi(rstr.c_str()) + 1;
                        fend   = flength;
                    }
                } else {
                    fbegin = atoi(lstr.c_str());
                    if (rstr.empty()) // 设定右范围
                        fend = flength;
                    else
                        fend = atoi(rstr.c_str());
                }
                if (fbegin < 0 || fbegin > flength || fend < 0 || fend > flength || fbegin > fend) {
                    string re_if_range; // Range 请求范围越界
                    if (req->find_header(
                            "If-Range",
                            re_if_range
                        )) { // 若存在条件范围控制If-Range则修正
                        fbegin = 0;
                        fend   = flength;
                    } else { // 不存在条件范围控制If-Range则返回错误
                        resp->set_status_code("416");
                        resp->add_header_pair("Content-Range", "*/" + std::to_string(flength));
                        return false;
                    }
                } else {
                    // 文件最后修改时间
                    resp->add_header_pair(
                        "Last-Modified",
                        mimicry::toHttpTimeStr(fstat.mtime).c_str()
                    );
                    resp->set_status_code("206");
                }
            }
        }
        if (doRange == false) { // 当请求不指定range或正则表达式匹配失败时
            fbegin = 0;
            fend   = flength;
        }
        const size_t readLen = fend - fbegin;
        char*        buffer  = new char[readLen + 1]{};
        ifile.seekg(fbegin, std::ios::beg);
        ifile.read(buffer, readLen);
        const size_t real_readLen = ifile.gcount();
        ifile.close();
        if (doRange) {
            resp->add_header_pair(
                "Content-Range",
                "bytes " + std::to_string(fbegin) + "-" + std::to_string(fbegin + real_readLen - 1)
                    + "/" + std::to_string(flength)
            );
        }
        if (enable_ETag) {
            resp->add_header_pair("ETag", fETag);
        }
        resp->add_header_pair("Date", mimicry::toHttpTimeStr(time(NULL)).c_str());
        resp->clear_cache_header();
        resp->append_output_body(buffer, real_readLen);
        delete[] buffer;
        // 尝试添加content-type，
        // 部分文件请求需要类型才可以正常使用，比如.js需要指定类型，浏览器才会运行它
        auto reMIME = mimicry::getFileMIME(path);
        if (reMIME.empty() == false) {
            resp->add_header_pair("Content-Type", reMIME.c_str());
        }
        return true;
    } else
        return false;
}

bool MyHttpFile_c::out_dir(
    MyHttpTask_c*      in_task,
    const std::string& in_dir_path,
    const std::string& in_defaultFileName
) {
    auto          req  = in_task->get_req();
    const string& path = req->get_path();
    string        router_path{req->get_router_path()};
    while (false == router_path.empty() && (router_path.back() == '*' || router_path.back() == '/')
    ) {
        router_path.pop_back();
    }
    if (path.size() > router_path.size()) {
        auto file_path = in_dir_path + string(path.c_str() + router_path.size());
        if (file_path.back() == '/' && in_defaultFileName.empty() == false) {
            file_path += in_defaultFileName;
        }
        return MyHttpFile_c::out_file(in_task, file_path);
    } else
        return false;
}

bool MyHttpFile_c::upload_file(
    MyHttpTask_c*      in_task,
    const std::string& in_path,
    int                max_size,
    int                body_type
) {
    string data, type;
    in_task->get_req()->find_header("Content-Type", type);
    if ((body_type & mimicry::HttpBodyType_e::Form_data_int)
        && type == mimicry::HttpBodyType_e::Form_data) {
        auto&& form = in_task->get_req()->get_form_data();
        if (form.empty() == false) {
            for (auto it = form.begin(); it != form.end(); ++it)
                if (it->second.isFile) {
                    data = std::move(it->second.value);
                }
        }
    }
    if (data.empty() && (body_type & mimicry::HttpBodyType_e::Binary_int)
        && (type == mimicry::HttpBodyType_e::Binary)) {
        data = std::move(in_task->get_req()->get_parsed_body());
    }
    if (data.empty() == false && (max_size < 0 || data.size() <= size_t(max_size))) {
        return MyHttpFile_c::save_file(data, in_path);
    }
    return false;
}

bool MyHttpFile_c::save_file(const string& in_data, const std::string& in_path) {
    bool rebool = false;
    if (mimicry::createFile(in_path)) {
        ofstream ofile{in_path, std::ios::binary};
        if (ofile.is_open()) {
            ofile.write(in_data.c_str(), in_data.size());
            rebool = true;
        }
        ofile.close();
    }
    return rebool;
}

size_t MyHttpFile_c::read_file(
    const std::string& in_path,
    std::string&       re_data,
    size_t             shift,
    int                read_len
) {
    ifstream ifile;
    ifile.open(in_path, std::fstream::binary);
    if (ifile.is_open()) {
        size_t flength = 0, rlen = 0, real_rlen = 0;
        ifile.seekg(0, std::ios::end);
        flength = size_t(ifile.tellg()) - shift; // 计算文件大小
        if (read_len > 0)
            rlen = read_len;
        else
            rlen = flength;
        char* read_str = new char[rlen + 1]{};
        ifile.seekg(shift, std::ios::beg);
        ifile.read(read_str, rlen);
        real_rlen = ifile.gcount();
        ifile.close();
        re_data = string(read_str, real_rlen);
        delete[] read_str;
        return real_rlen;
    }
    return 0;
}

bool mimicry::getFileStat(const std::string& in_path, mimicry::myStat& in_stat) {
    struct stat buf;
    if (stat(in_path.c_str(), &buf) == 0) {
        in_stat.st_size = buf.st_size;
        in_stat.atime   = buf.st_atime;
        in_stat.ctime   = buf.st_ctime;
        in_stat.mtime   = buf.st_mtime;
        return true;
    } else
        return false;
}
