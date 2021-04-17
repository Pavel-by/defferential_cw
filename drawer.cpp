#include "drawer.h"
#include <iostream>

// --------------- temp code --------------------
QVector3D const vertices[] = {
    QVector3D(1, 1, -1),
    QVector3D(-1, 1, -1),
    QVector3D(-1, 1, 1),
    QVector3D(1, 1, 1),
    QVector3D(1, -1, -1),
    QVector3D(-1, -1, -1),
    QVector3D(-1, -1, 1),
    QVector3D(1, -1, 1),
};

const int edgesCount = 6 * 4;
int const edges[] = {
    0, 3, 2, 1,
    4, 5, 6, 7,
    5, 1, 2, 6,
    2, 3, 7, 6,
    0, 4, 7, 3,
    0, 1, 5, 4,
};

// length for cube: 6 edges, 6 vertices in edge (2 triangles)
const int bufferLength = 6 * 6;

QVector3D * writeEdge(QVector3D *target, const int *edge) {
    target[0] = vertices[edge[0]];
    target[1] = vertices[edge[2]];
    target[2] = vertices[edge[1]];
    target[3] = vertices[edge[0]];
    target[4] = vertices[edge[3]];
    target[5] = vertices[edge[2]];
    return target + 6;
}

// --------------- temp code end ----------------

Drawer::Drawer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    initMatrices();
    initBuffer();
}

Drawer::~Drawer() {
    _baseProgram->release();
    _baseProgram->removeAllShaders();
}

void Drawer::initBaseProgram() {
    _baseProgram = new QOpenGLShaderProgram(this);
    _baseProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/base.vert");
    _baseProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/base.frag");

    if (_baseProgram->link()) {
        std::cout << "Shaders program was linked";
    } else {
        std::cout << "Cannot link shaders program";
    }
}
void Drawer::initMatrices() {
    _proj = QMatrix4x4();
    _view = QMatrix4x4();
    _model = QMatrix4x4();

    _view.lookAt(QVector3D(4, 4, 4), QVector3D(0, 0, 0), QVector3D(0, 0, 1));
}

void Drawer::initBuffer() {
    _buffer = new QVector3D[bufferLength];
    QVector3D *tmp = _buffer;
    for (int i = 0; i < edgesCount; i += 4) {
        tmp = writeEdge(tmp, edges + i);
    }
}

void Drawer::initializeGL() {
    initBaseProgram();
}

void Drawer::resizeGL(int w, int h) {
    _proj = QMatrix4x4();
    _proj.perspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
}

void Drawer::paintGL() {
    if (!_baseProgram->isLinked()) {
        std::cout << "Shaders are not linked; can not paint";
        return;
    }

    if (!_baseProgram->bind()) {
        std::cout << "Cannot bind shaders; can not paint";
        return;
    }

    int projLoc = _baseProgram->uniformLocation("proj");
    int viewLoc = _baseProgram->uniformLocation("view");
    int modelLoc = _baseProgram->uniformLocation("model");
    int vertexLocation = _baseProgram->attributeLocation("vertex");
    _baseProgram->setUniformValue(projLoc, _proj);
    _baseProgram->setUniformValue(viewLoc, _view);
    _baseProgram->setUniformValue(modelLoc, _model);
    _baseProgram->enableAttributeArray(vertexLocation);
    _baseProgram->setAttributeArray(vertexLocation, _buffer);
    glDrawArrays(GL_TRIANGLES, 0, bufferLength);
    _baseProgram->disableAttributeArray(vertexLocation);
}

void Drawer::mouseMoveEvent(QMouseEvent* event) {
}

void Drawer::mousePressEvent(QMouseEvent* event) {
}

void Drawer::mouseReleaseEvent(QMouseEvent *event) {
}
