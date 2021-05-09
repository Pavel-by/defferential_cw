#include "iceberg.h"

Iceberg::Iceberg()
{
    QVector<QVector3D> vertices = {
        QVector3D(1, 1, -1),
        QVector3D(-1, 1, -1),
        QVector3D(-1, 1, 1),
        QVector3D(1, 1, 1),
        QVector3D(1, -1, -1),
        QVector3D(-1, -1, -1),
        QVector3D(-1, -1, 1),
        QVector3D(1, -1, 1),
    };
    QVector3D* verticesData = vertices.data();
    edges = {
        {{verticesData[0],
          verticesData[3],
          verticesData[2],
          verticesData[1]}},
        {{verticesData[4],
          verticesData[5],
          verticesData[6],
          verticesData[7]}},
        {{verticesData[5],
          verticesData[1],
          verticesData[2],
          verticesData[6]}},
        {{verticesData[2],
          verticesData[3],
          verticesData[7],
          verticesData[6]}},
        {{verticesData[0],
          verticesData[4],
          verticesData[7],
          verticesData[3]}},
        {{verticesData[0],
          verticesData[1],
          verticesData[5],
          verticesData[4]}},
    };
    timer.setInterval(100);
    poly = new Polyhedron(edges);
    poly->computeNewState(0.01);
}


Iceberg::~Iceberg() {

}
