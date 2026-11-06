/**
 * @file matrix.cpp
 * @brief Реализация библиотеки «Калькулятор матриц».
 */
#include "matrix.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <istream>
#include <new>
#include <ostream>
#include <stdexcept>
#include <string>

namespace matrix {

namespace {

/// Проверка, что матрица валидна: есть данные, размеры положительны, все строки выделены.
bool is_valid(const Matrix& m) {
    if (m.data == nullptr || m.rows <= 0 || m.cols <= 0) {
        return false;
    }
    for (int i = 0; i < m.rows; ++i) {
        if (m.data[i] == nullptr) {
            return false;
        }
    }
    return true;
}

/// Бросает std::invalid_argument, если матрица невалидна.
void require_valid(const Matrix& m, const char* function_name) {
    if (!is_valid(m)) {
        throw std::invalid_argument(std::string(function_name) + ": matrix is empty or invalid");
    }
}

/// Параметры сокращённого вывода большой матрицы.
struct PrintLayout {
    int head_rows;  ///< Сколько первых строк выводить.
    int tail_rows;  ///< Сколько последних строк выводить.
    int head_cols;  ///< Сколько первых столбцов выводить.
    int tail_cols;  ///< Сколько последних столбцов выводить.
    bool cut_rows;  ///< Пропущены ли строки.
    bool cut_cols;  ///< Пропущены ли столбцы.
};

/// Расчёт, какие строки и столбцы выводить.
PrintLayout make_layout(const Matrix& m) {
    PrintLayout layout{};
    layout.cut_rows = m.rows > kMaxRowsToShow;
    layout.cut_cols = m.cols > kMaxColsToShow;
    layout.head_rows = layout.cut_rows ? kMaxRowsToShow / 2 : m.rows;
    layout.tail_rows = layout.cut_rows ? kMaxRowsToShow / 2 : 0;
    layout.head_cols = layout.cut_cols ? kMaxColsToShow / 2 : m.cols;
    layout.tail_cols = layout.cut_cols ? kMaxColsToShow / 2 : 0;
    return layout;
}

/// Вывод одной строки матрицы с учётом пропуска средних столбцов.
void print_row(std::ostream& out, const double* row, int cols, const PrintLayout& layout) {
    out << '[';
    for (int j = 0; j < layout.head_cols; ++j) {
        out << std::setw(kPrintWidth) << row[j];
    }
    if (layout.cut_cols) {
        out << std::setw(kPrintWidth) << "...";
    }
    for (int j = cols - layout.tail_cols; j < cols; ++j) {
        out << std::setw(kPrintWidth) << row[j];
    }
    out << " ]\n";
}

/// Вывод строки-разделителя «...» на месте пропущенных строк.
void print_gap_row(std::ostream& out, const PrintLayout& layout) {
    const int cells = layout.head_cols + layout.tail_cols + (layout.cut_cols ? 1 : 0);
    out << '[';
    for (int j = 0; j < cells; ++j) {
        out << std::setw(kPrintWidth) << "...";
    }
    out << " ]\n";
}

}  // namespace

Matrix create_matrix(int rows, int cols) {
    // 1. Проверка корректности размеров
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("create_matrix: matrix dimensions must be positive integers");
    }

    Matrix mat;
    mat.rows = rows;
    mat.cols = cols;

    // 2. Выделение памяти для массива указателей на строки (обнулён)
    mat.data = new double*[rows]();

    // 3. Выделение памяти для каждой строки с инициализацией нулями
    try {
        for (int i = 0; i < rows; ++i) {
            mat.data[i] = new double[cols]();
        }
    } catch (const std::bad_alloc&) {
        // Освобождение уже выделенной памяти при ошибке и повторный выброс
        free_matrix(mat);
        throw;
    }
    return mat;
}

void free_matrix(Matrix& m) {
    // 1. Защита от повторного освобождения
    if (m.data == nullptr) {
        m.rows = 0;
        m.cols = 0;
        return;
    }
    // 2. Освобождение каждой строки
    for (int i = 0; i < m.rows; ++i) {
        delete[] m.data[i];
        m.data[i] = nullptr;
    }
    // 3. Освобождение массива указателей
    delete[] m.data;
    // 4. Обнуление структуры (защита от use-after-free)
    m.data = nullptr;
    m.rows = 0;
    m.cols = 0;
}

Matrix matrix_add(Matrix a, Matrix b) {
    require_valid(a, "matrix_add");
    require_valid(b, "matrix_add");
    if (a.rows != b.rows || a.cols != b.cols) {
        throw std::invalid_argument("matrix_add: matrices must have the same dimensions");
    }
    Matrix result = create_matrix(a.rows, a.cols);
    for (int i = 0; i < a.rows; ++i) {
        for (int j = 0; j < a.cols; ++j) {
            result.data[i][j] = a.data[i][j] + b.data[i][j];
        }
    }
    return result;
}

Matrix matrix_multiply(Matrix a, Matrix b) {
    require_valid(a, "matrix_multiply");
    require_valid(b, "matrix_multiply");
    if (a.cols != b.rows) {
        throw std::invalid_argument(
            "matrix_multiply: number of columns of A must equal number of rows of B");
    }
    Matrix result = create_matrix(a.rows, b.cols);  // заполнена нулями
    // Порядок i-k-j: внутренний цикл идёт по строкам result и b последовательно
    for (int i = 0; i < a.rows; ++i) {
        double* result_row = result.data[i];
        for (int k = 0; k < a.cols; ++k) {
            const double a_ik = a.data[i][k];
            const double* b_row = b.data[k];
            for (int j = 0; j < b.cols; ++j) {
                result_row[j] += a_ik * b_row[j];
            }
        }
    }
    return result;
}

Matrix matrix_transpose(Matrix m) {
    require_valid(m, "matrix_transpose");
    Matrix result = create_matrix(m.cols, m.rows);
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            result.data[j][i] = m.data[i][j];
        }
    }
    return result;
}

void print_matrix(Matrix m) {
    print_matrix(m, std::cout);
}

void print_matrix(Matrix m, std::ostream& out) {
    // 1. Проверка валидности матрицы
    if (!is_valid(m)) {
        out << "[Empty matrix]\n";
        return;
    }
    // 2. Настройка формата вывода с сохранением прежних настроек потока
    const std::ios_base::fmtflags old_flags = out.flags();
    const std::streamsize old_precision = out.precision();
    out << std::fixed << std::setprecision(kPrintPrecision);

    // 3. Вывод первых строк, разделителя и последних строк
    const PrintLayout layout = make_layout(m);
    for (int i = 0; i < layout.head_rows; ++i) {
        print_row(out, m.data[i], m.cols, layout);
    }
    if (layout.cut_rows) {
        print_gap_row(out, layout);
        for (int i = m.rows - layout.tail_rows; i < m.rows; ++i) {
            print_row(out, m.data[i], m.cols, layout);
        }
    }
    // 4. Информация о размере, если матрица выведена сокращённо
    if (layout.cut_rows || layout.cut_cols) {
        out << "Matrix " << m.rows << "x" << m.cols << " (showing "
            << layout.head_rows + layout.tail_rows << "x" << layout.head_cols + layout.tail_cols
            << ")\n";
    }
    out.flags(old_flags);
    out.precision(old_precision);
}

Matrix matrix_from_array(const double* data, int rows, int cols) {
    if (data == nullptr) {
        throw std::invalid_argument("matrix_from_array: data is nullptr");
    }
    Matrix m = create_matrix(rows, cols);  // проверяет размеры
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            m.data[i][j] = data[i * cols + j];
        }
    }
    return m;
}

Matrix read_matrix(std::istream& in) {
    int rows = 0;
    int cols = 0;
    if (!(in >> rows >> cols)) {
        throw std::invalid_argument("read_matrix: expected two integers (rows cols)");
    }
    if (rows <= 0 || cols <= 0 || rows > kMaxInputDimension || cols > kMaxInputDimension) {
        throw std::invalid_argument("read_matrix: dimensions must be in range [1, " +
                                    std::to_string(kMaxInputDimension) + "]");
    }
    Matrix m = create_matrix(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!(in >> m.data[i][j])) {
                free_matrix(m);  // не допускаем утечки при ошибке ввода
                throw std::invalid_argument("read_matrix: element [" + std::to_string(i) + "][" +
                                            std::to_string(j) + "] is not a number");
            }
        }
    }
    return m;
}

double matrix_average(Matrix m) {
    // 1. Проверка: пустая матрица / nullptr / неположительные размеры
    require_valid(m, "matrix_average");

    // 2. Количество элементов (в double, чтобы rows * cols не переполнило int)
    const double count = static_cast<double>(m.rows) * static_cast<double>(m.cols);

    // 3. Обычное суммирование
    double sum = 0.0;
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            sum += m.data[i][j];
        }
    }
    if (std::isfinite(sum)) {
        return sum / count;
    }

    // 4. Переполнение суммы: суммируем элементы, заранее делённые на count
    double mean = 0.0;
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            mean += m.data[i][j] / count;
        }
    }
    return mean;
}

}  // namespace matrix
