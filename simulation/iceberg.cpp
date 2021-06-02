#include "iceberg.h"

Iceberg::Iceberg(Polyhedron* poly)
{
    timer.setInterval(20);
    materials = {
        &MaterialConfig::iceberg,
        &MaterialConfig::icebergBack,
    };
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    if (poly != nullptr) {
        assert(!poly->faces.isEmpty());
        _poly = poly;
        edges = poly->faces;
    } else {
        loadDefaultFigure();
    }
}

Iceberg::~Iceberg() {

}

void Iceberg::loadDefaultFigure() {
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
    for (Edge& edge : edges) {
        QVector<QVector3D> reversed;
        reversed.resize(edge.vertices.length());

        for (int i = 0; i < edge.vertices.length(); i++) {
            reversed[i] = edge.vertices[edge.vertices.length() - 1 - i];
        }
        edge.vertices = reversed;
    }
    scale(QVector3D(0.5f, 0.5f, 0.5f));
}

void Iceberg::stepForward(float step) {
    assert(_poly != nullptr);
    _poly->computeNewState(step);
    edges = _poly->faces;
    markNeedsPaint();
}

void Iceberg::startTimer() {
    assert(_poly != nullptr);
    _lastTime = 0;
    timer.start();
}

void Iceberg::pauseTimer() {
    timer.stop();
    _lastTime = 0;
}

void Iceberg::tick() {
    if (_poly == nullptr)
        return;

    if (_lastTime == 0) {
        _lastTime = clock();
        return;
    }

    clock_t now = clock();
    float diff = static_cast<float>(now - _lastTime) / CLOCKS_PER_SEC;
    _lastTime = now;
    stepForward(diff);
}

Polyhedron* Iceberg::generatePoly() {
    QVector<Edge> convertedEdges = edges;
    for (Edge& edge : convertedEdges) {
        for (QVector3D& v : edge.vertices) {
            v = model() * v;
        }
    }
    auto poly = new Polyhedron(convertedEdges);
    return poly;
}

Polyhedron* Iceberg::currentPoly() {
    return _poly;
}
