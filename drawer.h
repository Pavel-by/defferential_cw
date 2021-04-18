#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

struct Edge {
    QVector<const QVector3D*> vertices;
    int id;

    QVector3D normal() const {
        assert(vertices.size() >= 3);
        QVector3D v1 = *vertices[2] - *vertices[1],
                v2 = *vertices[0] - *vertices[1];
        return QVector3D::crossProduct(v1, v2);
    }
};

struct VertexData {
    QVector3D vertex;
    QVector4D color;
    QVector3D normal;
};

class Drawer : public QOpenGLWidget
{
    Q_OBJECT
public:
    Drawer(QWidget *parent = nullptr);
    ~Drawer() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QOpenGLShaderProgram* _baseProgram;
    QMatrix4x4 _proj;
    QMatrix4x4 _view;
    QMatrix4x4 _model;
    QVector<VertexData> _verticesArray;
    QVector<int> _indexesArray;
    QOpenGLBuffer _verticesArrayBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer _indexesArrayBuffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);


    void initBaseProgram();
    void initMatrices();
    void initArrays();
};

