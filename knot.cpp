#include "knot.h"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <QGLFunctions>
#include <QDebug>
#include <QVector3D>
#include <cmath>


//generate a knot with n nodes;

const double PI = 3.1415926535897932385;

Knot::Knot(int n)
{
    N = n;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0,1);
    auto dice = std::bind ( distribution, generator );

    QVector3D p_old;
    for(int i = 0; i < n; i++){
        //QVector3D p(2.0*cos(double(i)/n*2*PI),sin(double(i)/n*2*PI),0);
        QVector3D p(dice(),dice(),dice());
        data.push_back(p);

        if(i != 0 ){
            Segment s(p_old,p);
            segs.push_back(Segment(data[i-1],data[i]));
        }
    }
    segs.push_back(Segment(data[n-1],data[0]));

    dsafe = 0.01;
}

Knot::~Knot(){
    glDeleteBuffers(2, vbo);
}

void Knot::print(){

}

int Knot::len(){
    return data.size();
}

//compute the closest distance btw two line segment
double Knot::dclose(){
    double min = std::numeric_limits<double>::max();

    for(auto i = segs.begin();i!=segs.end();i++){
        for(auto j = i+1; j!= segs.end(); j++){
            double dis = distance(*i,*j);
             if (dis < min)
                min = dis;
        }
    }
    return min;
}

void Knot::compute_force() {
    force.resize(N);

    //unsigned int l = data.size();
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(j == i) continue;

            if(i == 0){
                if(j == N-1 || j == 1)
                    force[i] += fmechanic(data[i], data[j]);
                else
                    force[i] += felectric(data[i], data[j]);
            }else if(i == N-1){
                if(j == i-1 || j == 0)
                    force[i] += fmechanic(data[i], data[j]);
                else
                    force[i] += felectric(data[i], data[j]);
            }else{
                if(j == i-1 || j == i+1)
                    force[i] += fmechanic(data[i], data[j]);
                else
                    force[i] += felectric(data[i], data[j]);
            }
        }
    }
}

bool Knot::safeQ() {
    return dclose() > dsafe;
}

void Knot::evolve(){
    compute_force();
    //Knot tmp_knot = *this;

    vector<double> force_norm;
    for(QVector3D& p : force)
        force_norm.push_back(p.length());

    double max_norm = *max_element(force_norm.begin(), force_norm.end());
    //f is proportional to the step length
    vector<QVector3D> move;
    //the moving step is <= dsafe
    for(QVector3D& p : force){
        p = p/max_norm * 2;

        if(p.length() > dsafe)
            move.push_back(p.normalized()*dsafe);
        else
            move.push_back(p);
    }

    for(int i = 0; i < N; i++){
        //move the bead
        data[i] += move[i];
        //check if the bead is in safe position
        if(!safeQ()){
            //move back the bead
            data[i] -= move[i];
        }
    }
}

void Knot::init() {
    initializeGLFunctions();

    glGenBuffers(2, vbo);

    QVector3D *vertices = &data[0];
    GLuint indices[N];

    //initialize the indices
    for(int i = 0; i < N; i++){
        indices[i] = i;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(QVector3D), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, N * sizeof(GLuint), indices, GL_STATIC_DRAW);

}

void Knot::draw(QGLShaderProgram *program){
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (const void *)offset);

    glPointSize(4.0);

    glDrawElements(GL_POINTS, N,GL_UNSIGNED_INT, 0);

    glDrawElements(GL_LINE_LOOP,N,GL_UNSIGNED_INT,0);
}


void Knot::drawCurve(){
    BezierCurve<QVector3D> curve;
//    curve.addControlPoint(QVector3D(1,0,0) , QVector3D(1,3,0), QVector3D(0,1,0));
//    curve.addControlPoint(QVector3D(0,.3,0) , QVector3D(3,1,0), QVector3D(0,.1,0));
//    curve.addControlPoint(QVector3D(1,0,0) , QVector3D(2,2,0), QVector3D(0,1,0));

    for(auto iter = data.begin(); iter != data.end()-1; iter++){
        QVector3D p0 = *iter;
        QVector3D p1 = *(iter+1);

        //this segment is from po to p1
        curve.addControlPoint(0.25*p1 + 0.75*p0, (p0+p1)/2, 0.25*p0 + 0.75*p1);
    }

    QVector3D p0 = data.back();
    QVector3D p1 = *data.begin();
    curve.addControlPoint(0.25*p1 + 0.75*p0, (p0+p1)/2, 0.25*p0 + 0.75*p1);
    p0 = *data.begin();
    p1 = *(data.begin() + 1);
    curve.addControlPoint(0.25*p1 + 0.75*p0, (p0+p1)/2, 0.25*p0 + 0.75*p1);

    curve.draw();
}
