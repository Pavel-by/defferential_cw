#include "cube.h"
#include "drawer.h"
#include <QMatrix3x3>
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>

// --------------- temp code --------------------
const QVector3D lightPosition = QVector3D(5.0f, 10.0f, 40.0f);
const QVector3D lightColor = QVector3D(1.0f, 1.0f, 1.0f);
// --------------- temp code end ----------------

Drawer::Drawer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    proj = QMatrix4x4();
    QObject::connect(&_viewWrapper, SIGNAL(changed()), this, SLOT(depenceChanged()));
    QObject::connect(&_figureWrapper, SIGNAL(changed()), this, SLOT(depenceChanged()));

    _figures = {
        Cube(),
        Cube(),
    };

    _figures[1].color = QVector4D(0.0f, 1.0f, 0.0f, 0.7f);
    _figures[1].scale(QVector3D(3, 3, 2));

    _figures[1].model.translate(QVector3D(0, 0, -3));

    _figureWrapper.figure = &_figures[0];
    _figureWrapper.viewWrapper = &_viewWrapper;
}

Drawer::~Drawer() {
}

void Drawer::depenceChanged() {
    update();
}

void Drawer::initializeGL() {
    _programWrapper.initialize();
    _programWrapper.bind();

    for (Figure& figure : _figures) {
        figure.initialize(_programWrapper.program());
    }
}

void Drawer::resizeGL(int w, int h) {
    proj = QMatrix4x4();
    proj.perspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
}

void Drawer::paintGL() {
    if (!_programWrapper.bind()) {
        std::cout << "Can not paint: shaders program is not binded" << std::endl;
        return;
    }
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QOpenGLShaderProgram* program = _programWrapper.program();

    program->setUniformValue("proj", proj);
    program->setUniformValue("view", _viewWrapper.matrix());
    program->setUniformValue("lightPosition", lightPosition);
    program->setUniformValue("lightColor", lightColor);

    for (Figure& figure : _figures) {
        figure.paint(program);
    }
}

void Drawer::mouseMoveEvent(QMouseEvent* event) {
    event->setAccepted(false);
    _viewWrapper.mouseMoveEvent(event);
    _figureWrapper.mouseMoveEvent(event);
    event->accept();
}

void Drawer::mousePressEvent(QMouseEvent* event) {
    event->setAccepted(false);
    _viewWrapper.mousePressEvent(event);
    _figureWrapper.mousePressEvent(event);
    event->accept();
}

void Drawer::mouseReleaseEvent(QMouseEvent *event) {
    event->setAccepted(false);
    _viewWrapper.mouseReleaseEvent(event);
    _figureWrapper.mouseReleaseEvent(event);
    event->accept();
}

void Drawer::keyPressEvent(QKeyEvent *event) {
    _viewWrapper.keyPressEvent(event);
}

void Drawer::keyReleaseEvent(QKeyEvent *event) {
    _viewWrapper.keyReleaseEvent(event);
}
