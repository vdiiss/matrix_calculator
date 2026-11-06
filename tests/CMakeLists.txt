include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Не устанавливать gtest вместе с проектом
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(matrix_tests test_matrix.cpp test_app.cpp)
target_link_libraries(matrix_tests PRIVATE matrix_app GTest::gtest_main)
target_compile_options(matrix_tests PRIVATE ${PROJECT_WARNINGS})

# Каждый TEST() регистрируется в CTest как отдельный тест
include(GoogleTest)
gtest_discover_tests(matrix_tests)
