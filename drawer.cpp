#include "cube.h"
#include "drawer.h"
#include "iceberg.h"
#include <QMatrix3x3>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QWheelEvent>
#include <iostream>

Drawer::Drawer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    QObject::connect(&_viewWrapper, SIGNAL(changed()), this, SLOT(depenceChanged()));

    float minTranslate = -10, maxTranslate = 10;
    float minRotate = 0, maxRotate = 360;

    QRandomGenerator *generator = QRandomGenerator::global();

    for (Figure* f : _figures) {
        QObject::connect(f, SIGNAL(changed()), this, SLOT(depenceChanged()));
    }

    Iceberg* iceb = new Iceberg();
    _figures.append(iceb);

    for (Figure* f: _figures) {
        QObject::connect(f, SIGNAL(changed()), this, SLOT(depenceChanged()));
    }

    /*for (int i = 0; i < 1; i++) {
        Figure *figure = new Cube();
        figure->translate(
                    QVector3D(
                        static_cast<float>(generator->generateDouble()) * (maxTranslate - minTranslate) + minTranslate,
                        static_cast<float>(generator->generateDouble()) * (maxTranslate - minTranslate) + minTranslate,
                        static_cast<float>(generator->generateDouble()) * (maxTranslate - minTranslate) + minTranslate
                        ));
        figure->rotate(
                    static_cast<float>(generator->generateDouble()) * (maxRotate - minRotate) + minRotate,
                    QVector3D(
                        static_cast<float>(generator->generateDouble()) * 2 - 1,
                        static_cast<float>(generator->generateDouble()) * 2 - 1,
                        static_cast<float>(generator->generateDouble()) * 2 - 1
                        ));
        QObject::connect(figure, SIGNAL(changed()), this, SLOT(depenceChanged()));
        _figures.append(figure);
    }*/
}

Drawer::~Drawer() {
}

void Drawer::depenceChanged() {
    update();
}

void Drawer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1, 1, 1, 1);
    for (Figure* figure : _figures) {
        figure->initialize();
    }
}

void Drawer::resizeGL(int w, int h) {
    float ratio = static_cast<float>(w) / static_cast<float>(h);
    projPerspective = QMatrix4x4();
    projPerspective.perspective(45.0f, ratio, 0.1f, 100.0f);
}

void writeColor(float* to, const QColor& color) {
    to[0] = static_cast<GLfloat>(color.redF());
    to[1] = static_cast<GLfloat>(color.greenF());
    to[2] = static_cast<GLfloat>(color.blueF());
    to[3] = static_cast<GLfloat>(color.alphaF());
}

void Drawer::paintGL() {
    glClearColor(1, 1, 1, 1);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFrontFace(GL_CCW);

    QMatrix4x4 view = _viewWrapper.matrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view.constData());


    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projPerspective.constData());

    for (Figure* figure : _figures) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf((view * figure->model()).constData());

        figure->paint();
    }
}


void Drawer::mouseMoveEvent(QMouseEvent* event) {
    event->setAccepted(false);
    _viewWrapper.mouseMoveEvent(event);
    event->accept();
}

void Drawer::mousePressEvent(QMouseEvent* event) {
    event->setAccepted(false);
    _viewWrapper.mousePressEvent(event);
    event->accept();
}

void Drawer::mouseReleaseEvent(QMouseEvent *event) {
    event->setAccepted(false);
    _viewWrapper.mouseReleaseEvent(event);
    event->accept();
}

void Drawer::keyPressEvent(QKeyEvent *event) {
    _viewWrapper.keyPressEvent(event);
}

void Drawer::keyReleaseEvent(QKeyEvent *event) {
    _viewWrapper.keyReleaseEvent(event);
}
