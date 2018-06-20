#include "make.h"
#include "fixed.h"
#include "math2d.h"
#include "math3d.h"
#include "gfx2d.h"
#include "gfx3d.h"

bmp_type _tex;

#include "proctri.h"


int main()
{
  view_type view;
  unsigned* x2_buff;
  ray_type cursor;
  point_type* first_point = NULL;
  tri_group* first_group = NULL;

  if (!init_mouse()) {
    printf ("Can't initialize mouse.");
    getchar();
    return 0;
  }
  set_mouse_play();
  init_keyb();
  _font = load_text();
  set_vesa_mode (TRUE_COLOR_320x200);
//  set_vesa_mode (TRUE_COLOR_640x480);
  x2_buff = create_x2_buff_32();
  init_view (&view, x2_buff, deg_to_rad(45), deg_to_rad(45) * 0.75);
  load_scene ("3dfx.dat", &cursor, NULL, NULL, &first_point, &first_group);

  load_tex ("wood2.tex", &_tex);

  while (true) {
    _r = 255; _g = 255; _b = 255;
//    clear_buff_32 (x2_buff);
    calc_view (cursor, &view.camera);
    calc_rel_3d_points
      (view.camera.pos, view.camera.angle_xz, view.camera.angle_yz,
      view.camera.angle_xy, first_point);
//    for (point_type* pt = first_point; pt != NULL; pt = pt->next)
//      show_corner (*pt, view);
    for (tri_group* g = first_group; g != NULL; g = g->next)
      render_group (*g, view);
//    show_x2_buff_32 (x2_buff, 0);
    show_cursor (view);
    for (point_type* p = first_point; p != NULL; p = p->next)
      show_corner (*p, view);
    bprint ("x ", view.camera.pos.x, 1);
    bprint ("y ", view.camera.pos.y, 2);
    bprint ("z ", view.camera.pos.z, 3);
    bprint ("xz ", rad_to_deg(view.camera.angle_xz), 4);
    bprint ("yz ", rad_to_deg(view.camera.angle_yz), 5);
//    show_text(x2_buff);
    show_frame (x2_buff);
    check_controls__make__ (&cursor, view, &first_point, &first_group);
  }

  return 0;
}
