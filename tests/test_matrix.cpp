/**
 * @file test_matrix.cpp
 * @brief Модульные тесты библиотеки «Калькулятор матриц» (Google Test).
 *
 * Группы тестов:
 *  1. Нормальные случаи;
 *  2. Граничные случаи;
 *  3. Некорректные случаи;
 *  4. Индивидуальное задание (вариант 6) — matrix_average.
 */
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "matrix.h"

using matrix::create_matrix;
using matrix::free_matrix;
using matrix::Matrix;
using matrix::matrix_add;
using matrix::matrix_average;
using matrix::matrix_from_array;
using matrix::matrix_multiply;
using matrix::matrix_transpose;
using matrix::print_matrix;
using matrix::read_matrix;

// ==================== 1. НОРМАЛЬНЫЕ СЛУЧАИ ====================

// 1.1. Создание матрицы: размеры, выделение памяти, инициализация нулями
TEST(MatrixTest, CreateMatrix_NormalCase) {
    Matrix a = create_matrix(3, 4);
    EXPECT_EQ(a.rows, 3);
    EXPECT_EQ(a.cols, 4);
    ASSERT_NE(a.data, nullptr);
    for (int i = 0; i < 3; ++i) {
        ASSERT_NE(a.data[i], nullptr);
        for (int j = 0; j < 4; ++j) {
            EXPECT_DOUBLE_EQ(a.data[i][j], 0.0);
        }
    }
    free_matrix(a);
}

// 1.2. Сложение матриц
TEST(MatrixTest, Addition_NormalCase) {
    const double a_values[] = {1, 2, 3, 4};
    const double b_values[] = {5, 6, 7, 8};
    Matrix a = matrix_from_array(a_values, 2, 2);
    Matrix b = matrix_from_array(b_values, 2, 2);
    Matrix c = matrix_add(a, b);
    EXPECT_EQ(c.rows, 2);
    EXPECT_EQ(c.cols, 2);
    EXPECT_DOUBLE_EQ(c.data[0][0], 6.0);
    EXPECT_DOUBLE_EQ(c.data[0][1], 8.0);
    EXPECT_DOUBLE_EQ(c.data[1][0], 10.0);
    EXPECT_DOUBLE_EQ(c.data[1][1], 12.0);
    free_matrix(a);
    free_matrix(b);
    free_matrix(c);
}

// 1.3. Умножение матриц
TEST(MatrixTest, Multiplication_NormalCase) {
    const double a_values[] = {1, 2, 3, 4};
    const double b_values[] = {5, 6, 7, 8};
    Matrix a = matrix_from_array(a_values, 2, 2);
    Matrix b = matrix_from_array(b_values, 2, 2);
    Matrix c = matrix_multiply(a, b);
    EXPECT_DOUBLE_EQ(c.data[0][0], 19.0);
    EXPECT_DOUBLE_EQ(c.data[0][1], 22.0);
    EXPECT_DOUBLE_EQ(c.data[1][0], 43.0);
    EXPECT_DOUBLE_EQ(c.data[1][1], 50.0);
    free_matrix(a);
    free_matrix(b);
    free_matrix(c);
}

// 1.4. Транспонирование
TEST(MatrixTest, Transpose_NormalCase) {
    const double values[] = {1, 2, 3, 4, 5, 6};
    Matrix a = matrix_from_array(values, 2, 3);
    Matrix t = matrix_transpose(a);
    EXPECT_EQ(t.rows, 3);
    EXPECT_EQ(t.cols, 2);
    EXPECT_DOUBLE_EQ(t.data[0][0], 1.0);
    EXPECT_DOUBLE_EQ(t.data[0][1], 4.0);
    EXPECT_DOUBLE_EQ(t.data[2][0], 3.0);
    EXPECT_DOUBLE_EQ(t.data[2][1], 6.0);
    free_matrix(a);
    free_matrix(t);
}

// 1.5. Освобождение памяти — структура обнуляется
TEST(MatrixTest, FreeMatrix_NormalCase) {
    Matrix a = create_matrix(3, 3);
    EXPECT_NO_THROW(free_matrix(a));
    EXPECT_EQ(a.data, nullptr);
    EXPECT_EQ(a.rows, 0);
    EXPECT_EQ(a.cols, 0);
}

// 1.6. Создание из массива — заполнение по строкам
TEST(MatrixTest, FromArray_RowMajorOrder) {
    const double values[] = {1, 2, 3, 4, 5, 6};
    Matrix m = matrix_from_array(values, 2, 3);
    EXPECT_DOUBLE_EQ(m.data[0][2], 3.0);
    EXPECT_DOUBLE_EQ(m.data[1][0], 4.0);
    EXPECT_DOUBLE_EQ(m.data[1][2], 6.0);
    free_matrix(m);
}

// 1.7. Вывод матрицы — формат с двумя знаками после запятой
TEST(MatrixTest, PrintMatrix_Format) {
    const double values[] = {1.5, 2.25, 3.75, 4.1};
    Matrix m = matrix_from_array(values, 2, 2);
    std::ostringstream out;
    print_matrix(m, out);
    EXPECT_EQ(out.str(), "[    1.50    2.25 ]\n[    3.75    4.10 ]\n");
    free_matrix(m);
}

// 1.8. Чтение матрицы из потока
TEST(MatrixTest, ReadMatrix_NormalCase) {
    std::istringstream in("2 3\n1 2 3\n4 5 6\n");
    Matrix m = read_matrix(in);
    EXPECT_EQ(m.rows, 2);
    EXPECT_EQ(m.cols, 3);
    EXPECT_DOUBLE_EQ(m.data[1][2], 6.0);
    free_matrix(m);
}

// ==================== 2. ГРАНИЧНЫЕ СЛУЧАИ ====================

// 2.1. Минимальный размер матрицы — 1 x 1
TEST(MatrixTest, CreateMatrix_MinimumSize) {
    Matrix a = create_matrix(1, 1);
    EXPECT_EQ(a.rows, 1);
    EXPECT_EQ(a.cols, 1);
    EXPECT_DOUBLE_EQ(a.data[0][0], 0.0);
    free_matrix(a);
}

// 2.2. Вектор-строка (1x3) x вектор-столбец (3x1) = матрица 1x1 (скалярное произведение)
TEST(MatrixTest, Multiplication_RowTimesColumn_1x1) {
    const double row_values[] = {1, 2, 3};
    const double col_values[] = {4, 5, 6};
    Matrix a = matrix_from_array(row_values, 1, 3);
    Matrix b = matrix_from_array(col_values, 3, 1);
    Matrix c = matrix_multiply(a, b);
    EXPECT_EQ(c.rows, 1);
    EXPECT_EQ(c.cols, 1);
    EXPECT_DOUBLE_EQ(c.data[0][0], 32.0);
    free_matrix(a);
    free_matrix(b);
    free_matrix(c);
}

// 2.3. Двойное транспонирование возвращает исходную матрицу
TEST(MatrixTest, TransposeTwice_ReturnsOriginal) {
    const double values[] = {1, 2, 3, 4, 5, 6};
    Matrix a = matrix_from_array(values, 2, 3);
    Matrix t = matrix_transpose(a);   // 3x2
    Matrix tt = matrix_transpose(t);  // 2x3 — снова как A
    ASSERT_EQ(tt.rows, a.rows);
    ASSERT_EQ(tt.cols, a.cols);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_DOUBLE_EQ(tt.data[i][j], a.data[i][j]);
        }
    }
    free_matrix(a);
    free_matrix(t);
    free_matrix(tt);
}

// 2.4. Вывод пустой матрицы
TEST(MatrixTest, PrintMatrix_EmptyMatrix) {
    const Matrix empty{};
    std::ostringstream out;
    print_matrix(empty, out);
    EXPECT_EQ(out.str(), "[Empty matrix]\n");
}

// 2.5. Сокращённый вывод большой матрицы 15x12
TEST(MatrixTest, PrintMatrix_BigMatrixSummary) {
    Matrix big = create_matrix(15, 12);
    std::ostringstream out;
    print_matrix(big, out);
    const std::string text = out.str();
    EXPECT_NE(text.find("..."), std::string::npos);
    EXPECT_NE(text.find("Matrix 15x12 (showing 10x8)"), std::string::npos);
    free_matrix(big);
}

// 2.6. print_matrix восстанавливает настройки потока
TEST(MatrixTest, PrintMatrix_RestoresStreamFormat) {
    Matrix m = create_matrix(1, 1);
    std::ostringstream out;
    const std::streamsize precision_before = out.precision();
    print_matrix(m, out);
    EXPECT_EQ(out.precision(), precision_before);
    EXPECT_FALSE(out.flags() & std::ios_base::fixed);
    free_matrix(m);
}

// ==================== 3. НЕКОРРЕКТНЫЕ СЛУЧАИ ====================

// 3.1. Создание матрицы с нулевым числом строк
TEST(MatrixTest, CreateMatrix_ZeroRows) {
    EXPECT_THROW(create_matrix(0, 3), std::invalid_argument);
}

// 3.2. Создание матрицы с отрицательным размером
TEST(MatrixTest, CreateMatrix_NegativeCols) {
    EXPECT_THROW(create_matrix(3, -1), std::invalid_argument);
}

// 3.3. Сложение матриц разных размеров
TEST(MatrixTest, Addition_SizeMismatch) {
    Matrix a = create_matrix(2, 2);
    Matrix b = create_matrix(3, 3);
    EXPECT_THROW(matrix_add(a, b), std::invalid_argument);
    free_matrix(a);
    free_matrix(b);
}

// 3.4. Умножение матриц с несовместимыми размерами
TEST(MatrixTest, Multiplication_SizeMismatch) {
    Matrix a = create_matrix(2, 3);
    Matrix b = create_matrix(2, 2);
    EXPECT_THROW(matrix_multiply(a, b), std::invalid_argument);
    free_matrix(a);
    free_matrix(b);
}

// 3.5. Повторное освобождение пустой матрицы безопасно
TEST(MatrixTest, FreeMatrix_EmptyIsSafe) {
    Matrix empty{nullptr, 0, 0};
    EXPECT_NO_THROW(free_matrix(empty));
    EXPECT_NO_THROW(free_matrix(empty));
}

// 3.6. Повторное освобождение настоящей матрицы безопасно (нет double free)
TEST(MatrixTest, FreeMatrix_TwiceIsSafe) {
    Matrix a = create_matrix(2, 2);
    free_matrix(a);
    EXPECT_NO_THROW(free_matrix(a));
}

// 3.7. Операции с пустой матрицей
TEST(MatrixTest, Operations_EmptyMatrixThrows) {
    const Matrix empty{};
    Matrix a = create_matrix(2, 2);
    EXPECT_THROW(matrix_add(empty, a), std::invalid_argument);
    EXPECT_THROW(matrix_multiply(a, empty), std::invalid_argument);
    EXPECT_THROW(matrix_transpose(empty), std::invalid_argument);
    free_matrix(a);
}

// 3.8. Создание из nullptr
TEST(MatrixTest, FromArray_NullptrThrows) {
    EXPECT_THROW(matrix_from_array(nullptr, 2, 2), std::invalid_argument);
}

// 3.9. Ввод букв вместо чисел
TEST(MatrixTest, ReadMatrix_LettersInsteadOfNumbers) {
    std::istringstream bad_size("abc 2");
    EXPECT_THROW(read_matrix(bad_size), std::invalid_argument);
    std::istringstream bad_element("2 2\n1 2\nx 4\n");
    EXPECT_THROW(read_matrix(bad_element), std::invalid_argument);
}

// 3.10. Недостаточно элементов и недопустимые размеры при вводе
TEST(MatrixTest, ReadMatrix_NotEnoughDataAndBadSize) {
    std::istringstream short_input("2 2\n1 2 3\n");
    EXPECT_THROW(read_matrix(short_input), std::invalid_argument);
    std::istringstream negative("-1 3\n");
    EXPECT_THROW(read_matrix(negative), std::invalid_argument);
    std::istringstream too_big("100000 2\n");
    EXPECT_THROW(read_matrix(too_big), std::invalid_argument);
}

// ==================== 4. ВАРИАНТ 6: matrix_average ====================

// 4.1. Типичный случай: (1 + 2 + 3 + 4) / 4 = 2.5
TEST(MatrixAverageTest, NormalCase) {
    const double values[] = {1, 2, 3, 4};
    Matrix m = matrix_from_array(values, 2, 2);
    EXPECT_DOUBLE_EQ(matrix_average(m), 2.5);
    free_matrix(m);
}

// 4.2. Отрицательные и дробные элементы
TEST(MatrixAverageTest, NegativeAndFractionalValues) {
    const double values[] = {-1.5, 2.5, -3.0, 6.0, 0.0, -4.0};
    Matrix m = matrix_from_array(values, 2, 3);
    EXPECT_DOUBLE_EQ(matrix_average(m), 0.0);
    free_matrix(m);
}

// 4.3. Граничный случай: матрица 1x1 — среднее равно единственному элементу
TEST(MatrixAverageTest, SingleElement) {
    const double value[] = {-7.25};
    Matrix m = matrix_from_array(value, 1, 1);
    EXPECT_DOUBLE_EQ(matrix_average(m), -7.25);
    free_matrix(m);
}

// 4.4. Граничный случай: нулевая матрица
TEST(MatrixAverageTest, ZeroMatrix) {
    Matrix m = create_matrix(4, 5);
    EXPECT_DOUBLE_EQ(matrix_average(m), 0.0);
    free_matrix(m);
}

// 4.5. Вектор-строка и вектор-столбец
TEST(MatrixAverageTest, RowAndColumnVectors) {
    const double values[] = {2, 4, 6, 8, 10};
    Matrix row = matrix_from_array(values, 1, 5);
    Matrix col = matrix_from_array(values, 5, 1);
    EXPECT_DOUBLE_EQ(matrix_average(row), 6.0);
    EXPECT_DOUBLE_EQ(matrix_average(col), 6.0);
    free_matrix(row);
    free_matrix(col);
}

// 4.6. Переполнение суммы: элементы около DBL_MAX — среднее остаётся конечным
TEST(MatrixAverageTest, LargeValuesNoOverflow) {
    const double big = std::numeric_limits<double>::max();
    const double values[] = {big, big, big, big};
    Matrix m = matrix_from_array(values, 2, 2);
    const double mean = matrix_average(m);
    EXPECT_TRUE(std::isfinite(mean));
    EXPECT_DOUBLE_EQ(mean, big);
    free_matrix(m);
}

// 4.7. Некорректный случай: пустая матрица
TEST(MatrixAverageTest, EmptyMatrixThrows) {
    const Matrix empty{};
    EXPECT_THROW(matrix_average(empty), std::invalid_argument);
}

// 4.8. Некорректный случай: nullptr при ненулевых размерах
TEST(MatrixAverageTest, NullDataThrows) {
    const Matrix broken{nullptr, 3, 3};
    EXPECT_THROW(matrix_average(broken), std::invalid_argument);
}

// 4.9. Некорректный случай: отрицательные размеры
TEST(MatrixAverageTest, NegativeSizeThrows) {
    Matrix m = create_matrix(2, 2);
    Matrix broken = m;
    broken.rows = -2;
    EXPECT_THROW(matrix_average(broken), std::invalid_argument);
    free_matrix(m);
}

// 4.10. Среднее после операций: среднее(A + B) = среднее(A) + среднее(B)
TEST(MatrixAverageTest, AverageOfSumEqualsSumOfAverages) {
    const double a_values[] = {1, 2, 3, 4, 5, 6};
    const double b_values[] = {10, 20, 30, 40, 50, 60};
    Matrix a = matrix_from_array(a_values, 2, 3);
    Matrix b = matrix_from_array(b_values, 2, 3);
    Matrix c = matrix_add(a, b);
    EXPECT_DOUBLE_EQ(matrix_average(c), matrix_average(a) + matrix_average(b));
    free_matrix(a);
    free_matrix(b);
    free_matrix(c);
}
