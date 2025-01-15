#include "MyDllHelper.h"
#include "GlobalUtil.h"

#ifdef _isWindows_d
// -- windows --

#include <wtypes.h>

class MyDllHelperWin_c : public MyDllHelper_c {
protected:

    HINSTANCE handle = nullptr;

public:

    MyDllHelperWin_c(HINSTANCE in_handle) {
        handle = in_handle;
    }

    static std::shared_ptr<MyDllHelperWin_c> getDllHelper(
        const std::string&  in_path,
        TgNullable_c<int>&& linux_mode = nullptr
    ) {
        HINSTANCE dll = LoadLibrary(in_path.c_str());
        if (nullptr != dll) {
            return std::make_shared<MyDllHelperWin_c>(dll);
        } else {
            return nullptr;
        }
    }

    virtual void* getFunction(const std::string& in_name) override {
        return GetProcAddress(handle, in_name.c_str());
    }

    virtual void close() override {
        if (nullptr == handle) {
            return;
        }
        FreeLibrary(handle);
        handle = nullptr;
    }

    ~MyDllHelperWin_c() {}
};

#elif _isLinux_d
// -- linux --
#include <dlfcn.h>

class MyDllHelperLinux_c : public MyDllHelper_c {
protected:

    void* handle = nullptr;

public:

    MyDllHelperLinux_c(void* in_handle) {
        handle = in_handle;
    }

    static std::shared_ptr<MyDllHelperLinux_c> getDllHelper(
        const std::string&  in_path,
        TgNullable_c<int>&& linux_mode = nullptr
    ) {
        void* dll = dlopen(in_path.c_str(), RTLD_LAZY);
        if (nullptr != dll) {
            return std::make_shared<MyDllHelperLinux_c>(dll);
        } else {
            return nullptr;
        }
    }

    virtual void* getFunction(const std::string& in_name) override {
        return dlsym(handle, in_name.c_str());
    }

    void close() override {
        if (nullptr == handle) {
            return;
        }
        dlclose(handle);
        handle = nullptr;
    }

    ~MyDllHelperLinux_c() {}
};
#endif

/// -- MyDllHelper_c --
std::shared_ptr<MyDllHelper_c> MyDllHelper_c::getDllHelper(
    const std::string&  in_path,
    TgNullable_c<int>&& linux_mode
) {
#ifdef WIN32
    // windows
    return MyDllHelperWin_c::getDllHelper(
        in_path,
        std::forward<TgNullable_c<int>>(linux_mode)
    );
#elif _isLinux_d
    // linux
    return MyDllHelperLinux_c::getDllHelper(
        in_path,
        std::forward<TgNullable_c<int>>(linux_mode)
    );
#endif
}
