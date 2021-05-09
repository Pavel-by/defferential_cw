#ifndef ICEBERG_H
#define ICEBERG_H

#include "figure.h"
#include "polyhedron.h"

#include <QTimer>



class Iceberg : public Figure
{
public:
    Iceberg();
    ~Iceberg();

private:
    Polyhedron* poly;
    QTimer timer;
};

#endif // ICEBERG_H
