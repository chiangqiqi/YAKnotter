#ifndef KNOT_H
#define KNOT_H
#include <vector>
#include <map>
#include <QVector3D>
#include <QGLFunctions>
#include <QGLShaderProgram>

#include "segment.h"
#include "BezierCurve.h"

const double H = 1.0;
const double ALPHA = 4;

const double BETA = 2;

using std::vector;
//QVector3D felectric(QVector3D p, QVector3D q);

class Knot : protected QGLFunctions
{
public:
    Knot(int n);
    ~Knot();

    void print();
    int len();
    double dclose();
    void compute_force();
    void evolve();
    void init();
    void draw(QGLShaderProgram *program);
    void drawCurve();

private:
    bool safeQ();
private:
    vector<QVector3D> force;
    vector<QVector3D> data;
    vector<Segment> segs;

    double dsafe;
    int N;

    GLuint vbo[2];
};

//electrical force
static QVector3D felectric(QVector3D p, QVector3D q) {
    QVector3D n = (p-q).normalized();
    return H * pow((p-q).length(), -2-ALPHA)* n;
}
//mechanical force
static QVector3D fmechanic(QVector3D p, QVector3D q) {
    QVector3D n = (q-p).normalized();
    return 10*H * pow((p-q).length(), 1 + BETA)* n;
}

#endif // KNOT_H
