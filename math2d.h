#ifndef INC_MATH2D
#define INC_MATH2D

#define PI D3DX_PI 
#define FL_CMP_PREC .001
#define DBL_CMP_PREC .000001 
#define point_2d D3DXVECTOR2

#include <math.h>
#include <stdio.h>

inline double direct_double (unsigned high, unsigned low);
double calc_inf ();
double calc_nan ();
float calc_finf ();
float calc_fnan ();

const static double _pos_inf = calc_inf ();
const static double _neg_inf = -calc_inf ();
const static double _nan = calc_nan ();

const static float _fpos_inf = calc_finf ();
const static float _fneg_inf = -calc_finf ();
const static float _fnan = calc_fnan ();

char _text_buff[40][40];

union d_i_union {
  double d;
  unsigned i[2];
};

inline bool approx_equal (float n1, float n2);
inline bool approx_equal (float n1, double n2);
inline bool approx_equal (double n1, float n2);
inline bool approx_equal (double n1, double n2);
inline bool approx_equal_low (double n1, double n2);

class line_2d {
public:
	double m, b;

	inline line_2d () {}
	inline line_2d (double lm, double lb)
	{
		m = lm;
		b = lb;
	}

	inline line_2d (double ind1, double dep1, double ind2, double dep2)
	{
		if (approx_equal (ind1, ind2)) {
			m = _pos_inf;
			b = ind1;
		}
		else {
			m = (dep2 - dep1) / (ind2 - ind1);
			b = dep1 - m * ind1;
		}
	}
};


void dprint (const char* text, int y)
{
	strcpy (_text_buff[y], text);
}


void dprint (const char* text, double n, int y)
{
	char buff[256];
	sprintf (buff, "%s%f", text, n);
	strcpy (_text_buff[y], buff);
}


inline bool approx_equal (float n1, float n2)
{
	if (n1 > n2)
		return (n1 - n2) < FL_CMP_PREC;
	else 
		return (n2 - n1) < FL_CMP_PREC;
}


inline bool approx_equal (double n1, double n2)
{
	if (n1 > n2)
		return (n1 - n2) < DBL_CMP_PREC;
	else 
		return (n2 - n1) < DBL_CMP_PREC;
}


inline bool approx_equal (float n1, double n2)
{
	if (n1 > n2)
		return (n1 - n2) < FL_CMP_PREC;
	else 
		return (n2 - n1) < FL_CMP_PREC;
}


inline bool approx_equal (double n1, float n2)
{
	if (n1 > n2)
		return (n1 - n2) < FL_CMP_PREC;
	else 
		return (n2 - n1) < FL_CMP_PREC;
}


inline bool approx_equal_low (double n1, double n2)
{
	// low precision complare

	if (n1 > n2)
		return (n1 - n2) < FL_CMP_PREC;
	else 
		return (n2 - n1) < FL_CMP_PREC;
}


inline bool approx_greater_inc (float n1, float n2)
{
	if (n1 > n2)
		return true;
	else
		return (n2 - n1) < FL_CMP_PREC;			
}


inline bool approx_greater_inc (double n1, double n2)
{
	if (n1 > n2)
		return true;
	else
		return (n2 - n1) < DBL_CMP_PREC;			
}


inline bool approx_greater_inc (float n1, double n2)
{
	if (n1 > n2)
		return true;
	else
		return (n2 - n1) < FL_CMP_PREC;			
}


inline bool approx_greater_inc (double n1, float n2)
{
	if (n1 > n2)
		return true;
	else
		return (n2 - n1) < FL_CMP_PREC;			
}


inline bool approx_lesser_inc (float n1, float n2)
{
	if (n1 < n2)
		return true;
	else
		return (n1 - n2) < FL_CMP_PREC;			
}


inline bool approx_lesser_inc (double n1, double n2)
{
	if (n1 < n2)
		return true;
	else
		return (n1 - n2) < DBL_CMP_PREC;			
}


inline bool approx_lesser_inc (float n1, double n2)
{
	if (n1 < n2)
		return true;
	else
		return (n1 - n2) < FL_CMP_PREC;			
}


inline bool approx_lesser_inc (double n1, float n2)
{
	if (n1 < n2)
		return true;
	else
		return (n1 - n2) < FL_CMP_PREC;			
}


inline bool approx_greater_exc (float n1, float n2)
{
	return n1 > (n2 + FL_CMP_PREC);	
}


inline bool approx_lesser_exc (float n1, float n2)
{
	return n1 < (n2 - FL_CMP_PREC);	
}


inline bool approx_zero (float n)
 {
	if (n > 0.0f)
		return n < FL_CMP_PREC;
	else
		return n > -FL_CMP_PREC;
}


inline bool approx_zero (double n)
 {
	if (n > 0.0)
		return n < DBL_CMP_PREC;
	else
		return n > -DBL_CMP_PREC;
}


inline bool approx_zero_low (double n)
{
	if (n > 0.0)
		return n < FL_CMP_PREC;
	else
		return n > -FL_CMP_PREC;
}


inline float sind (double a)
{
	return sinf((float)a);
}


inline float cosd (double a)
{
	return cosf((float)a);
}


inline double rad_to_deg (double a)
{
	return a * (180.0 / PI);
}


inline double frac (double n)
{
	return n - floor(n);
}


inline float frac (float n)
{
	return n - (float)floor((double)n);
}


inline double round (double n, double r)
{
	return r * floor((n + r * 0.5) / r);
}


inline double direct_double (unsigned high, unsigned low)
{
  d_i_union temp;

  temp.i[0] = low;
  temp.i[1] = high;
  
  return temp.d;
}


inline line_2d create_line (double m, double ind, double dep)
{
	line_2d result;

	result.m = m;
	result.b = dep - ind * m;

	return result;
}


inline point_2d lines_intersect (line_2d& l1, line_2d& l2)
{
	point_2d result;

	if (l1.m == _pos_inf) {
		result.x = float(l1.b);
		result.y = float(l2.m * l1.b + l2.b);
	}
	else if (l2.m == _pos_inf) {
		result.x = float(l2.b);
		result.y = float(l1.m * l2.b + l1.b);
	}
	else {
		result.x = float((l2.b - l1.b) / (l1.m - l2.m));
		result.y = float(l1.m * result.x + l1.b);
	}

	return result;
}


inline double simp_angle (double a)
{
	return atan2(sin(a), cos(a));
}


inline double simp_angle_pos (double angle)
{
	double a = atan2(sin(angle), cos(angle));

	if (a > 0)
		return a;	
	else
		return a + 2 * PI;
}


double calc_inf ()
{
	double den = 0.0;
	return 1 / den;
}


double calc_nan ()
{
	double den = 0.0;
	return 0.0 / den;
}


float calc_finf ()
{
	float den = 0.0f;
	return 1.0f / den;
}


float calc_fnan ()
{
	float den = 0.0f;
	return 0.0f / den;
}


int trin_cmp (float n1, float n2)
{
	// trinary compare

	if (approx_equal (n1, n2))
		return 0;
	else if (n1 < n2)
		return -1;
	else
		return 1;
}


float fof (line_2d line, float ind)
{
	return float(line.m * double(ind) + line.b);
}


#endif // !INC_MATH2D
