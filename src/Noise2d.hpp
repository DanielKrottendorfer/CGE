#include "Noisegenerator2d.hpp"

#ifndef NOISE2D
#define NOISE2D
class Noise2d
{
private:
    int a = 10;
    int b = 10;
    Noisegenerator2d ng = Noisegenerator2d(a, b, 2222222);
    int a1 = 2;
    int b1 = 2;
    Noisegenerator2d ng1 = Noisegenerator2d(a1, b1, 111111);

public:
    Noise2d(/* args */){};
    ~Noise2d(){};
    float noise(float x, float z)
    {
        float ty = ng.perlin(x * (((float)a) - 1.001), z * (((float)b) - 1.001));
        ty += ng1.perlin(x * (((float)a1) - 1.001), z * (((float)b1) - 1.001)) * 2.0;
        return ty;
    }
};

static Noise2d instance = Noise2d();

#endif
