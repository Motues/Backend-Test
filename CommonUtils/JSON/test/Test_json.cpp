#include <iostream>
#include <string>
#include "Utils/json.hpp"

int main() {
    std::string jsonString = R"({
        "name": "Alice",
        "age": 25,
        "isStudent": false,
        "courses": ["Math", "Physics"],
        "grades": {"Math": 90, "Physics": 85}
    })";

    try {
        Utils::JSON::Value parsedValue = Utils::JSON::Parser::parse(jsonString);

        // 示例：访问解析后的值
        if (parsedValue.type() == typeid(std::map<std::string, Utils::JSON::Value>)) {
            auto obj = std::any_cast<std::map<std::string, Utils::JSON::Value>>(parsedValue);
            std::cout << "Name: " << std::any_cast<std::string>(obj.at("name")) << std::endl;
            std::cout << "Age: " << std::any_cast<int>(obj.at("age")) << std::endl;
            std::cout << "Is Student: " << std::any_cast<bool>(obj.at("isStudent")) << std::endl;

            auto courses = std::any_cast<std::vector<Utils::JSON::Value>>(obj.at("courses"));
            std::cout << "Courses: ";
            for (const auto& course : courses) {
                std::cout << std::any_cast<std::string>(course) << " ";
            }
            std::cout << std::endl;

            auto grades = std::any_cast<std::map<std::string, Utils::JSON::Value>>(obj.at("grades"));
            std::cout << "Grades: " << std::any_cast<int>(grades.at("Math")) << ", "
                      << std::any_cast<int>(grades.at("Physics")) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
