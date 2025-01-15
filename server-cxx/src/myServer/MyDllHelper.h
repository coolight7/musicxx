#pragma once

#include "TgNullable.h"
#include <memory>
#include <string>

class MyDllHelper_c {
protected:
public:

    static std::shared_ptr<MyDllHelper_c> getDllHelper(
        const std::string&  in_path,
        TgNullable_c<int>&& linux_mode = nullptr
    );

    virtual void* getFunction(const std::string& in_name) = 0;
    virtual void  close()                                 = 0;

    virtual ~MyDllHelper_c() {}
};