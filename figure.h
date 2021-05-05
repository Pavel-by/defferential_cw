#pragma once

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <QVector>
#include <QOpenGLShaderProgram>

struct VertexData {
    QVector3D vertex;
    QVector4D color;
    QVector3D normal;
};

struct Edge {
    QVector<const QVector3D*> vertices;

    QVector3D normal() const {
        assert(vertices.size() >= 3);
        QVector3D v1 = *vertices[2] - *vertices[1],
                v2 = *vertices[0] - *vertices[1];
        return QVector3D::crossProduct(v1, v2);
    }
};

class Figure : QObject
{
    Q_OBJECT

signals:
    void changed();

public:
    Figure();
    Figure(const Figure& other);
    ~Figure() override;

    QVector3D center;
    QVector<QVector3D> vertices;
    QVector<Edge> edges;
    QMatrix4x4 model;
    QVector4D color = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);

    void initialize(QOpenGLShaderProgram* program);
    void paint(QOpenGLShaderProgram* program);

    void rotate(float angle, const QVector3D& vector);
    void scale(QVector3D vector);

    void markNeedsPaint();
    void markVertexChanged();

    Figure& operator=(const Figure& other);

private:
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _verticesBufferGL = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer _indicesBufferGL = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QVector<VertexData> _verticesBuffer;
    QVector<int> _indicesBuffer;

    bool _verticesChanged = true;

    void allocateBuffers();
};

