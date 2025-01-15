#include "MyRouter.h"

#include "MyHttpMethod.h"

using std::map;
using std::string;

bool MyRouter_c::add(const string& in_path, int in_method, const MyHttpProcess_t& in_fun) {
    string&& method = MyHttpMethod_c::intToStr(in_method);
    if (method.empty() == false) { // 检查method
        this->cacheMap.clear();
        const char* strp = in_path.c_str();
        while (*strp == '/')
            ++strp;
        string re_path{};
        auto   treep = this->routerTree.get_child(strp, re_path, true);
        return treep->set_fun_byMethod(in_fun, in_method);
    } else {
        return false;
    }
}

MyRouter_c::RouterTreePort_s*
    MyRouter_c::get_treep_nocache(const std::string& in_path, std::string& re_path) {
    const char* strp = in_path.c_str();
    while (*strp == '/') {
        ++strp;
    }
    auto treep = this->routerTree.get_child(strp, re_path);
    if (false == re_path.empty()) {
        re_path = "/" + re_path;
    }
    return treep;
}

const MyHttpProcess_t&
    MyRouter_c::get_nocache(const std::string& in_path, int in_method, std::string& re_path) {
    auto treep = this->get_treep_nocache(in_path, re_path);
    if (treep != nullptr) { // 判断是否存在路由位置
        const auto& fun = treep->get_fun_byMethod(in_method);
        return fun;
    }
    return Nullptr_MyHttpProcess;
}

const MyHttpProcess_t& MyRouter_c::get(const std::string& in_path, int in_method, string& re_path) {
    auto                          refind  = this->cacheMap.find(in_path);
    MyRouter_c::RouterTreePort_s* treeptr = nullptr;
    if (refind != this->cacheMap.end()) {
        treeptr = refind->second.treeptr;
        re_path = refind->second.router_path;
    } else {
        treeptr = this->get_treep_nocache(in_path, re_path);
    }
    if (treeptr != nullptr) { // 判断是否存在路由位置
        const auto& fun = treeptr->get_fun_byMethod(in_method);
        if (fun) {
            this->cacheMap.insert({
                in_path,
                {re_path, treeptr}
            });
        }
        return fun;
    }
    return Nullptr_MyHttpProcess;
}

MyHttpProcess_t MyRouter_c::remove(const std::string& in_path, int in_method) {
    string&& method = MyHttpMethod_c::intToStr(in_method);
    if (method.empty() == false) { // 检查method
        const char* strp = in_path.c_str();
        while (*strp == '/') {
            ++strp;
        }
        string re_path{};
        auto   treep = this->routerTree.get_child(strp, re_path, false);
        if (treep) {
            this->cacheMap.clear();
            int         index = MyHttpMethod_c::toIndex_one(in_method);
            const auto& fun   = treep->get_fun_byIndex(index);
            treep->set_fun_byIndex(nullptr, index);
            return fun;
        }
    }
    return nullptr;
}

void MyRouter_c::clear_cache() {
    this->cacheMap.clear();
}

void MyRouter_c::clear() {
    this->routerTree.clear_child();
}
