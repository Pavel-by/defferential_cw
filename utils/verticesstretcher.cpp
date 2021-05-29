#include "verticesstretcher.h"
#include <iostream>
#include <cmath>

#include <QErrorMessage>
#include <QFile>
#include <QMap>

VerticesStretcher::VerticesStretcher()
{

}

QVector<Edge> VerticesStretcher::constructEdges(const QList<QVector3D>& inputVertices) {
    QList<QVector3D> vertices;
    for (const QVector3D& vertice : inputVertices) {
        bool alreadyAdded = false;
        for (const QVector3D& addedVertice : vertices)
            alreadyAdded |= isSamePoint(addedVertice, vertice);
        if (!alreadyAdded)
            vertices.append(vertice);
    }

    if (!validateVertices(vertices))
        return {};

    // {
    //   <from1>: {<to1>, <to2>, ...},
    //   ...
    // }
    QMap<QVector3D, QSet<QVector3D>> used;
    QVector<Edge> edges;
    Edge edge;
    edge.vertices.resize(2);
    prepareFirstEdge(vertices, edge);
    finalizeEdge(vertices, used, edge);
    edges.append(edge);
    edge.vertices.resize(2);

    while (prepareEdge(used, edge)) {
        finalizeEdge(vertices, used, edge);
        edges.append(edge);
        edge.vertices.resize(2);
    }

    return edges;
}

void VerticesStretcher::prepareFirstEdge(const QList<QVector3D> &vertices, Edge &edge) {
    QVector3D p1 = vertices.first();
    for (const QVector3D& v : vertices)
        if (v.z() > p1.z())
            p1 = v;
    QVector3D normal = QVector3D(0,0,-1);
    QVector3D p2 = vertices[0];

    if (isSamePoint(p2, p1))
        p2 = vertices[1];

    for (const QVector3D& v : vertices) {
        if (QVector3D::dotProduct((p2 - p1).normalized(), normal) < QVector3D::dotProduct((v - p1).normalized(), normal))
            p2 = v;
    }
    edge.vertices[0] = p1;
    edge.vertices[1] = p2;
}

bool VerticesStretcher::prepareEdge(const QMap<QVector3D, QSet<QVector3D>> &used, Edge &edge) {
    for (const QVector3D& v1 : used.keys()) {
        for (const QVector3D& v2 : used[v1])
            if (!isUsed(used, v2, v1)) {
                edge.vertices[0] = v2;
                edge.vertices[1] = v1;
                return true;
            }
    }
    return false;
}

bool VerticesStretcher::isUsed(const QMap<QVector3D, QSet<QVector3D>> &used, const QVector3D &v1, const QVector3D &v2) {
    return used.contains(v1) && used[v1].contains(v2);
}

bool VerticesStretcher::isTwoDirectioned(const QMap<QVector3D, QSet<QVector3D>> &used, const QVector3D &v1, const QVector3D &v2) {
    return isUsed(used, v1, v2) && isUsed(used, v2, v1);
}

void VerticesStretcher::markUsed(QMap<QVector3D, QSet<QVector3D>>& used, const QVector3D& v1, const QVector3D& v2) {
    assert(!isUsed(used, v1, v2));
    used[v1].insert(v2);
}

void VerticesStretcher::finalizeEdge(const QList<QVector3D> &vertices, QMap<QVector3D, QSet<QVector3D>> &used, Edge &edge) {
    bool thirdInitialized = false;

    for (int i = 0; i < vertices.length(); i++) {
        const QVector3D& v = vertices[i];

        if (isSamePoint(edge.vertices[0], v) || isSamePoint(edge.vertices[1], v))
            continue;

        if (isUsed(used, v, edge.vertices[0]))
            continue;

        if (!thirdInitialized) {
            thirdInitialized = true;
            edge.vertices.append(v);
            continue;
        }

        if (QVector3D::dotProduct(v - edge.vertices[0], edge.normal()) > 0)
            edge.vertices[2] = v;
    }

    QVector3D planeNormal = edge.normal();
    edge.vertices.remove(2);

    QList<QVector3D> planeVertices;
    for (int i = 0; i < vertices.length(); i++) {
        const QVector3D& v = vertices[i];
        if (v.distanceToPlane(edge.vertices[0], planeNormal) == 0.0f)
            planeVertices.append(v);
    }

    while (edge.vertices.first() != edge.vertices.last()) {
        QVector3D startPoint = edge.vertices.last();
        QVector3D compareVector = (edge.vertices.last() - edge.vertices[edge.vertices.length() - 2]).normalized();
        QVector3D nearestPoint = startPoint - compareVector;

        for (const QVector3D& v : planeVertices) {
            if (isSamePoint(v, startPoint) || isSamePoint(v, edge.vertices.last()))
                continue;

            float currentCos = QVector3D::dotProduct(compareVector, (nearestPoint - startPoint).normalized());
            float testCos = QVector3D::dotProduct(compareVector, (v - startPoint).normalized());
            if (testCos > currentCos)
                nearestPoint = v;
        }

        edge.vertices.append(nearestPoint);
    }
    edge.vertices.removeLast();

    for (int i = 0; i < edge.vertices.length(); i++) {
        markUsed(used, edge.vertices[i], edge.vertices[(i+1) % edge.vertices.length()]);
    }
}

bool VerticesStretcher::validateVertices(const QList<QVector3D>& vertices) {
    if (vertices.size() < 4) {
        showDialog("Vertices count can not be less than 4");
        return false;
    }

    bool onePlane = true;
    QList<QVector3D>::const_iterator vertice_iterator;

    for (const QVector3D& vertice : vertices) {
        onePlane &= isSamePoint(vertice.distanceToPlane(
                                    vertices[0], vertices[1], vertices[2]
                ), 0.0f);

        if (!onePlane) break;
    }

    if (onePlane) {
        showDialog("All vertices placed on one plane (or they too close to each other)");
        return false;
    }

    for (const QVector3D& vertice : vertices) {
        if (std::abs(vertice.x()) > AxesLimits
                || std::abs(vertice.y()) > AxesLimits
                || std::abs(vertice.z()) > AxesLimits) {
            showDialog("Vertices coordinates limited to " + QString::number(AxesLimits) + " in axes");
            return false;
        }
    }

    return true;
}

void VerticesStretcher::showDialog(const QString& message) {
    QErrorMessage dialog;
    dialog.showMessage(message);
    dialog.exec();
}

bool VerticesStretcher::isSamePoint(float first, float second) {
    return std::abs(first - second) < 0.000001f;
}

bool VerticesStretcher::isSamePoint(const QVector3D& first, const QVector3D& second) {
    return isSamePoint(first.distanceToPoint(second), 0.0f);
}

bool operator<(const QVector3D &v1, const QVector3D &v2) {
    if (v1.x() != v2.x())
        return v1.x() < v2.x();

    if (v1.y() != v2.y())
        return v1.y() < v2.y();

    if (v1.z() != v2.z())
        return v1.z() < v2.z();

    return false;
}

uint qHash(const QVector3D &v, uint seed) {
    if (!seed)
        seed = 1;
    return qHash(v.x(), seed) ^ qHash(v.y(), seed * 11) ^ qHash(v.z(), seed * 41);
}
