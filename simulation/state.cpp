#include "state.h"


State::State(QVector3D c, QVector3D p, Quaternion q, QVector3D L)
{
    this->c = c;
    this->p = p;
    this->q = q;
    this->L = L;
}

const State operator/(const State &state, float divisor)
{
    return State(state.c / divisor, state.p / divisor, state.q / divisor, state.L / divisor);
}

const State operator*(float factor, const State &state)
{
    return State(state.c * factor, state.p * factor, state.q / (1/factor), state.L * factor);
}

const State operator*(const State &state, float factor)
{
    return factor * state;
}

const State operator+(const State &s1, const State &s2)
{
    return State(s1.c + s2.c, s1.p + s2.p, s1.q + s2.q, s1.L + s2.L);
}
