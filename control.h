#ifndef INC_CONTROL
#define INC_CONTROL

#include <windows.h>
#include <\programs\math2d.h>
#include <\programs\keycodes.h>
#include <\programs\dx90.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <mmsystem.h>


void ini_inp_stat ()
{
	SetCursorPos(1440 / 2, 900 / 2);
}


void get_keyb_stats (unsigned char* keyb, bool* delta)
{
	static bool init = false;
	static unsigned char prev_keyb[256];
	int i;

	if (!init) {
		for (i = 0; i < 256; i++)
			prev_keyb[i] = 0;
		init = true;
	}

	GetKeyboardState (keyb);

	for (i = 0; i < 256; i++)
		delta[i] = (keyb[i] != prev_keyb[i]);
			
	for (i = 0; i < 256; i++)
		prev_keyb[i] = keyb[i];	
}


void get_inp_stat (ray_3d& cursor, prim_pool& pp, tex_pool& tp)
{
	POINT mp;
	unsigned char keyb[256];
	bool delta[256];
	ray_3d view = calc_view (cursor);

	GetCursorPos (&mp);
	cursor.axz += double(mp.x - 1440 / 2) * (PI / 1000.0);
	cursor.ayz -= double(mp.y - 900 / 2) * (PI / 1000.0);
	
	get_keyb_stats(keyb, delta);
	exit (1);

	for (int i = 0; i < 10; i++)
		if (keyb[_KEY_1 + i] & 0x80)
			if (_build_env.tex_pal[i] != NULL)
				_build_env.curr_tex = _build_env.tex_pal[i];

	if (keyb[_KEY_F3] & 0x80)
		if (delta[_KEY_F3])
			load_world ("\\programs\\world.dat", cursor, pp, tp);
	if (keyb[_KEY_F2] & 0x80)
		if (delta[_KEY_F2])
			save_world ("\\programs\\world.dat", cursor, pp, tp);

	if (keyb[_KEY_A] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;
		
		calc_view_angles (axz, ayz);
		point_3d d = get_abs_components (1.0f, -axz, ayz, _axy);
		
		if (keyb[_KEY_SHIFT] & 0x80)
			if (delta[_KEY_A])
				cursor.pos = sum (d, cursor.pos);
			else
				;
		else
			cursor.pos = sum (d, cursor.pos);
	}
	if (keyb[_KEY_Z] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;
		
		calc_view_angles (axz, ayz);		
		point_3d d = get_abs_components (1.0f, -axz, ayz, _axy);
		
		if (keyb[_KEY_SHIFT] & 0x80)
			if (delta[_KEY_Z])
				cursor.pos = diff (d, cursor.pos);
			else
				;
		else
			cursor.pos = diff (d, cursor.pos);
	}
	if (keyb[_KEY_LEFT] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;		
		calc_view_angles (axz, ayz);		
		point_3dd d = get_abs_components (1.0, -PI / 2.0-axz, 0.0, _axy);

		if (keyb[_KEY_SHIFT] & 0x80) 
			if (delta[_KEY_LEFT]) {				
				cursor.pos.x -= (float)d.x;
				cursor.pos.y -= (float)d.y;
				cursor.pos.z -= (float)d.z;
			}
			else
				;
		else {
			cursor.pos.x -= (float)d.x;
			cursor.pos.y -= (float)d.y;
			cursor.pos.z -= (float)d.z;
		}
	}
	if (keyb[_KEY_RIGHT] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;		
		calc_view_angles (axz, ayz);		
		point_3dd d = get_abs_components (1.0, -PI / 2.0-axz, 0.0, _axy);

		if (keyb[_KEY_SHIFT] & 0x80) 
			if (delta[_KEY_RIGHT]) {				
				cursor.pos.x += (float)d.x;
				cursor.pos.y += (float)d.y;
				cursor.pos.z += (float)d.z;
			}
			else
				;
		else {
			cursor.pos.x += (float)d.x;
			cursor.pos.y += (float)d.y;
			cursor.pos.z += (float)d.z;
		}		
	}
	if (keyb[_KEY_UP] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;		
		calc_view_angles (axz, ayz);		
		
		ray_3d dir(cursor.pos, axz, ayz);		
		point_3d up = dir.up_vector();		
		point_3d d = up;

		if (keyb[_KEY_SHIFT] & 0x80) 
			if (delta[_KEY_UP]) {				
				cursor.pos.x += (float)d.x;
				cursor.pos.y += (float)d.y;
				cursor.pos.z += (float)d.z;
			}
			else
				;
		else {
			cursor.pos.x += (float)d.x;
			cursor.pos.y += (float)d.y;
			cursor.pos.z += (float)d.z;
		}						
	}
	if (keyb[_KEY_DOWN] & 0x80) {
		double axz = cursor.axz;
		double ayz = cursor.ayz;		
		calc_view_angles (axz, ayz);		
		
		ray_3d dir(cursor.pos, axz, ayz);		
		point_3d up = dir.up_vector();		
		point_3d d = up;

		if (keyb[_KEY_SHIFT] & 0x80) 
			if (delta[_KEY_DOWN]) {				
				cursor.pos.x -= (float)d.x;
				cursor.pos.y -= (float)d.y;
				cursor.pos.z -= (float)d.z;
			}
			else
				;
		else {
			cursor.pos.x -= (float)d.x;
			cursor.pos.y -= (float)d.y;
			cursor.pos.z -= (float)d.z;
		}						
	}
	//if (keyb[_KEY_A] & 0x80) 
	//	_axy += .01;
	//if (keyb[_KEY_D] & 0x80)
	//	_axy -= .01;
	if (keyb[_KEY_RETURN] & 0x80)
		if (delta[_KEY_RETURN])
			pp.create_unused_vert(cursor.pos);
	if (keyb[_KEY_LBUTTON] & 0x80)
		if (delta[_KEY_LBUTTON]) {
			D3DXVECTOR3 sel[256];
			int sel_ct = pp.get_sel_verts(sel);
			float d;
			bool create = false;
			float th = 10.0;
			bool rev;
			float start_y;
			float h;

			if (sel_ct == 0) {
				sel[0] = cursor.pos;
				sel[1] = offset_3d (cursor.pos, 20.0, cursor.axz, 0.0, 0.0);
				create = true;
			}
			if (sel_ct == 1) {
				sel[1] = cursor.pos;
				create = true;
			}
			else if (sel_ct == 2) 
				create = true;

			if (_build_env.curr_tex == NULL)
				create = false;

			rev = (point_rel_xz (view.pos, sel[0], sel[1]) == 1);

			if (create) {
				double a;
				d = (float)dist_xz (sel[0], sel[1]);
				
				if (approx_equal (sel[0].y, sel[1].y)) 
					h = 10.0f;
				else 
					h = (float)fabs(sel[1].y - sel[0].y);					

				if (sel[0].y < sel[1].y)
					start_y = sel[0].y;
				else
					start_y = sel[1].y;				
				
				if (approx_equal (sel[0].z, sel[1].z)) 
					if (rev)
						if (sel[1].x < sel[0].x) {
							a = angle_xz (sel[0], sel[1]);
							pp.create_wall (sel[0].x, start_y, sel[0].z, d, h, th, a, _build_env.curr_tex);
						}
						else {
							a = angle_xz (sel[1], sel[0]);
							pp.create_wall (sel[1].x, start_y, sel[1].z, d, h, th, a, _build_env.curr_tex);
						}												
					else
						if (sel[0].x < sel[1].x) {
							a = angle_xz (sel[0], sel[1]);
							pp.create_wall (sel[0].x, start_y, sel[0].z, d, h, th, a, _build_env.curr_tex);
						}
						else {
							a = angle_xz (sel[1], sel[0]);
							pp.create_wall (sel[1].x, start_y, sel[1].z, d, h, th, a, _build_env.curr_tex);
						}												
				else if (sel[0].z > sel[1].z) {
					if (rev) {
						a = angle_xz (sel[1], sel[0]);
						pp.create_wall (sel[1].x, start_y, sel[1].z, d, h, th, a, _build_env.curr_tex);
					}
					else {
						a = angle_xz (sel[0], sel[1]);
						pp.create_wall (sel[0].x, start_y, sel[0].z, d, h, th, a, _build_env.curr_tex);
					}
				}
				else 
					if (!rev) {
						a = angle_xz (sel[1], sel[0]);
						pp.create_wall (sel[1].x, start_y, sel[1].z, d, h, th, a, _build_env.curr_tex);
					}
					else {
						a = angle_xz (sel[0], sel[1]);
						pp.create_wall (sel[0].x, start_y, sel[0].z, d, h, th, a, _build_env.curr_tex);
					}

				del_marked_uv ();
			}
			pp.clear_marked ();
		}

	if (keyb[_KEY_C] & 0x80) 
		pp.prune_verts();
	if (keyb[_KEY_ESCAPE] & 0x80)
		pp.clear_sel_verts();
	if (keyb[_KEY_RBUTTON] & 0x80)
		if (delta[_KEY_RBUTTON]) {
			D3DXVECTOR3 p;
			pp.clear_marked();
			pp.in_scope_vert (cursor, p);	
			pp.select_marked ();
		}
	if (keyb[_KEY_S] & 0x80)
		if (delta[_KEY_S])
			_build_env.writable.snap = !_build_env.writable.snap;
	if (keyb[_KEY_M] & 0x80) 
		if (delta[_KEY_M]) {
			D3DXVECTOR3 p;
			if (pp.in_scope_vert (cursor, p))	
				cursor.pos = p;			
		}
	if (keyb[_KEY_I] & 0x80)
		if (delta[_KEY_I]) {
			ray_3d cs = calc_view (cursor);
			point_3d v_int_t;
			int n = pp.ray_int_prim (cs, v_int_t);	

			if (n != -1) {
				cursor.pos = v_int_t;

				poly_type* p = pp.poly_of_prim (n);
				pp.number_solids ();
				dprint ("n ", p->solid->n, 13);
			}
		}
	if (keyb[_KEY_P] & 0x80)
		if (delta[_KEY_P]) {
			D3DXVECTOR3 sel[256];
			//point_3d sorted[256];

			int sel_ct = pp.get_sel_verts(sel);
			//sort_by_dist (sel, sorted, sel_ct);			
			pp.create_poly (_build_env.curr_tex, pp.create_solid (NULL), sel, sel_ct, view.pos, true);
		}
	if (keyb[_KEY_N] & 0x80)
		if (delta[_KEY_N]) 
			create_blank_world (cursor, pp, tp);
	if (keyb[_KEY_Q] & 0x80)
		if (delta[_KEY_Q]) {
			point_3d sel[256];
			int ct = pp.get_sel_verts (sel);
			
			if (ct == 2) {
				point_3d temp1, temp2;
				extrapolate_quad (sel[0], sel[1], temp1, temp2);
				pp.create_unused_vert (temp1);
				pp.create_unused_vert (temp2);				
			}
			else if (ct == 3)
				pp.create_unused_vert (extrapolate_quad (sel[0], sel[1], sel[2]));
			else
				;
		}
	if (keyb[_KEY_F] & 0x80)
		if (delta[_KEY_F]) {
			point_3d sel[256];
			//point_3d sorted[256];

			int ct = pp.get_sel_verts (sel);			
			sort_by_angle (sel, ct, true);
			pp.create_floor (_build_env.curr_tex, sel, ct);
		}	
	if (keyb[_KEY_T] & 0x80)
		if (delta[_KEY_T]) {
			point_3d p3d;
			int prim = pp.ray_int_prim (view, p3d);
			poly_type* p = pp.poly_of_prim (prim);
			pp.set_texture (p->solid, _build_env.curr_tex);		
		}
	if (keyb[_KEY_D] & 0x80) 
		if (delta[_KEY_D]) {
			point_3d p3d;
			int prim = pp.ray_int_prim (view, p3d);
			
			if (prim != -1) {
				poly_type* p = pp.poly_of_prim (prim);
				//pp.number_solids ();
				//dprint ("sol num ", p->solid->n, 15);
				pp.del_solid (p->solid);
			}
		}
	if (keyb[_KEY_X] & 0x80)
		if (delta[_KEY_X]) {
			point_3d sel[256];
			int ct = pp.get_sel_verts (sel);
			plane_3d pl = calc_plane_3d (sel[0], sel[1], sel[2]);

			//dprint ("mx ", pl.mx, 8);
			//dprint ("my ", pl.my, 9);
			//dprint ("b ", pl.b, 10);

			//poly_type* polys[256]; 
			//	int pct = pp.get_polys_in_edge (sel[0], sel[1], pl, polys);

			//for (int i = 0; i < pct; i++)
			//	pp.del_poly (polys[i]);
			
			if (ct == 2) {
				poly_type* polys[256]; 
				int pct = pp.get_polys_in_edge (sel[0], sel[1], pl, true, polys);
				
				point_3d temp = offset_3d (sel[0], 1.0, -view.axz, view.ayz, 0.0);
				plane_3d plane = calc_plane_3d (sel[0], sel[1], temp);
				
				for (i = 0; i < pct; i++)
					pp.plane_cut_solid (plane, polys[i]->solid);
			}
			else if (ct > 2) {
				poly_type* polys[256]; 
				sort_by_angle (sel, ct, false);
				
				for (int j = 0; j < ct - 1; j++) {
					point_3d temp = offset_3d (sel[j + 0], 1.0, -view.axz, view.ayz, 0.0);
					plane_3d plane = calc_plane_3d (sel[j + 0], sel[j + 1], temp);				
					int pct = pp.get_polys_in_edge (sel[j + 0], sel[j + 1], pl, false, polys);
					
					for (i = 0; i < pct; i++) 
						pp.plane_cut_solid (plane, polys[i]->solid);				
				}

				point_3d temp = offset_3d (sel[ct - 1], 1.0, -view.axz, view.ayz, 0.0);
				plane_3d plane = calc_plane_3d (sel[ct - 1], sel[0], temp);				
				int pct = pp.get_polys_in_edge (sel[ct - 1], sel[0], pl, false, polys);

				for (i = 0; i < pct; i++)
					pp.plane_cut_solid (plane, polys[i]->solid);								
			}
			
		}
	if (keyb[_KEY_INSERT] & 0x80)
		if (delta[_KEY_INSERT])
			_test_ct++;
	if (keyb[_KEY_DELETE] & 0x80)
		if (delta[_KEY_DELETE])
			_test_ct--;
	if (keyb[_KEY_F5] & 0x80)
		if (delta[_KEY_F5])
			_build_env.writable.collis = !_build_env.writable.collis;

		if (keyb[_KEY_HOME] & 0x80)
		if (delta[_KEY_HOME])
			cursor.axz -= PI / 2.0;
	if (keyb[_KEY_END] & 0x80)
		if (delta[_KEY_END])
			cursor.axz += PI / 2.0;
	if (keyb[_KEY_PRIOR] & 0x80)
		if (delta[_KEY_PRIOR])
			cursor.ayz += PI / 2.0;
	if (keyb[_KEY_NEXT] & 0x80)
		if (delta[_KEY_NEXT])
			cursor.ayz -= PI / 2.0;
	if (keyb[_KEY_E] & 0x80)
		if (delta[_KEY_E]) {
			point_3d p;
			line_int_line (
				_user_vert_start, _user_vert_start->next, 
				_user_vert_start->next->next, _user_vert_start->next->next->next, p);
			create_test_point (p, 7);
			print_point ("intr ", p, 19);
		}

// keep angles within bounds
		
	if (cursor.axz > PI)
		cursor.axz = -PI;
	else
		if (cursor.axz < -PI)
			cursor.axz = PI;

	if (cursor.ayz > PI * 0.5)
		cursor.ayz = PI * 0.5;
	else
		if (cursor.ayz < -PI * 0.5)
			cursor.ayz = -PI * 0.5;
}


#endif // !INC_CONTROL