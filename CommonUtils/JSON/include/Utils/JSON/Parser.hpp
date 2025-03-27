#pragma once

#include <cctype>
#include <charconv>
#include <format>

#include "BasicType.hpp"


namespace Utils :: JSON {

class JsonParser {
public:
    JsonValue parse(std::string_view json) {
        size_t pos = 0;
        auto value = parse_value(json, pos);
        skip_whitespace(json, pos);
        if (pos != json.size()) {
            throw_parse_error("Unexpected trailing content", pos);
        }
        return value;
    }

private:
    // 辅助函数：跳过空白字符
    void skip_whitespace(std::string_view json, size_t& pos) noexcept {
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
    }

    // 核心解析函数
    JsonValue parse_value(std::string_view json, size_t& pos) {
        skip_whitespace(json, pos);
        if (pos >= json.size()) throw_parse_error("Unexpected EOF", pos);

        switch (json[pos]) {
            case '{': return parse_object(json, pos);
            case '[': return parse_array(json, pos);
            case '"': return parse_string(json, pos);
            case 't': case 'f': return parse_bool(json, pos);
            case 'n': return parse_null(json, pos);
            default:
                if (isdigit(json[pos]) || json[pos] == '-') {
                    return parse_number(json, pos);
                }
                throw_parse_error("Unexpected character", pos);
        }
    }

    // 解析对象 {}
    JsonValue parse_object(std::string_view json, size_t& pos) {
        JsonValue::Object obj;
        ++pos; // 跳过 '{'

        while (true) {
            skip_whitespace(json, pos);
            if (pos >= json.size()) throw_parse_error("Unclosed object", pos);

            // 检查闭合括号
            if (json[pos] == '}') {
                ++pos;
                break;
            }

            // 解析键
            std::string key = parse_string(json, pos);

            // 检查冒号
            skip_whitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                throw_parse_error("Expected ':' after key", pos);
            }
            ++pos; // 跳过 ':'

            // 解析值
            JsonValue value = parse_value(json, pos);
            obj.emplace(std::move(key), std::move(value));

            // 处理逗号或闭合
            skip_whitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                ++pos;
            } else if (pos < json.size() && json[pos] == '}') {
                continue; // 交给循环处理闭合
            } else {
                throw_parse_error("Expected ',' or '}' in object", pos);
            }
        }
        return JsonValue(std::move(obj));
    }

    // 解析数组 []
    JsonValue parse_array(std::string_view json, size_t& pos) {
        JsonValue::Array arr;
        ++pos; // 跳过 '['

        while (true) {
            skip_whitespace(json, pos);
            if (pos >= json.size()) throw_parse_error("Unclosed array", pos);

            if (json[pos] == ']') {
                ++pos;
                break;
            }

            // 解析元素
            arr.emplace_back(parse_value(json, pos));

            // 处理逗号或闭合
            skip_whitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                ++pos;
            } else if (pos < json.size() && json[pos] == ']') {
                continue;
            } else {
                throw_parse_error("Expected ',' or ']' in array", pos);
            }
        }
        return JsonValue(std::move(arr));
    }

    // 解析字符串（支持转义字符）
    std::string parse_string(std::string_view json, size_t& pos) {
        std::string str;
        ++pos; // 跳过开头引号

        while (pos < json.size()) {
            char c = json[pos++];

            if (c == '"') { // 闭合引号
                return str;
            }

            if (c == '\\') { // 处理转义字符
                if (pos >= json.size()) throw_parse_error("Unterminated escape", pos);
                c = json[pos++];
                switch (c) {
                    case '"':  str += '"';  break;
                    case '\\': str += '\\'; break;
                    case '/':  str += '/';  break;
                    case 'b':  str += '\b'; break;
                    case 'f':  str += '\f'; break;
                    case 'n':  str += '\n'; break;
                    case 'r':  str += '\r'; break;
                    case 't':  str += '\t'; break;
                    case 'u':  // Unicode处理（简化版）
                        throw_parse_error("Unicode escape not supported", pos-1);
                    default:
                        throw_parse_error("Invalid escape sequence", pos-1);
                }
            } else {
                str += c;
            }
        }

        throw_parse_error("Unclosed string", pos);
    }

    // 解析布尔值
    JsonValue parse_bool(std::string_view json, size_t& pos) {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return JsonValue(true);
        }
        if (json.substr(pos, 5) == "false") {
            pos += 5;
            return JsonValue(false);
        }
        throw_parse_error("Invalid boolean value", pos);
    }

    // 解析null
    JsonValue parse_null(std::string_view json, size_t& pos) {
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return JsonValue(); // null
        }
        throw_parse_error("Invalid null value", pos);
    }

    // 高性能数字解析（C++17 from_chars）
    JsonValue parse_number(std::string_view json, size_t& pos) {
        const size_t start = pos;
        bool is_double = false;

        // 扫描数字字符
        while (pos < json.size()) {
            const char c = json[pos];
            if (isdigit(c) || c == '-' || c == '+' || c == 'e' || c == 'E' || c == '.') {
                if (c == '.' || c == 'e' || c == 'E') is_double = true;
                ++pos;
            } else {
                break;
            }
        }

        // 实际解析
        double num;
        auto result = std::from_chars(
            json.data() + start,
            json.data() + pos,
            num
        );

        if (result.ec == std::errc::invalid_argument) {
            throw_parse_error("Invalid number format", start);
        }

        return JsonValue(num);
    }

    // 错误处理
    [[noreturn]] void throw_parse_error(std::string_view msg, size_t pos) {
        throw std::runtime_error(
            std::string("JSON parse error at position ")
            + std::to_string(pos) + ": " + std::string(msg)
        );
    }
};
}