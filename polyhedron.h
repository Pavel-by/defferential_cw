﻿#pragma once

#include <math.h>
#include "drawer.h"
#include "integralcalculator.h"

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

    QVector3D Farch;
    QVector3D cUnderWater;
    QVector3D calculateForces();
    QVector3D calculateOmega();
    QVector3D calculateTau();

    Edge createWaterlineEdge(QVector<QVector<QVector3D>> waterlineVertices);

    void x_dot(QVector3D c, QVector3D p, QMatrix4x4 R, QVector3D L, QVector3D& c_dot, QVector3D& p_dot, QMatrix4x4& R_dot, QVector3D& L_dot);

public:
    Polyhedron(QVector<Edge> faces, double density = 916.7);

    QVector<Edge> faces;
	double density, mass;

    void setState(QVector3D c, QVector3D p, QMatrix4x4 R, QVector3D L);
    void computeNewState(float h);
    void underWater(double zWater = 0);
};
