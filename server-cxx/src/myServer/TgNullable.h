#pragma once

#include <map>
#include <memory>
#include <mutex>

#define _ifNotNull_d(ptr, code) \
    if (ptr != nullptr) {       \
        code                    \
    }

#define _ifNotNullElse_d(ptr, notNullDoCode, nullDoCode) \
    if (ptr != nullptr) {                                \
        notNullDoCode                                    \
    } else {                                             \
        nullDoCode                                       \
    }

template<typename T = int>
class TgNullable_c {
protected:

    std::unique_ptr<T> _pointer;

public:

    T* valuePtr = nullptr;

    TgNullable_c() {}

    TgNullable_c(T* in_value) :
        valuePtr(in_value) {}

    TgNullable_c(std::unique_ptr<T>&& in_value) :
        _pointer(std::move<T>(in_value)) {}

    TgNullable_c(T&& in_value) :
        _pointer(std::make_unique<T>(std::forward<T>(in_value))) {}

    TgNullable_c(const T& in_value) :
        _pointer(std::make_unique<T>(in_value)) {}

    TgNullable_c(TgNullable_c<T>&& other) {
        *this = std::move(other);
    }

    TgNullable_c(const TgNullable_c& other) = delete;

    bool isNull() {
        return (nullptr == get());
    }

    bool isNotNull() {
        return (false == isNull());
    }

    T* get() {
        if (nullptr != valuePtr) {
            return valuePtr;
        } else {
            return _pointer.get();
        }
    }

    /// 调用前需要自行保证非null
    T getValue() {
        assert(nullptr != get());
        return *get();
    }

    T getValue(T&& temp) {
        auto it = get();
        if (nullptr == it) {
            return temp;
        } else {
            return *it;
        }
    }

    void setValue(T&& in_data) {
        auto it = get();
        if (nullptr != it) {
            *it = std::forward<T>(in_data);
        } else {
            valuePtr = nullptr;
            _pointer = std::make_unique<T>(std::forward<T>(in_data));
        }
    }

    void setValue(const T& in_data) {
        auto it = get();
        if (nullptr != it) {
            *it = in_data;
        } else {
            valuePtr = nullptr;
            _pointer = std::make_unique<T>(in_data);
        }
    }

    TgNullable_c<T>& operator=(TgNullable_c<T>&& other) {
        this->valuePtr = other.valuePtr;
        this->_pointer = std::move(other._pointer);
        other.valuePtr = nullptr;
        return *this;
    }

    TgNullable_c<T>& operator=(T&& in_data) {
        setValue(std::forward<T>(in_data));
        return *this;
    }

    TgNullable_c<T>& operator=(T* in_data) {
        valuePtr = in_data;
        _pointer = nullptr;
        return *this;
    }

    TgNullable_c<T>& operator=(std::unique_ptr<T>&& in_data) {
        valuePtr = nullptr;
        _pointer = std::move(in_data);
        return *this;
    }

    template<typename _PointerType = int>
    friend bool operator==(TgNullable_c<T> nullablePtr, _PointerType* ptr) {
        if (nullptr != nullablePtr.valuePtr) {
            return (ptr == nullablePtr.valuePtr);
        } else {
            return (ptr == nullablePtr._pointer.get());
        }
    }

    friend bool operator==(TgNullable_c<T> nullablePtr, std::nullptr_t ptr) {
        return operator==(nullablePtr, (int*)ptr);
    }
};

template<typename _KeyType, typename _ValueType>
class TgMap_c : public std::map<_KeyType, TgNullable_c<_ValueType>> {
protected:

    mutable std::mutex _mutex;

public:

    using _BaseMapType = std::map<_KeyType, TgNullable_c<_ValueType>>;

    void insert(const _KeyType& key, const _ValueType& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        this->_BaseMapType::insert(std::pair{key, value});
    }

    // 删除键值对
    template<typename _Type>
    void erase(const _Type& key) {
        std::lock_guard<std::mutex> lock(_mutex);
        this->_BaseMapType::erase(key);
    }

    void remove(const _KeyType& key) {
        std::lock_guard<std::mutex> lock(_mutex);
        auto                        it = this->find(key);
        if (it != this->end()) {
            this->_BaseMapType::erase(it);
        }
    }

    // 检查是否包含键
    bool contains(const _KeyType& key) {
        std::lock_guard<std::mutex> lock(_mutex);
        return this->_BaseMapType::contains(key);
    }
};