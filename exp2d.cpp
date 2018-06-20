#include <stdio.h>
#include "gfx2d.h"


int main()
{
  set_vesa_mode(TRUE_COLOR_320x200);
  pxl (100, 100, 255, 255, 255);
  getchar();

  return 0;
}
