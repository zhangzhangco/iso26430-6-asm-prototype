#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace testkit {

using TestFn = std::function<void()>;

struct TestCase {
    std::string name;
    TestFn fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(std::string name, TestFn fn) {
        registry().push_back(TestCase {
            .name = std::move(name),
            .fn = std::move(fn),
        });
    }
};

}  // namespace testkit

#define TEST_CASE(name)                                                                 \
    void name();                                                                        \
    static testkit::Registrar name##_registrar(#name, name);                           \
    void name()

#define REQUIRE(condition)                                                              \
    do {                                                                                \
        if (!(condition)) {                                                             \
            throw std::runtime_error(std::string("requirement failed: ") + #condition); \
        }                                                                               \
    } while (false)
