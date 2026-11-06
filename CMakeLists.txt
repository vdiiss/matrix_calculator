cmake_minimum_required(VERSION 3.20)
project(Matrix_Calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(BUILD_TESTS "Build unit tests (Google Test)" ON)

# Предупреждения как ошибки для всего кода проекта (начиная с 3-й работы)
if(MSVC)
    set(PROJECT_WARNINGS /W4 /WX)
else()
    set(PROJECT_WARNINGS -Wall -Wextra -Wpedantic -Werror)
endif()

# ---- Библиотека «Калькулятор матриц» ----
add_library(matrix STATIC src/matrix.cpp)
target_include_directories(matrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(matrix PRIVATE ${PROJECT_WARNINGS})

# ---- Сценарии приложения (демо, интерактивный режим) ----
add_library(matrix_app STATIC src/app.cpp)
target_link_libraries(matrix_app PUBLIC matrix)
target_compile_options(matrix_app PRIVATE ${PROJECT_WARNINGS})

# ---- Консольное приложение ----
add_executable(matrix_calculator src/main.cpp)
target_link_libraries(matrix_calculator PRIVATE matrix_app)
target_compile_options(matrix_calculator PRIVATE ${PROJECT_WARNINGS})

# ---- Тесты ----
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
