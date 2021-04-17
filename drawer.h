#pragma once

#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

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
    QVector3D *_buffer;

    void initBaseProgram();
    void initMatrices();
    void initBuffer();
};

