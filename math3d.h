#ifndef INC_MATH3D
#define INC_MATH3D

#include <d3dx9.h>
#include <stdio.h>
#include <\programs\math2d.h>

#define point_3d D3DXVECTOR3
#define PLANE_Y_PLANE _neg_inf
#define PLANE_XY_PLANE _pos_inf
#define PLANE_LESSER -1
#define PLANE_EQUAL 0
#define PLANE_GREATER 1
#define ORIG point_3d(0.0f, 0.0f, 0.0f)

int _test_point_n = 0;
bool _test_point_create = true;
int _test_ct = 0;

extern void create_test_point (point_3d p, int n);
extern void create_test_point (point_3d p, int n1, int n2);
extern void create_nu_test_point (point_3d p);
extern void print_point (char *test, point_3d p, int y);

//extern int _test_point_n;

struct point_2dd {
	double x, y;
};

struct point_3dd {
	double x, y, z;
};

void sort_by_angle (point_3d* p, int ct);
point_3d get_abs_components (float dist, double axz, double ayz);
point_3d rotate (point_3d p, point_3d c, double axz, double ayz, double axy);
point_3dd rotate_d (point_3dd p, point_3dd c, double axz, double ayz, double axy);
void rotate (float& x, float& y, float cx, float cy, double a);
void rotate_d (double& x, double& y, double cx, double cy, double a);
point_3d rotate_b (point_3d p, point_3d c, double axz, double ayz, double axy);
D3DMATRIX scale_mat (float scale);
point_3d offset_3d (point_3d start, double dist, double axz, double ayz, double axy);
bool visible_side (point_3d ref, point_3d p1, point_3d p2, point_3d p3);

class plane_3d {
public:
	double mx, my, b;

	inline plane_3d () {}
	inline plane_3d (point_3d p1, point_3d p2, point_3d p3)
	{
		
	}
};

class line_3d {
public:
	double mx, my, bx, by;
	
	inline line_3d () {}
	inline line_3d (point_3d p1, point_3d p2) 
	{
		if (approx_equal (p1.z, p2.z))
			if (approx_equal (p1.x, p2.x)) {
				mx = _pos_inf;
				my = _pos_inf;
				bx = p1.x;
				by = p1.z;
			}
			else {
				line_2d xy(p1.x, p1.y, p2.x, p2.y);
				mx = _pos_inf;
				my = xy.m;
				bx = p1.z;
				by = xy.b;
			}
		else {
			line_2d x(p1.z, p1.x, p2.z, p2.x);
			line_2d y(p1.z, p1.y, p2.z, p2.y);
			mx = x.m;
			bx = x.b;
			my = y.m;
			by = y.b;
		}
	}
};


class ray_3d {
public:
	point_3d pos;
	double axz, ayz;

	ray_3d () {}
	ray_3d (double x, double y, double z, double a_xz, double a_yz)
	{
		pos.x = (float)x;
		pos.y = (float)y;
		pos.z = (float)z;
		axz = a_xz;
		ayz = a_yz;
	}

	ray_3d (point_3d p, double a_xz, double a_yz)
	{
		pos = p;
		axz = a_xz;
		ayz = a_yz;
	}

	point_3d cv_vector() {
		point_3d begin(0.0f, 0.0f, 1.0f);
		point_3d center(0.0f, 0.0f, 0.0f);
		point_3d delta = rotate (begin, center, -axz, ayz, _axy);

		point_3d result;
		result.x = pos.x + delta.x;
		result.y = pos.y + delta.y;
		result.z = pos.z + delta.z;

		return result;
	}

	point_3d up_vector() {
		point_3d begin(0.0f, 1.0f, 0.0f);
		point_3d center(0.0f, 0.0f, 0.0f);
		
		return rotate_b (begin, center, -axz, ayz, _axy);
	}

	point_3d right_vector() {
		point_3d begin(0.0f, 1.0f, 0.0f);
		point_3d center(0.0f, 0.0f, 0.0f);
		
		return rotate_b (begin, center, -(PI * .5) - axz, ayz, _axy);
	}

	inline line_3d cv_line_3d ()
	{
		point_3d p = offset_3d(pos, 1.0, -axz, ayz, 0.0);
		line_3d result(pos, p);
	
		//result.mx = tan(axz);	//dx / dz
		//result.my = p.y / p.z;
		//result.bx = pos.x - pos.z * result.mx;
		//result.by = pos.y - pos.z * result.my;	

		return result;
	}	
};


bool is_inval_vert (point_3d v)
{
	if (v.x == _fpos_inf)
		return true;
	if (v.x == _fneg_inf)
		return true;

	if (v.y == _fpos_inf)
		return true;
	if (v.y == _fneg_inf)
		return true;

	if (v.z == _fpos_inf)
		return true;
	if (v.z == _fneg_inf)
		return true;

	return false;
}


bool approx_equal (point_3d p1, point_3d p2)
{
	if (approx_equal (p1.x, p2.x))
		if (approx_equal (p1.y, p2.y))
			if (approx_equal (p1.z, p2.z))
				return true;

	return false;
}


point_3d calc_center (point_3d* p, int ct)
{
	point_3d result;
	point_3dd acc;

	acc.x = acc.y = acc.z = 0.0;

	for (int i = 0; i < ct; i++) {
		acc.x += double(p[i].x);	
		acc.y += double(p[i].y);
		acc.z += double(p[i].z);
	}

	result.x = float(acc.x / ct);
	result.y = float(acc.y / ct);
	result.z = float(acc.z / ct);

	return result;
}


point_3d get_abs_components (float dist, double axz, double ayz, double axy)
{
	point_3d result;

	point_3d begin(0.0f, 0.0f, dist);
	point_3d center(0.0f, 0.0f, 0.0f);

	return rotate (begin, center, axz, ayz, axy);
}


point_3dd get_abs_components (double dist, double axz, double ayz, double axy)
{
	point_3d result;
	point_3dd begin, center;

	begin.x = 0.0;
	begin.y = 0.0;
	begin.z = dist;	
	center.x = center.y = center.z = 0;

	return rotate_d (begin, center, axz, ayz, axy);
}


point_3d rotate (point_3d p, point_3d c, double axz, double ayz, double axy)
{
	rotate (p.x, p.y, c.x, c.y, axy);
	rotate (p.z, p.y, c.z, c.y, ayz);
	rotate (p.x, p.z, c.x, c.z, axz);
	
	return p;
}


point_3dd rotate_d (point_3dd p, point_3dd c, double axz, double ayz, double axy)
{
	rotate_d (p.x, p.y, c.x, c.y, axy);
	rotate_d (p.z, p.y, c.z, c.y, ayz);
	rotate_d (p.x, p.z, c.x, c.z, axz);
	
	return p;
}


point_3d rotate_b (point_3d p, point_3d c, double axz, double ayz, double axy)
{
	//rotate (p.x, p.y, c.x, c.y, axy);
	rotate (p.z, p.y, c.z, c.y, ayz);
	rotate (p.x, p.z, c.x, c.z, axz);

	return p;
}


point_3d rotate_c (point_3d p, point_3d c, double axz, double ayz, double axy)
{
	//rotate (p.x, p.y, c.x, c.y, axy);
	rotate (p.x, p.z, c.x, c.z, axz);
	rotate (p.z, p.y, c.z, c.y, ayz);
	
	return p;
}


void rotate (float& x, float& y, float cx, float cy, double a)
{
	float dx = x - cx;
	float dy = y - cy;

	float ndx = dx * cosd(a) - dy * sind(a);
	float ndy = dy * cosd(a) + dx * sind(a);
	
	x = cx + ndx;
	y = cy + ndy;
}


void rotate_d (double& x, double& y, double cx, double cy, double a)
{
	double dx = x - cx;
	double dy = y - cy;

	double ndx = dx * cos(a) - dy * sin(a);
	double ndy = dy * cos(a) + dx * sin(a);
	
	x = cx + ndx;
	y = cy + ndy;
}


point_3d offset_3d (point_3d start, double dist, double axz, double ayz, double axy) 
{
	point_3d result = get_abs_components ((float)dist, axz, ayz, axy);
	
	result.x += start.x;
	result.y += start.y;
	result.z += start.z;

	return result;
}


point_3d offset_3d (point_3d start, float dx, float dy, float dz)
{
	start.x += dx;
	start.y += dy;
	start.z += dz;

	return start;
}


point_3d offset_3d (point_3d start, double dist, double a)
{
	point_3d result;

	//msg(a);
	//char buff[100];
	//sprintf(buff, "%f", a);
	//MessageBox(_main_win.handle, buff, "", MB_OK);

	result.x = start.x + float(dist * cos(a));
	result.y = start.y;
	result.z = start.z + float(dist * sin(a));

	return result;
}


D3DMATRIX scale_mat (float scale)
{
	D3DMATRIX m;

	m._11 = scale; 
    m._12 = 0.0f;
    m._13 = 0.0f;
	m._14 = 0.0f;

	m._21 = 0.0f;
    m._22 = scale;
    m._23 = 0.0f;
	m._24 = 0.0f;

	m._31 = 0.0f;
    m._32 = 0.0f;
    m._33 = scale;
	m._34 = 0.0f;

	m._41 = 0.0f;
	m._42 = 0.0f;
	m._43 = 0.0f;
	m._44 = 1.0;

	return m;
}


point_3d cv_normal (double axz, double ayz)
{
	return get_abs_components(1.0f, axz, ayz, 0.0);
	//ray_3d ray(0.0,0.0,0.0, axz, ayz);
	//return ray.cv_vector();
}


double square (double n) 
{
	return n * n;
}


float square (float n)
{
	return n * n;
}


point_3d diff (point_3d p1, point_3d p2)
{
	point_3d result;

	result.x = p2.x - p1.x;
	result.y = p2.y - p1.y;
	result.z = p2.z - p1.z;

	return result;
}


point_3d diff_rev (point_3d p1, point_3d p2)
{
	point_3d result;

	result.x = p1.x - p2.x;
	result.y = p1.y - p2.y;
	result.z = p1.z - p2.z;

	return result;
}


point_3d sum (point_3d p1, point_3d p2)
{
	point_3d result;

	result.x = p1.x + p2.x;
	result.y = p1.y + p2.y;
	result.z = p1.z + p2.z;

	return result;
}


double dist_3d (point_3d p1, point_3d p2)
{
	point_3d d = diff (p1, p2);
	return sqrt (square((double)d.x) + square((double)d.y) + square((double)d.z));
}


double dist_xz (point_3d p1, point_3d p2)
{
	point_3d d = diff (p1, p2);
	return sqrt (square((double)d.x) + square((double)d.z));
}


double angle_xz (point_3d p1, point_3d p2)
{
	return 2 * PI + (atan2(double(p2.z - p1.z), double(p2.x - p1.x)));
}


bool line_3d_int_plane (line_3d& line, plane_3d& plane, point_3d& result)
{	
	if (line.mx == _pos_inf)
		if (line.my == _pos_inf)
			if (plane.mx == PLANE_Y_PLANE) {
				result.x = (float)line.bx;
				result.y = (float)plane.b;
				result.z = (float)line.by;
				return true;
			}
			else if (plane.mx == PLANE_XY_PLANE) 
				if (approx_zero (plane.my))
					return false;
				else {
					result.y = float((line.bx - plane.b) / plane.my);
					result.x = (float)line.bx;
					result.z = (float)line.by;
					return true;
				}
			else 
				if (approx_zero (plane.my)) 
					return false;
				else {
					result.x = (float)line.bx;
					result.z = (float)line.by;
					result.y = float((result.z - plane.b - plane.mx * result.x) / plane.my);
					return true;
				}
		else
			if (plane.mx == PLANE_Y_PLANE)
				if (approx_zero (line.my))
					return false;
				else {
					result.x = float((plane.b - line.by) / line.my);
					result.y = float(plane.b);
					result.z = float(line.bx);
					return true;
				}
			else if (plane.mx == PLANE_XY_PLANE)
				if (approx_equal (1.0, line.my * plane.my))
					return false;
				else {
					result.y = float((line.my * plane.b + line.by) / (1.0 - line.my * plane.my));
					result.x = float(plane.my * result.y + plane.b);
					result.z = (float)line.bx;
					return true;
				}
			else 
				if (approx_zero (plane.mx + plane.my * line.my)) 
					return false;
				else {
					result.x = float((line.bx - plane.my * line.by - plane.b) / (plane.mx + plane.my * line.my));
					result.y = float(line.my * result.x + line.by);
					result.z = (float)line.bx;
					return true;
				}
	else
		if (plane.mx == PLANE_Y_PLANE) 
			if (approx_zero (line.my))
				return false;
			else {
				result.z = float((plane.b - line.by) / line.my);
				result.y = (float)plane.b;
				result.x = float(line.mx * result.z + line.bx);
				return true;
			}		
		else if (plane.mx == PLANE_XY_PLANE) 
			if (approx_zero (line.mx - plane.my * line.my))
				return false;
			else {
				result.z = float((plane.my * line.by + plane.b - line.bx) / (line.mx - plane.my * line.my));
				result.x = float(line.mx * result.z + line.bx);
				result.y = float(line.my * result.z + line.by);
				return true;
			}
		else
			if (approx_zero (1.0 - plane.mx * line.mx - plane.my * line.my))
				return false;
			else {
				result.z = float((plane.mx * line.bx + plane.my * line.by + plane.b) / 
					(1.0 - plane.mx * line.mx - plane.my * line.my));
				result.x = float(line.mx * result.z + line.bx);
				result.y = float(line.my * result.z + line.by);
				return true;
			}
}


line_2d calc_xz_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.x, p1.z, p2.x, p2.z);
	return result;
}


line_2d calc_dxdy_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.y, p1.x, p2.y, p2.x);
	return result;
}


line_2d calc_dydx_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.x, p1.y, p2.x, p2.y);
	return result;
}


line_2d calc_dzdy_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.y, p1.z, p2.y, p2.z);
	return result;
}


line_2d calc_dydz_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.z, p1.y, p2.z, p2.y);
	return result;
}


line_2d calc_dzdx_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.x, p1.z, p2.x, p2.z);
	return result;
}


line_2d calc_dxdz_line (point_3d p1, point_3d p2)
{
	line_2d result(p1.z, p1.x, p2.z, p2.x);
	return result;
}


line_3d calc_line (point_3d p1, point_3d p2) 
{
	line_3d result(p1, p2);
	return result;
}


line_3d calc_x_line (point_3d p1, point_3d p2) 
{
	line_3d result;

	line_2d y(p1.x, p1.y, p2.x, p2.y);
	line_2d z(p1.x, p1.z, p2.x, p2.z);

	result.mx = z.m;
	result.bx = z.b;
	result.my = y.m;
	result.by = y.b;

	return result;
}


line_3d calc_y_line (point_3d p1, point_3d p2) 
{
	line_3d result;

	line_2d x(p1.y, p1.x, p2.y, p2.x);
	line_2d z(p1.y, p1.z, p2.y, p2.z);

	result.mx = x.m;
	result.bx = x.b;
	result.my = z.m;
	result.by = z.b;

	return result;
}


double calc_dzdx_b (point_3d p1, point_3d p2)
{
	double m = (p2.z - p1.z) / (p2.x - p1.x);
	return p1.z - m * p1.x;
}


double calc_dzdx_b (point_3d p, double m)
{
	return p.z - m * p.x;
}


double calc_dzdx_m (point_3d p1, point_3d p2)
{
	return (p2.z - p1.z) / (p2.x - p1.x);
}


double calc_dxdz_b (point_3d p1, point_3d p2)
{
	double m = (p2.x - p1.x) / (p2.z - p1.z);
	return p1.x - m * p1.z;
}


double calc_dxdz_b (point_3d p, double m)
{
	return p.x - m * p.z;
}


double calc_dxdz_m (point_3d p1, point_3d p2)
{
	return (p2.x - p1.x) / (p2.z - p1.z);
}


plane_3d calc_plane_3d (point_3d p1, point_3d p2, point_3d p3)
{
	// z = mx * x + my * y + b
	// mx = dz/dx
	// my = dz/dy
	// b = z intersect

	// y-plane: mx = nan
	// xy-plane: mx = +inf
	// general: else
	
	plane_3d plane;
	point_3d p3d_1, p3d_2;
	point_2d p2d_1, p2d_2;
	line_2d l2d_1, l2d_2;
	line_3d l3d_1, l3d_2;
	
	static bool dp = true;

	if (approx_equal (p1.y, p2.y))
		if (approx_equal (p1.y, p3.y)) { // y1 = y2 = y3
			//dprint ("y-plane", 3);
			// y-plane
			plane.mx = PLANE_Y_PLANE;
			plane.b = (double)p1.y;
		}
		else // y1 = y2 != y3
			if (approx_equal (p1.x, p2.x)) {
				// xy-plane
				//dprint ("xy-plane", 3);
				plane.mx = PLANE_XY_PLANE;
				plane.my = double(p3.x - p1.x) / double(p3.y - p1.y);
				plane.b = p1.x - plane.my * p1.y;
			}
			else // general 
				if (approx_equal (p1.z, p2.z)) {
					plane.mx = 0.0;
					l2d_1 = calc_dzdy_line (p1, p3);
					plane.my = l2d_1.m;
					plane.b = l2d_1.b;
				}
				else {				
					l2d_1 = calc_xz_line (p1, p2);
					l2d_2 = calc_dzdx_line (p1, p2);
					p3d_1.z = float(l2d_2.m * p3.x + l2d_2.b);

					plane.mx = l2d_1.m; 					
					plane.my = double(p3.z - p3d_1.z) / double(p3.y - p1.y);

					plane.b = p1.z - plane.mx * p1.x - plane.my * p1.y;
				}
	else if (approx_equal (p1.y, p3.y)) // y1 = y3 != y2
		if (approx_equal (p1.x, p3.x)) {
			// xy-plane
			//dprint ("xy-plane 2", 3);
			plane.mx = PLANE_XY_PLANE;
			plane.my = double(p2.x - p1.x) / double(p2.y - p1.y);
			plane.b = p1.x - plane.my * p1.y;
		}
		else // general
			if (approx_equal (p1.z, p3.z)) {
				//dprint ("general 2 a", 8);
				plane.mx = 0.0;
				l2d_1 = calc_dzdy_line (p1, p2);
				plane.my = l2d_1.m;
				plane.b = l2d_1.b;
			}
			else {
				l2d_1 = calc_xz_line (p1, p3);
				l2d_2 = calc_dzdx_line (p1, p3);
				p3d_1.z = float(l2d_2.m * p2.x + l2d_2.b);

				plane.mx = l2d_1.m; 					
				plane.my = double(p2.z - p3d_1.z) / double(p2.y - p1.y);

				plane.b = p1.z - plane.mx * p1.x - plane.my * p1.y;
			}
	else if (approx_equal (p2.y, p3.y)) // y1 != y2 = y3
		if (approx_equal (p2.x, p3.x)) {
			// xy-plane
			//dprint ("xy-plane 3", 3);
			plane.mx = PLANE_XY_PLANE;
			plane.my = double(p2.x - p1.x) / double(p2.y - p1.y);
			plane.b = p1.x - plane.my * p1.y;
		}
		else  // general
			if (approx_equal (p2.z, p3.z)) {
				plane.mx = 0.0;
				l2d_1 = calc_dzdy_line (p1, p2);
				plane.my = l2d_1.m;
				plane.b = l2d_1.b;
			}
			else {
				l2d_1 = calc_xz_line (p2, p3);
				l2d_2 = calc_dzdx_line (p2, p3);
				p3d_1.z = float(l2d_2.m * p1.x + l2d_2.b);

				plane.mx = l2d_1.m; 					
				plane.my = double(p1.z - p3d_1.z) / double(p1.y - p2.y);

				plane.b = p1.z - plane.mx * (double)p1.x - plane.my * (double)p1.y;
			}
	else 
		if (approx_equal (p1.x, p2.x)) {
			l3d_1 = calc_y_line (p2, p3);
			p3d_1.x = float(l3d_1.mx * p1.y + l3d_1.bx);
			p3d_1.z = float(l3d_1.my * p1.y + l3d_1.by);
		
			if (approx_equal (p3d_1.x, p1.x)) {
				plane.mx = PLANE_XY_PLANE;
				plane.my = l3d_1.mx;
				plane.b = l3d_1.bx;
			}
			else {
				l3d_2 = calc_x_line (p2, p3);
				p3d_2.z = float(l3d_2.mx * p1.x + l3d_2.bx);
				p3d_2.y = float(l3d_2.my * p1.x + l3d_2.by);						
			
				l2d_1 = calc_dzdx_line (p3d_1, p3);
				l2d_2 = calc_dzdy_line (p3d_2, p3);
			
				plane.mx = l2d_1.m;
				plane.my = l2d_2.m;
				plane.b = p1.z - plane.mx * (double)p1.x - plane.my * (double)p1.y;			
			}
		}
		else {
			l3d_1 = calc_y_line (p1, p2);
			p3d_1.x = float(l3d_1.mx * p3.y + l3d_1.bx);
			p3d_1.z = float(l3d_1.my * p3.y + l3d_1.by);
		
			if (approx_equal (p3d_1.x, p3.x)) {
				plane.mx = PLANE_XY_PLANE;
				plane.my = l3d_1.mx;
				plane.b = l3d_1.bx;
			}
			else {
				l3d_2 = calc_x_line (p1, p2);
				p3d_2.z = float(l3d_2.mx * p3.x + l3d_2.bx);
				p3d_2.y = float(l3d_2.my * p3.x + l3d_2.by);						
			
				l2d_1 = calc_dzdx_line (p3d_1, p3);
				l2d_2 = calc_dzdy_line (p3d_2, p3);
			
				plane.mx = l2d_1.m;
				plane.my = l2d_2.m;
				plane.b = p1.z - plane.mx * (double)p1.x - plane.my * (double)p1.y;			
			}
		}	

	return plane;
}


inline bool point_in_tri_xy_u (point_3d p, bool side, point_3d t, point_3d l, point_3d r)
{
	line_2d le = calc_dxdy_line (t, l);
	line_2d re = calc_dxdy_line (t, r);
	
	if (side) {
		if (l.x > r.x) 
			if (approx_lesser_inc (p.x, le.m * p.y + le.b))
				if (approx_greater_inc (p.x, re.m * p.y + re.b))
					if (approx_greater_inc (p.y, l.y))
						return true;
	}		
	else {
		if (l.x < r.x) 
			if (approx_greater_inc (p.x, le.m * p.y + le.b))
				if (approx_lesser_inc (p.x, re.m * p.y + re.b))
					if (approx_greater_inc (p.y, l.y))
						return true;
	}

	return false;
}


inline bool point_in_tri_xy_d (point_3d p, bool side, point_3d b, point_3d l, point_3d r)
{
	line_2d le = calc_dxdy_line (b, l);
	line_2d re = calc_dxdy_line (b, r);

	if (side) {
		if (l.x > r.x) 
			if (approx_lesser_inc (p.x, le.m * p.y + le.b))
				if (approx_greater_inc (p.x, re.m * p.y + re.b))
					if (approx_lesser_inc (p.y, l.y))						
						return true;
	}
	else {
		if (l.x < r.x) 
			if (approx_greater_inc (p.x, le.m * p.y + le.b))
				if (approx_lesser_inc (p.x, re.m * p.y + re.b))
					if (approx_lesser_inc (p.y, l.y))
						return true;
	}

	return false;
}


inline bool point_in_tri_xy_l (point_3d p, bool side, point_3d t, point_3d l, point_3d b)
{
	line_2d lt = calc_dxdy_line (l, t);
	line_2d lb = calc_dxdy_line (l, b);
	line_2d r = calc_dxdy_line (t, b);
	float rx = float(r.m * l.y + r.b);

	if (side) {
		if (l.x > rx)
			if (approx_lesser_inc (p.x, lt.m * p.y + lt.b))
				if (approx_lesser_inc (p.x, lb.m * p.y + lb.b))
					if (approx_greater_inc (p.x, r.m * p.y + r.b))
						return true;
	}
	else {	
		if (l.x < rx)
			if (approx_greater_inc (p.x, lt.m * p.y + lt.b))
				if (approx_greater_inc (p.x, lb.m * p.y + lb.b))
					if (approx_lesser_inc (p.x, r.m * p.y + r.b))
						return true;
	}

	return false;
}


inline bool point_in_tri_xy_r (point_3d p, bool side, point_3d t, point_3d r, point_3d b)
{
	line_2d rt = calc_dxdy_line (r, t);
	line_2d rb = calc_dxdy_line (r, b);
	line_2d l = calc_dxdy_line (t, b);
	float lx = float(l.m * r.y + l.b);

	if (side) {
		if (lx > r.x)
			if (approx_greater_inc (p.x, rt.m * p.y + rt.b))
				if (approx_greater_inc (p.x, rb.m * p.y + rb.b))
					if (approx_lesser_inc (p.x, l.m * p.y + l.b))
						return true;
	}
	else {
		if (lx < r.x)
			if (approx_lesser_inc (p.x, rt.m * p.y + rt.b))
				if (approx_lesser_inc (p.x, rb.m * p.y + rb.b))
					if (approx_greater_inc (p.x, l.m * p.y + l.b))
						return true;
	}

	return false;
}


inline bool point_in_tri_yz_u (point_3d p, bool side, point_3d t, point_3d l, point_3d r)
{
	line_2d le = calc_dzdy_line (t, l);
	line_2d re = calc_dzdy_line (t, r);
	
	if (!side) {
		if (l.z > r.z) 
			if (approx_lesser_inc (p.z, le.m * p.y + le.b))
				if (approx_greater_inc (p.z, re.m * p.y + re.b))
					if (approx_greater_inc (p.y, l.y))
						return true;
	}		
	else {
		if (l.z < r.z) 
			if (approx_greater_inc (p.z, le.m * p.y + le.b))
				if (approx_lesser_inc (p.z, re.m * p.y + re.b))
					if (approx_greater_inc (p.y, l.y))
						return true;
	}

	return false;
}


inline bool point_in_tri_yz_d (point_3d p, bool side, point_3d b, point_3d l, point_3d r)
{
	line_2d le = calc_dzdy_line (b, l);
	line_2d re = calc_dzdy_line (b, r);

	if (!side) {
		if (l.z > r.z) 
			if (approx_lesser_inc (p.z, le.m * p.y + le.b))
				if (approx_greater_inc (p.z, re.m * p.y + re.b))
					if (approx_lesser_inc (p.y, l.y))						
						return true;
	}
	else {
		if (l.z < r.z) 
			if (approx_greater_inc (p.z, le.m * p.y + le.b))
				if (approx_lesser_inc (p.z, re.m * p.y + re.b))
					if (approx_lesser_inc (p.y, l.y))
						return true;
	}

	return false;
}


inline bool point_in_tri_yz_l (point_3d p, bool side, point_3d t, point_3d l, point_3d b)
{
	line_2d lt = calc_dzdy_line (l, t);
	line_2d lb = calc_dzdy_line (l, b);
	line_2d r = calc_dzdy_line (t, b);
	float rz = float(r.m * l.y + r.b);

	if (!side) {
		if (l.z > rz)
			if (approx_lesser_inc (p.z, lt.m * p.y + lt.b))
				if (approx_lesser_inc (p.z, lb.m * p.y + lb.b))
					if (approx_greater_inc (p.z, r.m * p.y + r.b))
						return true;
	}
	else {	
		if (l.z < rz)
			if (approx_greater_inc (p.z, lt.m * p.y + lt.b))
				if (approx_greater_inc (p.z, lb.m * p.y + lb.b))
					if (approx_lesser_inc (p.z, r.m * p.y + r.b))
						return true;
	}

	return false;
}


inline bool point_in_tri_yz_r (point_3d p, bool side, point_3d t, point_3d r, point_3d b)
{
	line_2d rt = calc_dzdy_line (r, t);
	line_2d rb = calc_dzdy_line (r, b);
	line_2d l = calc_dzdy_line (t, b);
	float lz = float(l.m * r.y + l.b);

	if (!side) {
		if (lz > r.z)
			if (approx_greater_inc (p.z, rt.m * p.y + rt.b))
				if (approx_greater_inc (p.z, rb.m * p.y + rb.b))
					if (approx_lesser_inc (p.z, l.m * p.y + l.b))
						return true;
	}
	else {
		if (lz < r.z)
			if (approx_lesser_inc (p.z, rt.m * p.y + rt.b))
				if (approx_lesser_inc (p.z, rb.m * p.y + rb.b))
					if (approx_greater_inc (p.z, l.m * p.y + l.b))
						return true;
	}

	return false;
}


inline bool point_in_tri_xz_u (point_3d p, bool side, point_3d t, point_3d l, point_3d r)
{
	line_2d le = calc_dxdz_line (t, l);
	line_2d re = calc_dxdz_line (t, r);
	
	if (!side) {
		if (l.x > r.x) 
			if (approx_lesser_inc (p.x, le.m * p.z + le.b))
				if (approx_greater_inc (p.x, re.m * p.z + re.b))
					if (approx_greater_inc (p.z, l.z))
						return true;
	}		
	else {
		if (l.x < r.x) 
			if (approx_greater_inc (p.x, le.m * p.z + le.b))
				if (approx_lesser_inc (p.x, re.m * p.z + re.b))
					if (approx_greater_inc (p.z, l.z))
						return true;
	}

	return false;
}


inline bool point_in_tri_xz_d (point_3d p, bool side, point_3d b, point_3d l, point_3d r)
{
	line_2d le = calc_dxdz_line (b, l);
	line_2d re = calc_dxdz_line (b, r);

	if (!side) {
		if (l.x > r.x) 
			if (approx_lesser_inc (p.x, le.m * p.z + le.b))
				if (approx_greater_inc (p.x, re.m * p.z + re.b))
					if (approx_lesser_inc (p.z, l.z))						
						return true;
	}
	else {
		if (l.x < r.x) 
			if (approx_greater_inc (p.x, le.m * p.z + le.b))
				if (approx_lesser_inc (p.x, re.m * p.z + re.b))
					if (approx_lesser_inc (p.z, l.z))
						return true;
	}

	return false;
}


inline bool point_in_tri_xz_l (point_3d p, bool side, point_3d t, point_3d l, point_3d b)
{
	line_2d lt = calc_dxdz_line (l, t);
	line_2d lb = calc_dxdz_line (l, b);
	line_2d r = calc_dxdz_line (t, b);
	float rx = float(r.m * l.z + r.b);

	if (!side) {
		if (l.x > rx) 
			if (approx_lesser_inc (p.x, lt.m * p.z + lt.b))
				if (approx_lesser_inc (p.x, lb.m * p.z + lb.b))
					if (approx_greater_inc (p.x, r.m * p.z + r.b))
						return true;
	}
	else {
		if (l.x < rx) 
			if (approx_greater_inc (p.x, lt.m * p.z + lt.b))
				if (approx_greater_inc (p.x, lb.m * p.z + lb.b))
					if (approx_lesser_inc (p.x, r.m * p.z + r.b))
						return true;
	}

	return false;
}


inline bool point_in_tri_xz_r (point_3d p, bool side, point_3d t, point_3d r, point_3d b)
{
	line_2d rt = calc_dxdz_line (r, t);
	line_2d rb = calc_dxdz_line (r, b);
	line_2d l = calc_dxdz_line (t, b);
	float lx = float(l.m * r.z + l.b);

	if (!side) {
		if (lx > r.x) 
			if (approx_greater_inc (p.x, rt.m * p.z + rt.b))
				if (approx_greater_inc (p.x, rb.m * p.z + rb.b))
					if (approx_lesser_inc (p.x, l.m * p.z + l.b))
						return true;
	}
	else {
		if (lx < r.x) 
			if (approx_lesser_inc (p.x, rt.m * p.z + rt.b))
				if (approx_lesser_inc (p.x, rb.m * p.z + rb.b))
					if (approx_greater_inc (p.x, l.m * p.z + l.b))
						return true;
	}

	return false;
}


bool point_in_tri_xy (point_3d p, bool side, point_3d v1, point_3d v2, point_3d v3)
{
	if (approx_equal (v1.y, v2.y))
		if (approx_equal (v3.y, v1.y)) // N/A
			return false; 
		else if (v3.y < v1.y) // 3, 1-2
			return point_in_tri_xy_d (p, side, v3, v1, v2);
		else // 1-2, 3
			return point_in_tri_xy_u (p, side, v3, v2, v1);		
	else if (v1.y < v2.y)
		if (approx_equal (v3.y, v1.y)) // 1-3, 2
			return point_in_tri_xy_u (p, side, v2, v1, v3);
		else if (approx_equal (v3.y, v2.y))	// 1, 2-3
			return point_in_tri_xy_d (p, side, v1, v2, v3); 
		else if (v3.y < v1.y) // 3, 1, 2
			return point_in_tri_xy_l (p, side, v2, v1, v3);
		else if (v3.y > v2.y) // 1, 2, 3
			return point_in_tri_xy_l (p, side, v3, v2, v1);
		else // 1, 3, 2
			return point_in_tri_xy_r (p, side, v2, v3, v1);
	else		
		if (approx_equal (v3.y, v2.y)) // 2-3, 1
			return point_in_tri_xy_u (p, side, v1, v3, v2);
		else if (approx_equal (v3.y, v1.y)) // 2, 1-3
			return point_in_tri_xy_d (p, side, v2, v3, v1);
		else if (v3.y < v2.y) // 3, 2, 1
			return point_in_tri_xy_r (p, side, v1, v2, v3);
		else if (v3.y > v1.y) // 2, 1, 3
			return point_in_tri_xy_r (p, side, v3, v1, v2);
		else // 2, 3, 1
			return point_in_tri_xy_l (p, side, v1, v3, v2);			
}

		
bool point_in_tri_yz (point_3d p, bool side, point_3d v1, point_3d v2, point_3d v3)
{
	if (approx_equal (v1.y, v2.y))
		if (approx_equal (v3.y, v1.y)) // N/A
			return false; 
		else if (v3.y < v1.y) // 3, 1-2
			return point_in_tri_yz_d (p, side, v3, v1, v2);
		else // 1-2, 3
			return point_in_tri_yz_u (p, side, v3, v2, v1);		
	else if (v1.y < v2.y)
		if (approx_equal (v3.y, v1.y)) // 1-3, 2
			return point_in_tri_yz_u (p, side, v2, v1, v3);
		else if (approx_equal (v3.y, v2.y))	// 1, 2-3
			return point_in_tri_yz_d (p, side, v1, v2, v3); 
		else if (v3.y < v1.y) // 3, 1, 2
			return point_in_tri_yz_l (p, side, v2, v1, v3);
		else if (v3.y > v2.y) // 1, 2, 3
			return point_in_tri_yz_l (p, side, v3, v2, v1);
		else // 1, 3, 2
			return point_in_tri_yz_r (p, side, v2, v3, v1);
	else		
		if (approx_equal (v3.y, v2.y)) // 2-3, 1
			return point_in_tri_yz_u (p, side, v1, v3, v2);
		else if (approx_equal (v3.y, v1.y)) // 2, 1-3
			return point_in_tri_yz_d (p, side, v2, v3, v1);
		else if (v3.y < v2.y) // 3, 2, 1
			return point_in_tri_yz_r (p, side, v1, v2, v3);
		else if (v3.y > v1.y)
			return point_in_tri_yz_r (p, side, v3, v1, v2);
		else // 2, 3, 1
			return point_in_tri_yz_l (p, side, v1, v3, v2);			
}


bool point_in_tri_xz (point_3d p, bool side, point_3d v1, point_3d v2, point_3d v3)
{
	if (approx_equal (v1.z, v2.z))
		if (approx_equal (v3.z, v1.z)) // N/A
			return false; 
		else if (v3.z < v1.z) // 3, 1-2
			return point_in_tri_xz_d (p, side, v3, v1, v2);
		else // 1-2, 3
			return point_in_tri_xz_u (p, side, v3, v2, v1);		
	else if (v1.z < v2.z)
		if (approx_equal (v3.z, v1.z)) // 1-3, 2
			return point_in_tri_xz_u (p, side, v2, v1, v3);
		else if (approx_equal (v3.z, v2.z))	// 1, 2-3
			return point_in_tri_xz_d (p, side, v1, v2, v3); 
		else if (v3.z < v1.z) // 3, 1, 2
			return point_in_tri_xz_l (p, side, v2, v1, v3);
		else if (v3.z > v2.z) // 1, 2, 3
			return point_in_tri_xz_l (p, side, v3, v2, v1);
		else // 1, 3, 2
			return point_in_tri_xz_r (p, side, v2, v3, v1);
	else		
		if (approx_equal (v3.z, v2.z)) // 2-3, 1
			return point_in_tri_xz_u (p, side, v1, v3, v2);
		else if (approx_equal (v3.z, v1.z)) // 2, 1-3
			return point_in_tri_xz_d (p, side, v2, v3, v1);
		else if (v3.z < v2.z) // 3, 2, 1
			return point_in_tri_xz_r (p, side, v1, v2, v3);
		else if (v3.z > v1.z) // 2, 1, 3
			return point_in_tri_xz_r (p, side, v3, v1, v2);
		else // 2, 3, 1
			return point_in_tri_xz_l (p, side, v1, v3, v2);			
}


bool point_in_tri (point_3d point, bool side, plane_3d plane, point_3d v1, point_3d v2, point_3d v3)
{
	if (plane.mx == PLANE_Y_PLANE)
		return point_in_tri_xz (point, side, v1, v2, v3);
	else if (plane.mx == PLANE_XY_PLANE)
		return point_in_tri_yz (point, side, v1, v2, v3);
	else
		return point_in_tri_xy (point, side, v1, v2, v3);
}


inline int point_rel_plane (point_3d point, plane_3d plane)
{
	if (plane.mx == PLANE_Y_PLANE)
		if (approx_equal (point.y, plane.b))
			return PLANE_EQUAL;
		else if (point.y > plane.b)
			return PLANE_GREATER;
		else
			return PLANE_LESSER;
	else if (plane.mx == PLANE_XY_PLANE) {
		double x = plane.my * point.y + plane.b;
		if (approx_equal (point.x, x))
			return PLANE_EQUAL;
		else if (point.x > x)
			return PLANE_GREATER;
		else
			return PLANE_LESSER;
	}
	else {
		double z = plane.mx * point.x + plane.my * point.y + plane.b;
		if (approx_equal (point.z, z))
			return PLANE_EQUAL;
		else if (point.z > z)
			return PLANE_GREATER;
		else
			return PLANE_LESSER;
	}
}


bool visible_side (point_3d p, point_3d p1, point_3d p2, point_3d p3)
{
	plane_3d plane = calc_plane_3d (p1, p2, p3);
	bool side = (point_rel_plane (p, plane) != 1);
	return point_in_tri (p1, side, plane, p1, p2, p3);
}


bool visible_side (point_3d* p)
{
	plane_3d plane = calc_plane_3d (p[0], p[1], p[2]);
	return point_in_tri (p[0], true, plane, p[0], p[1], p[2]);
}


int point_rel_xz (point_3d p, point_3d p1, point_3d p2)
{
	if (approx_equal (p1.x, p2.x))
		if (approx_equal (p.x, p1.x))
			return 0;
		else if (p.x < p1.x)
			return -1;
		else 
			return 1;
	else {
		line_2d l = calc_dzdx_line (p1, p2);
		float z = float(l.m * p.x + l.b);

		if (approx_equal (p.z, z))
			return 0;
		else if (p.z < z)
			return -1;
		else 
			return 1;
	}
}


point_3d extrapolate_quad (point_3d p1, point_3d p2, point_3d p3)
{
	double d_12 = dist_3d (p1, p2);
	double d_23 = dist_3d (p2, p3);
	double d_31 = dist_3d (p3, p1);
	point_3d corner, end1, end2;

	if (d_12 > d_23)
		if (d_31 > d_12) { // 31
			end1 = p1;
			end2 = p3;
			corner = p2;
		}
		else { // 12
			end1 = p1;
			end2 = p2;
			corner = p3;
		}
	else
		if (d_31 > d_23) { // 31
			end1 = p1;
			end2 = p3;
			corner = p2;
		}
		else { // 23
			end1 = p2;
			end2 = p3;
			corner = p1;
		}

	point_3d delta = diff (corner, end1);	
	return sum (end2, delta);
}


void extrapolate_quad (point_3d p1, point_3d p2, point_3d& result1, point_3d& result2)
{
	point_3d delta = diff (p1, p2);

	if (approx_zero (delta.z)) {
		result1.x = p1.x;
		result1.y = p1.y + delta.y;
		result1.z = p1.z;
	}
	else {
		result2 = extrapolate_quad (p1, p2, result1);
		result1.x = p1.x;
		result1.y = p1.y;
		result1.z = p1.z + delta.z;
	}

	result2 = extrapolate_quad (p1, p2, result1);
}


point_3d calc_normal (plane_3d plane, bool side)
{
	point_3d result;

	if (plane.mx == PLANE_Y_PLANE)
		if (side) {
			result.x = 0.0f;
			result.y = -1.0f;
			result.z = 0.0f;
		}
		else {
			result.x = 0.0f;
			result.y = 1.0f;
			result.z = 0.0f;
		}
	else if (plane.mx == PLANE_XY_PLANE)
		if (side) {
			double a = atan(plane.my);
			result.x = (float)cos(a);
			result.y = (float)sin(a);
			result.z = 0.0f;
		}
		else {
			double a = PI + atan(plane.my);
			result.x = float(cos(a));
			result.y = float(sin(a));
			result.z = 0.0f;
		}
	else
		if (side) {
			double a = PI + atan(plane.mx);
			result.x = float(sin(a));
			result.y = 0.0f;
			result.z = float(cos(a));
		}
		else {
			double a = atan(plane.mx);
			result.x = float(sin(a));
			result.y = 0.0f;
			result.z = float(cos(a));
		}	

	return result;
}

	
inline bool closer (point_3d p, point_3d p1, point_3d p2)
{
	double d1 = dist_3d (p, p1);
	double d2 = dist_3d (p, p2);

	return d1 < d2;
}


void sort_by_dist (point_3d* p_in, point_3d* p_out, int n)
{
	int closest;
	bool exclude[512];
	int i, j;
	int curr;

	for (i = 0; i < n; i++)
		exclude[i] = false;
	
	curr = 0;
	
	for (i = 0; i < n; i++) {
		p_out[i] = p_in[curr];
		exclude[curr] = true;
		
		closest = -1;		
		for (j = 0; j < n; j++) {
			if (!exclude[j])
				if (closest == -1)
					closest = j;
				else
					if (closer (p_in[curr], p_in[j], p_in[closest]))
						closest = j;
		}	

		curr = closest;
	}		
}


inline float interpolate (float x1, float y1, float x2, float y2, float x)
{
	line_2d l(x1, y1, x2, y2);

	return float(l.m * x + l.b);
}


inline double interpolate (double x1, double y1, double x2, double y2, double x)
{
	line_2d l(x1, y1, x2, y2);

	return l.m * x + l.b;
}


inline bool line_int_line_xz (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	line_2d a = calc_dzdx_line (a1, a2);
	line_2d b = calc_dzdx_line (b1, b2);

	if (approx_equal_low (a.m, b.m))
		return false;

	if (a.m == _pos_inf)
		;
	else
		if (b.m == _pos_inf)
			;
		else {
			result.x = float((b.b - a.b) / (a.m - b.m));
			result.z = float(a.m * result.x + a.b);
			result.y = 0.0f;
			return true;
		}			
}


inline bool line_int_line (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	line_3d a(a1, a2);
	line_3d b(b1, b2);
	bool boo;

	if (approx_equal_low (a.mx, b.mx) && approx_equal_low (a.my, b.my))
		return false;
	if (approx_equal (a1, a2))
		return false;
	if (approx_equal (b1, b2))
		return false;

	if (a.mx == _pos_inf)
		if (a.my == _pos_inf)
			if (b.mx == _pos_inf)
				if (b.my == _pos_inf)
					boo = false;
				else {
					result.y = float(b.my * a.bx + b.by);
					result.z = float(a.by);
					result.x = float(a.bx);
					boo = true;
				}
			else {
				double z = a.by;
				result.z = float(z);
				result.x = float(a.bx);
				result.y = float(b.my * z + b.by);
				boo = true;
			}
		else
			if (b.mx == _pos_inf)
				if (b.my == _pos_inf) {
					result.y = float(a.my * b.bx + a.by);
					result.z = float(b.by);
					result.x = float(b.bx);
			//		dprint ("joe momma 2", 12);
					boo = true;
				}
				else 
					if (approx_equal_low (a.my, b.my))
						boo = false;
					else {
						double x = float((b.by - a.by) / (a.my - b.my));
						result.x = float(x);
						result.y = float(a.my * x + a.by);
						result.z = float(a.bx);

						boo = true;
					}
			else {
				double z = a.bx;
				result.x = float(b.mx * z + b.bx);
				result.y = float(b.my * z + b.by);
				result.z = float(z);
				boo = true;
			}
	else if (b.mx == _pos_inf)
		if (b.my == _pos_inf) {
			double z = b.by;
			result.z = float(z);
			result.x = float(b.bx);
			result.y = float(a.my * z + a.by);
			//dprint ("joe moma ", 15);
			boo = true;
		}
		else {
			double z = b.bx;
			result.z = float(z);
			result.x = float(a.mx * z + a.bx);
			result.y = float(a.my * z + a.by);
			boo = true;
		}
	else if (approx_equal_low (a.mx, b.mx)) 
		if (approx_equal_low (a.my, b.my))
			boo = false;
		else {
			double z = (b.by - a.by) / (a.my - b.my);
			result.z = float(z);
			result.x = float(a.mx * z + a.bx);
			result.y = float(a.my * z + a.by);
		
			boo = true;
		}
	else 
		if (approx_equal_low (a.mx, b.mx))
			boo = false;
		else {
			double z = (b.bx - a.bx) / (a.mx - b.mx);
			result.z = float(z);
			result.x = float(a.mx * z + a.bx);
			result.y = float(a.my * z + a.by);

			boo = true;
		}

	return boo;
}


inline bool line_int_line (point_3d p1, point_3d p2, line_3d b, point_3d& result)
{
	line_3d a(p1, p2);
	
	if (a.mx == _pos_inf)
		if (a.my == _pos_inf)
			if (b.mx == _pos_inf)
				if (b.my == _pos_inf)
					return false;
				else {
					result.y = float(b.my * a.bx + b.by);
					result.z = float(a.by);
					result.x = float(a.bx);
					return true;
				}
			else {
				double z = a.by;
				result.z = float(z);
				result.x = float(a.bx);
				result.y = float(b.my * z + b.by);
				return true;
			}
		else
			if (b.mx == _pos_inf)
				if (b.my == _pos_inf) {
					result.y = float(a.my * b.bx + a.by);
					result.z = float(b.by);
					result.x = float(b.bx);
					return true;
				}
				else {
					double x = float((b.by - a.by) / (a.my - b.my));
					result.x = float(x);
					result.y = float(a.my * x + a.by);
					result.z = float(a.bx);
					return true;
				}
			else {
				double z = a.bx;
				result.x = float(b.mx * z + b.bx);
				result.y = float(b.my * z + b.by);
				result.z = float(z);
				return true;
			}
	else if (b.mx == _pos_inf)
		if (b.my == _pos_inf) {
			double z = b.by;
			result.z = float(z);
			result.x = float(b.bx);
			result.y = float(a.my * z + a.by);
			return true;
		}
		else {
			double z = b.bx;
			result.z = float(z);
			result.x = float(a.mx * z + a.bx);
			result.y = float(a.my * z + a.by);
			return true;
		}
	else if (approx_equal_low (a.mx, b.mx)) {
		double z = (b.by - a.by) / (a.my - b.my);
		result.z = float(z);
		result.x = float(a.mx * z + a.bx);
		result.y = float(a.my * z + a.by);
		
		return true;
	}
	else {
		double z = (b.bx - a.bx) / (a.mx - b.mx);
		result.z = float(z);
		result.x = float(a.mx * z + a.bx);
		result.y = float(a.my * z + a.by);

		return true;
	}
}


inline bool point_in_line_seg_inc (point_3d p1, point_3d p2, point_3d p)
{
	if (!approx_equal (p1.x, p2.x))
		if (p1.x < p2.x) {
			if (approx_greater_inc (p.x, p1.x))
				if (approx_lesser_inc (p.x, p2.x))
					return true;
		}
		else {
			if (approx_greater_inc (p.x, p2.x))
				if (approx_lesser_inc (p.x, p1.x))
					return true;
		}
	else if (!approx_equal (p1.y, p2.y))
		if (p1.y < p2.y) {
			if (approx_greater_inc (p.y, p1.y))
				if (approx_lesser_inc (p.y, p2.y))
					return true;
		}
		else {
			if (approx_greater_inc (p.y, p2.y))
				if (approx_lesser_inc (p.y, p1.y))
					return true;
		}
	else
		if (p1.z < p2.z) {
			if (approx_greater_inc (p.z, p1.z))
				if (approx_lesser_inc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_inc (p.z, p2.z))
				if (approx_lesser_inc (p.z, p1.z))
					return true;
		}

	return false;
}


inline bool point_in_line_seg_exc (point_3d p1, point_3d p2, point_3d p)
{
	if (!approx_equal (p1.x, p2.x))
		if (p1.x < p2.x) {
			if (approx_greater_exc (p.x, p1.x))
				if (approx_lesser_exc (p.x, p2.x))
					return true;
		}
		else {
			if (approx_greater_exc (p.x, p2.x))
				if (approx_lesser_exc (p.x, p1.x))
					return true;
		}
	else if (!approx_equal (p1.y, p2.y))
		if (p1.y < p2.y) {
			if (approx_greater_exc (p.y, p1.y))
				if (approx_lesser_exc (p.y, p2.y))
					return true;
		}
		else {
			if (approx_greater_exc (p.y, p2.y))
				if (approx_lesser_exc (p.y, p1.y))
					return true;
		}
	else
		if (p1.z < p2.z) {
			if (approx_greater_exc (p.z, p1.z))
				if (approx_lesser_exc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_exc (p.z, p2.z))
				if (approx_lesser_exc (p.z, p1.z))
					return true;
		}

	return false;
}


inline bool point_in_line_seg_xz_inc (point_3d p1, point_3d p2, point_3d p)
{
	if (!approx_equal (p1.x, p2.x))
		if (p1.x < p2.x) {
			if (approx_greater_inc (p.x, p1.x))
				if (approx_lesser_inc (p.x, p2.x))
					return true;
		}
		else {
			if (approx_greater_inc (p.x, p2.x))
				if (approx_lesser_inc (p.x, p1.x))
					return true;
		}
	else if (!approx_equal (p1.z, p2.z))
		if (p1.z < p2.z) {
			if (approx_greater_inc (p.z, p1.z))
				if (approx_lesser_inc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_inc (p.z, p2.z))
				if (approx_lesser_inc (p.z, p1.z))
					return true;
		}
	else
		if (p1.z < p2.z) {
			if (approx_greater_inc (p.z, p1.z))
				if (approx_lesser_inc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_inc (p.z, p2.z))
				if (approx_lesser_inc (p.z, p1.z))
					return true;
		}

	return false;
}


inline bool point_in_line_seg_xz_exc (point_3d p1, point_3d p2, point_3d p)
{
	if (!approx_equal (p1.x, p2.x))
		if (p1.x < p2.x) {
			if (approx_greater_exc (p.x, p1.x))
				if (approx_lesser_exc (p.x, p2.x))
					return true;
		}
		else {
			if (approx_greater_exc (p.x, p2.x))
				if (approx_lesser_exc (p.x, p1.x))
					return true;
		}
	else if (!approx_equal (p1.z, p2.z))
		if (p1.z < p2.z) {
			if (approx_greater_exc (p.z, p1.z))
				if (approx_lesser_exc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_exc (p.z, p2.z))
				if (approx_lesser_exc (p.z, p1.z))
					return true;
		}
	else
		if (p1.z < p2.z) {
			if (approx_greater_exc (p.z, p1.z))
				if (approx_lesser_exc (p.z, p2.z))
					return true;
		}
		else {
			if (approx_greater_exc (p.z, p2.z))
				if (approx_lesser_exc (p.z, p1.z))
					return true;
		}

	return false;
}


inline bool line_seg_intersect (point_3d a1, point_3d a2, point_3d b1, point_3d b2)
{
	point_3d result;
	
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_exc (a1, a2, result))
			if (point_in_line_seg_exc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_inc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_inc (a1, a2, result))
			if (point_in_line_seg_inc (b1, b2, result)) {				
				if (result.y > -1.0) {
					/*
					create_test_point (a1, 0, RGB_RED);
					create_test_point (a2, 1, RGB_RED);
				
					if (approx_equal (b1, b2)) {
						create_test_point (b1, 2, RGB_BLUE);
						create_test_point (b2, 3, RGB_BLUE);
					}
					else {
						create_test_point (b1, 2, RGB_CYAN);
						create_test_point (b2, 3, RGB_CYAN);
					}

					create_test_point (result, 4, RGB_GREEN);
					print_point ("r ", result, 19);
					*/
				}				

				return true;
			}

	return false;
}


inline bool line_seg_intersect_exc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_exc (a1, a2, result))
			if (point_in_line_seg_exc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_xz (point_3d a1, point_3d a2, point_3d b1, point_3d b2)
{
	point_3d result;
	
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_exc (a1, a2, result))
			if (point_in_line_seg_exc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_xz_inc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_inc (a1, a2, result))
			if (point_in_line_seg_inc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_xz_exc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_xz_exc (a1, a2, result))
			if (point_in_line_seg_xz_exc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_inc_exc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_inc (a1, a2, result))
			if (point_in_line_seg_exc (b1, b2, result)) 
				return true;	

	return false;
}


inline bool line_seg_intersect_exc_inc (point_3d a1, point_3d a2, point_3d b1, point_3d b2, point_3d& result)
{
	if (line_int_line (a1, a2, b1, b2, result))
		if (point_in_line_seg_exc (a1, a2, result))
			if (point_in_line_seg_inc (b1, b2, result)) 
				return true;	

	return false;
}


inline double xy_angle (point_3d p1, point_3d p2)
{
	point_3d delta = diff (p1, p2);
	return simp_angle_pos (atan2 (delta.y, delta.x));
}


inline double xz_angle (point_3d p1, point_3d p2)
{
	point_3d delta = diff (p1, p2);
	return simp_angle_pos (atan2 (delta.z, delta.x));
}


inline double yz_angle (point_3d p1, point_3d p2)
{
	point_3d delta = diff (p1, p2);
	return simp_angle_pos (atan2 (delta.y, delta.z));
}


inline void swap (point_3d& p1, point_3d& p2)
{
	point_3d temp = p1;
	p1 = p2;
	p2 = temp;
}


void sort_by_angle (point_3d* p, int ct, bool face)
{
	plane_3d plane = calc_plane_3d (p[0], p[1], p[2]);
	point_3d center = calc_center (p, ct);
	
	if (!face) {
		if (plane.mx == PLANE_Y_PLANE)
			for (int i = 0; i < ct; i++) {
				double a = _neg_inf;
				for (int j = i; j < ct; j++)
					if (xz_angle (p[j], center) > a) {
						a = xz_angle (p[j], center);
						swap (p[i], p[j]);
					}					
			}
		else if (plane.mx == PLANE_XY_PLANE)
			for (int i = 0; i < ct; i++) {
				double a = _neg_inf;
				for (int j = i; j < ct; j++)
					if (yz_angle (p[j], center) > a) {
						a = yz_angle (p[j], center);
						swap (p[i], p[j]);
					}
			}
		else
			for (int i = 0; i < ct; i++) {
				double a = _neg_inf;
				for (int j = i; j < ct; j++)
					if (xy_angle (p[j], center) > a) {
						a = xy_angle (p[j], center);	
						swap (p[i], p[j]);						
					}					
			}
	}
	else {
		if (plane.mx == PLANE_Y_PLANE)
			for (int i = 0; i < ct; i++) {
				double a = _pos_inf;
				for (int j = i; j < ct; j++)
					if (xz_angle (p[j], center) < a) {
						a = xz_angle (p[j], center);
						swap (p[i], p[j]);
					}					
			}
		else if (plane.mx == PLANE_XY_PLANE)
			for (int i = 0; i < ct; i++) {
				double a = _pos_inf;
				for (int j = i; j < ct; j++)
					if (yz_angle (p[j], center) < a) {
						a = yz_angle (p[j], center);
						swap (p[i], p[j]);
					}
			}
		else
			for (int i = 0; i < ct; i++) {
				double a = _pos_inf;
				for (int j = i; j < ct; j++)
					if (xy_angle (p[j], center) < a) {
						a = xy_angle (p[j], center);	
						swap (p[i], p[j]);						
					}					
			}
	}		
}


int point_rel_cw_edge_xy (point_3d& p, point_3d& p1, point_3d& p2)
{
	// inside = -1
	// outside = 1
	// on edge = 0

	if (approx_equal (p1.x, p2.x))
		if (p1.y < p2.y) 
			return trin_cmp (p1.x, p.x);
		else 
			return trin_cmp (p.x, p1.x);			
	else
		if (p1.x < p2.x) {
			line_2d line = calc_dydx_line (p1, p2);
			float y = (float)fof (line, p.x);
			return trin_cmp (p.y, y);
		}
		else {
			line_2d line = calc_dydx_line (p1, p2);
			float y = (float)fof (line, p.x);
			return trin_cmp (y, p.y);
		}
}


int point_rel_cw_edge_yz (point_3d& p, point_3d& p1, point_3d& p2)
{
	// inside = -1
	// outside = 1
	// on edge = 0

	if (approx_equal (p1.z, p2.z))
		if (p1.y < p2.y) 
			return trin_cmp (p1.z, p.z);
		else 
			return trin_cmp (p.z, p1.z);			
	else 
		if (p1.z < p2.z) {
			line_2d line = calc_dydz_line (p1, p2);
			float y = (float)fof (line, p.z);
			return trin_cmp (p.y, y);
		}
		else {
			line_2d line = calc_dydz_line (p1, p2);
			float y = (float)fof (line, p.z);
			return trin_cmp (y, p.y);
		}
}


int point_rel_cw_edge_xz (point_3d& p, point_3d& p1, point_3d& p2)
{
	// inside = -1
	// outside = 1
	// on edge = 0

	if (approx_equal (p1.x, p2.x))
		if (p1.z < p2.z) 
			return trin_cmp (p1.x, p.x);
		else 
			return trin_cmp (p.x, p1.x);			
	else 
		if (p1.x < p2.x) {
			line_2d line = calc_dzdx_line (p1, p2);
			float z = (float)fof (line, p.x);
			return trin_cmp (p.z, z);
		}
		else {
			line_2d line = calc_dzdx_line (p1, p2);
			float z = (float)fof (line, p.x);
			return trin_cmp (z, p.z);
		}
}


int point_in_poly (point_3d& p, point_3d* vert, int ct, int& edge_n)
{
	// returns:
	//		0 - on edge
	//		+1 - outside
	//		-1 - inside
	
	int rel = -1;
	int r, i;
	plane_3d plane = calc_plane_3d (vert[0], vert[1], vert[2]);

	if (plane.mx == PLANE_Y_PLANE) {
		for (i = 0; i < ct - 1; i++) {
			r = point_rel_cw_edge_xz (p, vert[i], vert[i + 1]);
			if (r == 1) {
				rel = r;
				edge_n = i;
			}
			else
				if (r == 0)
					if (point_in_line_seg_inc (vert[i], vert[i + 1], p))
						rel = 0;
					else
						rel = 1;
		}

		r = point_rel_cw_edge_xz (p, vert[ct - 1], vert[0]);
		if (r == 1) {
			rel = r;
			edge_n = ct - 1;
		}
		else
			if (r == 0)
				if (point_in_line_seg_inc (vert[ct - 1], vert[0], p))
					rel = 0;
				else
					rel = 1;
	}
	else if (plane.mx == PLANE_XY_PLANE) {
		for (i = 0; i < ct - 1; i++) {
			r = point_rel_cw_edge_yz (p, vert[i], vert[i + 1]);
			if (r == 1) {
				rel = r;
				edge_n = i;
			}
			else
				if (r == 0)
					if (point_in_line_seg_inc (vert[i], vert[i + 1], p))
						rel = 0;
					else
						rel = 1;
		}

		r = point_rel_cw_edge_yz (p, vert[ct - 1], vert[0]);
		if (r == 1) {
			rel = r;
			edge_n = ct - 1;
		}
		else
			if (r == 0)
				if (point_in_line_seg_inc (vert[ct - 1], vert[0], p))
					rel = 0;
				else
					rel = 1;
	}
	else {
		for (i = 0; i < ct - 1; i++) {
			r = point_rel_cw_edge_xy (p, vert[i], vert[i + 1]);
			if (r == 1) {
				rel = r;
				edge_n = i;
			}
			else 
				if (r == 0)
					if (point_in_line_seg_inc (vert[i], vert[i + 1], p))
						rel = 0;
					else
						rel = 1;
		}

		r = point_rel_cw_edge_xy (p, vert[ct - 1], vert[0]);
		if (r == 1) {
			rel = r;
			edge_n = ct - 1;
		}
		else
			if (r == 0)
				if (point_in_line_seg_inc (vert[ct - 1], vert[0], p))
					rel = 0;
				else
					rel = 1;
	}
	
	return rel;
}


int point_in_poly_xz (point_3d& p, point_3d* vert, int ct, int& edge_n)
{
	int i, r, rel = -1;

	for (i = 0; i < ct - 1; i++) {
		r = point_rel_cw_edge_xz (p, vert[i], vert[i + 1]);
		if (r == 1) {
			rel = r;
			edge_n = i;
		}
		else
			if (r == 0)
				if (point_in_line_seg_inc (vert[i], vert[i + 1], p))
					rel = 0;
				else
					rel = 1;
	}

	r = point_rel_cw_edge_xz (p, vert[ct - 1], vert[0]);
	if (r == 1) {
		rel = r;
		edge_n = ct - 1;
	}
	else
		if (r == 0)
			if (point_in_line_seg_inc (vert[ct - 1], vert[0], p))
				rel = 0;
			else
				rel = 1;

	return rel;
}


bool planes_equal (plane_3d& p1, plane_3d& p2)
{	
	if (!approx_equal_low (p1.mx, p2.mx))
		return false;
	else if (!approx_equal_low (p1.my, p2.my))
		return false;
	else if (!approx_equal_low (p1.b, p2.b))
		return false;
	else
		return true;
}


void calc_angles (plane_3d& plane, bool side, double& ax, double& ay)
{
	if (plane.mx == PLANE_Y_PLANE) {
		;
	}
	else if (plane.mx == PLANE_XY_PLANE) {
		ax = -PI / 2.0;
		ay = atan(plane.my);
	}
	else {
		double x = atan(plane.mx);// + PI / 2.0;
		double y = atan(plane.my);// + PI / 2.0;

		if (side) {
			ax = x;
			ay = -atan(tan(y) * cos(x));
		}
		else {
			ax = PI + x;
			ay = atan(tan(y) * cos(x));
		}
	}

	dprint ("ay ", rad_to_deg(ay), 13);
}


inline plane_3d calc_plane_3d (point_3d* p)
{
	return calc_plane_3d (p[0], p[1], p[2]);
}


bool polys_touch (point_3d* p1, point_3d* p2, int ct1, int ct2, int side1, int side2)
{
	int i;
	int dum;
	bool touch = false;
	
	for (i = 0; i < ct1; i++)
		if (point_in_poly (p1[i], p2, ct2, dum) == side2)
			touch = true;

	for (i = 0; i < ct2; i++)
		if (point_in_poly (p2[i], p1, ct1, dum) == side1)
			touch = true;

	return touch;
}


bool seg_int_plane_inc (point_3d p1, point_3d p2, plane_3d plane, point_3d& result)
{
	int rel_1 = point_rel_plane (p1, plane);
	int rel_2 = point_rel_plane (p2, plane);

	if (rel_1 == 0)
		if (rel_2 == 0)
			return false;

	if (rel_1 != 1)
		if (rel_2 != -1) {
			line_3d line = calc_line (p1, p2);
			return line_3d_int_plane (line, plane, result);
		}

	if (rel_1 != -1)
		if (rel_2 != 1) {
			line_3d line = calc_line (p1, p2);
			return line_3d_int_plane (line, plane, result);
		}

	return false;
}


bool seg_int_plane_exc (point_3d p1, point_3d p2, plane_3d plane, point_3d& result)
{
	if (point_rel_plane (p1, plane) == 1)
		if (point_rel_plane (p2, plane) == -1) {
			line_3d line = calc_line (p1, p2);
			return line_3d_int_plane (line, plane, result);
		}

	if (point_rel_plane (p1, plane) == -1)
		if (point_rel_plane (p2, plane) == 1) {
			line_3d line = calc_line (p1, p2);
			return line_3d_int_plane (line, plane, result);
		}

	return false;
}


bool path_pen_plane (point_3d pa, point_3d pb, plane_3d plane, bool side)
{
	int rel_a = point_rel_plane (pa, plane);
	int rel_b = point_rel_plane (pb, plane);

	if (side) {
		if (rel_a >= 0)
			if (rel_b == -1)
				return true;
	}
	else {
		if (rel_a <= 0)
			if (rel_b == 1)
				return true;
	}

	return false;
}


bool seg_pen_plane_inc (point_3d p1, point_3d p2, plane_3d plane)
{
	int rel_1 = point_rel_plane (p1, plane);
	int rel_2 = point_rel_plane (p2, plane);

	return rel_1 != rel_2;
}


bool seg_pen_plane_exc (point_3d p1, point_3d p2, plane_3d plane)
{
	int rel_1 = point_rel_plane (p1, plane);
	int rel_2 = point_rel_plane (p2, plane);

	return rel_1 && rel_2 && (rel_1 != rel_2);
}


bool seg_pen_plane_inc (point_3d p1, point_3d p2, plane_3d plane, point_3d& result)
{
	if (seg_pen_plane_inc (p1, p2, plane)) {
		line_3d l(p1, p2);
		line_3d_int_plane (l, plane, result);		
		return true;
	}
	else
		return false;
}


bool seg_pen_plane_exc (point_3d p1, point_3d p2, plane_3d plane, point_3d& result)
{
	if (seg_pen_plane_exc (p1, p2, plane)) {
		line_3d l(p1, p2);
		line_3d_int_plane (l, plane, result);		
		return true;
	}
	else
		return false;
}


int edge_y_sign (point_3d p1, point_3d p2)
{
	return trin_cmp (p1.x, p2.x);
}


void set_closer (point_3d p, point_3d ref, point_3d& cl)
{
	if (closer (ref, p, cl))
		cl = p;
}


inline point_3d get_extr_y (point_3d* p, int ct, int sign)
{	
	point_3d result = p[0];
	
	for (int i = 1; i < ct; i++)
		if (trin_cmp (p[i].y, result.y) == sign)
			result = p[i];

	return result;
}


inline bool approx_equal (line_3d l1, line_3d l2)
{
	if (approx_equal (l1.mx, l2.mx))
		if (approx_equal (l1.my, l2.my))
			if (approx_equal (l1.bx, l2.bx))
				if (approx_equal (l1.by, l2.by))
					return true;

	return false;
}


inline bool colinear (point_3d p1, point_3d p2, point_3d p3)
{
	return approx_equal (calc_line (p1, p2), calc_line (p1, p3));
}


bool is_non_zero_area (point_3d* p, int ct)
{
	int i, n = -1;

	if (ct < 3)
		return false;

	for (i = 1; i < ct; i++)
		if (!approx_equal (p[0], p[i])) 
			n = i;

	if (n == -1)
		return false;

	for (i = 1; i < ct; i++)
		if (!approx_equal (p[0], p[i]))
			if (!approx_equal (p[n], p[i]))
				return true;

//	for (i = 1; i < ct; i++)		
//		if (approx_equal (p[0], p[i]))
//			if (!colinear (p[0], p[n], p[i]))
//				return true;

	return false;
}


bool is_poly_cw_in_xy (point_3d* vert, int ct)
{
	point_3d c = calc_center (vert, ct);
	return point_rel_cw_edge_xy (c, vert[0], vert[1]) == -1;
}


bool is_poly_cw_in_xz (point_3d* vert, int ct)
{
	point_3d c = calc_center (vert, ct);
	return point_rel_cw_edge_xz (c, vert[0], vert[1]) == -1;
}


bool is_poly_cw_in_yz (point_3d* vert, int ct)
{
	point_3d c = calc_center (vert, ct);
	return point_rel_cw_edge_yz (c, vert[0], vert[1]) == -1;
}


point_3d point_on_plane_xz (float x, float z, plane_3d plane)
{
	point_3d res;

	res.x = x;
	res.y = float((z - plane.mx * x - plane.b) / plane.my);
	res.z = z;

	return res;
}


point_3d point_on_plane_xz (point_3d p, plane_3d plane)
{
	return point_on_plane_xz (p.x, p.z, plane);
}


bool paralell (point_3d p1a, point_3d p1b, point_3d p2a, point_3d p2b)
{
	line_3d l1(p1a, p1b);
	line_3d l2(p2a, p2b);

	return approx_equal_low (l1.mx, l2.mx) && approx_equal_low (l1.my, l2.my);
}


int seg_int_poly (point_3d* p, int ct, point_3d p1, point_3d p2, point_3d* result)
{
	point_3d r;
	int rct = 0;
	int i;

	dprint ("ct: ", ct, 16);

	for (i = 0; i < ct - 1; i++)
		if (line_seg_intersect_inc (p1, p2, p[i], p[i + 1], r))
			result[rct++] = r;

	if (line_seg_intersect_inc (p1, p2, p[i], p[0], r))
		result[rct++] = r;
	
	return rct;
}


bool polys_intersect (point_3d* p1, int ct1, point_3d* p2, int ct2, point_3d* nter, int& nct)
{
	int i;
	int dum;
	point_3d p;
	nct = 0;

	if (ct1 < 3)
		return false;

	if (ct2 < 3)
		return false;

	for (i = 0; i < ct1; i++)
		if (point_in_poly (p1[i], p2, ct2, dum) != 1)
			nter[nct++] = p1[i];

	for (i = 0; i < ct2; i++)
		if (point_in_poly (p2[i], p1, ct1, dum) != 1)
			nter[nct++] = p2[i];

	for (i = 0; i < ct1 - 1; i++) 
		nct += seg_int_poly (p2, ct2, p1[i], p1[i + 1], &nter[nct]);

	nct += seg_int_poly (p2, ct2, p1[i], p1[0], &nter[nct]);
	
	return is_non_zero_area (nter, nct);
}


bool polys_intersect (point_3d* p1, int ct1, point_3d* p2, int ct2)
{
	int i;
	int dum;
	point_3d nter[256];
	int nct = 0;
	point_3d p;

//	for (i = 0; i < ct1; i++)
//		if (point_in_poly (p1[i], p2, ct2, dum) != 1)
//			nter[nct++] = p1[i];

//	for (i = 0; i < ct2; i++)
//		if (point_in_poly (p2[i], p1, ct1, dum) != 1)
//			nter[nct++] = p2[i];

//	for (i = 0; i < ct1 - 1; i++) 
//		nct += seg_int_poly (p2, ct2, p1[i], p1[i + 1], &nter[nct]);

//	nct += seg_int_poly (p2, ct2, p1[i], p1[0], &nter[nct]);
//		if (line_seg_intersect_inc (p1[i], p1[i + 1], ], area[i + 1], p))
	
//	for (i = 0; i < nct; i++)
//		create_test_point (nter[i], i);

	return is_non_zero_area (nter, nct);
}


int point_rel_poly_inc (point_3d p, point_3d* v, int ct)
{
	// 1 - cw side of poly
	// 0 - on poly
	// -1 - cc side of poly

	plane_3d plane = calc_plane_3d (v);
	
	if (visible_side (v))
		return -point_rel_plane (p, plane);
	else
		return point_rel_plane (p, plane);
}


int plane_int_hex_solid (plane_3d plane, point_3d* hex_solid, point_3d* result)
{
	//
	//   4---------0
	//   |\        |\
	//   | \       | \
	//   |  7------+--3
	//   |  |      |  |
	//   5--+------1  |
	//    \ |       \ |
	//     \|	     \|
	//      6---------2

	static int seg[24] = {0,4,  3,7,  2,6,  1,5,  0,1,  1,2,  2,3,  3,0,  4,5,  5,6,  6,7,  7,4};
	point_3d p;	
	int res_ct = 0;
	int i = 0;

	for (; i < 8; i += 2)
		if (seg_pen_plane_inc (hex_solid[seg[i]], hex_solid[seg[i + 1]], plane, p))
			result[res_ct++] = p;

	for (; i < 24; i += 2)
		if (seg_pen_plane_exc (hex_solid[seg[i]], hex_solid[seg[i + 1]], plane, p))
			result[res_ct++] = p;

	return res_ct;
}


bool poly_int_hex_solid (point_3d* poly, int vert_ct, point_3d* hex_solid, point_3d* result, int& res_ct)
{
	point_3d int_poly[256];
	plane_3d plane = calc_plane_3d (poly);
	int int_ct = plane_int_hex_solid (plane, hex_solid, int_poly);

	if (int_ct) {
		sort_by_angle (int_poly, int_ct, visible_side (poly));
		return polys_intersect (poly, vert_ct, int_poly, int_ct, result, res_ct);
	}
	else
		return false;
}


inline void perp_to_plane (plane_3d plane, bool side, double& ax, double& ay)
{
	double x, y;
	calc_angles (plane, side, x, y);
	ax = x + PI / 2.0;
	ay = y + PI / 2.0;
}

	
#endif //!INC_MATH3D