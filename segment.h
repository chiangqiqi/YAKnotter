#ifndef SEGMENT_H
#define SEGMENT_H
#include <QVector3D>

class Segment
{
public:
    Segment();
    Segment(QVector3D p, QVector3D q);

    //return the vector of the line seg
    QVector3D vec() const;
    friend double distance(const Segment& s1,const Segment& s2);
private:
    QVector3D from;
    QVector3D to;
};


#endif // SEGMENT_H
