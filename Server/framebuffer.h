#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <sys/mman.h>
#include <linux/fb.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "meta_info.h"

extern struct fb_fix_screeninfo fixed_info;
extern struct fb_var_screeninfo var_info;
extern int fb_fd;
extern size_t length;


#endif
/*
int munmap(void addr[.length], size_t length);
*/
// read the framebuffer and map it to your user space
// compress it
// send it

// so create one function to read and copy, another to compress and another to send - modularize

int initFrameBuffer(){
    fb_fd = open("/dev/fb0", O_RDWR); // file descriptor for framebuffer
    if (fb_fd < 0){
        return -1;
    }
    ioctl(fb_fd, FBIOGET_FSCREENINFO ,&fixed_info); // this will tell driver to assign all the elements in the struct proper values
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_info); // contains information like rgb values, etc.
    return 0;
}


uint8_t* getFrameBufferPtr(){
    // void *mmap(void addr[.length], size_t length, int prot, int flags, int fd, off_t offset);
    void* addr = NULL; // Maps the framebuffer value to the next free page in program memory
    length = var_info.yres * fixed_info.line_length; // gives total bytes of information on the screen
    int prot = PROT_READ | PROT_WRITE; // protection flags to tell the kernel what access you want
    int flags = MAP_SHARED; // flag means changes made to mapped memory affects everyone accessing it
    off_t offset = 0; // offset of display
    uint8_t *fb_ptr = mmap(addr, length, prot, flags, fb_fd, offset); //maps fb space to process space
    // uint8_t is 1 raw, unsigned byte
    if(fb_ptr == MAP_FAILED){
        perror("Mapping to process space failed");
        exit(1);
    }
    return fb_ptr;
}

// now that we have the raw framebuffer, copy the value from fb to a buffer sized length.
// This will freeze one frame, and you can compress it.

uint8_t* getFrame(uint8_t* fb_ptr){
    uint8_t* frame = malloc(length);
    if(!frame){
        perror("Failed to create frame");
        exit(1);
    }
    memcpy(frame, fb_ptr, length);
    return frame;
}

int compressFrame(uint8_t* frame){
    size_t max_compressed_size = compressBound(length); //finds out algorithmically max size of compressed image
    uint8_t compressed_frame = malloc(max_compressed_size);
    if(!compressed_frame){
        perror("Error in allocating buffer for compressed packet");
        exit(1);
    }
    uLongf compressed_length = max_compressed_size;
    int ret = compress(compressed_frame, &compressed_length, frame, length);
    if (ret != Z_OK){
        fprintf(stderr, "Compression failed: %d\n", ret);
        exit(1);
    }
}

//Need a new struct since mac and windows don't support same structs.
int sendInfo(int sockfd){
    struct fb_meta_fix fix_meta;
    fix_meta.line_length = fixed_info.line_length;

    struct fb_meta_var var_meta;
    var_meta.xres = var_info.xres;
    var_meta.yres = var_info.yres;
    var_meta.bits_per_pixel = var_info.bits_per_pixel;
    var_meta.red_offset = var_info.red.offset;
    var_meta.red_length = var_info.red.length;
    var_meta.green_offset = var_info.green.offset;
    var_meta.green_length = var_info.green.length;
    var_meta.blue_offset = var_info.blue.offset;
    var_meta.blue_length = var_info.blue.length;

    // Send them over socket
    write(sockfd, &fix_meta, sizeof(fix_meta));
    write(sockfd, &var_meta, sizeof(var_meta));

}