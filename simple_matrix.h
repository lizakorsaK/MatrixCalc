#ifndef LIZAKORSAK_BACKEND_SIMPLE_MATRIX_HPP
#define LIZAKORSAK_BACKEND_SIMPLE_MATRIX_HPP

#include <exception>
#include <iostream>
#include <string>
#include <cmath>
#include <initializer_list>
#include <QTableWidget>

class DMatrix {
    double** mat {nullptr};
    size_t h {0};
    size_t w {0};

    constexpr const static double EPS = 1e-9;

    void resize(size_t height, size_t width, double filler = 0) {
        h = height;
        w = width;

        mat = new double*[h];
        for (size_t i = 0; i < h; ++i){
            mat[i] = new double[w];
            for (size_t j = 0; j < w; ++j)
                mat[i][j] = filler;
            }


    }

public:
    //C-tors
    DMatrix() = default;
    DMatrix(size_t height, size_t width) {
        resize(height, width);
    }
    DMatrix(size_t height, size_t width, double filler){
        resize(height, width, filler);
    }
    DMatrix(const DMatrix& ref) : DMatrix(ref.h, ref.w) {
        for (size_t i = 0; i < ref.h; ++i)
            for(size_t j = 0; j < ref.w; ++j)
                mat[i][j] = ref.mat[i][j];
    }
    DMatrix(std::initializer_list<std::initializer_list<double>> list) {
        w = list.size();
        h = list.begin()->size();

        resize(h,w);

        for (const auto& column : list)
            if (column.size() != h)
                throw std::invalid_argument("Incorrect initialization via std::initializer_list");

        size_t i = 0;
        size_t j = 0;

        for (const auto& column : list) {
            for (const auto& element : column) {
                mat[i][j] = element;
                ++j;
            }
            j = 0;
            ++i;
        }
    }
    DMatrix&operator=(const DMatrix& ref) {
        resize(ref.h, ref.w);
        for (size_t i = 0; i < ref.h; ++i)
            for(size_t j = 0; j < ref.w; ++j)
                mat[i][j] = ref.mat[i][j];

        return *this;
    }

    // QWidgetTableWrappers
    DMatrix(QTableWidget* table) : DMatrix(table->rowCount(), table->columnCount()) {
        for (size_t i = 0; i < h; ++i)
            for(size_t j = 0; j < w; ++j)
                mat[i][j] = table->item(i,j)->text().toDouble();

    }
    void install(QTableWidget* table) {
        table->setRowCount(h);
        table->setColumnCount(w);
        for (size_t i = 0; i < h; ++i)
            for(size_t j = 0; j < w; ++j)
                if (table->item(i,j) == nullptr)
                    table->setItem(i,j, new QTableWidgetItem(QString::number(mat[i][j])));
                else table->item(i,j)->setText(QString::number(mat[i][j]));
    }

    // Std::vec wrapper
    std::vector<std::vector<double>> toStdVectorOfVectors() const {
        std::vector<std::vector<double>> result;
        result.resize(w);
        for (auto line : result)
            line.resize(h);

        for (size_t i = 0; i < h; ++i)
            for(size_t j = 0; j < w; ++j)
                result[i][j] = mat[i][j];
    }

    // Accessor
    double& operator()(size_t i, size_t j) {
        return mat[i][j];
    }
    double operator()(size_t i, size_t j) const{
        return mat[i][j];
    }

    // Transforms
    DMatrix transpose() {
        if (!isSquare())
            throw std::logic_error("Операция транспонирования возможна только для квадратных матриц");

        DMatrix tmp(h,w);
        for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < w; ++j)
                tmp.mat[i][j] = mat[j][i];

        return tmp;
    }
    DMatrix inverse() {


        if (!isSquare())
            throw std::logic_error("Обратная матрица существует только для квадратных матриц");


        double detValue = det();
        if (detValue == 0)
            throw std::runtime_error("Определитель матрицы равер нулю - обратной не существует");


        DMatrix result(adjoint());
        result *= (double) 1 / detValue;
        return result.transpose();
    }

    DMatrix adjoint() const {
        DMatrix result(h,w);

        for (size_t i = 0; i < result.h; ++i)
            for (size_t j = 0; j < result.w; ++j)
                result.mat[i][j] = cofactor(i,j)*std::pow(-1, i+j);
        return result;
    }
    // Matrix bin o-tors
    DMatrix operator+(const DMatrix& rhe) const {
        if (h != rhe.h or w != rhe.w)
            throw std::invalid_argument("Размерности матриц не совпадают");

        DMatrix result(*this);
        for (size_t i = 0; i < result.h; ++i)
            for (size_t j = 0; j < result.w; ++j)
                result.mat[i][j] += rhe.mat[i][j];
        return result;
    }
    DMatrix operator-(const DMatrix& rhe) const {
        if (h != rhe.h or w != rhe.w)
            throw std::invalid_argument("Размерности матриц не совпадают");

        DMatrix result(*this);
        for (size_t i = 0; i < result.h; ++i)
            for (size_t j = 0; j < result.w; ++j)
                result.mat[i][j] -= rhe.mat[i][j];
        return result;
    }
    DMatrix operator*(const DMatrix& rhe) const {
        if (w != rhe.h)
            throw std::invalid_argument("Матрицы не согласованны");

        DMatrix result(h, rhe.w, 0);

        for (size_t i = 0; i < result.h; ++i)
            for (size_t j = 0; j < result.w; ++j)
                for (size_t k = 0; k < w; ++k)
                    result.mat[i][j] += mat[i][k] * rhe.mat[k][j];

        return result;
    }

    // Matrix scalars
    DMatrix operator*(double value) const {
        DMatrix result(h, w, 0);

        for (size_t i = 0; i < result.h; ++i)
            for (size_t j = 0; j < result.w; ++j)
                    result.mat[i][j] = mat[i][j] * value;

        return result;
    }
    DMatrix& operator*=(double value) {
        for (size_t i = 0; i < h; ++i)
            for (size_t j = 0; j < w; ++j)
                    mat[i][j] *= value;

        return *this;
    }
    double det() {
        if (!isSquare())
            throw std::logic_error("Детерминант находим только в квадратных матрицах");

        DMatrix tmp = *this;

        const size_t size = h;
        double det = 1;



        for (size_t i=0; i < size; ++i) {
            size_t k = i;
            for (size_t j=i+1; j<size; ++j)
                if (std::abs (mat[j][i]) > std::abs (mat[k][i]))
                    k = j;
            if (std::abs (mat[k][i]) < EPS) {
                det = 0;
                break;
            }
            std::swap(mat[i], mat[k]);
            if (i != k)
                det = -det;
            det *= mat[i][i];
            for (size_t j=i+1; j<size; ++j)
                mat[i][j] /= mat[i][i];
            for (size_t j=0; j<size; ++j)
                if (j != i && std::abs (mat[j][i]) > EPS)
                    for (size_t s = i + 1; s < size; ++s)
                        mat[j][s] -= mat[i][s] * mat[j][i];

        }

        *this = tmp;

        return det;
    }
    double cofactor(size_t I, size_t J) const {
        DMatrix result(h-1,w-1);

        for (size_t i = 0, x = 0; i < result.h; ++i, ++x) {
            if (i == I) ++x;
            for (size_t j = 0, y = 0; j < result.w; ++j, ++y) {
                if (j == J) ++y;
                result.mat[i][j] = mat[x][y];
            }
        }

        return result.det();
    }

    // Booleans
    bool isSquare() const {
        return h == w;
    }

    // getters
    size_t getH() const {
        return h;
    }

    size_t getW() const {
        return w;
    }

    // stream output
    friend std::ostream&operator<<(std::ostream& stream, const DMatrix& matrix) {
        for (size_t i = 0; i < matrix.h; ++i) {
            for (size_t j = 0; j < matrix.w; ++j)
                stream << matrix.mat[i][j] << ' ';
            stream << '\n';
        }
        return stream;
    }
};

#endif //LIZAKORSAK_BACKEND_SIMPLE_MATRIX_HPP
