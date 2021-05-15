#include "lightconfig.h"

LightConfig LightConfig::base = {
    GL_LIGHT0,
    QVector4D(100, 100, 100, 1),
    QColor(62, 62, 62),
    QColor(184, 246, 255),
    QColor(149, 255, 237),
};
LightConfig LightConfig::baseReversed = {
    GL_LIGHT1,
    QVector3D(1, 1, 1),
    QColor(62, 62, 62),
    QColor(90, 123, 128),
    QColor(75, 128, 118),
};
