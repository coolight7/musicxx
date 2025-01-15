#include "MyHttpMethod.h"
#include <algorithm>
#include <cctype>
#include <cmath>


using std::string;

bool MyHttpMethod_c::isAllow(int in_m1, int in_m2) {
    return in_m1 & in_m2;
}

bool MyHttpMethod_c::isAllow(const string& in_m1, int in_m2) {
    return MyHttpMethod_c::strToInt(in_m1) & in_m2;
}

bool MyHttpMethod_c::isAllow_strOne(const string& in_m1, int in_m2) {
    return MyHttpMethod_c::strToInt_one(in_m1) & in_m2;
}

bool MyHttpMethod_c::isAllow(const string& in_m1, const string& in_m2) {
    if (in_m1.size() >= in_m2.size())
        return in_m1.find(in_m2) != string::npos;
    else
        return in_m2.find(in_m1) != string::npos;
}

int MyHttpMethod_c::strToInt(const string& in_str) {
    string upStr{in_str};
    // ×ª»»×ÖÄ¸Îª´óÐ´
    std::transform(upStr.begin(), upStr.end(), upStr.begin(), ::toupper);
    int reInt = 0;
    for (int i = MyHttpMethod_e::HttpMethod_size; i-- > 0;) {
        if (upStr.find(myHttpMethodArr[i].mStr) != string::npos) {
            reInt = reInt | myHttpMethodArr[i].mInt;
        }
    }
    return reInt;
}

int MyHttpMethod_c::strToInt_one(const string& in_str) {
    string upStr{in_str};
    std::transform(upStr.begin(), upStr.end(), upStr.begin(), ::toupper);
    for (int i = 0; i < MyHttpMethod_e::HttpMethod_size; ++i) {
        if (upStr == myHttpMethodArr[i].mStr) {
            return myHttpMethodArr[i].mInt;
        }
    }
    return 0;
}

string MyHttpMethod_c::intToStr(int in_int) {
    string reStr;
    for (int i = MyHttpMethod_e::HttpMethod_size; i-- > 0;) {
        if (in_int & myHttpMethodArr[i].mInt) {
            reStr += myHttpMethodArr[i].mStr + " ";
        }
    }
    return reStr;
}

string MyHttpMethod_c::intToStr_one(int in_int) {
    for (int i = 0; i < MyHttpMethod_e::HttpMethod_size; ++i) {
        if (in_int & myHttpMethodArr[i].mInt) {
            return myHttpMethodArr[i].mStr;
        }
    }
    return "";
}

int MyHttpMethod_c::toIndex_one(int in_method) {
    int reNum = int(log2(in_method));
    if (reNum >= 0 && reNum < MyHttpMethod_e::HttpMethod_size)
        return reNum;
    else
        return -1;
}

int MyHttpMethod_c::toIndex_one(const string& in_method) {
    string upStr{in_method};
    std::transform(upStr.begin(), upStr.end(), upStr.begin(), ::toupper);
    for (int i = 0; i < MyHttpMethod_e::HttpMethod_size; ++i) {
        if (upStr == myHttpMethodArr[i].mStr) {
            return i;
        }
    }
    return -1;
}
