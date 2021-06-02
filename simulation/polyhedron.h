#pragma once

#include <QMatrix4x4>
#include <math.h>
#include "figure.h"
#include "quaternion.h"
#include "state.h"

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

    void x_dot(State state, State& state_dot);
    void underWater(double zWater = 0);
    void setState(State state);

    bool isSamePoint(float value, float second);
    bool isSamePoint(const QVector3D& first, const QVector3D& second);

public:
    Polyhedron(QVector<Edge> faces, double density = 916.7);

    QVector<Edge> faces;
    QVector<Edge> underwaterFaces;
	double density, mass;

    void computeNewState(float h);

    const QMatrix4x4& model();

    const QMatrix4x4& getR();
    const QVector3D& getC();
};

