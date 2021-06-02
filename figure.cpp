#include "figure.h"
#include <iostream>

Figure::Figure():QObject()
{

}

Figure::~Figure() {
    if (isAttached())
        detach();
}

IcebergQOpenGLFunctions *Figure::getFuncs() {
    auto funcs = context->versionFunctions<IcebergQOpenGLFunctions>();
    if (!funcs) {
        qDebug() << "Versions functions are null\n";
        exit(1);
    }
    return funcs;
}

void Figure::attach(QOpenGLContext *context) {
    assert(this->context == nullptr);
    this->context = context;
}

void Figure::paint() {
    auto funcs = getFuncs();

    for (MaterialConfig *config : materials) {
        config->use(context);
    }

    funcs->glColor3f(1,0,0);
    for (const Edge& edge : edges) {
        QVector3D normal = edge.normal();

        if (_drawRibs) {
            funcs->glBegin(GL_LINE_LOOP);
            for (const QVector3D& v : edge.vertices)
                glVertex3d(v.x(), v.y(), v.z());
            funcs->glEnd();
        } else {
            funcs->glNormal3f(normal.x(), normal.y(), normal.z());
            glBegin(GL_POLYGON);
            for (const QVector3D& v : edge.vertices)
                glVertex3d(v.x(), v.y(), v.z());
            glEnd();
        }
    }
}

void Figure::detach() {
    assert(isAttached());
    context = nullptr;
}

bool Figure::isAttached() {
    return context != nullptr;
}

void Figure::markNeedsPaint() {
    emit changed();
}

void Figure::rotate(float angle, const QVector3D& vector) {
    _modelRotation.rotate(angle, vector);
}

void Figure::scale(QVector3D vector) {
    _modelScale.scale(vector);
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

void Figure::setDrawRibs(bool drawRibs) {
    _drawRibs = drawRibs;
    markNeedsPaint();
}
