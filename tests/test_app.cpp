/**
 * @file test_app.cpp
 * @brief Интеграционные тесты консольного приложения (модуль app):
 *        разбор аргументов, интерактивный ввод, демонстрационный режим.
 */
#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "app.h"

// Без аргументов — демонстрационный режим
TEST(AppTest, ParseMode_DefaultIsDemo) {
    const char* argv[] = {"matrix_calculator"};
    EXPECT_EQ(app::parse_mode(1, argv), app::Mode::Demo);
}

// Распознавание всех ключей и неизвестного аргумента
TEST(AppTest, ParseMode_Flags) {
    const char* interactive[] = {"matrix_calculator", "--interactive"};
    const char* short_interactive[] = {"matrix_calculator", "-i"};
    const char* help[] = {"matrix_calculator", "--help"};
    const char* unknown[] = {"matrix_calculator", "--abc"};
    const char* too_many[] = {"matrix_calculator", "-i", "-h"};
    EXPECT_EQ(app::parse_mode(2, interactive), app::Mode::Interactive);
    EXPECT_EQ(app::parse_mode(2, short_interactive), app::Mode::Interactive);
    EXPECT_EQ(app::parse_mode(2, help), app::Mode::Help);
    EXPECT_EQ(app::parse_mode(2, unknown), app::Mode::Unknown);
    EXPECT_EQ(app::parse_mode(3, too_many), app::Mode::Unknown);
    EXPECT_EQ(app::parse_mode(0, nullptr), app::Mode::Demo);
}

// Интерактивный режим: корректный ввод
TEST(AppTest, Interactive_ValidInput) {
    std::istringstream in("2 2\n1 2\n3 4\n");
    std::ostringstream out;
    std::ostringstream err;
    EXPECT_EQ(app::run_interactive(in, out, err), app::kExitSuccess);
    EXPECT_NE(out.str().find("matrix_average(M) = 2.5"), std::string::npos);
    EXPECT_TRUE(err.str().empty());
}

// Интерактивный режим: буквы вместо чисел — сообщение об ошибке, без сбоя
TEST(AppTest, Interactive_InvalidInput) {
    std::istringstream in("2 2\n1 two\n3 4\n");
    std::ostringstream out;
    std::ostringstream err;
    EXPECT_EQ(app::run_interactive(in, out, err), app::kExitError);
    EXPECT_NE(err.str().find("is not a number"), std::string::npos);
}

// Демонстрационный режим отрабатывает полностью
TEST(AppTest, Demo_RunsSuccessfully) {
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;
    EXPECT_EQ(app::run(app::Mode::Demo, in, out, err), app::kExitSuccess);
    EXPECT_NE(out.str().find("matrix_average(A) = 2.5"), std::string::npos);
    EXPECT_TRUE(err.str().empty());
}

// Неизвестный аргумент — справка в поток ошибок и код kExitUsage
TEST(AppTest, Run_UnknownModePrintsUsage) {
    std::istringstream in;
    std::ostringstream out;
    std::ostringstream err;
    EXPECT_EQ(app::run(app::Mode::Unknown, in, out, err), app::kExitUsage);
    EXPECT_NE(err.str().find("matrix_calculator"), std::string::npos);
}
