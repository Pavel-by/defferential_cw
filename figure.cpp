#include "figure.h"
#include <iostream>

Figure::Figure():QObject()
{

}

Figure::~Figure() {
    if (isAttached())
        detach();
}

QOpenGLFunctions_3_3_Compatibility *Figure::getFuncs() {
    auto funcs = context->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
    if (!funcs) {
        qDebug() << "Vertions functions are null\n";
        exit(1);
    }
    return funcs;
}

void Figure::attach(QOpenGLContext *context) {
    assert(this->context == nullptr);
    this->context = context;

    auto funcs = getFuncs();

    //gl->initializeOpenGLFunctions();

    funcs->glGenVertexArrays(1, &_vao);
    funcs->glGenBuffers(1, &_vbo);
    funcs->glGenBuffers(1, &_veo);

    funcs->glBindVertexArray(_vao);
    funcs->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _veo);

    funcs->glEnableClientState(GL_VERTEX_ARRAY);
    funcs->glVertexPointer(3, GL_FLOAT, sizeof(VertexData), 0);

    funcs->glEnableClientState(GL_NORMAL_ARRAY);
    funcs->glNormalPointer(GL_FLOAT, sizeof(VertexData), reinterpret_cast<GLvoid*>(sizeof(QVector3D)));

    funcs->glBindVertexArray(0);
    funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);
    funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    funcs->glDisableClientState(GL_VERTEX_ARRAY);
    funcs->glDisableClientState(GL_NORMAL_ARRAY);
}

void Figure::paint() {
    auto funcs = getFuncs();

    for (MaterialConfig *config : materials) {
        config->use(context);
    }

    allocateBuffers();
    funcs->glColor3f(1,0,0);
    funcs->glBindVertexArray(_vao);
    funcs->glDrawElements(GL_TRIANGLES, _indicesBuffer.size(), GL_UNSIGNED_INT, nullptr);
    funcs->glBindVertexArray(0);
}

void Figure::detach() {
    assert(isAttached());
    auto funcs = getFuncs();
    funcs->glDeleteVertexArrays(1, &_vao);
    funcs-> glDeleteBuffers(1, &_vbo);
    funcs->glDeleteBuffers(1, &_veo);
    context = nullptr;
    markVertexChanged();
}

bool Figure::isAttached() {
    return context != nullptr;
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
    QMatrix4x4 normalMatrix = model().inverted().transposed();

    for (const Edge& edge : edges) {
        for (int i = 2; i < edge.vertices.size(); i++) {
            _indicesBuffer[indexPosition++] = vertexPosition;
            _indicesBuffer[indexPosition++] = vertexPosition + i - 1;
            _indicesBuffer[indexPosition++] = vertexPosition + i;
        }
        QVector3D normal = edge.normal();
        for (const QVector3D& vertex : edge.vertices) {
            _verticesBuffer[vertexPosition++] = {
                vertex,
                -normal//(normalMatrix * normal).normalized()
            };
        }
    }

    auto funcs = getFuncs();
    funcs->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    funcs->glBufferData(GL_ARRAY_BUFFER, _verticesBuffer.size() * sizeof(VertexData), _verticesBuffer.constData(), GL_DYNAMIC_DRAW);
    funcs->glBindBuffer(GL_ARRAY_BUFFER, 0);

    funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _veo);
    funcs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer.size() * sizeof(GLuint), _indicesBuffer.constData(), GL_DYNAMIC_DRAW);
    funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Figure::markNeedsPaint() {
    emit changed();
}

void Figure::markVertexChanged() {
    _verticesChanged = true;
}

void Figure::rotate(float angle, const QVector3D& vector) {
    _modelRotation.rotate(angle, vector);
    markVertexChanged();
}

void Figure::scale(QVector3D vector) {
    _modelScale.scale(vector);
    markVertexChanged();
}

void Figure::translate(QVector3D vector) {
    _modelTranslate.translate(_modelRotation * vector);
}

QMatrix4x4 Figure::model() const {
    return _modelTranslate * _modelRotation * _modelScale;
}

void Figure::clearModel() {
    _modelRotation = QMatrix4x4();
    _modelTranslate = QMatrix4x4();
    _modelScale = QMatrix4x4();
}

void Figure::setTranslate(QVector3D vector) {
    _modelTranslate = QMatrix4x4();
    _modelTranslate.translate(vector);
}
