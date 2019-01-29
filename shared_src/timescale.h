#ifndef TIMESCALE_H
#define TIMESCALE_H


class Timescale
{
public:
    static void setScale(double scale);
    static double getScale();

private:
    static double m_scaleFactor;
};

#endif // TIMESCALE_H
