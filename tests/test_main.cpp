#include <iostream>

#include "testkit.hpp"

int main() {
    int failed = 0;
    for (const auto& test : testkit::registry()) {
        try {
            test.fn();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& ex) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << ": " << ex.what() << '\n';
        }
    }

    if (failed > 0) {
        std::cerr << failed << " test(s) failed\n";
        return 1;
    }

    std::cout << testkit::registry().size() << " test(s) passed\n";
    return 0;
}
