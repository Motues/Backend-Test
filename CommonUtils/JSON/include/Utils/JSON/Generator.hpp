#pragma once

#include "BasicType.hpp"
#include <format>
#include <variant>
#include <string>
#include <vector>
#include <map>

namespace Utils::JSON {

class JsonGenerator {
public:
    // 主生成函数
    std::string generate(const JsonValue& value, bool pretty = false, int indent = 0) {
        return std::visit(
            [](const auto& val) -> std::string {
                using T = std::decay_t<decltype(val)>; // 获取类型
                if constexpr (std::is_same_v<T, std::monostate>) {
                    return "null";
                } else if constexpr (std::is_same_v<T, bool>) {
                    return val ? "true" : "false";
                } else if constexpr (std::is_same_v<T, double>) {
                    return format_number(val);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return format_string(val);
                } else if constexpr (std::is_same_v<T, JsonValue::Array>) {
                    return format_array(val, pretty, indent);
                } else if constexpr (std::is_same_v<T, JsonValue::Object>) {
                    return format_object(val, pretty, indent);
                }
            },
            value.value
        );
    }

private:
    // 生成工具函数
    static std::string indent_str(int level) {
        return std::string(level * 4, ' ');
    }

    // 数字格式化（自动区分整数和浮点数）
    static std::string format_number(double num) {
        // 检查是否为整数
        if (num == static_cast<int64_t>(num)) {
            return std::to_string(static_cast<int64_t>(num));
        }

        // 使用charconv保证精度
        std::array<char, 32> buffer;
        auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), num);
        if (result.ec == std::errc()) {
            return std::string(buffer.data(), result.ptr);
        }
        return std::format("{:.16g}", num); // 保底方案
    }

    // 字符串转义处理
    static std::string format_string(const std::string& s) {
        std::string output = "\"";
        for (char c : s) {
            switch (c) {
                case '"':  output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\b': output += "\\b"; break;
                case '\f': output += "\\f"; break;
                case '\n': output += "\\n"; break;
                case '\r': output += "\\r"; break;
                case '\t': output += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        // 控制字符转Unicode
                        output += std::format("\\u{:04x}", static_cast<unsigned char>(c));
                    } else {
                        output += c;
                    }
            }
        }
        return output + "\"";
    }

    // 数组生成（支持格式化）
    std::string format_array(const JsonValue::Array& arr, bool pretty, int indent) {
        if (arr.empty()) return "[]";

        std::string output = "[";
        const std::string newline = pretty ? "\n" : "";
        const std::string space = pretty ? " " : "";
        const std::string current_indent = indent_str(indent);
        const std::string child_indent = pretty ? indent_str(indent + 1) : "";

        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) output += "," + space;

            if (pretty) {
                output += newline + child_indent;
            }

            output += generate(arr[i], pretty, indent + 1);
        }

        if (pretty) {
            output += newline + current_indent;
        }
        return output + "]";
    }

    // 对象生成（支持格式化）
    std::string format_object(const JsonValue::Object& obj, bool pretty, int indent) {
        if (obj.empty()) return "{}";

        std::string output = "{";
        const std::string newline = pretty ? "\n" : "";
        const std::string space = pretty ? " " : "";
        const std::string current_indent = indent_str(indent);
        const std::string child_indent = pretty ? indent_str(indent + 1) : "";

        size_t count = 0;
        for (const auto& [key, value] : obj) {
            if (count++ > 0) output += "," + space;

            if (pretty) {
                output += newline + child_indent;
            }

            output += format_string(key) + ":" + space;
            output += generate(value, pretty, indent + 1);
        }

        if (pretty) {
            output += newline + current_indent;
        }
        return output + "}";
    }
};

}
