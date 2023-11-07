#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>
#include <initializer_list>
#include <cmath>
#include <QPointF>
#include <QPolygon>

class Matrix {
public:
    Matrix(int rows, int cols);
    explicit Matrix(std::initializer_list<std::initializer_list<double>> list);
    Matrix(const Matrix &other);
    ~Matrix();

    Matrix& operator=(const Matrix &other);
    QVector<double>& operator[](int index);
    const QVector<double>& operator[](int index) const;
    Matrix operator*(const Matrix &other) const;
    QVector<double> operator*(const QVector<double>& vec) const;
    QPolygonF transformPolygon(const QPolygonF& polygon) const;
    QPointF transformPoint(const QPointF &point) const;

    static Matrix identity(int size);
    static Matrix translate(double tx, double ty, double tz);
    static Matrix scale(double sx, double sy, double sz);
    static Matrix rotateX(double angle);
    static Matrix rotateY(double angle);
    static Matrix rotateZ(double angle);

    int rowCount() const { return data.size(); }
    int colCount() const { return data[0].size(); }

private:
    QVector<QVector<double>> data;

    void allocateData(int rows, int cols);
};

#endif // MATRIX_H
