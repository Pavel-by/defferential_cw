#ifndef ICEBERG_H
#define ICEBERG_H

#include "figure.h"
#include "polyhedron.h"

#include <QTimer>



class Iceberg : public Figure
{
    Q_OBJECT
private slots:
    void tick();

public:
    Iceberg(Polyhedron* poly = nullptr);
    ~Iceberg() override;

    void stepForward(float step);

    void startTimer();
    void pauseTimer();
    Polyhedron* generatePoly();

    void loadDefaultFigure();

private:
    Polyhedron* _poly = nullptr;
    QTimer timer;
    clock_t _lastTime = 0;
};

#endif // ICEBERG_H
