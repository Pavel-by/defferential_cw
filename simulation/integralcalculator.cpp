#include "integralcalculator.h"

void IntegralCalculator::compProjectionIntegrals(Edge * f)
{
    double a0, a1, da;
    double b0, b1, db;
    double a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
    double a1_2, a1_3, b1_2, b1_3;
    double C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
    double Cab, Kab, Caab, Kaab, Cabb, Kabb;
    int i;

    P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0;

    for (i = 0; i < f->vertices.length(); i++) {
        a0 = f->vertices[i][A];
        b0 = f->vertices[i][B];
        a1 = f->vertices[(i + 1) % f->vertices.length()][A];
        b1 = f->vertices[(i + 1) % f->vertices.length()][B];
        da = a1 - a0;
        db = b1 - b0;
        a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
        b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
        a1_2 = a1 * a1; a1_3 = a1_2 * a1;
        b1_2 = b1 * b1; b1_3 = b1_2 * b1;

        C1 = a1 + a0;
        Ca = a1 * C1 + a0_2; Caa = a1 * Ca + a0_3; Caaa = a1 * Caa + a0_4;
        Cb = b1 * (b1 + b0) + b0_2; Cbb = b1 * Cb + b0_3; Cbbb = b1 * Cbb + b0_4;
        Cab = 3 * a1_2 + 2 * a1*a0 + a0_2; Kab = a1_2 + 2 * a1*a0 + 3 * a0_2;
        Caab = a0 * Cab + 4 * a1_3; Kaab = a1 * Kab + 4 * a0_3;
        Cabb = 4 * b1_3 + 3 * b1_2*b0 + 2 * b1*b0_2 + b0_3;
        Kabb = b1_3 + 2 * b1_2*b0 + 3 * b1*b0_2 + 4 * b0_3;

        P1 += db * C1;
        Pa += db * Ca;
        Paa += db * Caa;
        Paaa += db * Caaa;
        Pb += da * Cb;
        Pbb += da * Cbb;
        Pbbb += da * Cbbb;
        Pab += db * (b1*Cab + b0 * Kab);
        Paab += db * (b1*Caab + b0 * Kaab);
        Pabb += da * (a1*Cabb + a0 * Kabb);
    }

    P1 /= 2.0;
    Pa /= 6.0;
    Paa /= 12.0;
    Paaa /= 20.0;
    Pb /= -6.0;
    Pbb /= -12.0;
    Pbbb /= -20.0;
    Pab /= 24.0;
    Paab /= 60.0;
    Pabb /= -60.0;
}

void IntegralCalculator::compFaceIntegrals(Edge * f)
{
    QVector3D n;
    double w;
    double k1, k2, k3, k4;

    compProjectionIntegrals(f);

    w = - f->normal()[X] * f->vertices[0][X]
            - f->normal()[Y] * f->vertices[0][Y]
            - f->normal()[Z] * f->vertices[0][Z];

    n = f->normal();
    k1 = 1 / n[C]; k2 = k1 * k1; k3 = k2 * k1; k4 = k3 * k1;

    Fa = k1 * Pa;
    Fb = k1 * Pb;
    Fc = -k2 * (n[A] * Pa + n[B] * Pb + w * P1);

    Faa = k1 * Paa;
    Fbb = k1 * Pbb;
    Fcc = k3 * (SQR(n[A])*Paa + 2 * n[A] * n[B] * Pab + SQR(n[B])*Pbb
        + w * (2 * (n[A] * Pa + n[B] * Pb) + w * P1));

    Faaa = k1 * Paaa;
    Fbbb = k1 * Pbbb;
    Fccc = -k4 * (CUBE(n[A])*Paaa + 3 * SQR(n[A])*n[B] * Paab
        + 3 * n[A] * SQR(n[B])*Pabb + CUBE(n[B])*Pbbb
        + 3 * w*(SQR(n[A])*Paa + 2 * n[A] * n[B] * Pab + SQR(n[B])*Pbb)
        + w * w*(3 * (n[A] * Pa + n[B] * Pb) + w * P1));

    Faab = k1 * Paab;
    Fbbc = -k2 * (n[A] * Pabb + n[B] * Pbbb + w * Pbb);
    Fcca = k3 * (SQR(n[A])*Paaa + 2 * n[A] * n[B] * Paab + SQR(n[B])*Pabb
        + w * (2 * (n[A] * Paa + n[B] * Pab) + w * Pa));
}

void IntegralCalculator::compVolumeIntegrals(Polyhedron *p)
{
    Edge *f;
    double nx, ny, nz;
    int i;

    T0 = T1[X] = T1[Y] = T1[Z]
        = T2[X] = T2[Y] = T2[Z]
        = TP[X] = TP[Y] = TP[Z] = 0;

    for (i = 0; i < p->faces.length(); i++) {

        f = &(p->faces[i]);

        nx = fabs(f->normal()[X]);
        ny = fabs(f->normal()[Y]);
        nz = fabs(f->normal()[Z]);
        if (nx > ny && nx > nz) C = X;
        else C = (ny > nz) ? Y : Z;
        A = (C + 1) % 3;
        B = (A + 1) % 3;

        compFaceIntegrals(f);

        T0 += f->normal()[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

        T1[A] += f->normal()[A] * Faa;
        T1[B] += f->normal()[B] * Fbb;
        T1[C] += f->normal()[C] * Fcc;
        T2[A] += f->normal()[A] * Faaa;
        T2[B] += f->normal()[B] * Fbbb;
        T2[C] += f->normal()[C] * Fccc;
        TP[A] += f->normal()[A] * Faab;
        TP[B] += f->normal()[B] * Fbbc;
        TP[C] += f->normal()[C] * Fcca;
    }

    T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
    T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
    TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;
}

double IntegralCalculator::getMass(Polyhedron *p)
{
    return p->density * T0;
}

QVector3D IntegralCalculator::getCenterOfMass(Polyhedron *p)
{
    QVector3D c;
    if (T0 != 0) {
        c[X] = T1[X] / T0;
        c[Y] = T1[Y] / T0;
        c[Z] = T1[Z] / T0;
    }
    return c;
}

QMatrix4x4 IntegralCalculator::getInertiaTensor(Polyhedron *p)
{
    compVolumeIntegrals(p);
    double mass = getMass(p);
    QVector3D c = getCenterOfMass(p);

    QMatrix4x4 J;
    J(X, X) = p->density * (T2[Y] + T2[Z]);
    J(Y, Y) = p->density * (T2[Z] + T2[X]);
    J(Z, Z) = p->density * (T2[X] + T2[Y]);
    J(X, Y) = J(Y, X) = -p->density * TP[X];
    J(Y, Z) = J(Z, Y) = -p->density * TP[Y];
    J(Z, X) = J(X, Z) = -p->density * TP[Z];

    J(X, X) -= mass * (c[Y] * c[Y] + c[Z] * c[Z]);
    J(Y, Y) -= mass * (c[Z] * c[Z] + c[X] * c[X]);
    J(Z, Z) -= mass * (c[X] * c[X] + c[Y] * c[Y]);
    J(X, Y) = J(Y, X) += mass * c[X] * c[Y];
    J(Y, Z) = J(Z, Y) += mass * c[Y] * c[Z];
    J(Z, X) = J(X, Z) += mass * c[Z] * c[X];

    return J;
}
