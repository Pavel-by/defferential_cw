#ifndef STATE_H
#define STATE_H

#include <QVector3D>
#include "quaternion.h"


struct State
{
    QVector3D c;
    QVector3D p;
    Quaternion q;
    QVector3D L;
    State(QVector3D c = QVector3D(0, 0, 0), QVector3D p = QVector3D(0, 0, 0), Quaternion q = Quaternion(), QVector3D L = QVector3D(0, 0, 0));
    friend const State operator*(float factor, const State &state);
    friend const State operator*(const State &state, float factor);
    friend const State operator+(const State &s1, const State &s2);
    friend const State operator/(const State &state, float divisor);

};


#endif // STATE_H
