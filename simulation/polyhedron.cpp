#include "polyhedron.h"
#include "integralcalculator.h"
#include "log.h"

using namespace std;

std::string vtos(QVector3D v) {
    return QString(QString::number(v.x()) + " " + QString::number(v.y()) + " " + QString::number(v.z()) + " ").toUtf8().constData();
}

string mtos(QMatrix4x4 m) {
    return QString(QString::number(m(0, 0)) + " " + QString::number(m(0, 1)) + " " + QString::number(m(0, 2)) + " " + QString::number(m(0, 3)) + "\n" +
                   QString::number(m(1, 0)) + " " + QString::number(m(1, 1)) + " " + QString::number(m(1, 2)) + " " + QString::number(m(1, 3)) + "\n" +
                   QString::number(m(2, 0)) + " " + QString::number(m(2, 1)) + " " + QString::number(m(2, 2)) + " " + QString::number(m(2, 3)) + "\n" +
                   QString::number(m(3, 0)) + " " + QString::number(m(3, 1)) + " " + QString::number(m(3, 2)) + " " + QString::number(m(3, 3))).toUtf8().constData();
}

Polyhedron::Polyhedron(QVector<Edge> faces, double density)
{
    int numFaces = faces.length();
    this->faces.resize(numFaces);
    for (int i = 0; i < numFaces; i++) {
        this->faces[i] = (const Edge&) faces[i];
    }
    if (density > DENSITY_OF_WATER) {
        this->density = 916.7;
    } else {
        this->density = density;
    }

    IntegralCalculator calc;
    J = calc.getInertiaTensor(this);
    c = calc.getCenterOfMass(this);
    mass = calc.getMass(this);
}

void Polyhedron::computeNewState(float h) {
    /*
        k1 = x_dot(x)
        k2 = x_dot(x + h/2*k1)
        k3 = x_dot(x + h/2*k2)
        k4 = x_dot(x + h*k3)
    */
    QVector3D cNew, pNew, LNew;
    Quaternion qNew;

    QVector3D k1_c, k1_p, k1_L;
    Quaternion k1_q;
    x_dot(c, p, q, L, k1_c, k1_p, k1_q, k1_L);

    QVector3D k2_c, k2_p, k2_L;
    Quaternion k2_q;
    x_dot(c + h/2*k1_c, p + h/2*k1_p, q + h/2*k1_q, L + h/2*k1_L, k2_c, k2_p, k2_q, k2_L);

    QVector3D k3_c, k3_p, k3_L;
    Quaternion k3_q;
    x_dot(c + h/2*k2_c, p + h/2*k2_p, q + h/2*k2_q, L + h/2*k2_L, k3_c, k3_p, k3_q, k3_L);

    QVector3D k4_c, k4_p, k4_L;
    Quaternion k4_q;
    x_dot(c + h*k3_c, p + h*k3_p, q + h*k3_q, L + h*k3_L, k4_c, k4_p, k4_q, k4_L);

    cNew = c + h*(k1_c/6 + k2_c/3 + k3_c/3 + k4_c/6);
    pNew = p + h*(k1_p/6 + k2_p/3 + k3_p/3 + k4_p/6);
    qNew = q + h*(k1_q/6.0f + k2_q/3.0f + k3_q/3.0f + k4_q/6.0f);
    LNew = L + h*(k1_L/6 + k2_L/3 + k3_L/3 + k4_L/6);
    setState(cNew, pNew, qNew, LNew);
}

QMatrix4x4 skewMatrix(QVector3D vec)
{
    float m[] = {0, -vec.z(), vec.y(), 0,
                 vec.z(), 0, -vec.x(), 0,
                 -vec.y(), vec.x(), 0, 0,
                0, 0, 0, 0};
    return QMatrix4x4(m);
}

void Polyhedron::x_dot(QVector3D c, QVector3D p, Quaternion q, QVector3D L, QVector3D& c_dot, QVector3D& p_dot, Quaternion& q_dot, QVector3D& L_dot)
{
    Polyhedron * tmp = new Polyhedron(this->faces);
    tmp->setState(c, p, q, L);
    c_dot = tmp->p/tmp->mass;
    p_dot = tmp->calculateForces();
    QVector3D tmpOmega = tmp->calculateOmega();
    Quaternion qOmega(0, tmpOmega);
    q_dot = (qOmega * tmp->q) / 2;
    L_dot = tmp->calculateTau();
    delete tmp;
}

QVector3D Polyhedron::calculateOmega() {
    QMatrix4x4 Jcurr = R*J*R.transposed();

    //IntegralCalculator ic;
    //QMatrix4x4 Jcurr = ic.getInertiaTensor(this);

    QMatrix4x4 Jinv = Jcurr.inverted();
    QVector3D omega = Jinv * L;
    return omega;
}

QVector3D Polyhedron::calculateForces()
{
    QVector3D v = p;
    QVector3D mg = mass * G * QVector3D(0, 0, -1);
    underWater();
    return mg + Farch - 0.2*v;
}

QVector3D Polyhedron::calculateTau()
{
    if (Farch == QVector3D(0, 0, 0)) {
        return QVector3D(0, 0, 0);
    }
    return QVector3D::crossProduct(cUnderWater - c, mass * G * QVector3D(0, 0, -1) + Farch) - 0.2*L;
}

void Polyhedron::setState(QVector3D c, QVector3D p, Quaternion q, QVector3D L)
{
    this->p = p;
    this->L = L;
    this->q = q;

    QMatrix4x4 R = q.toMatrix();

    for (int i = 0; i < faces.length(); i++) {
        for (int j = 0; j < faces[i].vertices.length(); j++) {
            faces[i].vertices[j] -= this->c;
            faces[i].vertices[j] = this->R.inverted() * faces[i].vertices[j];
            faces[i].vertices[j] = R * faces[i].vertices[j];
            faces[i].vertices[j] += c;
        }
    }
    this->c = c;
    this->R = R;
}

void Polyhedron::underWater(double zWater)
{
    QVector<QVector<QVector3D>> waterlineVertices;
    waterlineVertices.resize(faces.length());
    QVector<Edge> newFaces;
    for (int i = 0; i < faces.length(); i++) {
        waterlineVertices[i].resize(2);
        QVector<QVector3D> vertices;
        for (int j = 0; j < faces[i].vertices.length(); j++) {
            int ifirst = 0, ilast =0;
            QVector3D curr = faces[i].vertices[j], next = faces[i].vertices[(j + 1) % faces[i].vertices.length()];
            if (curr.z() > zWater && next.z() <= zWater) {
                float lambda = (zWater - curr.z()) / (next.z() - curr.z());
                QVector3D intersec(curr.x() + (next.x() - curr.x())*lambda,
                                   curr.y() + (next.y() - curr.y())*lambda,
                                   zWater);
                waterlineVertices[i][0] = intersec;
                vertices.append(intersec);
                ifirst = (j + 1) % faces[i].vertices.length();
            }
            if (curr.z() <= zWater) {
                vertices.append(QVector3D(faces[i].vertices[j]));
                if (next.z() > zWater) {
                    float lambda = (zWater - curr.z()) / (next.z() - curr.z());
                    QVector3D intersec(curr.x() + (next.x() - curr.x())*lambda,
                                       curr.y() + (next.y() - curr.y())*lambda,
                                       zWater);
                    waterlineVertices[i][1] = intersec;
                    vertices.append(intersec);
                    ilast = j;
                }
            }
        }
        if (vertices.length() > 0) {
            Edge newFace;
            newFace.vertices = vertices;
            newFaces.append(newFace);
        }
    }
    if (waterlineVertices.length() > 0) {
        Edge newEdge = createWaterlineEdge(waterlineVertices);
        if (newEdge.vertices.length() > 0) {
            newFaces.append(newEdge);
        }
    }

    Polyhedron * uw = new Polyhedron(newFaces, density);
    float volume = uw->mass / uw->density;
    Farch = DENSITY_OF_WATER * G * volume * QVector3D(0, 0, 1);
    cUnderWater = uw->c;
    delete uw;
}

Edge Polyhedron::createWaterlineEdge(QVector<QVector<QVector3D>> waterlineVertices)
{
    Edge newEdge;
    QVector<QVector3D> vertices;

    for (int i = 0; i < waterlineVertices.length(); i++) {
        if (isSamePoint(waterlineVertices[i][0], waterlineVertices[i][1])) {
            continue;
        } else {
            vertices.append(waterlineVertices[i][0]);
            vertices.append(waterlineVertices[i][1]);
            waterlineVertices.remove(i);
            break;
        }
    }
    if (vertices.length() == 0) {
        return {};
    }
    while (!isSamePoint(vertices.first(), vertices.last()) && waterlineVertices.length() > 0) {
        for (int i = 0; i < waterlineVertices.length(); i++) {
            if (isSamePoint(waterlineVertices[i][0], waterlineVertices[i][1])) {
                waterlineVertices.remove(i);
            } else if (isSamePoint(vertices.last(), waterlineVertices[i][0])) {
                vertices.append(waterlineVertices[i][1]);
                waterlineVertices.remove(i);
                break;
            }
        }
    }
    if (isSamePoint(vertices.first(), vertices.last())) {
        vertices.remove(vertices.length() - 1);
    } else {
        vertices.clear();
    }
    newEdge.vertices = vertices;
    return newEdge;
}

const QMatrix4x4& Polyhedron::getR() {
    return R;
}

const QVector3D& Polyhedron::getC() {
    return c;
}
bool Polyhedron::isSamePoint(float first, float second) {
    return std::abs(first - second) < 0.000001f;
}

bool Polyhedron::isSamePoint(const QVector3D& first, const QVector3D& second) {
    return isSamePoint(first.distanceToPoint(second), 0.0f);
}
