#include "water.h"

#include <light/materialconfig.h>

Water::Water() {
    scale(QVector3D(4, 4, 1000));
    translate(QVector3D(0, 0, -1000));
    materials = {
        &MaterialConfig::waterFront,
        &MaterialConfig::waterBack,
    };
}
