/* Copyright 2004 Martin Renold <martinxyz@gmx.ch>
   Released as public domain.
*/

#include <gtk/gtk.h>
#include <string.h>
#include <math.h>
#include "surface.h"

Surface *
new_surface (int w, int h)
{
  Surface * s;
  s = g_new0 (Surface, 1);

  s->w = w;
  s->h = h;

  // ugh. hope that's all correct now
  s->block_h = (h-1) / BLOCKSIZE + 1;
  
  s->xsize_shl = BLOCKBITS;
  while ((1 << s->xsize_shl) < w) s->xsize_shl++;

  s->block_w = 1 << (s->xsize_shl - BLOCKBITS);

  /*
  g_print ("requested: %d %d\n", s->w, s->h);
  g_print ("blocksize: %d %d\n", s->block_w, s->block_h);
  g_print ("finally  : %d %d\n", s->block_w*BLOCKSIZE, s->block_h*BLOCKSIZE);
  */

  g_assert (s->block_w * BLOCKSIZE >= w);
  g_assert (s->block_h * BLOCKSIZE >= h);

  s->rgb = g_new0 (guchar, 3*s->block_w*BLOCKSIZE*s->block_h*BLOCKSIZE);
  return s;
}

void
free_surface (Surface * s)
{
  g_free (s->rgb);
  g_free (s);
}

void 
surface_renderpattern (Surface * s)
{
  int x, y, r, g, b;
  guchar *rgb;

  for (y = 0; y < s->h; y++) {
    for (x = 0; x < s->w; x++) {
      rgb = PixelXY(s, x, y);
      r = x % 256;
      g = y % 256;
      b = (x*x+y*y) % 256;
      rgb[3*x + 0] = r;
      rgb[3*x + 1] = g;
      rgb[3*x + 2] = b;
    }
  }
}

void
surface_clear (Surface * s)
{
  // clear rgb buffer to white
  memset (s->rgb, 255, s->block_w*BLOCKSIZE*s->block_h*BLOCKSIZE*3);
}

void
surface_render (Surface * s,
                guchar * dst, int rowstride,
                int x0, int y0,
                int w, int h, int bpp)
{
  // could be optimized much, important if big brush is used
  int x, y, add;
  if (bpp == 3*8) {
    add = 3;
  } else if (bpp == 4*8) {
    add = 4;
  } else {
    g_assert (0);
    return;
  }

  guchar * rgb_line = dst;
  guchar * rgb_dst;
  guchar * rgb_src;
  for (y = y0; y < y0 + h; y++) {
    rgb_dst = rgb_line;
    for (x = x0; x < x0 + w; x++) {
      rgb_src = PixelXY(s, x, y);
      rgb_dst[0] = rgb_src[0];
      rgb_dst[1] = rgb_src[1];
      rgb_dst[2] = rgb_src[2];
      rgb_dst += add;
    }
    rgb_line += rowstride;
  }
}


void
surface_load (Surface * s, guchar * src,
              int rowstride, int w, int h, int bpp)
{
  int x, y, add;
  if (bpp == 3*8) {
    add = 3;
  } else if (bpp == 4*8) {
    add = 4;
  } else {
    g_assert (0);
    return;
  }

  if (w > s->w) w = s->w;
  if (h > s->h) h = s->h;

  guchar * rgb_line = src;
  guchar * rgb_dst;
  guchar * rgb_src;
  for (y = 0; y < 0 + h; y++) {
    rgb_src = rgb_line;
    for (x = 0; x < 0 + w; x++) {
      rgb_dst = PixelXY(s, x, y);
      rgb_dst[0] = rgb_src[0];
      rgb_dst[1] = rgb_src[1];
      rgb_dst[2] = rgb_src[2];
      rgb_src += add;
    }
    rgb_line += rowstride;
  }
}