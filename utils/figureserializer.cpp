#include "figureserializer.h"
#include "verticesstretcher.h"

#include <QErrorMessage>
#include <QFile>

FigureSerializer::FigureSerializer()
{

}

void FigureSerializer::load(const QString& filename, Figure* figure) {
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        showDialog("Failed to open file");
        return;
    }

    QTextStream in(&file);

    QList<QVector3D> vertices;
    qint32 count = 0;

    in >> count;

    for (int i = 0; i < count; i++) {
        float x, y, z;
        in >> x >> y >> z;

        if (in.status() == QTextStream::ReadCorruptData) {
            showDialog("File contains corrupted data");
            return;
        }

        vertices.append(QVector3D(x, y, z));
    }

    QVector<Edge> edges = VerticesStretcher().constructEdges(vertices);

    if (edges.isEmpty())
        return;

    QVector3D min = edges[0].vertices[0], max = edges[0].vertices[0];

    for (const Edge& edge : edges) {
        for (const QVector3D& v : edge.vertices) {
            min.setX(std::min(min.x(), v.x()));
            min.setY(std::min(min.y(), v.y()));
            min.setZ(std::min(min.z(), v.z()));

            max.setX(std::max(max.x(), v.x()));
            max.setY(std::max(max.y(), v.y()));
            max.setZ(std::max(max.z(), v.z()));
        }
    }

    QVector3D center = (min + max) / 2;

    for (Edge& edge : edges) {
        for (QVector3D& v : edge.vertices) {
            v -= center;
        }
    }

    QVector3D diff = max - min;

    for (Edge& edge : edges) {
        for (QVector3D& v : edge.vertices) {
            v /= diff;
        }
    }

    figure->edges = edges;
    figure->clearModel();
    figure->markVertexChanged();
    figure->markNeedsPaint();
}

void FigureSerializer::save(Figure *figure, const QString& filename) {
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showDialog("Failed to open file");
        return;
    }

    QList<QVector3D> vertices;

    for (const Edge& edge : figure->edges) {
        for (const QVector3D& vertice : edge.vertices)
            if (!vertices.contains(vertice))
                vertices.append(vertice);
    }

    QTextStream out(&file);

    out << vertices.size() << " ";
    for (const QVector3D& vertice : vertices)
        out << vertice.x() << " " << vertice.y() << " " << vertice.z() << " ";

    file.close();
}

void FigureSerializer::showDialog(const QString& message) {
    QErrorMessage dialog;
    dialog.showMessage(message);
    dialog.exec();
}
