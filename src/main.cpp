/**
 * @file main.cpp
 * @brief Точка входа консольного приложения «Калькулятор матриц».
 *
 * main только определяет режим работы и запускает его; вся логика
 * находится в модулях app и matrix.
 */
#include <iostream>

#include "app.h"

int main(int argc, char* argv[]) {
    const app::Mode mode = app::parse_mode(argc, argv);
    return app::run(mode, std::cin, std::cout, std::cerr);
}
