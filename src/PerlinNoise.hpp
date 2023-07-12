
# pragma once
# include <cstdint>
# include <numeric>
# include <algorithm>
# include <random>
#include <iostream>

using namespace std;

class PerlinNoise
{
public:

    int p[256] = {5, 83, 17, 241, 29, 117, 6, 11, 184, 80, 107, 94, 149, 253, 141, 162, 68, 167, 220, 26, 102, 79, 225, 177, 67, 20, 134, 230, 9, 43, 236, 30, 41, 208, 214, 144, 198, 62, 251, 154, 221, 174, 21, 4, 113, 13, 212, 52, 226, 88, 179, 227, 175, 24, 82, 213, 56, 132, 97, 234, 201, 166, 112, 19, 188, 53, 176, 238, 0, 122, 232, 109, 160, 116, 23, 216, 189, 247, 18, 39, 36, 27, 118, 239, 92, 165, 185, 14, 133, 209, 86, 173, 78, 101, 35, 57, 219, 158, 64, 164, 197, 76, 38, 194, 231, 211, 229, 170, 51, 163, 131, 135, 73, 61, 105, 250, 233, 199, 44, 128, 22, 130, 161, 153, 8, 228, 183, 47, 178, 71, 204, 37, 110, 171, 66, 114, 50, 46, 203, 95, 74, 143, 195, 152, 55, 215, 72, 69, 249, 181, 12, 31, 202, 186, 1, 121, 248, 138, 60, 65, 147, 49, 16, 124, 40, 156, 125, 140, 59, 224, 25, 77, 120, 245, 89, 252, 223, 98, 28, 169, 190, 126, 146, 200, 217, 242, 254, 237, 129, 87, 7, 84, 32, 235, 187, 222, 255, 111, 148, 123, 103, 142, 137, 210, 93, 168, 85, 180, 104, 54, 206, 207, 192, 119, 157, 151, 63, 246, 2, 218, 99, 136, 10, 182, 150, 91, 115, 139, 205, 70, 75, 58, 127, 172, 240, 81, 3, 196, 48, 191, 15, 34, 108, 90, 244, 159, 155, 106, 145, 193, 100, 45, 33, 42, 243, 96};

    static inline double interpolate(double t, double a, double b) {
        return a + t * (b - a);
    }

    static inline double gradient(unsigned int hash, double x, double y, double z) {
        int h = int(hash & 15);
        double u = 0.0;
        double v = 0.0;
        if(h < 8){
            u = x;
        } else {
            u = y;
        }

        if(h < 4){
            v = y;
        } else {
            if(h == 12 || h == 14){
                v = x;
            } else {
                v = z;
            }
        }
        double result = 0.0;
        if((h & 1) == 0){
            result += u;
        } else {
            result -= u;
        }

        if((h & 2) == 0){
            result += v;
        } else {
            result -= v;
        }
        return result;
    }

    PerlinNoise(unsigned int seed) {

    }

    double noise(double x, double y, double z) const {
        const unsigned int X = static_cast<unsigned int>(floor(x)) & 255;
        const unsigned int Y = static_cast<unsigned int>(floor(y)) & 255;
        const unsigned int Z = static_cast<unsigned int>(floor(z)) & 255;

        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        const double u = x * x * x * (x * (x * 6 - 15) + 10);
        const double v = y * y * y * (y * (y * 6 - 15) + 10);
        const double w = z * z * z * (z * (z * 6 - 15) + 10);

        int A  = p[X & 255] + Y;
        int AA = p[A & 255] + Z;
        int AB = p[(A + 1) & 255] + Z;
        int B  = p[(X + 1) & 255] + Y;
        int BA = p[B & 255] + Z;
        int BB = p[(B + 1) & 255] + Z;

        return interpolate(w, interpolate(v, interpolate(u, gradient(p[AA & 255], x, y, z),
            gradient(p[BA & 255], x - 1, y, z)),
            interpolate(u, gradient(p[AB & 255], x, y - 1, z),
            gradient(p[BB & 255], x - 1, y - 1, z))),
            interpolate(v, interpolate(u, gradient(p[(AA + 1) & 255], x, y, z - 1),
            gradient(p[(BA + 1) & 255], x - 1, y, z - 1)),
            interpolate(u, gradient(p[(AB + 1) & 255], x, y - 1, z - 1),
            gradient(p[(BB + 1) & 255], x - 1, y - 1, z - 1))));
    }

    double octaveNoise(double x, double y, double z, unsigned int num_octaves) const {
        double result = 0.0;
        double amp = 1.0;

        for (unsigned int i = 0; i < num_octaves; ++i) {
            result += noise(x, y, z) * amp;
            x *= 2.0;
            y *= 2.0;
            z *= 2.0;
            amp *= 0.5;
        }

        return result;
    }
};