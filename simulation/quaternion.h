#ifndef QUATERNION_H
#define QUATERNION_H

#include <QVector3D>
#include <QMatrix4x4>

class Quaternion
{
private:
    float s;
    QVector3D v;
public:
    Quaternion(float s = 1, QVector3D v = QVector3D(0, 0, 0));
    Quaternion &operator*=(float factor);
    Quaternion &operator+=(const Quaternion &other);
    QMatrix4x4 toMatrix();
    std::string toString() {
        return QString(QString::number(s) + " " + QString::number(v.x()) + " " + QString::number(v.y()) + " " + QString::number(v.z()) + " ").toUtf8().constData();
    }
    friend const Quaternion operator+(const Quaternion &q1, const Quaternion &q2);
    friend const Quaternion operator*(const Quaternion &q1, const Quaternion &q2);
    friend const Quaternion operator*(float factor, const Quaternion &quaternion);
    friend const Quaternion operator*(const Quaternion &quaternion, float factor);
    friend const Quaternion operator/(const Quaternion &quaternion, float divisor);
};

#endif // QUATERNION_H
