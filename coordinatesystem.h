#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "common.h"

class CoordinateSystem
{
public:
    ld xscale, yscale;
    ld xpitch, ypitch;

public:
    CoordinateSystem();

    ld transformX(ld x) {
        return xscale * (x + xpitch);
    }

    ld transformY(ld y) {
        return yscale * (y + ypitch);
    }

    ld invX(ld x) {
        return x / xscale - xpitch;
    }

    ld invY(ld y) {
        return y / yscale - ypitch;
    }
};

#endif // COORDINATESYSTEM_H
