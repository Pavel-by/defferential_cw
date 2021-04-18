#include "drawer.h"
#include <iostream>

// --------------- temp code --------------------
const QVector4D figureColor = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);
const QVector3D lightPosition = QVector3D(5.0f, 10.0f, 40.0f);
const QVector3D lightColor = QVector3D(1.0f, 1.0f, 1.0f);

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

// counterclock-wise vertices in edges
const QVector<Edge> edges = {
    {{vertices + 0,
      vertices + 3,
      vertices + 2,
      vertices + 1}, 1},
    {{vertices + 4,
      vertices + 5,
      vertices + 6,
      vertices + 7}, 2},
    {{vertices + 5,
      vertices + 1,
      vertices + 2,
      vertices + 6}, 3},
    {{vertices + 2,
      vertices + 3,
      vertices + 7,
      vertices + 6}, 4},
    {{vertices + 0,
      vertices + 4,
      vertices + 7,
      vertices + 3}, 5},
    {{vertices + 0,
      vertices + 1,
      vertices + 5,
      vertices + 4}, 6},
};

// length for cube: 6 edges, 6 vertices in edge (2 triangles)
const int bufferLength = 6 * 6;

void writeEdge(QVector<VertexData>& arrayBuffer, QVector<int>& indexBuffer, const Edge* edge) {
    QVector3D normal = edge->normal();
    int startIndex = arrayBuffer.size();
    for (int i = 0; i < edge->vertices.size(); i++) {
        VertexData data;
        data.vertex = *edge->vertices[i];
        data.color = figureColor;
        data.normal = normal;
        arrayBuffer.append(data);
    }
    for (int i = 2; i < edge->vertices.size(); i++) {
        indexBuffer.append(startIndex);
        indexBuffer.append(startIndex + i - 1);
        indexBuffer.append(startIndex + i);
    }
}

// --------------- temp code end ----------------

Drawer::Drawer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
}

Drawer::~Drawer() {
    _baseProgram->release();
    _baseProgram->removeAllShaders();
}

void Drawer::initBaseProgram() {
    _baseProgram = new QOpenGLShaderProgram(this);
    if (!_baseProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/base.vert")) {
        std::cout << "Cannot compile vertex shader" << std::endl;
        std::cout << _baseProgram->log().data() << std::endl;
        return;
    }
    _baseProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/base.frag");

    if (_baseProgram->link()) {
        std::cout << "Shaders program was linked" << std::endl;
    } else {
        std::cout << "Cannot link shaders program" << std::endl;
    }
}
void Drawer::initMatrices() {
    _proj = QMatrix4x4();
    _view = QMatrix4x4();
    _model = QMatrix4x4();

    _view.lookAt(QVector3D(4, 4, 4), QVector3D(0, 0, 0), QVector3D(0, 0, 1));
}

void Drawer::initArrays() {
    _verticesArray.clear();
    _indexesArray.clear();
    for (const Edge& edge : edges) {
        writeEdge(_verticesArray, _indexesArray, &edge);
    }
}

void Drawer::initializeGL() {
    initBaseProgram();
    initMatrices();
    initArrays();

    _verticesArrayBuffer.create();
    _verticesArrayBuffer.bind();
    _verticesArrayBuffer.allocate(_verticesArray.constData(), _verticesArray.size() * sizeof(VertexData));
    _verticesArrayBuffer.release();

    _indexesArrayBuffer.create();
    _indexesArrayBuffer.bind();
    _indexesArrayBuffer.allocate(_indexesArray.constData(), _indexesArray.size() * sizeof(GLuint));
    _indexesArrayBuffer.release();
}

void Drawer::resizeGL(int w, int h) {
    _proj = QMatrix4x4();
    _proj.perspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
}

void Drawer::paintGL() {
    if (!_baseProgram->isLinked()) {
        std::cout << "Shaders are not linked; can not paint" << std::endl;
        return;
    }

    if (!_baseProgram->bind()) {
        std::cout << "Cannot bind shaders; can not paint" << std::endl;
        return;
    }
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int projLoc = _baseProgram->uniformLocation("proj");
    int viewLoc = _baseProgram->uniformLocation("view");
    int modelLoc = _baseProgram->uniformLocation("model");
    int lightColorLocation = _baseProgram->uniformLocation("lightColor");
    int lightPositionLocation = _baseProgram->uniformLocation("lightPosition");
    _baseProgram->setUniformValue(projLoc, _proj);
    _baseProgram->setUniformValue(viewLoc, _view);
    _baseProgram->setUniformValue(modelLoc, _model);
    _baseProgram->setUniformValue(lightColorLocation, lightColor);
    _baseProgram->setUniformValue(lightPositionLocation, lightPosition);


    int vertexLocation = _baseProgram->attributeLocation("vertex");
    int colorLocation = _baseProgram->attributeLocation("color");
    int normalLocation = _baseProgram->attributeLocation("normal");

    _verticesArrayBuffer.bind();

    int offset = 0;
    _baseProgram->enableAttributeArray(vertexLocation);
    _baseProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);
    _baseProgram->enableAttributeArray(colorLocation);
    _baseProgram->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VertexData));

    offset += sizeof(QVector4D);
    _baseProgram->enableAttributeArray(normalLocation);
    _baseProgram->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    _indexesArrayBuffer.bind();

    glDrawElements(GL_TRIANGLES, _indexesArray.size(), GL_UNSIGNED_INT, 0);
}

void Drawer::mouseMoveEvent(QMouseEvent* event) {
}

void Drawer::mousePressEvent(QMouseEvent* event) {
}

void Drawer::mouseReleaseEvent(QMouseEvent *event) {
}
