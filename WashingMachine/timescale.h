#ifndef TIMESCALE_H
#define TIMESCALE_H


class Timescale
{
private:
    static double m_scaleFactor;
public:
    static void setScale(double scale);
    static double getScale();
};

#endif // TIMESCALE_H
