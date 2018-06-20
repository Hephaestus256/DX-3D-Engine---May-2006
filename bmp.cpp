#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <dpmi.h>
#include <io.h>
#include <pc.h>
#include <dos.h>
#include <sys\farptr.h>
#include <go32.h>
#include "math2d.h"

#define HIGH_COLOR_320x200 0x10e
#define TRUE_COLOR_320x200 0x10f
#define HIGH_COLOR_640x480 0x111
#define TRUE_COLOR_640x480 0x112
#define HIGH_COLOR_800x600 0x114
#define TRUE_COLOR_800x600 0x115

// 14-bytes
struct {
  short bf_type;
  long bf_size;
  long bf_reserved;
  long bf_off_bits; 
} bmp_file_header;

// 40 bytes
struct {
  long bi_size;
  long bi_width;
  long bi_height;
  short bi_planes;
  short bi_bit_count;
  long bi_compression;
  long bi_size_image;
  long bi_x_in_meter;
  long bi_y_in_meter;
  long bi_clr_used;
  long bi_clr_important;
} bmp_info_header;

struct VESA_INFO {
  char legit[4] __attribute__ ((packed));
  unsigned short version __attribute__ ((packed));
  unsigned long oem_string_ptr __attribute__ ((packed));
  unsigned char capabilities[4] __attribute__ ((packed));
  unsigned long video_mode_ptr __attribute__ ((packed));
  unsigned short total_memory __attribute__ ((packed));
  unsigned short oem_software_rev __attribute__ ((packed));
  unsigned long oem_vendor_name_Ptr __attribute__ ((packed));
  unsigned long oem_product_name_ptr __attribute__ ((packed));
  unsigned long oem_product_rev_ptr __attribute__ ((packed));
  unsigned char reserved[222] __attribute__ ((packed));
  unsigned char oem_data[256] __attribute__ ((packed));
} vesa_info;


struct MODE_INFO
{
  unsigned short mode_attributes __attribute__ ((packed));
  unsigned char win_a_attributes __attribute__ ((packed));
  unsigned char win_b_attributes __attribute__ ((packed));
  unsigned short win_granularity __attribute__ ((packed));
  unsigned short win_size __attribute__ ((packed));
  unsigned short win_a_segment __attribute__ ((packed));
  unsigned short win_b_segment __attribute__ ((packed));
  unsigned long win_func_ptr __attribute__ ((packed));
  unsigned short bytes_per_scanline __attribute__ ((packed));
  unsigned short x_res __attribute__ ((packed));
  unsigned short y_resolution __attribute__ ((packed));
  unsigned char x_char_size __attribute__ ((packed));
  unsigned char y_char_size __attribute__ ((packed));
  unsigned char number_of_planes __attribute__ ((packed));
  unsigned char bits_per_pixel __attribute__ ((packed));
  unsigned char number_of_banks __attribute__ ((packed));
  unsigned char memory_model __attribute__ ((packed));
  unsigned char bank_size __attribute__ ((packed));
  unsigned char number_of_image_pages __attribute__ ((packed));
  unsigned char reserved_page __attribute__ ((packed));
  unsigned char red_mask_size __attribute__ ((packed));
  unsigned char red_mask_pos __attribute__ ((packed));
  unsigned char green_mask_size __attribute__ ((packed));
  unsigned char green_mask_pos __attribute__ ((packed));
  unsigned char blue_mask_size __attribute__ ((packed));
  unsigned char blue_mask_pos __attribute__ ((packed));
  unsigned char reserved_mask_size __attribute__ ((packed));
  unsigned char reserved_mask_pos __attribute__ ((packed));
  unsigned char direct_color_mode_info __attribute__ ((packed));
  unsigned char* phys_base_ptr __attribute__ ((packed));
  unsigned long off_screen_mem_offset __attribute__ ((packed));
  unsigned short off_screen_mem_size __attribute__ ((packed));
  unsigned char reserved[206] __attribute__ ((packed));
} _mode_info;
 
int get_vesa_info();
int set_vesa_mode(int mode);
int set_vesa_mode(int mode);
int get_vesa_mode();
int get_mode_info();
void set_vesa_bank(int bank_number);
char* load_bmp (const char* filename, int width, int height);
void show_bmp_true (char* bmp, int w, int h);
void show_bmp_high (char* bmp, int w, int h);
void set_palette_mode (int mode);
short cv_true_to_high (int true_red, int true_green, int true_blue);
void write_bmp_high (char* bmp, const char* filename, int w, int h);
void write_bmp_true (char* bmp, const char* filename, int w, int h,
  int ow, int oh);
inline double frac (double n);
inline double tl_area (double x, double y);
inline double tr_area (double x, double y);
inline double bl_area (double x, double y);
inline double br_area (double x, double y);
inline double le_area (double x, double y);
inline double re_area (double x, double y);
inline double te_area (double x, double y);
inline double be_area (double x, double y);


union texel_type {
  unsigned long dw;
  struct {
    unsigned char x, r, g, b;
  } el;
};


int main()
{
  get_vesa_info();
//  printf ("%i", set_vesa_mode(HIGH_COLOR_640x480));
  get_mode_info();
  set_vesa_mode (TRUE_COLOR_640x480);
  
  char* bmp = load_bmp ("\\textures\\metal3.bmp",256, 256);
//  char* bmp = load_bmp ("\\mydocu~1\\tex2c.bmp", 64, 64);
  show_bmp_true (bmp, 256, 256);
//  getch();
//  printf ("...");
  write_bmp_true (bmp, "\\tex\\metal2.tex",256, 256,256,256);
//  printf ("\nDone.");
  
//  set_vesa_mode(0x03);
}


int get_vesa_info()
{
  __dpmi_regs r;
  long dosbuf = __tb & 0xFFFFF;

  for (int c = 0; c < sizeof(VESA_INFO); c++)
    _farpokeb (_dos_ds, dosbuf + c, 0);

  dosmemput ("VBE2", 4, dosbuf);
  r.x.ax = 0x4F00;
  r.x.di = dosbuf & 0xF;
  r.x.es = (dosbuf >> 4) & 0xFFFF;
  __dpmi_int (0x10, &r);

  if (r.h.ah)
    return -1;
  else {
    dosmemget (dosbuf, sizeof(VESA_INFO), &vesa_info);
    if (strncmp(vesa_info.legit, "VESA", 4))
      return -1;
    else
      return 0;
  }
}

 
int set_vesa_mode(int mode)
{
  __dpmi_regs r;

  r.x.ax = 0x4F02;
  r.x.bx = mode;
  __dpmi_int(0x10, &r);

  if (r.h.ah)
    return -1;
  else
    return 0;
}


int get_mode_info()
{
  __dpmi_regs r;
  long dosbuf = __tb & 0xFFFFF;

  for (int c = 0; c < sizeof(MODE_INFO); c++)
    _farpokeb(_dos_ds, dosbuf + c, 0);

  r.x.ax = 0x4F01;
  r.x.di = dosbuf & 0xF;
  r.x.es = (dosbuf>>4) & 0xFFFF;
  r.x.cx = 257;
  __dpmi_int(0x10, &r);
  
  if (r.h.ah)
    return 0;
  else {
    dosmemget(dosbuf, sizeof(MODE_INFO), &_mode_info);
    return 1;
  }
}


void set_vesa_bank(int bank_number)
{
__dpmi_regs r;

  r.x.ax = 0x4F05;
  r.x.bx = 0;
  r.x.dx = bank_number;
  __dpmi_int(0x10, &r);
}


void set_palette_mode (int mode)
{
  __dpmi_regs r;

  r.x.ax = 0x4F08;
  r.x.bx = mode << 8;
  __dpmi_int(0x10, &r);
}


char* load_bmp (const char* filename, int width, int height)
{
  int handle, len;
  char* bitmap;

  if (_dos_open(filename, 0x02, &handle)) {
    set_vesa_mode(0x3);
    printf ("Can't load bitmap \"%s\"'\n", filename);
    getchar();
    exit(0);
  }
  len = filelength(handle);
  if ((bitmap = (char*) calloc(len, sizeof(char*))) ==
  NULL) {
    set_vesa_mode(0x3);
    printf ("Can't init %i bytes for bitmap \"%s\"\n", len, filename);
    getchar();
    exit(0);
  }
  _dos_read(handle, bitmap, len, NULL);
  _dos_close(handle);

  return bitmap;
}


void show_bmp_true (char* bmp, int w, int h)
{
  const int sw = 640, sl = 480, bpp = 3;
  
  int i = 0;
  
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
      set_vesa_bank((x * bpp + 0 + ((sl - y - 1) * sw * 3)) / 65536);
      _farpokeb (_dos_ds, 0xa0000 + ((x * bpp + 0 + ((sl - y - 1) * sw * 3)) & 65535),
        int(bmp[54 + i + 0] * 1.0));
      set_vesa_bank((x * bpp + 1 + ((sl - y - 1) * sw * 3)) / 65536);
      _farpokeb (_dos_ds, 0xa0000 + ((x * bpp + 1 + ((sl - y - 1) * sw * 3)) & 65535),
        int(bmp[54 + i + 1] * 1.0));
      set_vesa_bank((x * bpp + 2 + ((sl - y - 1) * sw * 3)) / 65536);
      _farpokeb (_dos_ds, 0xa0000 + ((x * bpp + 2 + ((sl - y - 1) * sw * 3)) & 65535),
        int(bmp[54 + i + 2] * 1.0));
      i += 3;
    }
}


void show_bmp_high (char* bmp, int w, int h)
{
  const int sw = 640, sl = 480, bpp = 2;
  
  int i = 54;
  
  for (int y = 0; y < 250; y++)
    for (int x = 0; x < 256; x++) {
      set_vesa_bank((x * bpp + 0 + ((sl - y) * sw * bpp)) / 65536);
      _farpokew (_dos_ds, 0xa0000 + ((x * bpp + 0 + ((sl - y) * sw * bpp)) & 65535),
        cv_true_to_high(bmp[i + 0], bmp[i + 1], bmp[i + 2]));
      i += 3;
    }
}


void write_bmp_high (char* bmp, const char* filename, int w, int h)
{
  const int sw = 640, sl = 480, bpp = 2;
  
  int i;
  int handle, len;
  char* bitmap;

  if (_dos_creat(filename, 0x0, &handle)) {
    printf ("Can't open \"%s\"'\n", filename);
    getchar();
    return;
  }

  char v1 = 8;
  short v2 = 250;
  _dos_write(handle, &v1, 1, NULL);
  _dos_write(handle, &v2, 2, NULL);

  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
//      set_vesa_bank((x * bpp + 0 + ((sl - y) * sw * bpp)) / 65536);
      short v = cv_true_to_high (
        bmp[54 + x * 3 + (250 - y) * 256 * 3 + 0],
        bmp[54 + x * 3 + (250 - y) * 256 * 3 + 1],
        bmp[54 + x * 3 + (250 - y) * 256 * 3 + 2]);
      _dos_write(handle, &v, 2, NULL);

//      _farpokew (_dos_ds, 0xa0000 + ((x * bpp + 0 + ((sl - y) * sw * bpp)) & 65535),
//        cv_true_to_high(bmp[i + 0], bmp[i + 1], bmp[i + 2]));
//      i += 3;
    }

  _dos_close(handle);
}


void write_bmp_true (char* bmp, const char* filename, int w, int h,
  int ow, int oh)
{
  const int sw = 640, sl = 480, bpp = 3;
  
  int i;
  int handle, len;
  char* bitmap;

  if (_dos_creat(filename, 0x0, &handle)) {
    printf ("Can't open \"%s\"'\n", filename);
    getchar();
    return;
  }

//  char v1 = 8;
//  short v2 = 250;
//  _dos_write(handle, &v1, 1, NULL);
//  _dos_write(handle, &v2, 2, NULL);

  for (int y = 0; y < oh; y++)
    for (int x = 0; x < ow; x++) {
//      set_vesa_bank((x * bpp + 0 + ((sl - y) * sw * bpp)) / 65536);
      unsigned char v = bmp[54 + x * 3 + ((h - 1) - y) * w * 3 + 0];
      _dos_write(handle, &v, 1, NULL);
      v = bmp[54 + x * 3 + ((h - 1) - y) * w * 3 + 1];
      _dos_write(handle, &v, 1, NULL);
      v = bmp[54 + x * 3 + ((h - 1) - y) * w * 3 + 2];
      _dos_write(handle, &v, 1, NULL);

//      _farpokew (_dos_ds, 0xa0000 + ((x * bpp + 0 + ((sl - y) * sw * bpp)) & 65535),
//        cv_true_to_high(bmp[i + 0], bmp[i + 1], bmp[i + 2]));
//      i += 3;
    }

  _dos_close(handle);
}


short cv_true_to_high (int true_red, int true_green, int true_blue)
{
  int hi_red = (true_red >> 3) + 1;
  int hi_green = (true_green >> 2) + 1;
  int hi_blue = (true_blue >> 3) + 1;

  return (hi_red) + (hi_green << 5) + (hi_blue << 11);
}


double frac (double n)
{
  return n - floor(n);
}


double tl_area (double x, double y)
{
  return (1.0 - frac(x)) * (1.0 - frac(y));
}


double tr_area (double x, double y)
{
  return frac(x) * (1.0 - frac(y));
}


double bl_area (double x, double y)
{
  return (1.0 - frac(x)) * frac(y);
}


double br_area (double x, double y)
{
  return frac(x) * frac(y);
}


double te_area (double x, double y)
{
  return 1.0 - frac(y);
}


double le_area (double x, double y)
{
  return 1.0 - frac(x);
}


double re_area (double x, double y)
{
  return frac(x);
}


double be_area (double x, double y)
{
  return frac(y);
}


double calc_texel_area (double x_scale, double y_scale)
{
  1.0 / (x_scale * y_scale);
}


int_pair get_tl_texel (point_2d p)
{
  int_pair r;

  r.x = (int)floor(p.x);
  r.y = (int)floor(p.y);
  
  return r;
}


int_pair get_bl_texel (point_2d p)
{
  int_pair r;

  r.x = (int)floor(p.x);
  r.y = (int)ceil(p.y);
  
  return r;
}


int_pair get_tr_texel (point_2d p)
{
  int_pair r;

  r.x = (int)ceil(p.x);
  r.y = (int)floor(p.y);
  
  return r;
}


int_pair get_br_texel (point_2d p)
{
  int_pair r;

  r.x = (int)ceil(p.x);
  r.y = (int)ceil(p.y);
  
  return r;
}


texel_type get_texel (texel_type* buff, int_pair p, int w)
{
  return buff[p.x + (p.y << w)];
}


void mix_texels
  (double& r, double& g, double& b, double x, double y, double dx, double dy)
{
//  int_pair tl = get_tl_texel (x, y);
//  int_pair tr = get_tr_texel (x + dx, y);
//  int_pair bl = get_bl_texel (x, y + dy);
//  int_pair br = get_br_texel (x + dx, y + dy);

//  double ra, ga, ba;

//  ra =
}
