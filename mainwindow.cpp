#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "matrix.h"

#include <QLabel>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    timer->start(10);

    angle = 0;

    labelStatus = new QLabel(this);
    labelStatus->setText(QString::number(angle));
    labelStatus->setGeometry(QRect(10, 10, 200, 30));
}

void MainWindow::onTimerTimeout() {
    angle += 0.3;
    if (angle >= 360)
        angle -= 360;
    labelStatus->setText(QString::number(angle));
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QPointF>
#include <QList>
#include <cmath>

QList<QPointF> findIntersections(
    const QPointF &center, double radius,
    const QPointF &lineStart, const QPointF &lineEnd)
{
    QList<QPointF> intersections;

    // Уравнение окружности: (x - h)^2 + (y - k)^2 = r^2
    double h = center.x();
    double k = center.y();
    double r = radius;

    // Компоненты направления линии
    double dx = lineEnd.x() - lineStart.x();
    double dy = lineEnd.y() - lineStart.y();

    // Параметры уравнения квадрата (Ax^2 + Bx + C = 0)
    double A = dx * dx + dy * dy;
    double B = 2 * (dx * (lineStart.x() - h) + dy * (lineStart.y() - k));
    double C = (lineStart.x() - h) * (lineStart.x() - h) + (lineStart.y() - k) * (lineStart.y() - k) - r * r;

    // Дискриминант
    double D = B * B - 4 * A * C;

    if (D >= 0) {
        // Корни уравнения
        double t1 = (-B + std::sqrt(D)) / (2 * A);
        double t2 = (-B - std::sqrt(D)) / (2 * A);

        QPointF intersection(lineStart.x() + t1 * dx, lineStart.y() + t1 * dy);

        // Проверка углов пересечения и их добавление, если они находятся в диапазоне дуги
        double angle1 = std::atan2(intersection.y() - k, intersection.x() - h) * 180 / M_PI;

        // Нормализация углов
        if (angle1 < 0) angle1 += 360;

        intersections.push_back(intersection);

    }

    return intersections;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPointF earthCenter(0, 0);

    int centerX = this->width() / 2;
    int centerY = this->height() / 2;
    Matrix translateCenterMatrix = Matrix::translate(centerX, centerY, 0);
    earthCenter = translateCenterMatrix.transformPoint(earthCenter);

    int earthRadius = qMin(this->width(), this->height()) / 5; // Радиус шара

    // Рисуем светлую половину
    painter.setBrush(QColor(200, 200, 255)); // Светлый оттенок синего

    painter.drawPie(earthCenter.x() - earthRadius,
                    earthCenter.y() - earthRadius,
                    2 * earthRadius, 2 * earthRadius,
                    45 * 16,
                    180 * 16);

    // Рисуем темную половину
    painter.setBrush(QColor(0, 0, 128)); // Темный оттенок синего
    painter.drawPie(earthCenter.x() - earthRadius,
                    earthCenter.y() - earthRadius,
                    2 * earthRadius,
                    2 * earthRadius,
                    225 * 16,
                    180 * 16);

    int sunRadius = earthRadius / 2.5;
    float sunDistance = earthRadius * 2;
    QPointF sunCenter = earthCenter;
    Matrix sunTranslateMatrix = Matrix::translate(
                -sunDistance,
                -sunDistance,
                0);
    sunCenter = sunTranslateMatrix.transformPoint(sunCenter);

    painter.setBrush(Qt::yellow);
    painter.drawEllipse(sunCenter, sunRadius, sunRadius);

    int moonRadius = earthRadius / 5;
    float moonDistance = earthRadius * 1.4;
    QPointF moonCenter = earthCenter;
    Matrix moonTranslateMatrix = Matrix::translate(
                -moonDistance * cos(angle / 180 * M_PI),
                -moonDistance * sin(angle / 180 * M_PI),
                0);
    moonCenter = moonTranslateMatrix.transformPoint(moonCenter);

    QPointF sunToMoon = sunCenter - moonCenter;
    double sunMoonAngle = atan2(sunToMoon.y(), sunToMoon.x());
    sunMoonAngle = - sunMoonAngle * 180.0 / M_PI;

    // Рисуем светлую сторону луны, которая будет обращена к солнцу
    painter.setBrush(Qt::gray); // Цвет светлой стороны луны
    painter.drawPie(moonCenter.x() - moonRadius,
                    moonCenter.y() - moonRadius,
                    2 * moonRadius, 2 * moonRadius,
                    (sunMoonAngle - 90) * 16,
                    180 * 16);

    // Рисуем темную сторону луны
    painter.setBrush(QColor(50, 50, 50)); // Цвет темной стороны луны
    painter.drawPie(moonCenter.x() - moonRadius,
                    moonCenter.y() - moonRadius,
                    2 * moonRadius, 2 * moonRadius,
                    (sunMoonAngle + 90) * 16,
                    180 * 16);

    // Создаем QPainterPath для дуги окружности (светлой стороны земли)
    QPainterPath earthPath;
    earthPath.arcMoveTo(earthCenter.x() - earthRadius,
                        earthCenter.y() - earthRadius,
                        2 * earthRadius, 2 * earthRadius,
                        45);
    earthPath.arcTo(earthCenter.x() - earthRadius,
                    earthCenter.y() - earthRadius,
                    2 * earthRadius,
                    2 * earthRadius,
                    45, 180);


    // Нормализация вектора
    qreal length = std::sqrt(sunToMoon.x() * sunToMoon.x() + sunToMoon.y() * sunToMoon.y());
    QPointF sunToMoonNormalized(sunToMoon.x() / length, sunToMoon.y() / length);

    // Получение перпендикуляра единичной длины
    QPointF perpendicular(-sunToMoonNormalized.y(), sunToMoonNormalized.x());

    // Создаем QPainterPath для линии
    QPainterPath linePath;
    QPointF lineStart = moonCenter + perpendicular * moonRadius;
    QPointF lineEnd = sunCenter + perpendicular * sunRadius; // и заканчивается в центре Солнца

    QPointF direction = lineEnd - lineStart;
    double linePathlength = std::sqrt(direction.x() * direction.x() +
                                      direction.y() * direction.y());
    QPointF unitVector(direction.x() / linePathlength, direction.y() / linePathlength);

    lineStart -= 200 * unitVector;


    QList<QPointF> intersections1 = findIntersections(earthCenter, earthRadius, lineStart, lineEnd);
    for (auto point: intersections1)
        painter.drawEllipse(point, 5, 5);
}
