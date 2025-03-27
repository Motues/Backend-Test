#include <iostream>
#include <string>
#include "Utils/json.hpp"

using namespace Utils::JSON;
int main() {
    // 构造JSON
    std::string data = R"({
        "name": "John",
        "age": 30,
        "scores": [95.5, 88.0, null],
    })";

    // 解析JSON
    JsonParser parser;
    JsonValue parsed = parser.parse(data);


    // 访问scores数组
    if (auto scores = parsed.get<std::vector<JsonValue>>()) {
        std::cout << "Scores: ";
        for (const auto& score : *scores) {
            if (auto num = score.get<double>()) {
                std::cout << *num << " ";
            } else if (score.get<std::monostate>()) {
                std::cout << "null ";
            }
        }
        std::cout << std::endl;
    }

    // 访问metadata对象
    if (auto metadata = parsed.get<JsonValue::Object>()) {
        std::cout << "Metadata:" << std::endl;
        for (const auto& [key, value] : *metadata) {
            std::cout << "  " << key << ": ";
            if (auto str = value.get<JsonValue::String>()) {
                std::cout << *str;
            } else if (auto num = value.get<JsonValue::Number>()) {
                std::cout << *num;
            } else if (auto bool_val = value.get<JsonValue::Boolean>()) {
                std::cout << (*bool_val ? "true" : "false");
            } else if (auto arr = value.get<JsonValue::Array>()) {
                std::cout << "[";
                for (const auto& item : *arr) {
                    if (auto item_str = item.get<JsonValue::Number>()) {
                        std::cout << *item_str << " ";
                    }
                }
                std::cout << "]";
            }
            std::cout << std::endl;
        }
    }

}
