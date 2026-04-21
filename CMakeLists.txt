cmake_minimum_required(VERSION 3.20)

project(iso26430_6_asm_impl VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ASM_BUILD_TESTS "Build test targets" ON)

find_package(OpenSSL REQUIRED)

add_library(asm_core
    src/asm/codec.cpp
    src/asm/event_log.cpp
    src/asm/le_key.cpp
    src/asm/query_spb.cpp
    src/asm/service.cpp
    src/klv/klv.cpp
    src/security/openssl_init.cpp
    src/security/tls_profile.cpp
    src/security/certificate_utils.cpp
    src/transport/socket.cpp
    src/transport/tls_context.cpp
    src/transport/tls_client.cpp
    src/transport/tls_server.cpp
)

target_include_directories(asm_core PUBLIC include)
target_link_libraries(asm_core PUBLIC OpenSSL::SSL OpenSSL::Crypto)
target_compile_options(asm_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(asm_responder src/apps/asm_responder.cpp)
target_link_libraries(asm_responder PRIVATE asm_core)

add_executable(asm_initiator src/apps/asm_initiator.cpp)
target_link_libraries(asm_initiator PRIVATE asm_core)

if(ASM_BUILD_TESTS)
    enable_testing()

    add_executable(asm_tests
        tests/test_main.cpp
        tests/test_codec.cpp
        tests/test_service.cpp
        tests/test_tls.cpp
    )
    target_link_libraries(asm_tests PRIVATE asm_core)
    target_compile_options(asm_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(asm_tests PRIVATE ASM_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

    add_test(NAME asm_tests COMMAND asm_tests)
endif()
