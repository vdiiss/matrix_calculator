/**
 * @file app.cpp
 * @brief Реализация сценариев консольного приложения «Калькулятор матриц».
 */
#include "app.h"

#include <exception>
#include <iostream>
#include <new>
#include <ostream>
#include <stdexcept>
#include <string>

#include "matrix.h"

namespace app {

namespace {

using matrix::Matrix;

/// Число строк матрицы для демонстрации сокращённого вывода.
constexpr int kBigRows = 15;
/// Число столбцов матрицы для демонстрации сокращённого вывода.
constexpr int kBigCols = 12;
/// Очень большое значение для демонстрации защиты от переполнения суммы.
constexpr double kHugeValue = 1.0e308;

/// RAII-обёртка: освобождает матрицу при выходе из области видимости (в т. ч. при исключении).
class ScopedMatrix {
public:
    explicit ScopedMatrix(Matrix m) noexcept : m_(m) {}
    ~ScopedMatrix() {
        matrix::free_matrix(m_);
    }
    ScopedMatrix(const ScopedMatrix&) = delete;
    ScopedMatrix& operator=(const ScopedMatrix&) = delete;

    const Matrix& get() const noexcept {
        return m_;
    }

private:
    Matrix m_;
};

/// Заголовок раздела демонстрации.
void print_header(std::ostream& out, const std::string& title) {
    out << "\n=== " << title << " ===\n";
}

/// Вывод матрицы с подписью и размером.
void print_named(std::ostream& out, const std::string& name, const Matrix& m) {
    out << name << " (" << m.rows << "x" << m.cols << "):\n";
    matrix::print_matrix(m, out);
}

/// Вывод среднего арифметического элементов матрицы (вариант 6).
void print_average(std::ostream& out, const std::string& name, const Matrix& m) {
    const double mean = matrix::matrix_average(m);  // может бросить исключение до вывода
    out << "matrix_average(" << name << ") = " << mean << '\n';
}

/// Сложение и умножение матриц 2x2.
void demo_add_multiply(std::ostream& out) {
    print_header(out, "Базовые операции: сложение и умножение");
    const double a_values[] = {1, 2, 3, 4};
    const double b_values[] = {5, 6, 7, 8};
    ScopedMatrix a(matrix::matrix_from_array(a_values, 2, 2));
    ScopedMatrix b(matrix::matrix_from_array(b_values, 2, 2));
    print_named(out, "A", a.get());
    print_named(out, "B", b.get());

    ScopedMatrix sum(matrix::matrix_add(a.get(), b.get()));
    print_named(out, "A + B", sum.get());

    ScopedMatrix product(matrix::matrix_multiply(a.get(), b.get()));
    print_named(out, "A * B", product.get());
}

/// Транспонирование прямоугольной матрицы.
void demo_transpose(std::ostream& out) {
    print_header(out, "Транспонирование");
    const double values[] = {1, 2, 3, 4, 5, 6};
    ScopedMatrix c(matrix::matrix_from_array(values, 2, 3));
    ScopedMatrix t(matrix::matrix_transpose(c.get()));
    print_named(out, "C", c.get());
    print_named(out, "C^T", t.get());
}

/// Индивидуальное задание, вариант 6: среднее арифметическое элементов.
void demo_average(std::ostream& out) {
    print_header(out, "Вариант 6: среднее арифметическое элементов матрицы");
    const double a_values[] = {1, 2, 3, 4};
    ScopedMatrix a(matrix::matrix_from_array(a_values, 2, 2));
    print_named(out, "A", a.get());
    print_average(out, "A", a.get());

    const double d_values[] = {-4, 0, 4, -2.5, 1.5, 3, 7, -9, 0.5};
    ScopedMatrix d(matrix::matrix_from_array(d_values, 3, 3));
    print_named(out, "D", d.get());
    print_average(out, "D", d.get());

    const double single_value[] = {42};
    ScopedMatrix s(matrix::matrix_from_array(single_value, 1, 1));
    print_named(out, "S", s.get());
    print_average(out, "S", s.get());

    const double huge_values[] = {kHugeValue, kHugeValue, kHugeValue, kHugeValue};
    ScopedMatrix h(matrix::matrix_from_array(huge_values, 2, 2));
    out << "H (2x2) — все элементы равны 1e308, сумма переполняет double\n";
    print_average(out, "H", h.get());

    const Matrix empty{};
    try {
        print_average(out, "empty", empty);
    } catch (const std::invalid_argument& e) {
        out << "matrix_average(empty) -> исключение: " << e.what() << '\n';
    }
}

/// Обработка ошибок: несовместимые размеры и некорректные параметры.
void demo_errors(std::ostream& out) {
    print_header(out, "Обработка ошибок");
    ScopedMatrix a(matrix::create_matrix(2, 2));
    ScopedMatrix b(matrix::create_matrix(3, 3));
    try {
        ScopedMatrix sum(matrix::matrix_add(a.get(), b.get()));
    } catch (const std::invalid_argument& e) {
        out << "matrix_add(2x2, 3x3) -> исключение: " << e.what() << '\n';
    }

    ScopedMatrix c(matrix::create_matrix(2, 3));
    try {
        ScopedMatrix product(matrix::matrix_multiply(c.get(), a.get()));
    } catch (const std::invalid_argument& e) {
        out << "matrix_multiply(2x3, 2x2) -> исключение: " << e.what() << '\n';
    }

    try {
        ScopedMatrix bad(matrix::create_matrix(0, 3));
    } catch (const std::invalid_argument& e) {
        out << "create_matrix(0, 3) -> исключение: " << e.what() << '\n';
    }
}

/// Сокращённый вывод большой матрицы 15x12.
void demo_big_matrix(std::ostream& out) {
    print_header(out, "Сокращённый вывод большой матрицы");
    ScopedMatrix big(matrix::create_matrix(kBigRows, kBigCols));
    for (int i = 0; i < kBigRows; ++i) {
        for (int j = 0; j < kBigCols; ++j) {
            big.get().data[i][j] = static_cast<double>(i * kBigCols + j);
        }
    }
    print_named(out, "Big", big.get());
    print_average(out, "Big", big.get());
}

}  // namespace

Mode parse_mode(int argc, const char* const* argv) {
    if (argc <= 1 || argv == nullptr) {
        return Mode::Demo;
    }
    if (argc > 2 || argv[1] == nullptr) {
        return Mode::Unknown;
    }
    const std::string arg = argv[1];
    if (arg == "--demo") {
        return Mode::Demo;
    }
    if (arg == "--interactive" || arg == "-i") {
        return Mode::Interactive;
    }
    if (arg == "--help" || arg == "-h") {
        return Mode::Help;
    }
    return Mode::Unknown;
}

int run_demo(std::ostream& out) {
    out << "Калькулятор матриц (ЛР3). Демьянов В. М., ИУ5-13/4\n";
    demo_add_multiply(out);
    demo_transpose(out);
    demo_average(out);
    demo_errors(out);
    demo_big_matrix(out);
    return kExitSuccess;
}

int run_interactive(std::istream& in, std::ostream& out, std::ostream& err) {
    out << "Введите размеры матрицы (строки столбцы), затем элементы по строкам:\n";
    try {
        ScopedMatrix m(matrix::read_matrix(in));
        print_named(out, "Матрица", m.get());
        print_average(out, "M", m.get());
        ScopedMatrix t(matrix::matrix_transpose(m.get()));
        print_named(out, "Транспонированная матрица", t.get());
    } catch (const std::invalid_argument& e) {
        err << "Ошибка ввода: " << e.what() << '\n';
        return kExitError;
    }
    return kExitSuccess;
}

void print_usage(std::ostream& out) {
    out << "Использование: matrix_calculator [--demo | --interactive | --help]\n"
           "  --demo, без аргументов  демонстрация всех операций\n"
           "  -i, --interactive       ввод матрицы: rows cols, затем элементы\n"
           "  -h, --help              эта справка\n";
}

int run(Mode mode, std::istream& in, std::ostream& out, std::ostream& err) {
    try {
        switch (mode) {
            case Mode::Demo:
                return run_demo(out);
            case Mode::Interactive:
                return run_interactive(in, out, err);
            case Mode::Help:
                print_usage(out);
                return kExitSuccess;
            case Mode::Unknown:
                break;
        }
        print_usage(err);
        return kExitUsage;
    } catch (const std::bad_alloc&) {
        err << "Ошибка: недостаточно памяти\n";
    } catch (const std::exception& e) {
        err << "Ошибка: " << e.what() << '\n';
    }
    return kExitError;
}

}  // namespace app
