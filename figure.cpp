#include "figure.h"
#include <iostream>

Figure::Figure():QObject()
{

}

Figure::Figure(const Figure& other) {
    *this = other;
}

Figure::~Figure() {}

void Figure::initialize(QOpenGLShaderProgram* program) {
    _vao.create();
    _vao.bind();

    _verticesBufferGL.create();
    _verticesBufferGL.bind();

    int offset = 0;
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    program->enableAttributeArray("color");
    program->setAttributeBuffer("color", GL_FLOAT, offset, 4, sizeof(VertexData));

    offset += sizeof(QVector4D);
    program->enableAttributeArray("normal");
    program->setAttributeBuffer("normal", GL_FLOAT, offset, 3, sizeof(VertexData));

    _indicesBufferGL.create();
    _indicesBufferGL.bind();

    _vao.release();
}

void Figure::paint(QOpenGLShaderProgram* program) {
    allocateBuffers();
    program->setUniformValue("model", model);
    _vao.bind();
    glDrawElements(GL_TRIANGLES, _indicesBuffer.size(), GL_UNSIGNED_INT, 0);
    _vao.release();
}

void Figure::allocateBuffers() {
    if (!_verticesChanged)
        return;

    _verticesChanged = false;

    int verticesCount = 0;
    int indicesCount = 0;

    for (const Edge& edge : edges) {
        verticesCount += edge.vertices.size();
        indicesCount += (edge.vertices.size() - 2) * 3;
    }

    _verticesBuffer.resize(verticesCount);
    _indicesBuffer.resize(indicesCount);

    int vertexPosition = 0, indexPosition = 0;

    for (const Edge& edge : edges) {
        for (int i = 2; i < edge.vertices.size(); i++) {
            _indicesBuffer[indexPosition++] = vertexPosition;
            _indicesBuffer[indexPosition++] = vertexPosition + i - 1;
            _indicesBuffer[indexPosition++] = vertexPosition + i;
        }
        QVector3D normal = edge.normal();
        for (const QVector3D* vertex : edge.vertices) {
            _verticesBuffer[vertexPosition++] = {
                *vertex,
                color,
                normal
            };
        }
    }

    _verticesBufferGL.bind();
    _verticesBufferGL.allocate(_verticesBuffer.constData(), _verticesBuffer.size() * sizeof(VertexData));
    _indicesBufferGL.bind();
    _indicesBufferGL.allocate(_indicesBuffer.constData(), _indicesBuffer.size() * sizeof(GLuint));
}

void Figure::markNeedsPaint() {
    emit changed();
}

void Figure::markVertexChanged() {
    _verticesChanged = true;
}

Figure& Figure::operator=(const Figure& other) {
    vertices = other.vertices;
    edges = other.edges;
    model = other.model;
    color = other.color;
    _verticesChanged = true;
    return *this;
}

void Figure::rotate(float angle, const QVector3D& vector) {
    QVector3D t = model * center;
    model.translate(-t);
    model.rotate(angle, vector);
    model.translate(t);
}

void Figure::scale(QVector3D vector) {
    QVector3D t = model * center;
    model.translate(-t);
    model.scale(vector);
    model.translate(t);
}
