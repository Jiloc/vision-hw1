#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if (y < 0) y = 0;
    else if (y >= im.h) y = im.h - 1;
    if (x < 0) x = 0;
    else if (x >= im.w) x = im.w - 1;
    // if (c < 0) c = 0;
    // else if (c >= im.c) c = im.c - 1;
    return im.data[c * im.w * im.h + y * im.w + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (y < 0 || y >= im.h || x < 0 || x >= im.w) return;
    im.data[c * im.w * im.h + y * im.w + x] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, sizeof(float) * im.w * im.h * im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    int n = im.w * im.h;
    for (int i = 0; i < n; i++)
    {
        gray.data[i] = .299 * im.data[i] + .587 * im.data[i + n] + .114 * im.data[i + n * 2];
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    int n = im.h * im.w;
    int offset = c * n;
    for (int i = 0; i < n; i++){
        im.data[i + offset] += v;
    }
}

void clamp_image(image im)
{
    int n = im.h * im.w * im.c;
    for (int i = 0; i < n; i++) {
        if (im.data[i] < 0.) im.data[i] = 0;
        else if (im.data[i] > 1.) im.data[i] = 1.;
    }
}

void scale_image(image im, int c, float v)
{
    int n = im.h * im.w;
    int offset = c * n;
    for (int i = offset; i < n + offset; i++) {
        im.data[i] *= v;
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int n = im.w * im.h;
    for (int i = 0; i < n; i++)
    {
        float r = im.data[i];
        float g = im.data[i + n];
        float b = im.data[i + n * 2];
        float m = three_way_min(r, g, b);
        float V = three_way_max(r, g, b);
        float C = V - m;
        float S;
        if (V == 0){
            S = 0;
        }
        else {
            S = C / V;
        } 

        float H1;
        if (C == 0){
            H1 = 0;
        }
        else if (V == r) {
            H1 = (g - b) / C;
        }
        else if (V == g) {
            H1 = ((b - r) / C) + 2; 
        }
        else {
            H1 = ((r - g) / C) + 4;
        }
        float H = H1 < 0 ? (H1 / 6) + 1 : H1 / 6;
        im.data[i] = H;
        im.data[i + n] = S;
        im.data[i + n * 2] = V;
    }
}

void hsv_to_rgb(image im)
{
    int n = im.w * im.h;
    for (int i = 0; i < n; i++)
    {
        float H = im.data[i];
        float S = im.data[i + n];
        float V = im.data[i + n * 2];
        
        float C = V * S;
        float H1 = H * 6;

        float X = C * (1 - fabs(fmod(H1, 2) - 1));
        float m = V - C;

        float r1, g1, b1;
        if (H1 < 1) {
            r1 = C;
            g1 = X;
            b1 = 0;
        }
        else if (H1 < 2) {
            r1 = X;
            g1 = C;
            b1 = 0;
        }
        else if (H1 < 3) {
            r1 = 0;
            g1 = C;
            b1 = X;
        }
        else if (H1 < 4) {
            r1 = 0;
            g1 = X;
            b1 = C;
        }
        else if (H1 < 5) {
            r1 = X;
            g1 = 0;
            b1 = C;
        }
        else {
            r1 = C;
            g1 = 0;
            b1 = X;
        }
        float r = r1 + m;
        float g = g1 + m;
        float b = b1 + m;
        im.data[i] = r;
        im.data[i + n] = g;
        im.data[i + 2 * n] = b;
    }
}
