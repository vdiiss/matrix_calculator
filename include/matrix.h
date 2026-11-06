/**
 * @file matrix.h
 * @brief Библиотека «Калькулятор матриц»: создание и освобождение матриц,
 *        базовые операции и индивидуальное задание (вариант 6).
 * @author Демьянов В. М., группа ИУ5-13/4
 * @version 1.0
 *
 * Способ обработки ошибок — исключения:
 *  - std::invalid_argument — некорректные размеры, несовместимые или пустые
 *    матрицы, nullptr вместо данных, некорректный ввод;
 *  - std::bad_alloc — не удалось выделить память.
 */
#pragma once

#include <iosfwd>

namespace matrix {

/// Максимальное число строк, которое print_matrix() выводит без сокращения.
inline constexpr int kMaxRowsToShow = 10;
/// Максимальное число столбцов, которое print_matrix() выводит без сокращения.
inline constexpr int kMaxColsToShow = 8;
/// Количество знаков после запятой при выводе элементов.
inline constexpr int kPrintPrecision = 2;
/// Ширина поля под один элемент при выводе.
inline constexpr int kPrintWidth = 8;
/// Максимальный размер (строк или столбцов) матрицы, читаемой read_matrix().
inline constexpr int kMaxInputDimension = 1000;

/**
 * @brief Структура матрицы.
 *
 * Хранит двумерный динамический массив и его размеры.
 * Пустая (невалидная) матрица: data == nullptr, rows == 0, cols == 0.
 * @warning Память освобождается через free_matrix().
 */
struct Matrix {
    double** data = nullptr;  ///< Указатель на двумерный массив (массив строк).
    int rows = 0;  ///< Количество строк.
    int cols = 0;  ///< Количество столбцов.
};

/**
 * @brief Создание матрицы заданного размера.
 * @param rows Количество строк (> 0).
 * @param cols Количество столбцов (> 0).
 * @return Matrix Матрица rows x cols, инициализированная нулями.
 * @exception std::invalid_argument Если rows <= 0 или cols <= 0.
 * @exception std::bad_alloc Если не удалось выделить память
 *            (уже выделенная память при этом освобождается).
 * @note Время выполнения: O(rows * cols).
 * @see free_matrix()
 * @code
 * matrix::Matrix a = matrix::create_matrix(3, 3);
 * matrix::free_matrix(a);
 * @endcode
 */
Matrix create_matrix(int rows, int cols);

/**
 * @brief Освобождение памяти, занятой матрицей.
 *
 * Освобождает каждую строку и массив указателей, затем обнуляет структуру
 * (data = nullptr, rows = cols = 0). Повторный вызов и вызов для пустой
 * матрицы безопасны (защита от double free).
 * @param m Матрица, память которой нужно освободить (передаётся по ссылке).
 */
void free_matrix(Matrix& m);

/**
 * @brief Сложение двух матриц: C[i][j] = A[i][j] + B[i][j].
 * @param a Первая матрица M x N.
 * @param b Вторая матрица M x N.
 * @return Matrix Новая матрица M x N — результат сложения.
 * @exception std::invalid_argument Если матрица пустая или размеры не совпадают.
 * @note Время выполнения: O(M * N).
 * @warning Результат нужно освобождать через free_matrix().
 */
Matrix matrix_add(Matrix a, Matrix b);

/**
 * @brief Умножение двух матриц: C = A x B.
 *
 * Используется порядок циклов i-k-j: строки обеих матриц обходятся
 * последовательно, что эффективно для кэша процессора.
 * @param a Матрица M x N.
 * @param b Матрица N x K.
 * @return Matrix Результат M x K.
 * @exception std::invalid_argument Если матрица пустая или cols(a) != rows(b).
 * @note Время выполнения: O(M * N * K).
 * @warning Результат нужно освобождать через free_matrix().
 */
Matrix matrix_multiply(Matrix a, Matrix b);

/**
 * @brief Транспонирование матрицы: T[j][i] = M[i][j].
 * @param m Исходная матрица M x N.
 * @return Matrix Новая матрица N x M.
 * @exception std::invalid_argument Если матрица пустая.
 * @warning Результат нужно освобождать через free_matrix().
 */
Matrix matrix_transpose(Matrix m);

/**
 * @brief Вывод матрицы в стандартный поток вывода (std::cout).
 * @param m Матрица для вывода.
 * @see print_matrix(Matrix, std::ostream&)
 */
void print_matrix(Matrix m);

/**
 * @brief Вывод матрицы в заданный поток в читаемом формате.
 *
 * Числа выводятся с фиксированной точкой (2 знака) и выравниваются по
 * столбцам. Для пустой матрицы выводится «[Empty matrix]». Если матрица
 * больше kMaxRowsToShow x kMaxColsToShow, выводятся первые и последние
 * строки/столбцы, пропуски обозначаются «...», в конце печатается размер.
 * Настройки форматирования потока после вывода восстанавливаются.
 * @param m Матрица для вывода.
 * @param out Поток вывода.
 */
void print_matrix(Matrix m, std::ostream& out);

/**
 * @brief Создание матрицы из одномерного массива (заполнение по строкам).
 *
 * data[0]...data[cols-1] — первая строка, data[cols]...data[2*cols-1] —
 * вторая и т. д.
 * @param data Указатель на массив из rows * cols элементов.
 * @param rows Количество строк (> 0).
 * @param cols Количество столбцов (> 0).
 * @return Matrix Новая матрица с данными из массива.
 * @exception std::invalid_argument Если data == nullptr или размеры <= 0.
 * @code
 * const double arr[] = {1, 2, 3, 4, 5, 6};
 * matrix::Matrix m = matrix::matrix_from_array(arr, 2, 3);
 * @endcode
 */
Matrix matrix_from_array(const double* data, int rows, int cols);

/**
 * @brief Чтение матрицы из потока ввода.
 *
 * Формат: «rows cols», затем rows * cols чисел по строкам.
 * @param in Поток ввода.
 * @return Matrix Прочитанная матрица.
 * @exception std::invalid_argument Если вместо чисел введены буквы, данных
 *            не хватает или размеры вне диапазона [1, kMaxInputDimension].
 */
Matrix read_matrix(std::istream& in);

/**
 * @brief Вариант 6. Вычисление среднего арифметического всех элементов матрицы.
 *
 * mean = (сумма всех элементов) / (rows * cols).
 * Если при суммировании происходит переполнение (сумма не помещается в
 * double), среднее пересчитывается как сумма элементов, предварительно
 * делённых на их количество, — результат по модулю не превосходит
 * максимального по модулю элемента и переполнения не происходит.
 * @param m Матрица.
 * @return double Среднее арифметическое элементов.
 * @exception std::invalid_argument Если матрица пустая
 *            (data == nullptr, rows <= 0 или cols <= 0).
 * @note Время выполнения: O(rows * cols).
 * @code
 * const double arr[] = {1, 2, 3, 4};
 * matrix::Matrix m = matrix::matrix_from_array(arr, 2, 2);
 * double mean = matrix::matrix_average(m);  // 2.5
 * matrix::free_matrix(m);
 * @endcode
 */
double matrix_average(Matrix m);

}  // namespace matrix
