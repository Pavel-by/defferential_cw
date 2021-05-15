#include <QColor>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Compatibility>
#include <QVector3D>
#include <QVector4D>

#pragma once

struct LightConfig {
    static LightConfig base;
    static LightConfig baseReversed;

    GLenum light;
    QVector4D direction;
    QColor lightAmbient;
    QColor lightDiffuse;
    QColor lightSpecular;

    void use(QOpenGLContext *context) {
        auto funcs = context->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
        glEnable(light);
        float c[4];
        writeColor(c, lightAmbient);
        funcs->glLightfv(light, GL_AMBIENT, c);
        writeColor(c, lightDiffuse);
        funcs->glLightfv(light, GL_DIFFUSE, c);
        writeColor(c, lightSpecular);
        funcs->glLightfv(light, GL_SPECULAR, c);
        QVector4D tempV = -direction;
        funcs->glLightfv(light, GL_POSITION, reinterpret_cast<GLfloat*>(&tempV));
    }

    void writeColor(float* to, const QColor& color) {
        to[0] = static_cast<GLfloat>(color.redF());
        to[1] = static_cast<GLfloat>(color.greenF());
        to[2] = static_cast<GLfloat>(color.blueF());
        to[3] = static_cast<GLfloat>(color.alphaF());
    }
};
