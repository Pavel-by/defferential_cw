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
        x_n+1 = x_n + h(1/6*k1 + 1/3*k2 + 1/3*k3 + 1/6*k4)
    */
    State oldState(this->c, this->p, this->q, this->L);
    State k1, k2, k3, k4;
    x_dot(oldState, k1);
    x_dot(oldState + h/2.0f*k1, k2);
    x_dot(oldState + h/2.0f*k2, k3);
    x_dot(oldState + h*k3, k4);
    State newState = oldState + h*(k1/6.0f + k2/3.0f + k3/3.0f + k4/6.0f);
    setState(newState);
    underWater(0);
    QVector3D omega = calculateOmega();
    QVector3D omJ = omega * J;
    double rotationEnergy = 0.5 * QVector3D::dotProduct(omJ, omega);
    cout << "h: " << c.z() << endl;
    cout << "Rot en: " << rotationEnergy << endl;
}

QMatrix4x4 skewMatrix(QVector3D vec)
{
    float m[] = {0, -vec.z(), vec.y(), 0,
                 vec.z(), 0, -vec.x(), 0,
                 -vec.y(), vec.x(), 0, 0,
                0, 0, 0, 0};
    return QMatrix4x4(m);
}

void Polyhedron::x_dot(State state, State& state_dot)
{
    QVector<Edge> newFaces;
    newFaces.resize(faces.length());
    for (int i = 0; i < faces.length(); i++) {
        newFaces[i].vertices.resize(faces[i].vertices.length());
        for (int j = 0; j < faces[i].vertices.length(); j++) {
            newFaces[i].vertices[j] = faces[i].vertices[j] - this->c;
            newFaces[i].vertices[j] = this->R.inverted() * faces[i].vertices[j];
        }
    }
    Polyhedron * tmp = new Polyhedron(newFaces);
    tmp->setState(state);
    state_dot.c = tmp->p/tmp->mass;
    state_dot.p = tmp->calculateForces();
    QVector3D tmpOmega = tmp->calculateOmega();
    Quaternion qOmega(0, tmpOmega);
    state_dot.q = (qOmega * tmp->q) / 2.0f;
    state_dot.L = tmp->calculateTau();
    delete tmp;
}

QVector3D Polyhedron::calculateOmega() {
    QVector3D omega = R*J.inverted()*R.transposed() * L;
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
    float k = 0.2*(this->density * Farch.z() / DENSITY_OF_WATER / G / this->mass);
    QVector3D tau = 0.5 * QVector3D::crossProduct(cUnderWater - c, Farch) - k*L;
    return tau;
}

void Polyhedron::setState(State state)
{
    this->p = state.p;
    this->L = state.L;
    this->q = state.q;
    this->q.normalize();

    QMatrix4x4 R = this->q.toMatrix();

    for (int i = 0; i < faces.length(); i++) {
        for (int j = 0; j < faces[i].vertices.length(); j++) {
            faces[i].vertices[j] -= this->c;
            faces[i].vertices[j] = this->R.inverted() * faces[i].vertices[j];
            faces[i].vertices[j] = R * faces[i].vertices[j];
            faces[i].vertices[j] += state.c;
        }
    }
    this->c = state.c;
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
    underwaterFaces = newFaces;
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
    return std::abs(first - second) < 0.00001f;
}

bool Polyhedron::isSamePoint(const QVector3D& first, const QVector3D& second) {
    return isSamePoint((first -second).length(), 0.0f);
}
