/**
 * @file app.h
 * @brief Сценарии консольного приложения «Калькулятор матриц»:
 *        демонстрация, интерактивный режим, справка.
 * @author Демьянов В. М., группа ИУ5-13/4
 */
#pragma once

#include <iosfwd>

namespace app {

/// Код возврата при успешном завершении.
inline constexpr int kExitSuccess = 0;
/// Код возврата при ошибке во входных данных или вычислениях.
inline constexpr int kExitError = 1;
/// Код возврата при неверных аргументах командной строки.
inline constexpr int kExitUsage = 2;

/// Режим работы программы.
enum class Mode {
    Demo,         ///< Демонстрация всех операций (по умолчанию).
    Interactive,  ///< Ввод матрицы с клавиатуры (--interactive, -i).
    Help,         ///< Справка (--help, -h).
    Unknown       ///< Неизвестный аргумент.
};

/**
 * @brief Определение режима работы по аргументам командной строки.
 * @param argc Количество аргументов.
 * @param argv Массив аргументов (может быть nullptr при argc <= 1).
 * @return Mode Выбранный режим; Unknown для неизвестных или лишних аргументов.
 */
Mode parse_mode(int argc, const char* const* argv);

/**
 * @brief Демонстрация базовых операций и индивидуального задания (вариант 6).
 * @param out Поток вывода.
 * @return int kExitSuccess.
 * @exception std::bad_alloc Если не удалось выделить память.
 */
int run_demo(std::ostream& out);

/**
 * @brief Интерактивный режим: чтение матрицы, вывод её, среднего и транспонированной.
 * @param in Поток ввода (формат: rows cols, затем элементы по строкам).
 * @param out Поток для результатов.
 * @param err Поток для сообщений об ошибках.
 * @return int kExitSuccess или kExitError при некорректном вводе.
 */
int run_interactive(std::istream& in, std::ostream& out, std::ostream& err);

/**
 * @brief Вывод справки по запуску программы.
 * @param out Поток вывода.
 */
void print_usage(std::ostream& out);

/**
 * @brief Запуск программы в заданном режиме с перехватом всех исключений.
 * @param mode Режим работы.
 * @param in Поток ввода.
 * @param out Поток вывода.
 * @param err Поток ошибок.
 * @return int Код возврата программы.
 */
int run(Mode mode, std::istream& in, std::ostream& out, std::ostream& err);

}  // namespace app
