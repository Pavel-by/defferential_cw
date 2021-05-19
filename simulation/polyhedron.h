#pragma once

#include <QMatrix4x4>
#include <math.h>
#include "figure.h"
#include "quaternion.h"

#define DENSITY_OF_WATER 1030 /* kg/m3 */
#define G 9.80665


class Polyhedron
{
private:
    QMatrix4x4 J;   /* inertia tensor */
    QVector3D c; /*center of mass*/
    QVector3D p; /* linear momentum */
    QMatrix4x4 R; /* rotation matrix */
    QVector3D L; /* angular momentum */
    Quaternion q; /* quaternion of rotation */

    QVector3D Farch;
    QVector3D cUnderWater;
    QVector3D calculateForces();
    QVector3D calculateOmega();
    QVector3D calculateTau();

    Edge createWaterlineEdge(QVector<QVector<QVector3D>> waterlineVertices);

    void x_dot(QVector3D c, QVector3D p, Quaternion q, QVector3D L, QVector3D& c_dot, QVector3D& p_dot, Quaternion& q_dot, QVector3D& L_dot);
    void underWater(double zWater = 0);
    void setState(QVector3D c, QVector3D p, Quaternion q, QVector3D L);

    bool isSamePoint(float value, float second);
    bool isSamePoint(const QVector3D& first, const QVector3D& second);

public:
    Polyhedron(QVector<Edge> faces, double density = 916.7);

    QVector<Edge> faces;
	double density, mass;

    void computeNewState(float h);

    const QMatrix4x4& model();

    const QMatrix4x4& getR();
    const QVector3D& getC();
};

