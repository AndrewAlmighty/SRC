#include "timescale.h"

double Timescale::m_scaleFactor = 5.0;  //60 means 1 min in app is 1 real sec.

void Timescale::setScale(double scale)
{
    m_scaleFactor = scale;
}

double Timescale::getScale()
{
    return m_scaleFactor;
}
