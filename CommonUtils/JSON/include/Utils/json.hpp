#pragma once

#include <string>
#include <map>
#include <vector>
#include <any>
#include <sstream>
#include <iomanip>

namespace Utils :: JSON {
    using Value = std::any;

    class Parser {
    public:
        static Value parse(const std::string& json) {
            std::istringstream iss(json);
            return parseValue(iss);
        }

    private:
        static Value parseValue(std::istringstream& iss) {
            skipWhitespace(iss);
            char ch = iss.peek();
            if (ch == '"') return parseString(iss);
            if (ch == '{') return parseObject(iss);
            if (ch == '[') return parseArray(iss);
            if (ch == 't' || ch == 'f') return parseBool(iss);
            if (ch == 'n') return parseNull(iss);
            return parseNumber(iss);
        }

        static void skipWhitespace(std::istringstream& iss) {
            while (isspace(iss.peek())) iss.get();
        }

        static std::string parseString(std::istringstream& iss) {
            std::string str;
            iss.get(); // consume the opening quote
            while (iss.peek() != '"') {
                char ch = iss.get();
                if (ch == '\\') {
                    ch = iss.get();
                    switch (ch) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case 'r': str += '\r'; break;
                        case 'b': str += '\b'; break;
                        case 'f': str += '\f'; break;
                        case '"': str += '"'; break;
                        case '\\': str += '\\'; break;
                        default: str += ch; break;
                    }
                } else {
                    str += ch;
                }
            }
            iss.get(); // consume the closing quote
            return str;
        }

        static std::map<std::string, Value> parseObject(std::istringstream& iss) {
            std::map<std::string, Value> obj;
            iss.get(); // consume the opening brace
            skipWhitespace(iss);
            while (iss.peek() != '}') {
                std::string key = parseString(iss);
                skipWhitespace(iss);
                iss.get(); // consume the colon
                skipWhitespace(iss);
                Value value = parseValue(iss);
                obj[key] = value;
                skipWhitespace(iss);
                if (iss.peek() == ',') {
                    iss.get(); // consume the comma
                    skipWhitespace(iss);
                }
            }
            iss.get(); // consume the closing brace
            return obj;
        }

        static std::vector<Value> parseArray(std::istringstream& iss) {
            std::vector<Value> arr;
            iss.get(); // consume the opening bracket
            skipWhitespace(iss);
            while (iss.peek() != ']') {
                Value value = parseValue(iss);
                arr.push_back(value);
                skipWhitespace(iss);
                if (iss.peek() == ',') {
                    iss.get(); // consume the comma
                    skipWhitespace(iss);
                }
            }
            iss.get(); // consume the closing bracket
            return arr;
        }

        static bool parseBool(std::istringstream& iss) {
            std::string str;
            iss >> str;

            if (str == "true,") return true;
            if (str == "false,") return false;

            // 提供更详细的错误信息
            throw std::runtime_error("Invalid boolean value: \"" + str + "\"");
        }


        static std::nullptr_t parseNull(std::istringstream& iss) {
            std::string str;
            iss >> str;
            if (str == "null") return nullptr;
            throw std::runtime_error("Invalid null value");
        }

        static Value parseNumber(std::istringstream& iss) {
            std::string str;
            iss >> str;
            try {
                size_t pos;
                int intValue = std::stoi(str, &pos);
                if (pos == str.size()) return intValue;
            } catch (...) {}
            try {
                return std::stod(str);
            } catch (...) {
                throw std::runtime_error("Invalid number value");
            }
        }
    };

    class Generator {
    public:
        static std::string generate(const Value& value) {
            std::ostringstream oss;
            generateValue(value, oss);
            return oss.str();
        }

    private:
        static void generateValue(const Value& value, std::ostringstream& oss) {
            if (value.type() == typeid(std::nullptr_t)) {
                oss << "null";
            } else if (value.type() == typeid(bool)) {
                oss << std::boolalpha << std::any_cast<bool>(value);
            } else if (value.type() == typeid(int)) {
                oss << std::any_cast<int>(value);
            } else if (value.type() == typeid(double)) {
                oss << std::any_cast<double>(value);
            } else if (value.type() == typeid(std::string)) {
                oss << '"' << escapeString(std::any_cast<std::string>(value)) << '"';
            } else if (value.type() == typeid(std::map<std::string, Value>)) {
                oss << '{';
                bool first = true;
                for (const auto& [key, val] : std::any_cast<std::map<std::string, Value>>(value)) {
                    if (!first) oss << ", ";
                    first = false;
                    generateValueImpl(key, oss);
                    oss << ": ";
                    generateValue(val, oss);
                }
                oss << '}';
            } else if (value.type() == typeid(std::vector<Value>)) {
                oss << '[';
                bool first = true;
                for (const auto& val : std::any_cast<std::vector<Value>>(value)) {
                    if (!first) oss << ", ";
                    first = false;
                    generateValue(val, oss);
                }
                oss << ']';
            } else {
                throw std::runtime_error("Unsupported type");
            }
        }

        static void generateValueImpl(const std::string& value, std::ostringstream& oss) {
            oss << '"' << escapeString(value) << '"';
        }

        static std::string escapeString(const std::string& str) {
            std::ostringstream oss;
            for (char ch : str) {
                switch (ch) {
                    case '\n': oss << "\\n"; break;
                    case '\t': oss << "\\t"; break;
                    case '\r': oss << "\\r"; break;
                    case '\b': oss << "\\b"; break;
                    case '\f': oss << "\\f"; break;
                    case '"': oss << "\\\""; break;
                    case '\\': oss << "\\\\"; break;
                    default: oss << ch; break;
                }
            }
            return oss.str();
        }
    };
}