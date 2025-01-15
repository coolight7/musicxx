#ifndef HTTPMETHOD_H
#define HTTPMETHOD_H
#include <string>

// http请求类型枚举
class MyHttpMethod_e {
public:

    static constexpr int HttpMethod_size = 8, // http请求类型的数量
        Get = 1, Post = 2, Put = 4, Delete = 8, Head = 16, Options = 32, Trace = 64, Connect = 128,
                         Min = MyHttpMethod_e::Get, Max = MyHttpMethod_e::Connect;
};

class MyHttpMethodArrItem_s {
public:

    int         mNum; // 在数组中的下标
    int         mInt; // method的枚举数值
    std::string mStr; // 类型字符串名称

    MyHttpMethodArrItem_s(int in_mNum, int in_mInt, const std::string& in_mStr) {
        mNum = in_mNum;
        mInt = in_mInt;
        mStr = in_mStr;
    }
};

// HTTP请求类型数组，便于循环判断
static const MyHttpMethodArrItem_s myHttpMethodArr[MyHttpMethod_e::HttpMethod_size]
    = {MyHttpMethodArrItem_s(0, MyHttpMethod_e::Get, "GET"),
       MyHttpMethodArrItem_s(1, MyHttpMethod_e::Post, "POST"),
       MyHttpMethodArrItem_s(2, MyHttpMethod_e::Put, "PUT"),
       MyHttpMethodArrItem_s(3, MyHttpMethod_e::Delete, "DELETE"),
       MyHttpMethodArrItem_s(4, MyHttpMethod_e::Head, "HEAD"),
       MyHttpMethodArrItem_s(5, MyHttpMethod_e::Options, "OPTIONS"),
       MyHttpMethodArrItem_s(6, MyHttpMethod_e::Trace, "TRACE"),
       MyHttpMethodArrItem_s(7, MyHttpMethod_e::Connect, "CONNECT")};

// HTTP请求类型的相关操作方法
class MyHttpMethod_c {
public:

    // 判断in_m1与in_m2包含的请求类型是否有交集
    static bool isAllow(int in_m1, int in_m2);
    static bool isAllow(const std::string& in_m1, int in_m2);
    static bool isAllow_strOne(const std::string& in_m1, int in_m2);
    static bool isAllow(const std::string& in_m1, const std::string& in_m2);

    // 将包含请求类型的in_str转为int值表示
    static int strToInt(const std::string& in_str);
    /*将包含请求类型的in_str转为int值表示
     *	但要求in_str只包含了一个请求类型
     *	否则只返回按HttpMethod_e枚举顺序第一个找到的类型对应的int值
     */
    static int strToInt_one(const std::string& in_str);
    // 将使用int值表示的请求类型转为string表示
    static std::string intToStr(int in_int);
    /*将使用int值表示的请求类型转为string表示
     *	但要求in_int只表示一个请求类型
     *	否则只返回按HttpMethod_e枚举顺序第一个找到的类型对应的string
     */
    static std::string intToStr_one(int in_int);
    static int         toIndex_one(int in_method);
    static int         toIndex_one(const std::string& in_method);
};

#endif