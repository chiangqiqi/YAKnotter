#include "segment.h"
#include <QVector3D>

#define SMALL_NUM 0.005


Segment::Segment()
{

}

Segment::Segment(QVector3D p, QVector3D q){
    from = p;
    to = q;
}

QVector3D Segment::vec()const{
    return to-from;
}

double distance(const Segment& s1,const Segment& s2){
    QVector3D u = s1.vec();
    QVector3D v = s2.vec();
    QVector3D w = s2.from-s1.from;

    double a = QVector3D::dotProduct(u,u);
    double b = QVector3D::dotProduct(u,v);
    double c = QVector3D::dotProduct(v,v);         // always >= 0
    double d = QVector3D::dotProduct(u,w);
    double e = QVector3D::dotProduct(v,w);
    double D = a*c - b*b;        // always >= 0

    double    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    double    tc, tN, tD = D;

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d +  b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
    tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

    QVector3D  dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

    return dP.length();
}
