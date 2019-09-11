#include <math.h>
#include "image.h"

#include <stdio.h>

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, roundf(x), roundf(y), c);
}

image nn_resize(image im, int w, int h)
{
    image new_im = make_image(w, h, im.c);
    float w_step = im.w / (float) w;
    // float w_offset = (-0.5f / w_step) * w_step + w_step * 0.5f;
    float w_offset = -0.5f + w_step * 0.5f;
    float h_step = im.h / (float) h;
    //float h_offset = (-0.5f / h_step) * h_step + h_step * 0.5f;
    float h_offset = -0.5f + h_step * 0.5f;

    for (int c = 0; c < new_im.c; c++)
    {
        for (int y = 0; y < new_im.h; y++)
        {
            for (int x = 0; x < new_im.w; x++)
            {
                set_pixel(new_im, x, y, c, 
                          nn_interpolate(im, w_offset + w_step * x, 
                                         h_offset + h_step * y, c));
            }
            
        }        
    }
    return new_im;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    int left = (int) x;
    int right = left + 1;
    int top = (int) y;
    int bottom = top + 1;
    float d1 = x - left;
    float d2 = right - x;
    float d3 = y - top;
    float d4 = bottom - y;

    float a1 = d2 * d4;
    float a2 = d1 * d4;
    float a3 = d2 * d3;
    float a4 = d1 * d3;

    float v1 = get_pixel(im, left, top, c);
    float v2 = get_pixel(im, right, top, c);
    float v3 = get_pixel(im, left, bottom, c);
    float v4 = get_pixel(im, right, bottom, c);

    return v1 * a1 + v2 * a2 + v3 * a3 + v4 * a4;
}

image bilinear_resize(image im, int w, int h)
{
    image new_im = make_image(w, h, im.c);
    float w_step = im.w / (float) w;
    // float w_offset = (-0.5f / w_step) * w_step + w_step * 0.5f;
    float w_offset = -0.5f + w_step * 0.5f;
    float h_step = im.h / (float) h;
    //float h_offset = (-0.5f / h_step) * h_step + h_step * 0.5f;
    float h_offset = -0.5f + h_step * 0.5f;

    for (int c = 0; c < new_im.c; c++)
    {
        for (int y = 0; y < new_im.h; y++)
        {
            for (int x = 0; x < new_im.w; x++)
            {
                set_pixel(new_im, x, y, c, 
                          bilinear_interpolate(im, w_offset + w_step * x, 
                                               h_offset + h_step * y, c));
            }
            
        }        
    }
    return new_im;
}

