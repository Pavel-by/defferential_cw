#include "materialconfig.h";


MaterialConfig MaterialConfig::waterFront = {
    GL_FRONT,
    QColor(9, 47, 13, 50),
    QColor(66, 255, 14, 140),
    QColor(48, 161, 0, 100),
    QColor(),
    50,
};
MaterialConfig MaterialConfig::waterBack = {
    GL_BACK,
    QColor(9, 47, 13, 0),
    QColor(66, 255, 14, 0),
    QColor(48, 161, 0, 0),
    QColor(),
    50,
};
MaterialConfig MaterialConfig::iceberg = {
    GL_FRONT,
    QColor(47, 47, 47),
    QColor(198, 221, 255),
    QColor(0, 0, 0),
    //QColor(102, 140, 161),
    QColor(),
    128,
};
MaterialConfig MaterialConfig::icebergBack = {
    GL_BACK,
    QColor(47, 47, 47),
    QColor(198, 221, 255),
    QColor(102, 140, 161),
    QColor(255, 0, 0),
    128,
};
