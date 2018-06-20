#ifndef COLLIS_INC
#define COLLIS_INC

#include <\programs\dx90.h>
#include <\programs\math2d.h>
#include <\programs\math3d.h>


class collis {
	point_3d closest;
	prim_pool* pool;
	poly_type* poly;
	point_3d curr_t, tent_t;
	point_3d curr_b, tent_b;
	point_3d curr_ot, tent_ot;
	point_3d curr_ob, tent_ob;
	point_3d curr_nt, tent_nt;
	point_3d curr_nb, tent_nb;	
	float radius, height;
	point_3d result;
	plane_3d plane;
	point_3d d_fwd, d_right;
	bool side;
	point_3d vert[256];
	int dum;
	double perp_xz;
	point_3d top_hex[8], bot_hex[8], lef_hex[8], rig_hex[8], front_hex[8];

	bool pretest_hit ();
	bool front (point_3d& res);
	bool top (point_3d& res);
	bool bot (point_3d& res);
	bool left (point_3d& res);
	bool right (point_3d& res);
	bool area_touch_y (point_3d* path, point_3d* path_sorted, point_3d& res, int dir);		
	point_3d calc_new_loc (point_3d intersect);
	point_3d cut_path_tb (point_3d intersect);
	point_3d cut_path_f (point_3d intersect);
	void register_hit_y (bool& flag, point_3d& first, point_3d& cross, int dir, int& ct);
	bool parse_poly_edge_y (point_3d* area, point_3d p1, point_3d p2, int dir, bool& inside_hit, point_3d& first);
	void sort_path_y (point_3d* path, point_3d* path_sorted, int dir);			
	void sort_path_xz (point_3d* path, point_3d* path_sorted);
	void setup_area (point_3d* path, point_3d* area);
	int seg_rel_edge_xy_inc (point_3d i1, point_3d i2, point_3d e1, point_3d e2, point_3d* intersect, int& ict);
	int seg_rel_edge_xy_exc (point_3d i1, point_3d i2, point_3d e1, point_3d e2, point_3d* intersect, int& ict);
	int seg_rel_poly_xy (point_3d i1, point_3d i2, point_3d* intersect, int& ict);
	bool area_touch (point_3d* path, point_3d& res);
	bool point_in_area_xy (point_3d p, point_3d* a, point_3d* b);
	bool edge_in_area_xy (point_3d p1, point_3d p2, point_3d* a, point_3d* b, point_3d* intersect, int& ict);
	bool poly_in_area (point_3d* a, point_3d* b, point_3d* path, point_3d& result);
	point_3d cut_path (point_3d* path, point_3d res);
	bool edge_rel_line_xy (point_3d l1, point_3d l2, point_3d e1, point_3d e2, point_3d* intersect, int& ict);
	void edge_cross_poly_edges (point_3d e1, point_3d e2, point_3d* intersect, int& ict);
	void create_impact_points (double da, point_3d e1, point_3d e2, point_3d path, double rad, point_3d* r);
	bool area_cross_edge (double da, point_3d e1, point_3d e2, point_3d* path, point_3d& closer);
	point_3d closest_hit (double m, point_3d* intersect, int ict, point_3d* path, point_3d off);
	void sort_path_xz (point_3d* path);
	bool get_path (point_3d* path);
	point_3d calc_new_loc (point_3d p, point_3d curr_o, point_3d tent_o);
	point_3d cut_path_shield (point_3d off, line_2d shield);
	void check_shield (double da, point_3d* path, point_3d& r, bool& hit);
	point_3d get_shield_off (double da, point_3d e1, point_3d e2, double offset);
	void create_shield_path (double a, point_3d* hex_solid);
	void create_shield_paths (double a);
	point_3d closest_hit_top (point_3d* imps, int ict);
	point_3d closest_hit_bot (point_3d* imps, int ict);
	point_3d deflect_plane (point_3d p);
public:
	bool collis_detect (
		prim_pool* pp, poly_type* p, point_3d& c_t, point_3d& t_t, float rad, float height, point_3d& result);
} _collis;


bool collis::collis_detect (
	prim_pool* pp, poly_type* p, point_3d& c_t, point_3d& t_t, float rad, float h, point_3d& r)
{
	if (approx_equal (c_t, t_t))
		return false;

	pool = pp;
	poly = p;
	radius = rad;
	height = h;

	double ax, ay;
	pp->get_poly_unsorted (p, vert);
	plane = calc_plane_3d (vert);
	side = visible_side (vert);
	
	bool hit = false;

	sort_by_angle (vert, p->verts, false);
	calc_angles (plane, !side, ax, ay);
	perp_xz = ax - PI / 2.0;
	d_fwd = offset_3d (ORIG, -rad, ax + PI / 2.0);		
	d_right = offset_3d (ORIG, -rad, ax);

	curr_t = c_t;
	tent_t = t_t;
	curr_b = offset_3d (c_t, 0.0f, -h, 0.0f);
	tent_b = offset_3d (t_t, 0.0f, -h, 0.0f);
	
	curr_ot = sum (d_fwd, curr_t);
	tent_ot = sum (d_fwd, tent_t);
	curr_ob = sum (d_fwd, curr_b);
	tent_ob = sum (d_fwd, tent_b);

	curr_nt = diff (d_fwd, curr_t);
	tent_nt = diff (d_fwd, tent_t);
	curr_nb = diff (d_fwd, curr_b);
	tent_nb = diff (d_fwd, tent_b);

	//point_3d closest = tent_t;
	point_3d pt;
	r = tent_t;

	create_shield_paths (ax);

	if (top (pt)) {
		set_closer (pt, curr_t, r);
		hit = true;
	}

	if (bot (pt)) {
		set_closer (pt, curr_t, r);
		hit = true;
	}

	if (left (pt)) {
		pt = calc_new_loc (pt);
		set_closer (pt, curr_t, r);
		hit = true;
	}

	if (right (pt)) {
		pt = calc_new_loc (pt);
		set_closer (pt, curr_t, r);
		hit = true;
	}

	if (front (pt)) {
		pt = calc_new_loc (pt);
		set_closer (pt, curr_t, r);
		hit = true;
	}

	if (hit) {
		r = deflect_plane (r);
	}

	return hit;
}


bool collis::top (point_3d& res)
{
	point_3d intr[256];
	int ct = 0;
	bool h = poly_int_hex_solid (vert, poly->verts, top_hex, intr, ct);

//	for (int i = 0; i < ct; i++)
//		create_test_point (intr[i], i, RGB_GREEN);

	res = closest_hit_bot (intr, ct);
//	create_test_point (res, ct, RGB_YELLOW);
	if (h && (tent_t.y != curr_t.y))	
		return h;
	else 
		return false;
}


bool collis::bot (point_3d& res)
{
	point_3d intr[256];
	int ct = 0;
	bool h = poly_int_hex_solid (vert, poly->verts, bot_hex, intr, ct);

	for (int i = 0; i < ct; i++)
		create_test_point (intr[i], i, RGB_GREEN);

	res = closest_hit_bot (intr, ct);
//	create_test_point (res, ct, RGB_YELLOW);
	if (h && (tent_t.y != curr_t.y))	
		return h;
	else 
		return false;
}


bool collis::left (point_3d& res)
{
	point_3d intr[256];
	int ct;
	bool h = poly_int_hex_solid (vert, poly->verts, lef_hex, intr, ct);

	point_3d path[2];
	path[0] = point_on_plane_xz (curr_t, plane);
	path[1] = point_on_plane_xz (tent_t, plane);
	double m = calc_dzdx_m (lef_hex[0], lef_hex[1]);
	res = closest_hit (m, intr, ct, path, sum (d_right, ORIG));
	
	return h;
}


bool collis::right (point_3d& res)
{
	point_3d intr[256];
	int ct;
	bool h = poly_int_hex_solid (vert, poly->verts, rig_hex, intr, ct);

	point_3d path[2];
	path[0] = point_on_plane_xz (curr_t, plane);
	path[1] = point_on_plane_xz (tent_t, plane);
	double m = calc_dzdx_m (lef_hex[0], lef_hex[1]);
	res = closest_hit (m, intr, ct, path, diff (d_right, ORIG));
	
	return h;
}


bool collis::front (point_3d& res)
{
	point_3d intr[256];
	int ct;
	bool h = poly_int_hex_solid (vert, poly->verts, front_hex, intr, ct);
	
//	for (int i = 0; i < ct; i++)
//		create_test_point (intr[i], i);

	point_3d path[2];
	path[0] = point_on_plane_xz (curr_t, plane);
	path[1] = point_on_plane_xz (tent_t, plane);
	double m = calc_dzdx_m (front_hex[0], front_hex[1]);
	res = closest_hit (m, intr, ct, path, diff (d_fwd, ORIG));

	return h;
}
	

bool collis::area_touch_y (point_3d* path, point_3d* path_sorted, point_3d& res, int dir)
{
	point_3d area[4];
	int i;
	bool swallow;
	bool hit = false;
	bool inside;
	point_3d first;

	setup_area (path, area);

	swallow = true;
	inside = true;
//	bool zero = true;

	for (i = 0; i < 4; i++) {
		int rel = point_in_poly (area[i], vert, poly->verts, dum);
		
		if (rel != -1) 
			swallow = false;
		//if (rel != 0)
		//	zero = false;
		//if (rel == 1)
		//	inside = false;
	}

	if (swallow) {
		first = path_sorted[0];
		hit = true;
	} 
	else {
		first = path_sorted[1];
		bool inside_hit = true;
		
		for (i = 0; i < poly->verts - 1; i++)
			if (parse_poly_edge_y (area, vert[i], vert[i + 1], dir, inside_hit, first))
				hit = true;

		if (parse_poly_edge_y (area, vert[poly->verts - 1], vert[0], dir, inside_hit, first))
			hit = true;
			
		if (inside_hit) 
			first = path_sorted[0];
	}

//	if (hit) 
	
	res = first;		
	return hit;// && inside;
//	}
	//else if (inside) {
	//	res = path_sorted[0];
	//	return true;
	//}

//	return false;
}


bool collis::area_touch (point_3d* path, point_3d& res)
{
	point_3d a[2], b[2];
	point_3d intersect_a[8], intersect_b[8];
	int ict_a, ict_b;

	a[0] = diff (d_right, path[0]);
	a[1] = sum (d_right, path[0]); 
	b[0] = diff (d_right, path[1]);
	b[1] = sum (d_right, path[1]);

	int r1 = seg_rel_poly_xy (a[0], a[1], intersect_a, ict_a);
	int r2 = seg_rel_poly_xy (b[0], b[1], intersect_b, ict_b);

	dprint ("r1 ", r1, 7);
	dprint ("r2 ", r2, 8);

//	if (r1 != 1) {
//		res = cut_path (path, path[0]);
//		return true;
//	}
//	else {
//		if (r2 != 1) {
			//point_3d r;
			if (poly_in_area (a, b, path, res)) 
				return true;
//			else
//				dprint ("false", 9);
			//for (int i = 0; i < poly->verts; i++)
			//	if (point_in_area_xy (vert[i], a, b))
			//		dprint ("i area true ", 11);
//		}

	return false;
}


/*
bool collis::area_touch_xz (point_3d* path, point_3d* path_sorted, point_3d& res, int dir)
{
	point_3d area[4];
	int i;
	bool swallow;
	bool hit;
	bool inside;
	point_3d first;

	setup_area (path, area);

	swallow = true;
	inside = false;

	for (i = 0; i < 4; i++) {
		int rel = point_in_poly (area[i], vert, poly->verts, dum);
		
		if (rel == 1) 
			swallow = false;
		if (rel == -1)
			inside = true;
	}

	if (swallow) {
		first = path_sorted[0];
		hit = true;
	} 
	else {
		first = path_sorted[1];
		bool inside_hit = true;

		for (i = 0; i < poly->verts - 1; i++)
			if (parse_poly_edge_xz (area, vert[i], vert[i + 1], dir, inside_hit, first))
				hit = true;

		if (parse_poly_edge_xz (area, vert[poly->verts - 1], vert[0], dir, inside_hit, first))
			hit = true;
			
		if (inside_hit) 
			first = path_sorted[0];
	}

	if (hit)
		res = first;	

	return hit && inside;
}
*/

/*
point_3d collis::calc_new_loc (point_3d intersect)
{
	return intersect;

	line_2d cut;
	line_2d path = calc_dzdx_line (curr_ot, tent_ot);
	point_3d res;

	cut.m = plane.mx;
	cut.b = plane.my * intersect.y + plane.b;
	
	point_2d xz = lines_intersect (cut, path);
	res.x = xz.x;
	res.z = xz.y;
	
	if (approx_equal (curr_ot.x, tent_ot.x))
		res.y = interpolate (curr_ot.z, curr_ot.y, tent_ot.z, tent_ot.y, res.z);
	else
		res.y = interpolate (curr_ot.x, curr_ot.y, tent_ot.x, tent_ot.y, res.x);

	return res;
}
*/

point_3d collis::cut_path_tb (point_3d intersect)
{
	point_3d res;

	if (approx_equal (curr_t.y, tent_t.y))
		if (approx_equal (curr_t.x, tent_t.x)) {
			intersect = diff (d_fwd, intersect);

			line_2d line;
			line.m = plane.mx;
			line.b = intersect.z - intersect.x * line.m;
			
			res.z = float(line.m * curr_t.x + line.b);
			res.x = curr_t.x;
			res.y = curr_t.y;
		}
		else {
			line_2d line;
			line.m = plane.mx;
			line.b = plane.my * intersect.y + plane.b; 
			line_2d pa = calc_dzdx_line (curr_ot, tent_ot);
	
			line_3d zl(curr_ot, tent_ot);

			point_2d p = lines_intersect (line, pa);
			point_3d t;

			t.y = float(zl.my * p.y + zl.by);
			t.x = p.x;
			t.z = p.y;

			res = diff (d_fwd, t);
		}
	else {
		line_3d path = calc_y_line (curr_t, tent_t);

		res.y = intersect.y;
		res.x = float(path.mx * intersect.y + path.bx);
		res.z = float(path.my * intersect.y + path.by);
	}

	return res;
}


point_3d collis::cut_path_f (point_3d intersect)
{
	point_3d res;

	if (approx_equal (curr_t.y, tent_t.y))
		if (approx_equal (curr_t.x, tent_t.x)) {
			intersect = diff (d_fwd, intersect);

			line_2d line;
			line.m = plane.mx;
			line.b = intersect.z - intersect.x * line.m;
			
			res.z = float(line.m * curr_t.x + line.b);
			res.x = curr_t.x;
			res.y = curr_t.y;
		}
		else {
			line_2d line;
			line.m = plane.mx;
			line.b = plane.my * intersect.y + plane.b; 
			line_2d pa = calc_dzdx_line (curr_ot, tent_ot);
	
			line_3d zl(curr_ot, tent_ot);

			point_2d p = lines_intersect (line, pa);
			point_3d t;

			t.y = float(zl.my * p.y + zl.by);
			t.x = p.x;
			t.z = p.y;

			res = diff (d_fwd, t);
		}
	else {
		line_3d path = calc_y_line (curr_t, tent_t);

		res.y = intersect.y;
		res.x = float(path.mx * intersect.y + path.bx);
		res.z = float(path.my * intersect.y + path.by);
	}

	return res;
}


void collis::register_hit_y (bool& flag, point_3d& first, point_3d& cross, int dir, int& ct)
{
	flag = true;

	if (trin_cmp (cross.y, first.y) == -dir)
		first = cross;				

	ct++;
}


bool collis::parse_poly_edge_y (point_3d* area, point_3d p1, point_3d p2, int dir, bool& inside_hit, point_3d& first)
{
	point_3d cross = ORIG;
	int ct = 0;
	bool flag = false;

	if (point_in_poly (p1, area, 4, dum) == -1)
		register_hit_y (flag, first, p1, dir, ct);

	if (point_in_poly (p2, area, 4, dum) == -1)
		register_hit_y (flag, first, p2, dir, ct);

	if (line_seg_intersect_inc (area[0], area[1], p1, p2, cross)) // inc
		register_hit_y (flag, first, cross, dir, ct);

	//if (dir) {
		if (line_seg_intersect_inc (area[2], area[3], p1, p2, cross)) // inc
			register_hit_y (flag, first, cross, dir, ct);
	
		if (line_seg_intersect_inc (area[1], area[2], p1, p2, cross)) 
			register_hit_y (flag, first, cross, dir, ct);

		if (line_seg_intersect_inc (area[3], area[0], p1, p2, cross)) 
			register_hit_y (flag, first, cross, dir, ct);
	//}

	//if (ct)
	//	dprint ("ct here ", ct, 7);

	if (ct >= 2) 
		if (dir)
			if (edge_y_sign (p1, p2) == dir) 
				inside_hit = false;							
	
	return ct >= 2;
}


/*
bool collis::parse_poly_edge_xz (point_3d* area, point_3d p1, point_3d p2, int dir, bool& inside_hit, point_3d& first)
{
	point_3d cross = ORIG;
	int ct = 0;
	bool flag = false;

	if (point_in_poly (p1, area, 4, dum) == -1)
		register_hit_xz (flag, first, p1, dir, ct);

	if (point_in_poly (p2, area, 4, dum) == -1)
		register_hit_xz (flag, first, p2, dir, ct);

	if (line_seg_intersect_inc (area[0], area[1], p1, p2, cross)) 
		register_hit_xz (flag, first, cross, dir, ct);

	if (line_seg_intersect_inc (area[2], area[3], p1, p2, cross)) 
		register_hit_xz (flag, first, cross, dir, ct);
	
	if (line_seg_intersect_exc (area[1], area[2], p1, p2, cross)) 
		register_hit_xz (flag, first, cross, dir, ct);

	if (line_seg_intersect_exc (area[3], area[0], p1, p2, cross)) 
		register_hit_xz (flag, first, cross, dir, ct);

	if (ct == 2) 
		if (dir)
			if (edge_y_sign (p1, p2) == dir) 
				inside_hit = false;							
	
	return ct == 2;
}
*/

void collis::sort_path_y (point_3d* path, point_3d* path_sorted, int dir)
{
	if (dir == trin_cmp (path[1].y, path[0].y)) {
		path_sorted[0] = path[0];
		path_sorted[1] = path[1];
	}
	else {
		path_sorted[0] = path[1];			
		path_sorted[1] = path[0];			
	}	
}


void collis::sort_path_xz (point_3d* path, point_3d* path_sorted)
{
	double dist[2];

	dist[0] = dist_xz (path[0], curr_ot);
	dist[1] = dist_xz (path[1], curr_ot);

	if (dist[0] < dist[1]) {
		path_sorted[0] = path[0];
		path_sorted[1] = path[1];
	}
	else {
		path_sorted[0] = path[1];
		path_sorted[1] = path[0];
	}
}


void collis::sort_path_xz (point_3d* path)
{
	double dist[2];

	dist[0] = dist_xz (path[0], curr_t);
	dist[1] = dist_xz (path[1], curr_t);

	if (dist[0] > dist[1]) 
		swap (path[0], path[1]);
}


void collis::setup_area (point_3d* path, point_3d* area)
{
	if (path[0].y < path[1].y) {
		area[0] = diff (d_right, path[1]);
		area[1] = sum (d_right, path[1]); 
		area[2] = sum (d_right, path[0]);
		area[3] = diff (d_right, path[0]);
	}
	else {
		area[0] = diff (d_right, path[0]);
		area[1] = sum (d_right, path[0]); 
		area[2] = sum (d_right, path[1]);
		area[3] = diff (d_right, path[1]);
	}
}


bool collis::pretest_hit ()
{
	return true;
	/*
	if (plane.mx == PLANE_Y_PLANE)
		if (side)
			if (tent_t
	else if (plane.mx == PLANE_XY_PLANE)
	else
	*/
}


int collis::seg_rel_poly_xy (point_3d i1, point_3d i2, point_3d* intersect, int& ict)
{
	ict = 0;

	for (int i = 0; i < poly->verts - 1; i++)
		if (seg_rel_edge_xy_inc (i1, i2, vert[i], vert[i + 1], intersect, ict) == 1) 
			return 1;

	if (seg_rel_edge_xy_inc (i1, i2, vert[i], vert[0], intersect, ict) == 1) 
		return 1;

		//	for (int i = 0; i < 3 - 1; i++)
//		dprint ("sre ", seg_rel_edge_xy (i1, i2, vert[i], vert[i + 1], intersect, ict), 16 + i);

//	dprint ("sre ", seg_rel_edge_xy (i1, i2, vert[i], vert[0], intersect, ict), 16 + i);
//		return 1;
	
	if (ict)
		return 0;
	else
		return -1;
}


int collis::seg_rel_edge_xy_inc (point_3d i1, point_3d i2, point_3d e1, point_3d e2, point_3d* intersect, int& ict)
{
	int r1 = point_rel_cw_edge_xy (i1, e1, e2);
	int r2 = point_rel_cw_edge_xy (i2, e1, e2);
	point_3d r;
	bool border = false;

//	dprint ("cw1 ", r1, 13);
//	dprint ("cw2 ", r2, 14);

	if (r1 == r2) {
		if (r1 == -1)
			return -1;
		else
			return 1;
		//if (r1 != -1)
//			return 1;
	}
	else if (r1 == -1) {
		border = true;		
		if (line_seg_intersect_inc_exc (i1, i2, e1, e2, r)) {
			intersect[ict++] = r;
			return 0;
		}
		else
			return 1;
	}
	else if (r2 == -1) {
		border = true;
		if (line_seg_intersect_inc_exc (i1, i2, e1, e2, r)) {
			intersect[ict++] = r;
			return 0;
		}
		else
			return 1;
	}
	else if (r1 == 1)
		return 1;
	else if (r2 == 1)
		return 1;
	else
		return -1;

//		return -1;

	/*
	if (border) 
		return 0;
	else 
		return -1;
		*/
}


int collis::seg_rel_edge_xy_exc (point_3d i1, point_3d i2, point_3d e1, point_3d e2, point_3d* intersect, int& ict)
{
	int r1 = point_rel_cw_edge_xy (i1, e1, e2);
	int r2 = point_rel_cw_edge_xy (i2, e1, e2);
	point_3d r;
	bool border = false;

//	dprint ("cw1 ", r1, 13);
//	dprint ("cw2 ", r2, 14);

	if (r1 == r2) {
		if (r1 == -1)
			return -1;
		else
			return 1;
		//if (r1 != -1)
//			return 1;
	}
	else if (r1 == -1) {
		border = true;		
		if (line_seg_intersect_exc (i1, i2, e1, e2, r)) {
			intersect[ict++] = r;
			return 0;
		}
		else
			return 1;
	}
	else if (r2 == -1) {
		border = true;
		if (line_seg_intersect_exc (i1, i2, e1, e2, r)) {
			intersect[ict++] = r;
			return 0;
		}
		else
			return 1;
	}
	else if (r1 == 1)
		return 1;
	else if (r2 == 1)
		return 1;
	else
		return -1;

//		return -1;

	/*
	if (border) 
		return 0;
	else 
		return -1;
		*/
}


bool collis::point_in_area_xy (point_3d p, point_3d* a, point_3d* b)
{
	if (a[0].y > b[0].y) {
		if (approx_lesser_inc (p.y, a[0].y))
			if (approx_greater_inc (p.y, b[0].y)) 
				if (point_rel_cw_edge_xy (p, b[0], a[0]) != 1)
					if (point_rel_cw_edge_xy (p, a[1], b[1]) != 1)
						return true;
	}
	else {
		if (approx_lesser_inc (p.y, b[0].y))
			if (approx_greater_inc (p.y, a[0].y)) 
				if (point_rel_cw_edge_xy (p, a[0], b[0]) != 1)
					if (point_rel_cw_edge_xy (p, b[1], a[1]) != 1)
						return true;
	}

	return false;
}
			

/*
bool collis::edge_in_area_xy (point_3d p1, point_3d p2, point_3d* a, point_3d* b, point_3d* intersect, int& ict)
{
	point_3d rel1, rel2;
	point_3d* top;
	point_3d* bot;
	
	if (a[0].y > b[0].y) { 
		top = a;
		bot = b;
	}
	else {
		top = b;
		bot = a;
	}

	
}
*/

bool collis::edge_in_area_xy (point_3d p1, point_3d p2, point_3d* a, point_3d* b, point_3d* intersect, int& ict)
{
//	return false;
	int pct = ict;

	point_3d rel1, rel2;
	point_3d* top;
	point_3d* bot;
	
	if (a[0].y > b[0].y) { 
		top = a;
		bot = b;
	}
	else {
		top = b;
		bot = a;
	}
	
	bool r1 = edge_rel_line_xy (p1, p2, top[0], top[1], intersect, ict);     
	bool r3 = edge_rel_line_xy (p1, p2, bot[0], top[0], intersect, ict);
	bool r4 = edge_rel_line_xy (p1, p2, top[1], bot[1], intersect, ict);
	bool r2 = edge_rel_line_xy (p1, p2, bot[1], bot[0], intersect, ict);

	dprint ("t ", r1, 5 + 11);
	dprint ("l ", r3, 5 + 12);
	dprint ("r ", r4, 5 + 13);
	dprint ("b ", r2, 5 + 14);

	if (!(r1 && r2 && r3 && r4)) {
		ict = pct;
//		return false;
	}

	if (point_rel_cw_edge_xy (p1, top[0], top[1]) != 1)
		if (point_rel_cw_edge_xy (p1, top[1], bot[1]) != 1)
			if (point_rel_cw_edge_xy (p1, bot[1], bot[0]) != 1)
				if (point_rel_cw_edge_xy (p1, bot[0], top[0]) != 1)
					intersect[ict++] = p1;

	if (point_rel_cw_edge_xy (p2, top[0], top[1]) != 1)
		if (point_rel_cw_edge_xy (p2, top[1], bot[1]) != 1)
			if (point_rel_cw_edge_xy (p2, bot[1], bot[0]) != 1)
				if (point_rel_cw_edge_xy (p2, bot[0], top[0]) != 1)
					intersect[ict++] = p2;

				//	if (point_rel_cw_edge_xy (p2, top[1], top[0]) == -1)
//		if (point_rel_cw_edge_xy (p2, top[1], bot[1]) == -1)
//			if (point_rel_cw_edge_xy (p2, bot[1], bot[0]) == -1)
//				if (point_rel_cw_edge_xy (p2, bot[0], top[0]) == -1)
//					exit(3);//intersect[ict++] = p2;

		//intersect[ict++] = p2;

	/*
	if (seg_rel_edge_xy (p1, p2, b[1], b[0], intersect, ict) == 1)
		return false;
	if (seg_rel_edge_xy (p1, p2, a[1], b[1], intersect, ict) == 1)
		return false;
	if (seg_rel_edge_xy (p1, p2, b[0], a[0], intersect, ict) == 1)
		return false;
*/	
	/*		
		if (seg_rel_edge_xy (p1, p2, a[0], a[1], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, b[1], b[0], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, a[1], b[1], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, b[0], a[0], intersect, ict) == 1)
			return false;
*/
/*
		if (seg_rel_edge_xy (p1, p2, b[0], b[1], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, a[1], a[0], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, b[1], a[1], intersect, ict) == 1)
			return false;
		if (seg_rel_edge_xy (p1, p2, a[0], b[0], intersect, ict) == 1)
			return false;
*/
//	if (point_in_area_xy (p1, a, b))
//		intersect[ict++] = p1;
		
//	if (point_in_area_xy (p2, a, b))
//		intersect[ict++] = p2;

	return ict > 0;
}


bool collis::poly_in_area (point_3d* a, point_3d* b, point_3d* path, point_3d& r)
{
	bool hit = false;
	//point_3d area[4];
	//int i;
//	point_3d intersect[256];
//	int ict = 0;
//	point_3d p;
	//point_3d* top;
	//point_3d* bot;

/*
	//	if (approx_equal_low (a[0].y, b[0].y))
		dprint ("dy ", a[0].y - b[0].y, 17);

	if (a[0].y > b[0].y) {
		top = a;
		bot = b;
	}
	else {
		top = b;
		bot = a;
	}
	
	area[0] = top[0];
	area[1] = top[1];
	area[2] = bot[1];
	area[3] = bot[0];
*/
	//	point_3d res[2];
//	create_impact_points (vert[0], vert[1], path[1], radius, res);
	
//	for (i = 0; i < poly->verts - 1; i++)
//		if (area_cross_edge (vert[i], vert[i + 1], path, intersect, ict))
//			hit = true;

	//path[0] = point_on_plane_xz (curr_t.x, curr_t.z, plane);
	//path[1] = point_on_plane_xz (tent_t.x, tent_t.z, plane);

///*
	if (get_path (path)) {
//		check_shield (0.0, path, r, hit);
//		check_shield (PI / 2.0, path, r, hit);
//		check_shield (-PI / 2.0, path, r, hit);
	}
//*/

//	if (get_path (path))
//		if (area_cross_edge (vert[1], vert[2], path, r))
//			hit = true;

	//	dprint ("ict = ", ict, 19);

/*				
	for (i = 0; i < 4 - 1; i++)
		edge_cross_poly_edges (area[i], area[i + 1], intersect, ict);

	edge_cross_poly_edges (area[i], area[0], intersect, ict);
*/
/*
	for (i = 0; i < poly->verts - 1; i++) {
		for (j = 0; j < 4 - 1; j++) 
			if (line_seg_intersect_inc (vert[i], vert[i + 1], area[j], area[j + 1], p))
				intersect[ict++] = p;

		if (line_seg_intersect_inc (vert[i], vert[i + 1], area[j], area[0], p))
			intersect[ict++] = p;
	}
	
	for (j = 0; j < 4 - 1; j++) 
		if (line_seg_intersect_inc (vert[i], vert[0], area[j], area[j + 1], p))
			intersect[ict++] = p;
	if (line_seg_intersect_inc (vert[i], vert[0], area[j], area[0], p))
		intersect[ict++] = p;
*/
//	dprint ("ict = ", ict, 19);

	return hit;
		/*	
	
	
	  point_3d intersect[256];
	int ict = 0;

	//edge_in_area_xy (vert[0], vert[1], a, b, intersect, ict);

	for (int i = 0; i < poly->verts - 1; i++)
		edge_in_area_xy (vert[i], vert[i + 1], a, b, intersect, ict);
	edge_in_area_xy (vert[poly->verts - 1], vert[0], a, b, intersect, ict);

	dprint ("ict ", ict, 15);

	if (ict) {		
		r = get_extr_y (intersect, ict, trin_cmp (a[0].y, b[0].y));					
		r = cut_path (path, r);
		//		create_nu_test_point (r);
		return true;
	}
	else
		return false;
		*/
}


point_3d collis::closest_hit (double m, point_3d* intersect, int ict, point_3d* path, point_3d off)
{
	//double m = calc_dzdx_m (e1, e2);
	double b0 = calc_dzdx_b (path[0], m);
	double b1 = calc_dzdx_b (path[1], m);

	if (approx_equal (b0, b1)) {
		exit(1);
		return intersect[0];
	}
	else if (b0 < b1) {
		double min_b = calc_dzdx_b (sum (off, intersect[0]), m);
		//point_3d min = intersect[0];

		for (int i = 0; i < ict; i++) {
			double b = calc_dzdx_b (sum (off, intersect[i]), m);
		
			if (b < min_b) {
				min_b = b;
		//		min = intersect[i];
			}
		}
		line_2d l(m, min_b);
		line_2d pl = calc_dzdx_line (path[0], path[1]);
		point_2d ixz = lines_intersect (l, pl);
		
		return point_on_plane_xz (ixz.x, ixz.y, plane);		
	}
	else {
		double max_b = calc_dzdx_b (sum (off, intersect[0]), m);
		//point_3d max = intersect[0];

		for (int i = 0; i < ict; i++) {
			double b = calc_dzdx_b (sum (off, intersect[i]), m);
		
			if (b > max_b) {
				max_b = b;
				//max = intersect[i];
			}
		}

		line_2d l(m, max_b);
		line_2d pl = calc_dzdx_line (path[0], path[1]);
		point_2d ixz = lines_intersect (l, pl);
		
		return point_on_plane_xz (ixz.x, ixz.y, plane);		
	}
}


point_3d collis::cut_path (point_3d* path, point_3d res)
{	
	/*
	//line_3d e(e1, e2); 

	if (approx_equal (curr_t.x, tent_t.x))
		if (approx_equal (curr_t.z, tent_t.z)) 
			return res;
		else
			return res;
	else {
		line_3d e(e1, e2);
		line_3d l(e.mx, res.z - e.mx * res.x);
		line_3d pl = calc_dzdx_line (curr_t, tent_t);
		point_2d nter = lines_intersect (l, pl);
		point_3d r(nter.x, res.y, nter.y);

		point_3d p = diff (d_fwd, res);
		line_2d l(plane.mx, p.z - plane.mx * p.x);
		line_2d path_line = calc_dzdx_line (curr_t, tent_t);//path[0], path[1]);
		point_2d nter = lines_intersect (l, path_line);
		point_3d r(nter.x, p.y, nter.y);

		return r;	
	}
*/
	point_3d r;
	return r;
}

	
point_3d collis::calc_new_loc (point_3d p)
{
	point_3d r;

	if (!approx_equal (curr_t.x, tent_t.x)) {
		line_2d path = calc_dydx_line (curr_t, tent_t);
		r.x = p.x;
		r.z = p.z;
		r.y = float(path.m * r.x + path.b);
	}
	else if (!approx_equal (curr_t.z, tent_t.z)) {
		line_2d path = calc_dydz_line (curr_t, tent_t);
		r.x = p.x;
		r.z = p.z;
		r.y = float(path.m * r.z + path.b);
	}		
	else
		//if (curr_t.y > tent_t.y)
			r = p;
		//else
		//	r = offset_3d (p, 0.0f, height, 0.0f);

	return r;//diff (d_fwd, r);
}


point_3d collis::cut_path_shield (point_3d off, line_2d shield)
{
	//line_2d edge = calc_dzdx_line (e1, e2);
	line_2d path = calc_dzdx_line (diff (off, curr_t), diff (off, tent_t));

	point_2d in = lines_intersect (shield, path);	
	return point_3d (in.x, 0.0f, in.y);
}


point_3d collis::calc_new_loc (point_3d p, point_3d curr_o, point_3d tent_o)
{
	point_3d r;

	if (!approx_equal (curr_o.x, tent_o.x)) {
		line_2d path = calc_dydx_line (curr_o, tent_o);
		r.x = p.x;
		r.z = p.z;
		r.y = float(path.m * r.x + path.b);
	}
	else if (!approx_equal (curr_o.z, tent_o.z)) {
		line_2d path = calc_dydz_line (curr_o, tent_o);
		r.x = p.x;
		r.z = p.z;
		r.y = float(path.m * r.z + path.b);
	}		
	else
		//if (curr_t.y > tent_t.y)
			r = p;
		//else
		//	r = offset_3d (p, 0.0f, height, 0.0f);

	return r;//diff (d_fwd, r);
}


bool collis::edge_rel_line_xy (point_3d e1, point_3d e2, point_3d l1, point_3d l2, point_3d* intersect, int& ict)
{
	int pct = ict;

	if (seg_rel_edge_xy_inc (l1, l2, e1, e2, intersect, ict) == 1) {
		ict = pct;
		return false;
	}

	int r1 = point_rel_cw_edge_xy (e1, l1, l2);
	int r2 = point_rel_cw_edge_xy (e2, l1, l2);
	point_3d p;

	dprint ("e1 ", r1, 13);
	dprint ("e2 ", r2, 14);

	if (r1 == r2) 
		return r1 == -1;	
	else if (r1 == -1) {
		if (line_seg_intersect_inc_exc (l1, l2, e1, e2, p)) 
			intersect[ict++] = p;
		return true;
	}
	else if (r2 == -1) {
		if (line_seg_intersect_inc_exc (l1, l2, e1, e2, p)) 
			intersect[ict++] = p;
		return true;
	}
	else
		return false;
}


void collis::edge_cross_poly_edges (point_3d e1, point_3d e2, point_3d* intersect, int& ict)
{
	point_3d p;
	int i;

	for (i = 0; i < poly->verts - 1; i++)
		if (line_seg_intersect_inc (vert[i], vert[i + 1], e1, e2, p))
			intersect[ict++] = p;

	if (line_seg_intersect_inc (vert[i], vert[0], e1, e2, p))
		intersect[ict++] = p;		
}


bool collis::area_cross_edge (double da, point_3d e1, point_3d e2, point_3d* path, point_3d& closer)
{
	int i;
	point_3d area[4];
	double e_in = -PI / 2.0 + angle_xz (e1, e2);
	point_3d intersect[256];
	point_3d o_path[2];
	int ict = 0;
	point_3d p;

	o_path[0] = offset_3d (path[0], radius, e_in + da + 0.0 * PI);
	o_path[1] = offset_3d (path[1], radius, e_in + da + 0.0 * PI);

	area[0] = point_on_plane_xz (offset_3d (o_path[0], radius, e_in + da - PI / 2.0), plane);
	area[1] = point_on_plane_xz (offset_3d (o_path[0], radius, e_in + da + PI / 2.0), plane);
	area[2] = point_on_plane_xz (offset_3d (o_path[1], radius, e_in + da + PI / 2.0), plane);
	area[3] = point_on_plane_xz (offset_3d (o_path[1], radius, e_in + da - PI / 2.0), plane);

	// create_test_point (e1, 40, RGB_YELLOW);
	// create_test_point (e2, 41, RGB_YELLOW);
	// create_test_point (path[0], 2, RGB_GREEN);
	// create_test_point (path[1], 3, RGB_GREEN);
	
//	for (i = 0; i < 4; i++)
//		create_test_point (area[i], i + 0, RGB_BLUE);// + (i * 64));

//	for (i = 0; i < 2; i++)
//		create_test_point (path[i], 16 + i, RGB_GREEN);

	if (point_in_poly (e1, area, 4, dum) != 1)
		intersect[ict++] = e1;

	if (point_in_poly (e2, area, 4, dum) != 1)
		intersect[ict++] = e2;

	for (i = 0; i < 4; i++)
		if (point_in_poly (area[i], vert, poly->verts, dum) != 1)
			intersect[ict++] = area[i];

	for (i = 0; i < 4 - 1; i++)
		if (line_seg_intersect_inc (e1, e2, area[i], area[i + 1], p))
			intersect[ict++] = p;

	if (line_seg_intersect_inc (e1, e2, area[3], area[0], p))
		intersect[ict++] = p;

//	for (i = 0; i < ict; i++)
//		create_test_point (intersect[i], i + 25, RGB_GREEN);

	if (polys_intersect (vert, poly->verts, area, 4)) { 
		// polys_intersect (vert, poly->verts, area, 4)) { 
		//is_non_zero_area (intersect, ict)) {
		double m = calc_dzdx_m (area[0], area[1]);
		point_3d p;// = closest_hit (m, intersect, ict, o_path);
		//create_test_point (p, 39, RGB_BLUE);
		//create_test_point (p, 40, RGB_BLACK);
//		print_point ("res ", p, 15);
		//create_test_point (tent_t, 41, RGB_RED);
		//create_test_point (, 50, RGB_GREEN);
		//		point_3d p = cut_path_shield (off, shield);
//		closer = calc_new_loc (sum (off, p));
//		point_3d co = diff (off, curr_t);
//		point_3d to = diff (off, tent_t);
		closer = calc_new_loc (offset_3d (p, radius, PI + e_in + da));
		//create_test_point (closer, 33, RGB_GREEN);
		return true;		
	}
	else
		return false;

/*
	point_3d area[4];
	point_3d p;
	point_3d o_path[2];
	point_3d off = get_shield_off (da, e1, e2, radius);
	int i;
	point_3d intersect[256];
	int ict = 0;
	
	//off = ORIG;
	o_path[0] = diff (off, path[0]);
	o_path[1] = diff (off, path[1]);

	create_impact_points (da, e1, e2, o_path[0], radius, &area[2]);
	create_impact_points (da, e1, e2, o_path[1], -radius, &area[0]);

	line_2d shield = calc_dzdx_line (area[0], area[1]);
	//for (i = 0; i < 4; i++)

	if (!is_poly_cw_in_xz (area, 4)) {
		swap (area[0], area[1]);
		swap (area[2], area[3]);
	}

	int pct = ict;

	if (!da) {
		if (line_seg_intersect_inc (e1, e2, area[1], area[2], p))
			intersect[ict++] = p;

		if (line_seg_intersect_inc (e1, e2, area[3], area[0], p))
			intersect[ict++] = p;

		if (line_seg_intersect_inc (e1, e2, area[0], area[1], p))
			intersect[ict++] = p;

		if (line_seg_intersect_inc (e1, e2, area[2], area[3], p))
			intersect[ict++] = p;
	}
	//	if (pct != ict)

	if (point_in_poly (e1, area, 4, dum) != 1)
		intersect[ict++] = e1;

	if (point_in_poly (e2, area, 4, dum) != 1)
		intersect[ict++] = e2;
	
	if (!da) {
		for (i = 0; i < 4; i++)
			if (point_in_poly (area[i], vert, poly->verts, dum) != 1)
				intersect[ict++] = area[i];
	}

	if (is_non_zero_area (intersect, ict)) {	
		point_3d r = closest_hit (shield.m, intersect, ict, path);
		point_3d p = cut_path_shield (off, shield);
//		closer = calc_new_loc (sum (off, p));
		//point_3d co = diff (off, curr_t);
		//point_3d to = diff (off, tent_t);
		//closer = calc_new_loc (sum (off, r));

		//print_point ("hit ", closer, 18);
		//r = cut_path (path, r);
		//return true;
		return false;
	}
	else	
		return false;
		*/
}


void collis::create_impact_points (double da, point_3d e1, point_3d e2, point_3d path, double rad, point_3d* r)
{
//	point_3d p[2];
	point_3d p;
	double a = da + angle_xz (e1, e2);
	line_3d l;

	p = offset_3d (path, rad, PI + a);
	r[0] = point_on_plane_xz (p.x, p.z, plane);

	p = offset_3d (path, rad, a);
	r[1] = point_on_plane_xz (p.x, p.z, plane);

	/*
	p[0] = offset_3d (path, -rad, a);
	p[1] = offset_3d (p[0], 0.0f, 1.0f, 0.0f);
	l = calc_line (p[0], p[1]);
	line_3d_int_plane (l, plane, r[0]);

	p[0] = offset_3d (path, rad, a);
	p[1] = offset_3d (p[0], 0.0f, 1.0f, 0.0f);
	l = calc_line (p[0], p[1]);
	line_3d_int_plane (l, plane, r[1]);
	*/
}


bool collis::get_path (point_3d* path)
{
	bool hit[4];
	point_3d p[4];
	double a = 0;

	hit[0] = seg_int_plane_inc (offset_3d (curr_t, radius, a), offset_3d (tent_t, radius, a), plane, p[0]);
	hit[1] = seg_int_plane_inc (offset_3d (curr_b, radius, a), offset_3d (tent_b, radius, a), plane, p[1]);
	hit[2] = seg_int_plane_exc (offset_3d (curr_t, radius, a), offset_3d (curr_b, radius, a), plane, p[2]);
	hit[3] = seg_int_plane_exc (offset_3d (tent_t, radius, a), offset_3d (tent_b, radius, a), plane, p[3]);

	int ct = 0;

	for (int i = 0; i < 4; i++)
		if (hit[i])
			path[ct++] = p[i];

	sort_path_xz (path);
	return ct == 2;
}


void collis::check_shield (double a, point_3d* path, point_3d& r, bool& hit)
{
	point_3d area[4];

	area[0] = point_on_plane_xz (offset_3d (path[0], radius, a - PI / 2.0), plane);
	area[1] = point_on_plane_xz (offset_3d (path[0], radius, a + PI / 2.0), plane);
	area[2] = point_on_plane_xz (offset_3d (path[1], radius, a + PI / 2.0), plane);
	area[3] = point_on_plane_xz (offset_3d (path[1], radius, a - PI / 2.0), plane);
	
//	for (int i = 0; i < 4; i++)
//		create_test_point (area[i], i, RGB_BLACK + i * 256 * 64);
/*
	int i;
	point_3d p;

	for (i = 0; i < poly->verts - 1; i++)
		if (area_cross_edge (da, vert[i], vert[i + 1], path, p)) {
			hit = true;
			if (closer (curr_t, p, r))
				r = p;
		}

	if (area_cross_edge (da, vert[i], vert[0], path, p)) {
		hit = true;
		if (closer (curr_t, p, r))
			r = p;
	}			
*/
//	int n1 = 0;
//	int n2 = 1;
//	hit = area_cross_edge (da, vert[n1], vert[n2], path, r);
	//create_test_point (vert[n1], 15, RGB_YELLOW);
	//create_test_point (vert[n2], 16, RGB_YELLOW);	
}


point_3d collis::get_shield_off (double da, point_3d e1, point_3d e2, double offset)
{
	double a = da + PI / 2.0 + angle_xz (e1, e2);
	return offset_3d (ORIG, 1.0, a);
}


void collis::create_shield_path (double a, point_3d* hex_solid)
{
	point_3d neg_off = offset_3d (ORIG, radius * sqrt(2.0), a + PI / 2.0 - PI / 4.0);
	point_3d pos_off = offset_3d (ORIG, radius * sqrt(2.0), a + PI / 2.0 + PI / 4.0);

	hex_solid[0] = sum (curr_t, neg_off);
	hex_solid[1] = sum (curr_t, pos_off);
	hex_solid[2] = sum (curr_b, pos_off);
	hex_solid[3] = sum (curr_b, neg_off);

	hex_solid[4] = sum (tent_t, neg_off);
	hex_solid[5] = sum (tent_t, pos_off);
	hex_solid[6] = sum (tent_b, pos_off);
	hex_solid[7] = sum (tent_b, neg_off);
}


void collis::create_shield_paths (double a)
{
	point_3d neg_off = offset_3d (ORIG, radius * sqrt(2.0), a - PI / 2.0 - PI / 4.0);
	point_3d pos_off = offset_3d (ORIG, radius * sqrt(2.0), a - PI / 2.0 + PI / 4.0);

	top_hex[0] = sum (neg_off, curr_t); //
	top_hex[1] = sum (pos_off, curr_t); // 
	top_hex[2] = diff (neg_off, curr_t);
	top_hex[3] = diff (pos_off, curr_t);
	top_hex[4] = sum (neg_off, tent_t);
	top_hex[5] = sum (pos_off, tent_t);
	top_hex[6] = diff (neg_off, tent_t);
	top_hex[7] = diff (pos_off, tent_t);

	bot_hex[0] = sum (neg_off, curr_b); 
	bot_hex[1] = sum (pos_off, curr_b);
	bot_hex[2] = diff (neg_off, curr_b);
	bot_hex[3] = diff (pos_off, curr_b);
	bot_hex[4] = sum (neg_off, tent_b);
	bot_hex[5] = sum (pos_off, tent_b);
	bot_hex[6] = diff (neg_off, tent_b);
	bot_hex[7] = diff (pos_off, tent_b);

	front_hex[0] = top_hex[0]; // 
	front_hex[1] = top_hex[1]; // 
	front_hex[2] = bot_hex[1];
	front_hex[3] = bot_hex[0];
	front_hex[4] = top_hex[4];
	front_hex[5] = top_hex[5];
	front_hex[6] = bot_hex[5];
	front_hex[7] = bot_hex[4];

	lef_hex[0] = top_hex[1];
	lef_hex[1] = top_hex[2];
	lef_hex[2] = bot_hex[2];
	lef_hex[3] = bot_hex[1];
	lef_hex[4] = top_hex[5];
	lef_hex[5] = top_hex[6];
	lef_hex[6] = bot_hex[6];
	lef_hex[7] = bot_hex[5];

	rig_hex[0] = top_hex[0];
	rig_hex[1] = top_hex[3];
	rig_hex[2] = bot_hex[3];
	rig_hex[3] = bot_hex[0];
	rig_hex[4] = top_hex[4];
	rig_hex[5] = top_hex[7];
	rig_hex[6] = bot_hex[7];
	rig_hex[7] = bot_hex[4];
}
	

point_3d collis::closest_hit_top (point_3d* imps, int imp_ct)
{
	point_3d first = tent_t;

	if (tent_t.y > curr_t.y) { // up
		for (int i = 0; i < imp_ct; i++)
			if (imps[i].y < first.y)
				first = imps[i];

		line_2d xy = calc_dxdy_line (curr_t, tent_t);
		line_2d zy = calc_dzdy_line (curr_t, tent_t);

		return point_3d (float(xy.m * first.y + xy.b), first.y, float(zy.m * first.y + zy.b));
	}
	else { // down
		for (int i = 0; i < imp_ct; i++)
			if (imps[i].y > first.y)
				first = imps[i];	

		line_2d xy = calc_dxdy_line (curr_b, tent_b);
		line_2d zy = calc_dzdy_line (curr_b, tent_b);

		return offset_3d (point_3d (float(xy.m * first.y + xy.b), first.y, float(zy.m * first.y + zy.b)), 0.0f, height, 0.0f);
	}
}


point_3d collis::closest_hit_bot (point_3d* imps, int imp_ct)
{
	point_3d first;

	if (tent_t.y > curr_t.y) { // up
	    first = tent_t;		
		for (int i = 0; i < imp_ct; i++)
			if (imps[i].y < first.y)
				first = imps[i];

		line_2d xy = calc_dxdy_line (curr_t, tent_t);
		line_2d zy = calc_dzdy_line (curr_t, tent_t);

		return point_3d (fof(xy, first.y), first.y, fof(zy, first.y));
	}
	else { // down
		first = tent_b;
		for (int i = 0; i < imp_ct; i++)
			if (imps[i].y > first.y)
				first = imps[i];	

		line_2d xy = calc_dxdy_line (curr_b, tent_b);
		line_2d zy = calc_dzdy_line (curr_b, tent_b);
		
		return offset_3d (point_3d(fof(xy, first.y), first.y, fof(zy, first.y)), 0.0f, height, 0.0f);
	}
}


point_3d collis::deflect_plane (point_3d p)
{
	double ax, ay;
	point_3d r;
	
	perp_to_plane (plane, side, ax, ay);
	ray_3d ray(p, ax, ay);
	line_3d_int_plane (ray.cv_line_3d(), plane, r);

	return r;
}


#endif // !COLLIS_INC