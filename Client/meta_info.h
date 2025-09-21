// portable_framebuffer.h
#ifndef PORTABLE_FRAMEBUFFER_H
#define PORTABLE_FRAMEBUFFER_H

#include <stdint.h>

struct fb_meta_fix {
    uint32_t line_length;
};

struct fb_meta_var {
    uint32_t xres;
    uint32_t yres;
    uint32_t bits_per_pixel;
    uint32_t red_offset;
    uint32_t red_length;
    uint32_t green_offset;
    uint32_t green_length;
    uint32_t blue_offset;
    uint32_t blue_length;
};

#endif
