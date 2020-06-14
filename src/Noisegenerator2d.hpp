
#include <stdlib.h> /* srand, rand */
#include <math.h>
#include <iostream>

#ifndef NOISEGENERATOR2D
#define NOISEGENERATOR2D
class Noisegenerator2d
{
private:
    float ***Gradient;
    float dotGridGradient(int ix, int iy, float x, float y);
    int columns;
    int rows;

public:
    float perlin(float x, float y);
    ~Noisegenerator2d();
    Noisegenerator2d(int columns, int rows, int seed);
};

Noisegenerator2d::Noisegenerator2d(int columns, int rows, int seed) : columns(columns), rows(rows)
{
    srand(seed);
    this->Gradient = new float **[columns];
    for (int i = 0; i < columns; i++)
    {
        this->Gradient[i] = new float *[rows];
        for (int y = 0; y < rows; y++)
        {
            this->Gradient[i][y] = new float[2];
            int temp = rand();
            this->Gradient[i][y][0] = (float)sin(temp);
            this->Gradient[i][y][1] = (float)cos(temp);
        }
    }
}

Noisegenerator2d::~Noisegenerator2d()
{
    for (int i = 0; i < columns; i++)
    {
        for (int y = 0; y < rows; y++)
        {
            delete (this->Gradient[i][y]);
        }
        delete (this->Gradient[i]);
    }
    delete (this->Gradient);
}

//Quelle: https://en.wikipedia.org/wiki/Perlin_noise

float lerp(float a0, float a1, float w)
{
    return (1.0f - w) * a0 + w * a1;
}

// Computes the dot product of the distance and gradient vectors.
float Noisegenerator2d::dotGridGradient(int ix, int iy, float x, float y)
{

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * this->Gradient[iy][ix][0] + dy * this->Gradient[iy][ix][1]);
}

// Compute Perlin noise at coordinates x, y
float Noisegenerator2d::perlin(float x, float y)
{
    if ((float)columns < x || (float)rows < y)
    {
        printf("Eingabe zu Gross");
        return -1.0;
    }
    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = lerp(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = lerp(n0, n1, sx);

    value = lerp(ix0, ix1, sy);
    return value;
}
#endif
