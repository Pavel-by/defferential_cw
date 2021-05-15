#ifndef INTEGRALCALCULATOR_H
#define INTEGRALCALCULATOR_H

#include <QMatrix4x4>
#include "polyhedron.h"


#define X 0
#define Y 1
#define Z 2

#define SQR(x) ((x)*(x))
#define CUBE(x) ((x)*(x)*(x))


class IntegralCalculator
{
private:
    int A;   /* alpha */
    int B;   /* beta */
    int C;   /* gamma */

    /* projection integrals */
    double P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

    /* face integrals */
    double Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

    /* volume integrals */
    double T0, T1[3], T2[3], TP[3];

    void compProjectionIntegrals(Edge *f);
    void compFaceIntegrals(Edge *f);
    void compVolumeIntegrals(Polyhedron *p);

public:
    QMatrix4x4 getInertiaTensor(Polyhedron *p);
    QVector3D getCenterOfMass(Polyhedron *p);
    double getMass(Polyhedron *p);
};

#endif // INTEGRALCALCULATOR_H
