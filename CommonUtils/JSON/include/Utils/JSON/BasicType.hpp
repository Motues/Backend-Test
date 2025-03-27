#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <optional>



namespace Utils :: JSON {

class JsonValue {
public:
    using Array = std::vector<JsonValue>;
    using Object = std::unordered_map<std::string, JsonValue>;
    using String = std::string;
    using Number = double;
    using Boolean = bool;
    using Null = std::monostate;

    // 使用variant存储数据
    std::variant<
        Null ,                  // null
        Boolean ,               // true, false
        Number ,                // 1, 1.0
        String ,                // "river"
        Array,                  // [1, "like"]
        Object                  // { "key": "value" }
    > value;

    // 使用constexpr类型标签
    enum class Type {
        Null, Boolean, Number, String, Array, Object
    };

    // 类型推导构造函数
    template <typename T>
    JsonValue(T&& val) : value(std::forward<T>(val)) {}

    // 默认构造为null
    JsonValue() = default;

    // 类型查询方法
    constexpr Type type() const noexcept {
        return static_cast<Type>(value.index());
    }

    // 类型安全访问（C++17）
    template <typename T>
    std::optional<T> get() const {
        if (const T* p = std::get_if<T>(&value)) {
            return *p;
        }
        return std::nullopt;
    }

    // 链式访问支持
    template <typename T>
    T get_or(T&& default_value) const noexcept {
        return get<T>().value_or(std::forward<T>(default_value));
    }
};

}