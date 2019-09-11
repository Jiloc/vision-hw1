#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853
#define M_E 2.7182818284590452354

void l1_normalize(image im)
{
    float sum = 0;
    
    int n = im.w * im.h * im.c;

    for (int i = 0; i < n; i++)
    {
        sum += im.data[i];
    }

    for (size_t i = 0; i < n; i++)
    {
        im.data[i] /= sum;
    }
}

image make_box_filter(int w)
{
    int n = w * w;
    float v = 1.0 / (float) n;
    image im = make_image(w, w, 1);
    for (int i = 0; i < n; i++)
    {
        im.data[i] = v;
    }
    return im;
}


float convolve_pixel_channel(image im, int x, int y, int c, image filter, int fc)
{
    int start_x = x - (int) (filter.w * 0.5);
    int start_y = y - (int) (filter.h * 0.5);

    float sum = 0;
    for (int fy = 0; fy < filter.h; fy++)
    {
        for (int fx = 0; fx < filter.w; fx++)
        {
            sum += get_pixel(im, start_x + fx, start_y + fy, c) * get_pixel(filter, fx, fy, fc);
        }
    }
    return sum;
}


void convolve_image_channel(image im, int c, image filter, int filter_c, image res, int res_c, int override)
{
    for (int y = 0; y < im.h; y++)
    {
        for (int x = 0; x < im.w; x++)
        {   
            float v = convolve_pixel_channel(im, x, y, c, filter, filter_c);
            if (!override) 
            {
                v += get_pixel(res, x, y, res_c);
            }
            set_pixel(res, x, y, res_c, v);
        }
    }
}


image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || im.c == filter.c);

    image res = preserve ? make_image(im.w, im.h, im.c) : make_image(im.w, im.h, 1);

    int res_c = 0;
    int override = 1;

    for (int im_c = 0; im_c < im.c; im_c++)
    {
        convolve_image_channel(im, im_c, filter, im_c % filter.c, res, res_c, override);
        if (preserve)
        {
            res_c++;
        }
        else
        {
            override = 0;
        }
    }
    return res;
}


image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 4);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 1, 2, 0, -1);
    return filter;
}

image make_sharpen_filter()
{
    image im = make_image(3, 3, 1);
    set_pixel(im, 1, 0, 0, -1);
    set_pixel(im, 0, 1, 0, -1);
    set_pixel(im, 1, 1, 0, 5);
    set_pixel(im, 2, 1, 0, -1);
    set_pixel(im, 1, 2, 0, -1);
    return im;
}

image make_emboss_filter()
{
    image im = make_image(3, 3, 1);
    set_pixel(im, 0, 0, 0, -2);
    set_pixel(im, 1, 0, 0, -1);
    set_pixel(im, 0, 1, 0, -1);
    set_pixel(im, 1, 1, 0, 1);
    set_pixel(im, 2, 1, 0, 1);
    set_pixel(im, 1, 2, 0, 1);
    set_pixel(im, 2, 2, 0, 2);
    return im;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int n = 6 * sigma;
    if (n % 2 == 0) n++;

    image filter = make_image(n, n, 1);
    float sigma_sqr = pow(sigma, 2);

    float multiplier = 1 / (TWOPI * sigma_sqr);

    for (int y = 0; y < n; y++)
    {
        for (int x = 0; x < n; x++)
        {
            float v = multiplier * exp(-((pow(x - n / 2, 2) + pow(y - n / 2, 2))/(2 * sigma_sqr)));
            set_pixel(filter, x, y, 0, v);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{   
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im = make_image(a.w, a.h ,a.c);
    for (size_t c = 0; c < im.c; c++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            for (size_t x = 0; x < im.w; x++)
            {
                set_pixel(im, x, y, c, get_pixel(a, x, y, c) + get_pixel(b, x, y, c));
            }   
        }
    }
    return im;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im = make_image(a.w, a.h ,a.c);
    for (size_t c = 0; c < im.c; c++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            for (size_t x = 0; x < im.w; x++)
            {
                set_pixel(im, x, y, c, get_pixel(a, x, y, c) - get_pixel(b, x, y, c));
            }   
        }
    }
    return im;
}

image make_gx_filter()
{
    image im = make_image(3, 3, 1);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 0, 1, 0, -2);
    set_pixel(im, 0, 2, 0, -1);
    set_pixel(im, 2, 0, 0, 1);
    set_pixel(im, 2, 1, 0, 2);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

image make_gy_filter()
{
    image im = make_image(3, 3, 1);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 1, 0, 0, -2);
    set_pixel(im, 2, 0, 0, -1);
    set_pixel(im, 0, 2, 0, 1);
    set_pixel(im, 1, 2, 0, 2);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

void feature_normalize(image im)
{
    float min = 1;
    float max = 0;
    for (size_t c = 0; c < im.c; c++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            for (size_t x = 0; x < im.w; x++)
            {
                float v = get_pixel(im, x, y ,c);
                if (min > v) min = v;
                if (max < v) max = v;
            }
        }
    }

    for (size_t c = 0; c < im.c; c++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            for (size_t x = 0; x < im.w; x++)
            {
                float v = get_pixel(im, x, y ,c);
                set_pixel(im, x ,y , c, (v - min) / (max - min));
            }
        }
    }
}

image *sobel_image(image im)
{
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    image x_gradient = convolve_image(im, gx_filter, 0);
    image y_gradient = convolve_image(im, gy_filter, 0);
    
    free_image(gx_filter);
    free_image(gy_filter);

    image *sobel = calloc(2, sizeof(image));

    sobel[0] = make_image(im.w, im.h, 1);
    sobel[1] = make_image(im.w, im.h, 1);

    for (size_t y = 0; y < im.h; y++)
    {
        for (size_t x = 0; x < im.w; x++)
        {
            float x_v = get_pixel(x_gradient, x, y , 0);
            float y_v = get_pixel(y_gradient, x, y , 0);
            float magnitude = sqrtf(pow(x_v, 2) + pow(y_v, 2));
            float direction = atan2(y_v, x_v);  
            set_pixel(sobel[0], x, y, 0, magnitude);
            set_pixel(sobel[1], x, y, 0, direction);
        }
    }

    free_image(x_gradient);
    free_image(y_gradient);

    return sobel;
}

image colorize_sobel(image im)
{
    // image g3 = make_gaussian_filter(3);
    // im = convolve_image(im, g3, 1);

    image *sobel = sobel_image(im);
    
    feature_normalize(sobel[0]);
    feature_normalize(sobel[1]);

    image res = make_image(im.w, im.h, im.c);
    for (size_t y = 0; y < im.h; y++)
    {
        for (size_t x = 0; x < im.w; x++)
        {
            float m = get_pixel(sobel[0], x, y, 0);
            float d = get_pixel(sobel[1], x, y, 0);
            set_pixel(res, x, y, 0, d);
            set_pixel(res, x, y, 1, m);
            set_pixel(res, x, y, 2, m);
        }
    }
    free_image(sobel[1]);
    free_image(sobel[0]);
    free(sobel);
        
    hsv_to_rgb(res);
    return res;
}
