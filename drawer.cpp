﻿#include "cube.h"
#include "drawer.h"
#include <QMatrix3x3>
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>
#include <simulation/iceberg.h>

Drawer::Drawer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    QObject::connect(&_viewWrapper, SIGNAL(changed()), this, SLOT(depenceChanged()));
    QObject::connect(&_figureWrapper, SIGNAL(changed()), this, SLOT(depenceChanged()));

    _figureWrapper.viewWrapper = &_viewWrapper;
    _lights = {
        &LightConfig::base,
        &LightConfig::baseReversed,
    };
}

Drawer::~Drawer() {
}

void Drawer::addFigure(Figure *figure) {
    _figures.append(figure);
    QObject::connect(figure, SIGNAL(changed()), this, SLOT(depenceChanged()));

    if (isValid()) {
        makeCurrent();
        figure->attach(context());
    }
}

bool Drawer::removeFigure(Figure *figure) {
    if (!_figures.removeOne(figure)) {
        return false;
    }

    makeCurrent();
    figure->detach();
    QObject::disconnect(figure, SIGNAL(changed()), this, SLOT(depenceChanged()));
    return true;
}

void Drawer::depenceChanged() {
    update();
}

void Drawer::initializeGL() {
    auto funcs = getFuncs();
    funcs->initializeOpenGLFunctions();
    funcs->glClearColor(1, 1, 1, 1);
    for (Figure* figure : _figures) {
        figure->attach(context());
    }
}

void Drawer::resizeGL(int w, int h) {
    auto funcs = getFuncs();
    float ratio = static_cast<float>(w) / static_cast<float>(h);
    projPerspective = QMatrix4x4();
    projPerspective.perspective(45.0f, ratio, 0.1f, 2000.0f);
    funcs->glViewport(0, 0, w, h);
}

void writeColor(float* to, const QColor& color) {
    to[0] = static_cast<GLfloat>(color.redF());
    to[1] = static_cast<GLfloat>(color.greenF());
    to[2] = static_cast<GLfloat>(color.blueF());
    to[3] = static_cast<GLfloat>(color.alphaF());
}

void Drawer::paintGL() {
    auto funcs = getFuncs();
    funcs->glClearColor(1, 1, 1, 1);
    funcs->glEnable(GL_ALPHA_TEST);
    funcs->glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    funcs->glEnable(GL_BLEND);
    funcs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    funcs->glFrontFace(GL_CCW);

    funcs->glMatrixMode(GL_MODELVIEW);
    QMatrix4x4 view = _viewWrapper.matrix();
    funcs->glLoadMatrixf(view.constData());

    funcs->glMatrixMode(GL_PROJECTION);
    funcs->glLoadIdentity();

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    for (auto light : _lights)
        light->use(context());

    funcs->glMatrixMode(GL_PROJECTION);
    funcs->glLoadIdentity();
    funcs->glLoadMatrixf(projPerspective.constData());

    for (Figure* figure : _figures) {
        funcs->glMatrixMode(GL_MODELVIEW);
        funcs->glLoadMatrixf((view * figure->model()).constData());

        figure->paint();
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

IcebergQOpenGLFunctions* Drawer::getFuncs() {
    auto testFuncs = context()->versionFunctions();
    auto funcs = context()->versionFunctions<IcebergQOpenGLFunctions>();
    if (!funcs) {
        qDebug() << "Versions functions are null\n";
        exit(1);
    }
    return funcs;
}

FigureWrapper& Drawer::figureWrapper() {
    return _figureWrapper;
}

ViewMatrixWrapper& Drawer::viewWrapper() {
    return _viewWrapper;
}
