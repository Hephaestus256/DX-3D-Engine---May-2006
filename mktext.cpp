#include "gfx2d.h"


int main()
{
  int handle, len;
  
  set_vesa_mode (0x13);

  printf ("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  _dos_open("text.dat", 0x02, &handle);

  for (int c = 0; c < 256; c++) {
    true_clr rgb;
    
    printf ("%c\n", c);
    for (int y = 0; y < 8; y++)
      for (int x = 0; x < 8; x++) {
        rgb = rgb_point (x, 200 - 8 + y);
        _dos_write(handle, &rgb, 3, NULL);
      }
  }
  
  getchar();
  _dos_close(handle);

  return 0;
}




