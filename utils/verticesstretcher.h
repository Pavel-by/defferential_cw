#include "figure.h"

#pragma once

class VerticesStretcher
{
public:
    float AxesLimits = 1000;

    VerticesStretcher();

    QVector<Edge> constructEdges(const QList<QVector3D>& vertices);

private:
    void showDialog(const QString& message);
    bool isSamePoint(float value, float second);
    bool isSamePoint(const QVector3D& first, const QVector3D& second);
    bool validateVertices(const QList<QVector3D>& vertices);
    void prepareFirstEdge(const QList<QVector3D>& vertices, Edge& edge);
    bool prepareEdge(const QMap<QVector3D, QSet<QVector3D>>& used, Edge& edge);
    void finalizeEdge(const QList<QVector3D>& vertices, QMap<QVector3D, QSet<QVector3D>>& used, Edge& edge);
    bool isUsed(const QMap<QVector3D, QSet<QVector3D>>& used, const QVector3D& v1, const QVector3D& v2);
    bool isTwoDirectioned(const QMap<QVector3D, QSet<QVector3D>>& used, const QVector3D& v1, const QVector3D& v2);
    void markUsed(QMap<QVector3D, QSet<QVector3D>>& used, const QVector3D& v1, const QVector3D& v2);
    bool joinEdge(Edge& e1, Edge& e2);
};

// just to using in map
bool operator<(const QVector3D &e1, const QVector3D &e2);

// to use maps/sets
uint qHash(const QVector3D &v, uint seed = 0);
