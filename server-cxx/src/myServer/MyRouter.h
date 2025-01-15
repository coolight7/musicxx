#ifndef MYROUTER_H
#define MYROUTER_H

#include <map>
#include <string>

#include "MyHttpMethod.h"
#include "MyHttpTask.h"

// 路由
class MyRouter_c {
protected:

    // 路由 树节点
    struct RouterTreePort_s {
    protected:
    public:

        /// <summary>
        ///  路径
        /// </summary>
        std::string path;
        /// <summary>
        /// 对应Http方法的处理函数
        /// </summary>
        MyHttpProcess_t fun[MyHttpMethod_e::HttpMethod_size];
        /// <summary>
        /// 子节点
        /// </summary>
        std::map<std::string, RouterTreePort_s*> child;

        RouterTreePort_s(const std::string& in_path = "") noexcept :
            path(in_path) {
            for (int i = MyHttpMethod_e::HttpMethod_size; i-- > 0;)
                fun[i] = nullptr;
        }

        // 返回对应路径节点，不存在则返回nullptr
        RouterTreePort_s*
            get_child(const std::string& in_path, std::string& re_path, bool do_add = false) {
            return this->get_child(in_path.c_str(), re_path, do_add);
        }

        /// <summary>
        /// 获取对应路径[in_path]的节点，并返回路由路径到[re_path]
        /// </summary>
        /// <param name="in_path">待查找路径</param>
        /// <param name="re_path">节点真实路由路径；无对应节点时返回空</param>
        /// <param name="do_add">当路径对应节点不存在时，是否添加节点</param>
        /// <returns>对应路径节点，不存在则返回nullptr</returns>
        RouterTreePort_s*
            get_child(const char* in_path, std::string& re_path, bool do_add = false) {
            const char *strptr = in_path, *nextptr = in_path;
            for (;;) {
                if (*nextptr == '/') {
                    if (*(nextptr + 1) == '/') {
                        ++nextptr;
                    } else {
                        break;
                    }
                } else if (*nextptr != '\0') {
                    ++strptr;
                    ++nextptr;
                } else { // strp = '\0'，是最后一个子节点
                    auto it = child.find(in_path);
                    if (it != child.end()) { // 存在子节点
                        re_path = in_path;
                        return it->second;
                    } else {
                        if (do_add) {
                            auto treeptr   = new RouterTreePort_s(in_path);
                            child[in_path] = treeptr;
                            re_path        = in_path;
                            return treeptr;
                        } else {
                            it = child.find("*");
                            if (it != child.end()) {
                                re_path = "*";
                                return it->second;
                            } else {
                                re_path.clear();
                                return nullptr;
                            }
                        }
                    }
                }
            }
            // *strp = '/'，不是最后一个子节点
            std::string str{in_path, strptr};
            ++nextptr;
            auto it = child.find(str);
            if (it != child.end()) { // 如果存在子节点
                auto re_ptr = it->second->get_child(nextptr, re_path, do_add);
                if (re_ptr != nullptr) { // 有找到匹配的路径
                    re_path = str + "/" + re_path;
                }
                return re_ptr;
            } else {
                if (do_add) { // 如果需要新建子节点
                    auto treeptr = new RouterTreePort_s(str);
                    child[str]   = treeptr;
                    auto re_ptr  = treeptr->get_child(nextptr, re_path, do_add);
                    if (re_ptr != nullptr) {
                        re_path = str + "/" + re_path;
                    }
                    return re_ptr;
                } else { // 查找是否有通配符
                    it = child.find("*");
                    if (it != child.end()) {
                        re_path = "*";
                        return it->second;
                    } else { // 无查找结果，清空re_path并返回nullptr
                        re_path.clear();
                        return nullptr;
                    }
                }
            }
        }

        /* 使用类型枚举设置处理函数
         * 支持同时设置多个类型
         */
        bool set_fun_byMethod(const MyHttpProcess_t& in_fun, int in_method) {
            size_t donum = 0; // 记录设置函数的次数
            for (int i = 0; i < MyHttpMethod_e::HttpMethod_size; ++i) {
                if (MyHttpMethod_c::isAllow(in_method, myHttpMethodArr[i].mInt)) {
                    fun[myHttpMethodArr[i].mNum] = in_fun;
                    ++donum;
                }
            }
            if (donum > 0) {
                return true;
            } else {
                return false;
            }
        }

        /* 使用类型下标设置处理函数
         * 仅支持一个类型下标
         */
        bool set_fun_byIndex(const MyHttpProcess_t& in_fun, int in_index) {
            if (in_index >= 0 && in_index < MyHttpMethod_e::HttpMethod_size) {
                fun[in_index] = in_fun;
                return true;
            } else {
                return false;
            }
        }

        /* 用数组下标获取函数
         * 下表越界时返回nullptr
         * 指定下标的函数未定义时返回nullptr
         */
        const MyHttpProcess_t& get_fun_byIndex(int in_index) const {
            if (in_index >= 0 && in_index < MyHttpMethod_e::HttpMethod_size) {
                return fun[in_index];
            } else {
                return Nullptr_MyHttpProcess;
            }
        }

        /* 用请求类型获取函数
         * 指定请求类型对应的函数未定义时返回nullptr
         */
        const MyHttpProcess_t& get_fun_byMethod(int in_method) const {
            return this->get_fun_byIndex(MyHttpMethod_c::toIndex_one(in_method));
        }

        /* 清空子节点
         */
        void clear_child() {
            for (auto it = child.begin(); it != child.end();) {
                it->second->clear_child(); // 调用子节点的清理
                delete (it->second);       // 释放子节点
                child.erase(it);
                it = child.begin(); // 重置it的指向
            }
        }

        ~RouterTreePort_s() {
            this->clear_child();
        }
    };

    struct RouterCacheValue_s {
        /// <summary>
        /// 路由路径
        /// </summary>
        std::string router_path;
        /// <summary>
        /// 对应节点
        /// </summary>
        RouterTreePort_s* treeptr = nullptr;
    };

    // 路由字典树
    RouterTreePort_s routerTree;
    /* 哈希缓存
     * 如果使用缓存接口获取路由位置，获取成功将留下缓存
     * 下次使用相同的path获取时直接提取缓存
     * 添加或删除路由位置将清空缓存重新生成
     */
    std::unordered_map<std::string, RouterCacheValue_s> cacheMap;
    /// <summary>
    /// 不经过缓存，直接搜索对应路径[in_path]、对应Http方法[in_method]的节点
    /// </summary>
    /// <param name="in_path">待查找路径</param>
    /// <param
    /// name="re_path">返回该节点的真实路由路径；节点不存在时返回空</param>
    /// <returns>返回查找结果节点</returns>
    RouterTreePort_s* get_treep_nocache(const std::string& in_path, std::string& re_path);

public:

    MyRouter_c() noexcept :
        routerTree("/") {}

    /* 添加路由
     * 允许使用通配符 *
     * 允许同时设置多个类型枚举
     * 路径中连续的 / 将被视为仅一个 /
     * 即 /a//b///c 等同于 /a/b/c
     */
    bool add(const std::string& in_path, int in_method, const MyHttpProcess_t& in_fun);
    /* 判断是否存在路由位置，使用缓存
     * 有，且对应方法有处理函数：返回其处理函数指针，并赋值re_path
     * 有，但对应方法没有处理函数：返回nullptr，并赋值re_path
     * 没有:	返回nullptr，re_path置空
     */
    const MyHttpProcess_t& get(const std::string& in_path, int in_method, std::string& re_path);
    /* 判断是否存在路由位置，不使用缓存
     * 有，且对应方法有处理函数：返回其处理函数指针，并赋值re_path
     * 有，但对应方法没有处理函数：返回nullptr，并赋值re_path
     * 没有:	返回nullptr，re_path置空
     */
    const MyHttpProcess_t&
        get_nocache(const std::string& in_path, int in_method, std::string& re_path);
    /*移除指定的路由位置，并返回其处理函数
     * \param in_method_one 一次调用仅支持移除单一请求类型对应的处理函数
     */
    MyHttpProcess_t remove(const std::string& in_path, int in_method_one);
    /* 清理缓存
     */
    void clear_cache();
    // 清空路由
    void clear();
};

#endif // ! MYROUTER_H
