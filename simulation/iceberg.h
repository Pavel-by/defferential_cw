﻿#ifndef ICEBERG_H
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
    Iceberg();
    ~Iceberg() override;

    void stepForward(float step);

    void startTimer();
    void pauseTimer();
    void startSimulation();

private:
    Polyhedron* poly = nullptr;
    QTimer timer;
    clock_t _lastTime = 0;
};

#endif // ICEBERG_H