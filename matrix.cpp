#include "matrix.h"
#include <QPointF>
#include <stdexcept>

Matrix::Matrix(int rows, int cols) {
    allocateData(rows, cols);
}

Matrix::Matrix(const Matrix &other) : data(other.data) {
}

Matrix::~Matrix() {

}

Matrix& Matrix::operator=(const Matrix &other) {
    if (this != &other) { // protect against invalid self-assignment
        data = other.data;
    }
    return *this;
}

QVector<double>& Matrix::operator[](int index) {
    if (index < 0 || index >= data.size()) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

const QVector<double>& Matrix::operator[](int index) const {
    if (index < 0 || index >= data.size()) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

Matrix Matrix::operator*(const Matrix &other) const {
    if (colCount() != other.rowCount()) {
        throw std::invalid_argument("Matrices dimensions mismatch for multiplication.");
    }

    Matrix result(rowCount(), other.colCount());
    for (int i = 0; i < rowCount(); ++i) {
        for (int j = 0; j < other.colCount(); ++j) {
            for (int k = 0; k < colCount(); ++k) {
                result[i][j] += (*this)[i][k] * other[k][j];
            }
        }
    }
    return result;
}

QVector<double> Matrix::operator*(const QVector<double>& vec) const {
    if (this->colCount() != vec.size()) {
        throw std::invalid_argument("Matrix columns must match vector size for multiplication.");
    }

    QVector<double> result(this->rowCount(), 0.0);
    for (int i = 0; i < this->rowCount(); ++i) {
        for (int j = 0; j < this->colCount(); ++j) {
            result[i] += data[i][j] * vec[j];
        }
    }
    return result;
}

QPolygonF Matrix::transformPolygon(const QPolygonF& polygon) const {
    QPolygonF transformedPolygon;

    for (int i = 0; i < polygon.size(); i++) {

        Matrix pointMatrix(4, 1);
        pointMatrix[0][0] = polygon[i].x();
        pointMatrix[1][0] = polygon[i].y();
        pointMatrix[2][0] = 0;
        pointMatrix[3][0] = 1;

        Matrix result = (*this) * pointMatrix;

        QPointF transformedPoint(result[0][0] / result[3][0],
                result[1][0] / result[3][0]);
        transformedPolygon << transformedPoint;
    }

    return transformedPolygon;
}

QPointF Matrix::transformPoint(const QPointF &point) const {
    QVector<double> result = {point.x(), point.y(), 0, 1};
    result = (*this) * result; // Using matrix-vector multiplication
    return QPointF(result[0], result[1]);
}

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> list)
    : Matrix(list.size(), list.begin()->size()) {
    int row = 0;
    for (auto &rowList : list) {
        int col = 0;
        for (auto &elem : rowList) {
            data[row][col++] = elem;
        }
        ++row;
    }
}

void Matrix::allocateData(int rows, int cols) {
    data.resize(rows);
    for (int i = 0; i < rows; ++i) {
        data[i].resize(cols);
    }
}

Matrix Matrix::identity(int size) {
    Matrix result(size, size);
    for (int i = 0; i < size; ++i) {
        result[i][i] = 1.0;
    }
    return result;
}

Matrix Matrix::translate(double tx, double ty, double tz) {
    Matrix result = identity(4); // translation matrix is always 4x4 in homogeneous coordinates
    result[0][3] = tx;
    result[1][3] = ty;
    result[2][3] = tz;
    return result;
}

Matrix Matrix::scale(double sx, double sy, double sz) {
    Matrix result = identity(4); // scale matrix is always 4x4 in homogeneous coordinates
    result[0][0] = sx;
    result[1][1] = sy;
    result[2][2] = sz;
    return result;
}

Matrix Matrix::rotateX(double angle) {
    Matrix result = identity(4);
    double rad = angle * M_PI / 180.0; // convert to radians
    result[1][1] = cos(rad);
    result[1][2] = -sin(rad);
    result[2][1] = sin(rad);
    result[2][2] = cos(rad);
    return result;
}

Matrix Matrix::rotateY(double angle) {
    Matrix result = identity(4);
    double rad = angle * M_PI / 180.0; // convert to radians
    result[0][0] = cos(rad);
    result[0][2] = sin(rad);
    result[2][0] = -sin(rad);
    result[2][2] = cos(rad);
    return result;
}

Matrix Matrix::rotateZ(double angle) {
    Matrix result = identity(4);
    double rad = angle * M_PI / 180.0; // convert to radians
    result[0][0] = cos(rad);
    result[0][1] = -sin(rad);
    result[1][0] = sin(rad);
    result[1][1] = cos(rad);
    return result;
}

