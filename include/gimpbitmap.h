#ifndef GIMPBITMAP
#define GIMPBITMAP

// Media pantalla o lo que es lo mismo... una estructura de 32 x 32 píxeles
typedef struct  {
  unsigned int   width;
  unsigned int   height;
  unsigned int   bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char  pixel_data[32*32*3 + 1];
} gimp32x32bitmap;

#endif