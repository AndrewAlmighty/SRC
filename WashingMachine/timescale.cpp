#include "timescale.h"

double Timescale::m_scaleFactor = 5.0; //60 times faster means that 1 program minute is real 1 second

void Timescale::setScale(double scale)
{
    m_scaleFactor = scale;
}

double Timescale::getScale()
{
    return m_scaleFactor;
}
