#include "water.h"

#include <light/materialconfig.h>

Water::Water() {
    scale(QVector3D(4, 4, 4));
    translate(QVector3D(0, 0, -4));
    materials = {
        &MaterialConfig::waterFront,
        &MaterialConfig::waterBack,
    };
}
