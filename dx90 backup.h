#ifndef INC_DX90
#define INC_DX90

#define VERT_LIGHT_DEFAULT 0x00808080
#define VERT_SCALE 0.01f
#define VERT_SECT_LEN 4 * 256
#define INDEX_SECT_LEN 3 * 256
#define VECT_UNDEF D3DXVECTOR3(0.0f, 1.0f, 0.0f)

#include <d3dx9.h>
#include <\programs\win.h>
window _main_win("Main", "", 100, 100, 400, 300);

#include <\programs\math2d.h>
#include <\programs\math3d.h>
#include <\programs\err_3d.h>
#include <\programs\make.h>

D3DXMATRIX mat(ray_3d ray);

#include <\programs\test.h>

struct test_point {
	point_3d pos;
	bool used;
	DWORD color;
} _test_point[100];

struct base_tex_vert {
    D3DXVECTOR3 position;
	D3DXVECTOR3 normal; 
    D3DCOLOR color;    
	FLOAT tu, tv;   
};

struct add_tex_vert {
    D3DCOLOR color;    
	FLOAT tu, tv;   
};

struct tex_type {
	LPDIRECT3DTEXTURE9 tex;
	char name[40];
	double dim;
	int n;
	tex_type* next;
};

struct vert_flag_type {
	unsigned selected: 1;
	unsigned visible: 1;
	unsigned corner: 1;	
	unsigned deleted: 1;
	unsigned used: 1;
	unsigned marked: 1;
};

struct vert_buff_sect {
	LPDIRECT3DVERTEXBUFFER9 buff;
	int count;
	vert_buff_sect* next;
	vert_flag_type* flags;
};

struct index_buff_sect {
	LPDIRECT3DINDEXBUFFER9 buff;
	int count;
	index_buff_sect* next;
};

struct {
	struct {
		bool snap;
	} writable;
	tex_type* curr_tex;
	tex_type* tex_pal[10];
	tex_type* vert;
	tex_type* cursor;
} static _build_env;

point_3d calc_normal (point_3d* p, point_3d ref, bool side);
void calc_tex_coords (point_3d* p_list, int p_count, tex_type* tex, float* tu, float* tv);
D3DXVECTOR3	g_vEye(5.0f, 5.0f, -5.0f);    // Eye Position
D3DXVECTOR3	g_vLook(-0.5f, -0.5f, 0.5f);  // Look Vector
D3DXVECTOR3	g_vUp(0.0f, 1.0f, 0.0f);      // Up Vector
D3DXVECTOR3	g_vRight(1.0f, 0.0f, 0.0f);   // Right Vector
float simp_tex_coord (float t1, float t2);
void simp_tex_coord (int ct, double* in_u, double* in_v, float* out_u, float* out_v);
void setup_lights();
void set_dir_light(int num, float r, float g, float b, float amb, float dir, double axz, double ayz);
double calc_tex_coord (D3DXVECTOR3 p, double axz);
double calc_tex_coord (point_3d p, double x, double y);
double calc_tex_coord_xz (point_3d p, plane_3d plane, bool side);
void set_sprite_scale (float scale) ;
ray_3d calc_view (ray_3d cursor);
void update_index_buffer (int& i_ct, index_buff_sect*& i_sect);
void update_vert_buffer (int& v_ct, vert_buff_sect*& v_sect);
void update_index_buffer (index_buff_sect*& i_sect);
void update_vert_buffer (vert_buff_sect*& v_sect);
D3DXVECTOR3 calc_rel_vect (D3DXVECTOR3 pos, float kx, float ky, ray_3d& view);
void dprint (const char* text, int y);
void dprint (const char* text, double n, int y);
void show_text ();
void create_test_point (point_3d p, int n);

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_NORMAL)
#define D3DFVF_CUSTOMVERTEX2 (D3DFVF_DIFFUSE|D3DFVF_TEX1)//|D3DFVF_NORMAL)

class d3d_device {
public:	
	LPD3DXFONT font;
	LPDIRECT3DDEVICE9 device; // Rendering device
	D3DCAPS9 caps;
	LPD3DXSPRITE spr_2d, spr_3d;
	IDirect3DSurface9* back_buff;

	d3d_device(bool windowed);
	~d3d_device()
	{
		if(device != NULL)
			device->Release();

		if(d3d_obj != NULL)         
		   d3d_obj->Release();

		//IUnknown->Release();
	}
	void setup_mat();
	change_win_params (bool windowed, int x_res, int y_res);
private:
	LPDIRECT3D9 d3d_obj; // Used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
} _d3d_dev(false);


class tex_pool {
public:
	tex_type* tex_start;
	tex_type* tex_curr;

	tex_pool()
	{
		tex_curr = tex_start = NULL;
	}
	
	~tex_pool()
	{
		clear ();
	}	

	tex_type* load_3d(char* name)
	{
		tex_type* new_tex = new tex_type;

		if (tex_start == NULL) 
			tex_start = new_tex;
		else 
			tex_curr->next = new_tex;

		tex_curr = new_tex;
		
		FILE* dat = fopen (name, "rt");		
		
		if (dat == NULL) {
			msg("Cannot open file", name);
			PostQuitMessage(1);
			//exit(1);
		}

		char file[256];
		float dim;
		
		fscanf(dat, "%s", file);
		fscanf(dat, "%f", &dim);
		//msg(dim);

		fclose(dat);
		
		if (FAILED (D3DXCreateTextureFromFile(_d3d_dev.device, file, &new_tex->tex))) {
			msg("Cannot open file", file);
			exit(1);
		}		
		
		strcpy (new_tex->name, name);
		new_tex->dim = dim;
		new_tex->next = NULL;

		return new_tex;
	}

	tex_type* load_2d(char* name)
	{
		tex_type* new_tex = new tex_type;

		if (tex_start == NULL) 
			tex_start = new_tex;
		else 
			tex_curr->next = new_tex;

		tex_curr = new_tex;
		
		if (FAILED (D3DXCreateTextureFromFile(_d3d_dev.device, name, &new_tex->tex))) {
			msg("Cannot open file", name);
			exit(1);
		}
			
		new_tex->next = NULL;

		return new_tex;
	}

	void update_nums ()
	{
		int ct = 0;

		for (tex_type* t = tex_start; t != NULL; t = t->next) {
			t->n = ct;
			ct++;
		}
	}	

	void save (FILE* fp);
	void load (FILE* fp);
	tex_type** create_tex_ref ();
	void clear ();
private:
};

struct prim_type {
	tex_type* tex;
//	prim_type* next;
};

struct prim_sect {
	prim_type* prim;
	prim_type* next;
};

class solid_type {
public:
	tex_type* base_tex;
	solid_type* next;
	int n;	
};

struct poly_type {
	tex_type* tex;
	solid_type* solid;
	int verts;
	poly_type* next;
};

struct user_vert_type {
	D3DXVECTOR3 pos;
	vert_flag_type flags;
	user_vert_type* next;
};

user_vert_type* _user_vert_start = NULL;

class prim_pool {
public:
	prim_pool()
	{
		prim_ct = 0;
		vert_start = NULL;
		index_start = NULL;
		poly_start = NULL;
		solid_start = NULL;
	}

	~prim_pool ()
	{
		clear ();
	}

	poly_type* create_poly (tex_type* tex, solid_type* sol, point_3d* p_list, int p_count, point_3d ref, bool side);
	void get_poly (vert_buff_sect* v_sect, int vert0, base_tex_vert* vert, int vert_ct);
	void put_poly (vert_buff_sect* v_sect, int vert0, base_tex_vert* vert, int vert_ct);
	void set_texture (poly_type* poly, tex_type* tex);
	void get_tri (vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0, 
		base_tex_vert& p1, base_tex_vert& p2, base_tex_vert& p3);
	void put_tri (vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0, 
		base_tex_vert p1, base_tex_vert p2, base_tex_vert p3);
	int solid_num (solid_type* sol);
	void number_solids ();
	solid_type** create_sol_ref ();
	solid_type* solid_of_prim (int prim);
	poly_type* poly_of_prim (int prim_n);
	solid_type* create_solid (tex_type* base_tex);
	void clear ();
	void save_polys (FILE* fp);
	void load_polys (tex_type** tp, FILE* fp);
	void create_floor (tex_type* tex, point_3d* p, int ct);
	index_buff_sect* create_index_buff_sect(index_buff_sect* last, int count);
	vert_buff_sect* create_vert_buff_sect (vert_buff_sect* last, int count);
	poly_type* log_poly (tex_type* tex, solid_type* sol, int verts);
	void init_geo(tex_pool& tp);
	void render(tex_pool& tp, ray_3d& cam);
	void draw_verts(tex_pool& tp, ray_3d& view);
	void prune_verts();
	void clear_sel_verts();
	void create_unused_vert (D3DXVECTOR3 p);
	void show_stats ();
	void draw_poly (poly_type* poly, int& v_ct, int& i_ct, vert_buff_sect*& v_sect, index_buff_sect*& i_sect);
	void draw_prim (int& v_ct, int& i_ct, vert_buff_sect*& v_sect, index_buff_sect*& i_sect);
	int get_sel_verts (D3DXVECTOR3* result);
	void create_wall (float x, float y, float z, float w, float h, float d, double a, tex_type* tex);
	bool in_scope_vert (ray_3d& view, D3DXVECTOR3& p);
	void clear_marked ();
	void select_marked ();
	int ray_int_prim (ray_3d& cam, point_3d& result);
	void get_tri (vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0,
		point_3d& p1, point_3d& p2, point_3d& p3);
	
	void save_vert_buffers (FILE* fp);
	void load_vert_buffers (FILE* fp);
private:
	vert_buff_sect* vert_start;
	index_buff_sect* index_start;
	prim_type* prim_start;
	poly_type* poly_start;
	solid_type* solid_start;
	int prim_ct;

	int create_vert (D3DXVECTOR3 p, vert_buff_sect* sect, D3DXVECTOR3 n, float tu, float tv);
	void create_user_vert (D3DXVECTOR3 p);
	poly_type* create_rect (tex_type* tex, D3DXVECTOR3 start, float w, float h, double axz, double ayz);
	void create_prim (tex_type* tex, int v1, int v2, int v3);
	vert_buff_sect* get_last_vert_buff_sect();
	index_buff_sect* get_last_index_buff_sect();
	vert_buff_sect* create_vert_buff_sect();
	index_buff_sect* create_index_buff_sect();
	vert_buff_sect* get_vert_sect (int n);
	index_buff_sect* get_index_sect (int n);
};


d3d_device::change_win_params (bool windowed, int x_res, int y_res)
{
	D3DDISPLAYMODE d3d_dm; 
	d3d_obj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3d_dm);	

	if (windowed) {
		d3dpp.hDeviceWindow = _main_win.handle;	
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferCount = 1;  
    //	d3dpp.BackBufferWidth = 0;
    //	d3dpp.BackBufferHeight = 0;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	    	
	}
	else {
		d3dpp.hDeviceWindow = _main_win.handle;	    	
		d3dpp.Windowed = FALSE; 
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
		d3dpp.BackBufferCount = 1;  
    	d3dpp.BackBufferWidth = x_res;
    	d3dpp.BackBufferHeight = y_res;
    	d3dpp.BackBufferFormat = d3d_dm.Format;
	}

	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.EnableAutoDepthStencil = TRUE;

	device->Reset(&d3dpp);

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // NONE
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_DITHERENABLE, TRUE);	
	//device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
		
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	device->ShowCursor(FALSE);
}


d3d_device::d3d_device(bool windowed)
{    
	D3DDISPLAYMODE d3d_dm; 
	
	device = NULL;
	d3d_obj = NULL;
	
	d3d_obj = Direct3DCreate9(D3D_SDK_VERSION);
    d3d_obj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3d_dm);

	ZeroMemory (&d3dpp, sizeof(d3dpp));
	if (windowed) {
		d3dpp.hDeviceWindow = _main_win.handle;	
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferCount = 1;  
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	    	
	}
	else {
		d3dpp.hDeviceWindow = _main_win.handle;	    	
		d3dpp.Windowed = FALSE; 
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
		d3dpp.BackBufferCount = 1;  
    	d3dpp.BackBufferWidth = d3d_dm.Width; 
    	d3dpp.BackBufferHeight = d3d_dm.Height;
    	d3dpp.BackBufferFormat = d3d_dm.Format;
	}

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3d_obj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _main_win.handle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &device);
		
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_LIGHTING, TRUE);//FALSE);
    device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_DITHERENABLE, TRUE);	
	//device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	device->GetBackBuffer (0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buff);
	device->GetDeviceCaps(&caps);	
	
	//device->ShowCursor(FALSE);
	D3DXCreateSprite(device, &spr_2d);
	D3DXCreateSprite(device, &spr_3d);

	D3DXCreateFont(device, 25, 0, FW_BOLD, 0, FALSE, 
		                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		                 DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &font);
}


class stream_type {
public:
	LPDIRECT3DVERTEXBUFFER9 vert_buff[16];
	LPDIRECT3DINDEXBUFFER9 index_buff;

	stream_type(int vert_ct, int ind_ct)
	{
		index_buff = NULL;
		vert_buff[0] = NULL;

		verif(
			_d3d_dev.device->CreateVertexBuffer(vert_ct * sizeof(base_tex_vert), D3DUSAGE_DYNAMIC, 
									  D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &vert_buff[0], NULL),
			"Create Vertex Buffer"
		);

//		for (int i = 1; i < (int)_d3d_dev.caps.MaxStreams; i++)
//			verif(
//				_d3d_dev.device->CreateVertexBuffer(vert_ct * sizeof(add_tex_vert), D3DUSAGE_WRITEONLY, 
//									  D3DFVF_CUSTOMVERTEX2, D3DPOOL_DEFAULT, &vert_buff[i], NULL),
//				"Create Vertex Buffer n"
//			);
		verif(
			_d3d_dev.device->CreateIndexBuffer(ind_ct * sizeof(long), D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &index_buff, NULL),
			"Create index buff"
		);
	}

	~stream_type()
	{
		//MessageBox(_main_win.handle, "the heck", "what", NULL);
		vert_buff[0]->Release();
//		for (int i = 0; i < (int)_d3d_dev.caps.MaxStreams; i++)
//			vert_buff[i]->Release();
		index_buff->Release();
	}
private:	
};


/*
void init_geo4(stream_type& stream)
{
//    device->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
  //                                                0, D3DFVF_CUSTOMVERTEX,
    //                                              D3DPOOL_DEFAULT, &g_pVB, NULL );    
    
    base_tex_vert* pVertices;
    .vert_buff[0]->Lock( 0, 0, (void**)&pVertices, 0 );
    
    for( DWORD i=0; i<50; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(50-1);

        pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].color    = 0x00000000;
        pVertices[2*i+0].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+0].tv       = 1.0f;

        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].color    = 0x00ffffff;
        pVertices[2*i+1].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+1].tv       = 0.0f;
    }
    
	stream.vert_buff[0]->Unlock();
}
*/


vert_buff_sect* prim_pool::get_last_vert_buff_sect()
{
	vert_buff_sect* result = NULL;

	for (vert_buff_sect* i = vert_start; i != NULL; i = i->next)
		result = i;

	return result;
}


index_buff_sect* prim_pool::get_last_index_buff_sect()
{
	index_buff_sect* result = NULL;

	for (index_buff_sect* i = index_start; i != NULL; i = i->next)
		result = i;

	return result;
}


vert_buff_sect* prim_pool::create_vert_buff_sect()
{
	vert_buff_sect* last = get_last_vert_buff_sect();
	vert_buff_sect* nu = new vert_buff_sect;

	if (vert_start == NULL)
		vert_start = nu;
	else
		last->next = nu;

	nu->next = NULL;
	nu->count = 0;
	nu->flags = new vert_flag_type[VERT_SECT_LEN];
	verif(
		_d3d_dev.device->CreateVertexBuffer(VERT_SECT_LEN * sizeof(base_tex_vert), 
		D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &nu->buff, NULL),
		"Create Vertex Buffer"
	);

	return nu;
}


vert_buff_sect* prim_pool::create_vert_buff_sect (vert_buff_sect* last, int count)
{
	vert_buff_sect* nu = new vert_buff_sect;

	if (vert_start == NULL)
		vert_start = nu;
	else
		last->next = nu;

	nu->next = NULL;
	nu->count = count;
	nu->flags = new vert_flag_type[VERT_SECT_LEN];
	verif(
		_d3d_dev.device->CreateVertexBuffer(VERT_SECT_LEN * sizeof(base_tex_vert), 
		D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &nu->buff, NULL),
		"Create Vertex Buffer"
	);

	return nu;
}


index_buff_sect* prim_pool::create_index_buff_sect()	
{
	index_buff_sect* last = get_last_index_buff_sect();
	index_buff_sect* nu = new index_buff_sect;

	if (index_start == NULL) {
		index_start = nu;		
	}
	else {
		last->next = nu;
	}		

	nu->next = NULL;
	nu->count = 0;
	verif(
		_d3d_dev.device->CreateIndexBuffer(INDEX_SECT_LEN * sizeof(long), 
		D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &nu->buff, NULL),
		"Create index buff"
	);

	return nu;
}


index_buff_sect* prim_pool::create_index_buff_sect(index_buff_sect* last, int count)	
{
	index_buff_sect* nu = new index_buff_sect;

	if (index_start == NULL) 
		index_start = nu;		
	else {
		last->next = nu;
	}		

	nu->next = NULL;
	nu->count = count;
	verif(
		_d3d_dev.device->CreateIndexBuffer(INDEX_SECT_LEN * sizeof(long), 
		D3DUSAGE_DYNAMIC, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &nu->buff, NULL),
		"Create index buff"
	);

	return nu;
}


void draw_vert (D3DXVECTOR3 pos, float kx, float ky, tex_pool& tp, vert_flag_type& flag, ray_3d& view)
{
	D3DXVECTOR3 p, rel, rot;
	rel = calc_rel_vect (pos, kx, ky, view);
	//rotate_c(pos, view.pos, view.axz, -view.ayz, 0.0);
	//rel.x = rot.x - view.pos.x;
	//rel.y = rot.y - view.pos.y;
	//rel.z = rot.z - view.pos.z;
		
	if (rel.z > 0.5f) 
		if (!flag.deleted)
			if (flag.visible) {
				float scale = float((64.0 / rel.z) * 0.25);
				int w = int(64.0f * scale);	
				DWORD color;

				p.x = ((1440 / 2) + kx * rel.x / rel.z - w / 2) / scale;
				p.y = ((900 / 2) - ky * rel.y / rel.z - w / 2) / scale;		
				p.z = 0.0f;
		
				//if (flag.marked)
			//		color = 0xffff0000;
				//else
					if (flag.selected)
						color = 0xff0000ff;
					else
						color = 0xffffffff;

				set_sprite_scale (scale);
				_d3d_dev.spr_2d->Draw(_build_env.vert->tex, NULL, NULL, &p, color);	
			}
}


D3DXVECTOR3 calc_rel_vect (D3DXVECTOR3 pos, float kx, float ky, ray_3d& view)
{
	
	D3DXVECTOR3 rel;
	D3DXVECTOR3 rot = rotate_c(pos, view.pos, view.axz, -view.ayz, 0.0);

	rel.x = rot.x - view.pos.x;
	rel.y = rot.y - view.pos.y;
	rel.z = rot.z - view.pos.z;
	
	return rel;
}


bool map_vect (D3DXVECTOR3 pos, D3DXVECTOR2& result, float kx, float ky, ray_3d& view)
{
	D3DXVECTOR3 p, rel = calc_rel_vect (pos, kx, ky, view);
		
	if (rel.z > 0.5f) {
		result.x = ((1440 / 2) + kx * rel.x / rel.z);
		result.y = ((900 / 2) - ky * rel.y / rel.z);				
		return true;
	}
	else
		return false;
}


void prim_pool::draw_verts(tex_pool& tp, ray_3d& view)
{
	base_tex_vert* vert;
    D3DXVECTOR3 cursor = offset_3d(view.pos, -10.0, -view.axz, view.ayz, 0.0);

	_d3d_dev.spr_2d->Begin(0);

	D3DCOLOR color = 0x8800ffff;	
	float kx = float(tan(PI / 4.0) * (1440 / 2.0f) * 1.5);
	float ky = kx;

	for (vert_buff_sect* sect = vert_start; sect != NULL; sect = sect->next) {
		verif(
			sect->buff->Lock (0, 0, (void**)&vert, 0), 
			"lock"
		);
		
		for (int i = 0; i < sect->count; i++) {
	//		if (sect->flags[i].selected)
	//			msg ("sel");
	//		else
	//			msg ("unsel");
			draw_vert (vert[i].position, kx, ky, tp, sect->flags[i], view);
		}
		
		verif(
			sect->buff->Unlock(),
			"unlock"
		);	
	}

	for (user_vert_type* uv = _user_vert_start; uv != NULL; uv = uv->next) {
		//msg ("in loop");
		draw_vert (uv->pos, kx, ky, tp, uv->flags, view);
	}

	_d3d_dev.spr_2d->End();	
}


void prim_pool::save_vert_buffers (FILE* fp)
{
	base_tex_vert* vert;
	vert_buff_sect* v_sect;
	index_buff_sect* i_sect;
	int ct;
	int* index;

	ct = 0;
	for (v_sect = vert_start; v_sect != NULL; v_sect = v_sect->next) 
		ct++;
	fwrite (&ct, sizeof(int), 1, fp);

	for (v_sect = vert_start; v_sect != NULL; v_sect = v_sect->next) {
		verif(
			v_sect->buff->Lock (0, 0, (void**)&vert, 0), 
			"lock"
		);
		
		fwrite (&v_sect->count, sizeof(int), 1, fp);
		fwrite (vert, sizeof(base_tex_vert), v_sect->count, fp);
		fwrite (v_sect->flags, sizeof(vert_flag_type), v_sect->count, fp);
		
		verif(
			v_sect->buff->Unlock(),
			"unlock"
		);	
	}

	ct = 0;
	for (i_sect = index_start; i_sect != NULL; i_sect = i_sect->next) 
		ct++;
	fwrite (&ct, sizeof(int), 1, fp);

	for (i_sect = index_start; i_sect != NULL; i_sect = i_sect->next) {
		verif(
			i_sect->buff->Lock (0, 0, (void**)&index, 0), 
			"lock"
		);
		
		fwrite (&i_sect->count, sizeof(int), 1, fp);
		fwrite (index, sizeof(int), i_sect->count, fp);
			
		verif(
			i_sect->buff->Unlock(),
			"unlock"
		);	
	}	
}


void prim_pool::load_vert_buffers (FILE* fp)
{
	int ct;
	vert_buff_sect* prev_v = NULL;
	vert_buff_sect* curr_v = NULL;
	index_buff_sect* prev_i = NULL;
	index_buff_sect* curr_i = NULL;
	base_tex_vert* vert = NULL;
	int* index = NULL;
	int i;

	fread (&ct, sizeof(int), 1, fp);
		
	for (i = 0; i < ct; i++) {
		curr_v = create_vert_buff_sect (prev_v, 0);		
		fread (&curr_v->count, sizeof(int), 1, fp);

		verif(
			curr_v->buff->Lock (0, 0, (void**)&vert, 0), 
			"lock"
		);
		fread (vert, sizeof(base_tex_vert), curr_v->count, fp);
		verif(
			curr_v->buff->Unlock(),
			"unlock"
		);	

		fread (curr_v->flags, sizeof(curr_v->flags), curr_v->count, fp);		
		prev_v = curr_v;			
	}

	fread (&ct, sizeof(int), 1, fp);
		
	for (i = 0; i < ct; i++) {
		curr_i = create_index_buff_sect (prev_i, 0);		
		fread (&curr_i->count, sizeof(int), 1, fp);

		verif(
			curr_i->buff->Lock (0, 0, (void**)&index, 0), 
			"lock"
		);
		fread (index, sizeof(int), curr_i->count, fp);
		verif(
			curr_i->buff->Unlock(),
			"unlock"
		);	

		prev_i = curr_i;			
	}
}


poly_type* prim_pool::create_rect (tex_type* tex, D3DXVECTOR3 start, float w, float h, double axz, double ayz)
{
    // Quad verts:

	// 2---3
	// | \ |
	// 0---1
	
	//  prim 1: 0, 1, 2
	//  prim 2: 0, 2, 3
	
	// Create 4 vertices for quad

	float u[4], v[4]; // simplified texture coordinates
	double ru[4], rv[4]; // raw texture coordinates
	D3DXVECTOR3 p[4];
	int ind[4];
	int i;
	D3DXVECTOR3 n = offset_3d(D3DXVECTOR3(0.0f, 0.0f, 0.0f), -1.0, -(float)axz, -(float)ayz, 0.0);

	if (approx_equal (ayz, -PI / 2.0)) {
		p[2] = start;
		p[3] = offset_3d (start, w, axz);
		p[0] = offset_3d (start, h, axz + PI / 2.0);
		p[1] = offset_3d (p[3], h, axz + PI / 2.0);		

		for (i = 0; i < 4; i++) {
			ru[i] = p[i].x / tex->dim;//(calc_tex_coord (p[i], axz) / tex->dim);
			rv[i] = p[i].z / tex->dim;//(calc_tex_coord (p[i], axz + PI / 2.0) / tex->dim);		
		}		
	}
	else if (approx_equal (ayz, PI / 2.0)) {
		p[0] = start;
		p[1] = offset_3d (start, w, axz);
		p[2] = offset_3d (start, h, axz + PI / 2.0);
		p[3] = offset_3d (p[1], h, axz + PI / 2.0);		

		for (i = 0; i < 4; i++) {
			ru[i] = p[i].x / tex->dim;//(calc_tex_coord (p[i], axz) / tex->dim);
			rv[i] = p[i].z / tex->dim;//(calc_tex_coord (p[i], axz - PI / 2.0) / tex->dim);
		}
	}
	else {
		p[0] = start;
		p[1] = offset_3d (start, w, axz);
		p[2] = offset_3d (start, 0.0f, h, 0.0f);
		p[3] = offset_3d (p[1], 0.0f, h, 0.0f);

		for (i = 0; i < 4; i++) {
			ru[i] = (calc_tex_coord (p[i], axz) / tex->dim);
			rv[i] = (-p[i].y / tex->dim);
		}
	}	
	
	simp_tex_coord (4, ru, rv, u, v);

	vert_buff_sect* sect = get_vert_sect(4);
	
	for (i = 0; i < 4; i++) 
		ind[i] = create_vert(p[i], sect, n, u[i], v[i]);			
	
	// Create 2 triangles for quad	
	create_prim(tex, ind[1], ind[2], ind[3]);
	create_prim(tex, ind[0], ind[2], ind[1]);

	solid_type* sol = create_solid (tex);
	return log_poly (tex, sol, 4);
}


poly_type* prim_pool::log_poly (tex_type* tex, solid_type* sol, int verts)
{
	poly_type* nu = new poly_type;
	
	if (poly_start == NULL)
		poly_start = nu;
	else {
		poly_type* last;
		for (last = poly_start; last->next != NULL; last = last->next)
			;
		last->next = nu;
	}

	nu->next = NULL;
	nu->tex = tex;
	nu->verts = verts;
	nu->solid = sol;

	return nu;
}


void prim_pool::load_polys (tex_type** tp, FILE* fp)
{
	int ct;
	poly_type* prev;
	poly_type* nu;
	int tex;

	fread (&ct, sizeof(int), 1, fp);

	for (int i = 0; i < ct; i++) {
		nu = new poly_type;
		fread (nu, sizeof(poly_type), 1, fp);	
		
		fread (&tex, sizeof(int), 1, fp);
		nu->tex = tp[tex];
		
		if (i == 0)
			poly_start = nu;
		else
			prev->next = nu;				

		prim_ct += nu->verts - 2;
		prev = nu;		
	}
	/*
	if (ct) {
		poly_start = new poly_type;				
		fread (&poly_start, sizeof(poly_type), 1, fp);				
		
		fread (&tex, sizeof(int), 1, fp);
		poly_start->tex = tp[tex];
		
		prim_ct += poly_start->verts - 2;
		prev = poly_start;
	}

	for (int i = 1; i < ct; i++) {	
		nu = new poly_type;
		fread (nu, sizeof(poly_type), 1, fp);	
		
		fread (&tex, sizeof(int), 1, fp);
		nu->tex = tp[tex];
		
		prim_ct += nu->verts - 2;
		prev->next = nu;				
		prev = nu;
	}
*/	
	nu->next = NULL;	
}


void prim_pool::save_polys (FILE* fp)
{
	poly_type* p;
	int ct = 0;

	for (p = poly_start; p != NULL; p = p->next) 
		ct++;
	fwrite (&ct, sizeof(int), 1, fp);

	for (p = poly_start; p != NULL; p = p->next) {
		fwrite (p, sizeof(poly_type), 1, fp);
		fwrite (&p->tex->n, sizeof(int), 1, fp);
//		fwrite (&p->solid->n, sizeof(int), 1, fp);
	}
}


vert_buff_sect* prim_pool::get_vert_sect (int n)
{
	vert_buff_sect* last = get_last_vert_buff_sect();

	if (last == NULL)
		return create_vert_buff_sect();
	else 
		if (last->count + n > VERT_SECT_LEN)
			return create_vert_buff_sect();
		else
			return last;
}


index_buff_sect* prim_pool::get_index_sect (int n)
{
	index_buff_sect* last = get_last_index_buff_sect();

	if (last == NULL)
		return create_index_buff_sect();
	else
		if (last->count + n > INDEX_SECT_LEN)
			return create_index_buff_sect();
		else
			return last;
}


int prim_pool::create_vert (D3DXVECTOR3 p, vert_buff_sect* sect, D3DXVECTOR3 n, float tu, float tv) 
{
	base_tex_vert* vert;
	int vert_num = sect->count;	

	sect->buff->Lock (0, 0, (void**)&vert, 0);
	vert[vert_num].position = p;
	vert[vert_num].normal = n;//offset_3d(p, n.x, n.y, n.z);
	vert[vert_num].tu = tu;
	vert[vert_num].tv = tv;
	vert[vert_num].color = VERT_LIGHT_DEFAULT;
	sect->flags[vert_num].corner = true;
	sect->flags[vert_num].deleted = false;
	sect->flags[vert_num].selected = false;
	sect->flags[vert_num].visible = true;
	sect->flags[vert_num].used = true;
	sect->buff->Unlock();

	sect->count++;
	return sect->count - 1;
}


user_vert_type* get_last_user_vert()
{
	if (_user_vert_start == NULL)
		return NULL;
	else {
		user_vert_type* i;
		for (i = _user_vert_start; i->next != NULL; i = i->next)
			;
		return i;
	}
}


void prim_pool::create_user_vert (D3DXVECTOR3 p) 
{
	bool exist = false;

	for (user_vert_type* u = _user_vert_start; u != NULL; u = u->next)
		if (approx_equal (u->pos.x, p.x))
			if (approx_equal (u->pos.y, p.y))
				if (approx_equal (u->pos.z, p.z)) {
					u->flags.selected = true;
					exist = true;
				}

	if (!exist) {
		user_vert_type* nu = new user_vert_type;	
		nu->pos = p;
		nu->next = NULL;
		nu->flags.deleted = false;
		nu->flags.selected = true;
		nu->flags.visible = true;
	
		if (_user_vert_start == NULL) 
			_user_vert_start = nu;
		else {
			user_vert_type* temp = get_last_user_vert();
			temp->next = nu;
		}
	}
}


inline void create_user_vert (user_vert_type& uv)
{	
	user_vert_type* nu = new user_vert_type;	
	*nu = uv;
	nu->next = NULL;

	if (_user_vert_start == NULL) 
		_user_vert_start = nu;
	else {
		for (user_vert_type* i = _user_vert_start; i->next != NULL; i = i->next) 
			;	
		i->next = nu;
	}	
}


void prim_pool::create_unused_vert (D3DXVECTOR3 p)
{
	vert_buff_sect* sect = get_vert_sect(1);
	create_user_vert (p);
}


void prim_pool::prune_verts()	
{	
	for (user_vert_type* u = _user_vert_start; u != NULL; ) {
		user_vert_type* temp = u->next;
		delete u;
		u = temp;
	}
	
	_user_vert_start = NULL;
}


void prim_pool::clear_sel_verts()	
{
	base_tex_vert* vert;
	
	for (vert_buff_sect* sect = vert_start; sect != NULL; sect = sect->next) {
		sect->buff->Lock (0, 0, (void**)&vert, 0);
		
		for (int i = 0; i < sect->count; i++)
			sect->flags[i].selected = false;

		sect->buff->Unlock();	
	}

	for (user_vert_type* u = _user_vert_start; u != NULL; u = u->next)
		u->flags.selected = false;
}


void prim_pool::clear_marked ()
{
	//base_tex_vert* vert;
	
	for (vert_buff_sect* sect = vert_start; sect != NULL; sect = sect->next) {
		//sect->buff->Lock (0, 0, (void**)&vert, 0);
		
		for (int i = 0; i < sect->count; i++) {
			sect->flags[i].marked = false;							
		}

		//sect->buff->Unlock();	
	}

	for (user_vert_type* u = _user_vert_start; u != NULL; u = u->next)
		u->flags.marked = false;
}


bool prim_pool::in_scope_vert (ray_3d& cursor, D3DXVECTOR3& p_result)
{
	D3DXVECTOR2 p;
	D3DXVECTOR3 p3d;
	base_tex_vert* vert;
	ray_3d view = calc_view (cursor);

	float kx = float(tan(PI / 4.0) * (1440 / 2.0f) * 1.5);
	float ky = kx;
	float w = 64.0f * 1.0f;
	int n = 0;
	vert_buff_sect* s;
	bool v_result = false;
	bool u_result = false;
	user_vert_type* uv_result;
	float z = 10000.0;

	for (vert_buff_sect* sect = vert_start; sect != NULL; sect = sect->next) {
		sect->buff->Lock (0, 0, (void**)&vert, 0);
		
		for (int i = 0; i < sect->count; i++) {
			p3d = calc_rel_vect (vert[i].position, kx, ky, view);

			if (map_vect (vert[i].position, p, kx, ky, view))
				if (p.x > 1440 / 2.0 - w * .5)
					if (p.x < 1440 / 2.0  + w * .5)
						if (p.y > 900 / 2.0  - w * .5)
							if (p.y < 900 / 2.0  + w * .5) 
								if (p3d.z <= z) {
									z = p3d.z;
									s = sect;
									n = i;
									v_result = true;	
									p_result = vert[i].position;									
								}
		}

		sect->buff->Unlock();	
	}
	
	for (user_vert_type* u = _user_vert_start; u != NULL; u = u->next) {
		p3d = calc_rel_vect (u->pos, kx, ky, view);

		if (map_vect (u->pos, p, kx, ky, view))
			if (p.x > 1440 / 2.0 - w * .5)
				if (p.x < 1440 / 2.0  + w * .5)
					if (p.y > 900 / 2.0  - w * .5)
						if (p.y < 900 / 2.0  + w * .5) 
							if (p3d.z <= z) {
								z = p3d.z;
								uv_result = u;
								u_result = true;	
								p_result = u->pos;									
							}
	}

	if (u_result) {
		uv_result->flags.marked = true;
		return true;
	}
	else
		if (v_result) {
			//s->buff->Lock (0, 0, (void**)&vert, 0);
			s->flags[n].marked = true;
			//s->buff->Unlock();	
			return true;
		}	
		else
			return false;
}


void prim_pool::create_prim (tex_type* tex, int v1, int v2, int v3)
{
	index_buff_sect* sect = get_index_sect(3);
	long* ind = NULL;

	sect->buff->Lock(0, 0, (void**)&ind, 0);	
	ind[sect->count + 0] = v1;
	ind[sect->count + 1] = v2;
	ind[sect->count + 2] = v3;		
	sect->count += 3;
	sect->buff->Unlock();
	
	/*
	if (prim_start = NULL) {
		prim_start = new prim_type;
		prim_start->tex = tex;
		prim_start->next = NULL;
	}
	else {
		prim_type* last;
		prim_type* nu = new prim_type;

		for (last = prim_start; last->next != NULL; last = last->next)
			;
		
		last->next = nu;
		nu->tex = tex;
		nu->next = NULL;
	}
	*/
	prim_ct++;
	
}


void prim_pool::create_wall (float x, float y, float z, float w, float h, float d, double a, tex_type* tex)
{
	// 3    2
	// 0    1
	//char buff[80];
	//sprintf (buff, "x: %f, y: %f, z: %f", x, y, z);
	
	//dprint ("z1: ", z, 0);
	//dprint ("z2: ", z + d, 1);

	//msg (rad_to_deg(a), w);
	D3DXVECTOR3 p[5];

	p[0] = D3DXVECTOR3(x, y, z);
	p[1] = offset_3d(p[0], w, a);
	p[2] = offset_3d(p[1], d, a + PI / 2.0);
	p[3] = offset_3d(p[0], d, a + PI / 2.0);
	p[4] = offset_3d(p[0], 0.0, h, 0.0);

	poly_type* p1 = create_rect(tex, p[0], w, h, a, 0.0);	
	create_rect(tex, p[2], w, h, a + PI, 0.0);
	create_rect(tex, p[1], d, h, a + PI / 2.0, 0.0);
	create_rect(tex, p[3], d, h, a - PI / 2.0, 0.0);
	create_rect(tex, p[4], w, d, a, PI / 2.0);
	poly_type* p2 = create_rect(tex, p[0], w, d, a, -PI / 2.0);
	
	create_solid (tex);
}


void prim_pool::init_geo(tex_pool& tp)
{
	//for (float d = 0; d < 20.0f; d += 1.0)
	//vert_buff_sect* sect = get_vert_sect(4);

	//create_user_vert(D3DXVECTOR3(0.0f, 0.0f, 10.0f), sect, VECT_UNDEF, 0.0f, 0.0f);
	//create_user_vert(D3DXVECTOR3(0.0f, 10.0f, 10.0f), sect, VECT_UNDEF, 0.0f, 0.0f);
	//create_user_vert(D3DXVECTOR3(10.0f, 10.0f, 10.0f), sect, VECT_UNDEF, 0.0f, 0.0f);

	//float d = 10.0;
	//create_wall (0.0f, 0.0f, d * 0, 20.0f, 10.0f, 20.0f, 0.0 * PI / 4.0 + .00, tp);	
}
    

void transform (ray_3d ray)
{
    D3DXMATRIX mat_view, proj_mat, m;
	ray_3d cam = calc_view(ray);
		
	D3DXMatrixLookAtLH (&mat_view, &cam.pos, &cam.cv_vector(), &cam.up_vector()); 
	_d3d_dev.device->SetTransform (D3DTS_VIEW, &mat_view);

	D3DXMatrixPerspectiveFovLH (&proj_mat, D3DX_PI/4.0, 1440.0f/900.0f, 0.5f, 1000.0f);
    _d3d_dev.device->SetTransform (D3DTS_PROJECTION, &proj_mat);	
}


void draw_cursor(tex_pool& tp) {
	_d3d_dev.spr_2d->Begin(0);
	
	D3DXVECTOR3 p(1440 / 2 - 32, 900 / 2 - 32, 0.0f);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;
	
	set_sprite_scale (1.0);
	_d3d_dev.spr_2d->Draw(_build_env.cursor->tex, &r, NULL, &p, 0x88ff0000);
	
	_d3d_dev.spr_2d->End();
}


void prim_pool::show_stats ()
{
	int v_ct = 0;
	int i_ct = 0;

	for (vert_buff_sect* v = vert_start; v != NULL; v = v->next) {
		msg(v_ct, v->count);
		v_ct++;
	}

	for (index_buff_sect* i = index_start; i != NULL; i = i->next) {
		msg (-i_ct - 1);
		for (int n = 0; n < i->count; n++) {		
			long* ind = NULL;
			i->buff->Lock(0, 0, (void**)&ind, 0);	
			msg (ind[n + 0]);
			i->buff->Unlock();
		}
	
		//msg(i_ct, i->count);
		i_ct++;
	}

	msg("sect counts");
	msg(v_ct, i_ct);
}


void create_test_point (point_3d p, int n)
{
	_test_point[n].pos = p;
	_test_point[n].color = 0xffff0000;
	_test_point[n].used = true;
}


void delete_test_point (int n)
{
	_test_point[n].used = false;
}


void draw_test_points (tex_pool& tp, ray_3d& view)
{
	D3DXVECTOR3 p, rel, rot;

	float kx = float(tan(PI / 4.0) * (1440 / 2.0f) * 1.5);
	float ky = kx;
	
	_d3d_dev.spr_2d->Begin(0);

	for (int i = 0; i < 100; i++) {
		if (_test_point[i].used) {
			rel = calc_rel_vect (_test_point[i].pos, kx, ky, view);		

			if (rel.z > 0.5f) {
				float scale = float((64.0 / rel.z) * 0.25);
				int w = int(64.0f * scale);	
		
				p.x = ((1440 / 2) + kx * rel.x / rel.z - w / 2) / scale;
				p.y = ((900 / 2) - ky * rel.y / rel.z - w / 2) / scale;		
				p.z = 0.0f;		
		
				set_sprite_scale (scale);
				_d3d_dev.spr_2d->Draw(_build_env.vert->tex, NULL, NULL, &p, _test_point[i].color);	
			}
		}
	}

	_d3d_dev.spr_2d->End();	
}


void prim_pool::render (tex_pool& tp, ray_3d& cam)
{
	//int i;
    _d3d_dev.device->Clear (0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

    _d3d_dev.device->BeginScene();    
    transform (cam);
	
	set_dir_light(0, 1.0f, 0.99f, 0.99f, 0.0005f, 0.4f, 1.0 * PI / 6.0, 1.0 * PI / 6.0);
	set_dir_light(1, 1.0f, 0.99f, 0.99f, 0.2f, 0.4f, PI + 1.0 * PI / 6.0, PI + 1.0 * PI / 6.0);

//	set_dir_light(1, 0.999f, 0.9999f, 1.0f, 0.0051f, 0.9407f, cam.axz, cam.ayz);

	//setup_lights();
	_d3d_dev.device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    _d3d_dev.device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    _d3d_dev.device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    vert_buff_sect* v_sect = vert_start;
	index_buff_sect* i_sect = index_start;
	int v_ct = 0, i_ct = 0;

	verif(
		_d3d_dev.device->SetFVF (D3DFVF_CUSTOMVERTEX),
		"Set vertex declaration"
	);	

	if (i_sect != NULL)
		update_index_buffer (i_sect);
	if (i_sect != NULL)
		update_vert_buffer (v_sect);

	int ct = 0;
	for (poly_type* poly = poly_start; poly != NULL; poly = poly->next) {
		draw_poly (poly, v_ct, i_ct, v_sect, i_sect);
		ct++;
	}

	//dprint ("prim_ct ", prim_ct, 4);
	//_d3d_dev.device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 3, 9, 1);
	//_d3d_dev.device->DrawPrimitive (D3DPT_TRIANGLESTRIP, 0, 4);
	//_d3d_dev.device->DrawPrimitive (D3DPT_TRIANGLESTRIP, 4, 2);
///*
//	write();
	_d3d_dev.spr_3d->Begin(D3DXSPRITE_BILLBOARD);
	
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 64;
	r.bottom = 64;

	D3DXVECTOR3 c(r.right * 0.5f,r.right * 0.5f,10);
	D3DXVECTOR3 p(0,0,10);
	D3DCOLOR color(0xffffffff);

	//	_d3d_dev.spr_3d->Draw(tp.tex_start->next->tex, &r, &c, &p, color);
	
	//draw_test_point (offset_3d (D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1.0f, cam.axz, cam.ayz, 0.0f), tp, cam);
	//dprint ("
	
	//ray_3d r3d = calc_view (cam);
	//line_3d l3d = r3d.cv_line_3d();
	//dprint ("mx ", l3d.mx, 0);
	//dprint ("my ", l3d.my, 1);
	
	_d3d_dev.spr_3d->End();

	draw_verts(tp, calc_view(cam));
	ray_3d cs = calc_view (cam);

	point_3d v_int_t;
	int prim_n = ray_int_prim (cs, v_int_t);

	if (prim_n != -1)	
		create_test_point (v_int_t, 0);
	else
		delete_test_point (0);
	
	//dprint ("n: ", prim_n, 2);
	//dprint ("y: ", v_int_t.y, 1);
	//dprint ("z: ", v_int_t.z, 2);
	
	draw_test_points (tp, calc_view(cam));
	draw_cursor(tp);
	    	
	show_text ();
	_d3d_dev.device->EndScene();    
	
	_d3d_dev.device->Present (NULL, NULL, NULL, NULL);
//	*/
}


float simp_tex_coord (float t1, float t2)
{
	// Returns simplified t1 with respect to t2
	
	if (t1 < t2) // 1.....2
		//if (t1 >= 0)
			return frac(t1);
		//else
		//	return t1 - (float)floor(t1);
	else		 // 2.....1
		return frac(t2) + (t1 - t2);
}


void simp_tex_coord (int ct, double* in_u, double* in_v, float* out_u, float* out_v)
{
	int min_u, min_v;
	int i;

	min_u = min_v = 0;

	for (i = 0; i < ct; i++) {
		if (in_u[i] < in_u[min_u])
			min_u = i;
		if (in_v[i] < in_v[min_v])
			min_v = i;
	}

	for (i = 0; i < ct; i++) {
		if (i == min_u)
			out_u[i] = (float)frac(in_u[min_u]);
		else
			out_u[i] = float(frac(in_u[min_u]) + (in_u[i] - in_u[min_u]));

		if (i == min_v)
			out_v[i] = (float)frac(in_v[min_v]);
		else
			out_v[i] = float(frac(in_v[min_v]) + (in_v[i] - in_v[min_v]));
	}
}


double calc_tex_coord_xz (point_3d p, plane_3d plane, bool side)
{
	if (plane.mx == PLANE_XY_PLANE)
		if (side)
			return -p.z;
		else
			return p.z;
	else {
		point_3d p0;
		double m = plane.mx;
		double b = p.z - m * p.x;

		p0.x = float((-b * m) / (1.0 + m * m));
		p0.z = float(m * p0.x + b);

		if ((p0.x < p.x) == side)
			return dist_xz (p, p0);
		else
			return -dist_xz (p, p0);
	}
}


double calc_tex_coord (D3DXVECTOR3 p, double angle)
{
	double a = simp_angle (angle);

	if (approx_equal (a, PI / 4.0))
		return p.z / (float)sin(a);
	else if (approx_equal (a, PI / 2.0 + PI / 4.0))
		return p.x / (float)cos(a);
	else if (approx_equal (a, -PI / 4.0))
		return p.x / (float)cos(a);
	else if (approx_equal (a, -(PI / 2.0 + PI / 4.0)))
		return p.z / (float)sin(a);
	else if (a < PI / 4.0)
		if (a > -PI / 4.0)
			return p.x / (float)cos(a);
		else if (a > PI / 4.0 - PI)
			return p.z / (float)sin(a);
		else
			return p.x / (float)cos(a);
	else
		if (a < -PI / 4.0 + PI)
			return p.z / (float)sin(a);
		else
			return p.x / (float)cos(a);
}


double calc_tex_coord (point_3d p, double x, double y)
{
	double a = atan(tan(y) * sin(x));

	if (approx_zero (cos(y)))
		return -p.y;
	else
		return float(p.y / sin(a));
}


void set_dir_light(int num, float r, float g, float b, float amb, float dir, double axz, double ayz)
{
	// Set a direction light	
	D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    _d3d_dev.device->SetMaterial(&mtrl);

	D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
	
	light.Type = D3DLIGHT_DIRECTIONAL;
    light.Direction = cv_normal(axz, ayz);	
	light.Diffuse.r = r * dir;
    light.Diffuse.g = g * dir;
    light.Diffuse.b = b * dir;
	light.Ambient.r = r * amb;
	light.Ambient.g = g * amb;
	light.Ambient.b = b * amb;
	//light.Specular.r = 1.0f;
	//light.Specular.g = 1.0f;
	//light.Specular.b = 1.0f;

    _d3d_dev.device->SetLight(num, &light);
    _d3d_dev.device->LightEnable(num, TRUE);
    _d3d_dev.device->SetRenderState(D3DRS_LIGHTING, TRUE);
}


void set_sprite_scale (float scale) 
{
	D3DXMATRIX m = scale_mat(scale);
	_d3d_dev.spr_2d->SetTransform(&m);
}


void setup_lights()
{
  /*
	d3dLight.Type = D3DLIGHT_POINT;
d3dLight.Diffuse.r  = 1.0f;
d3dLight.Diffuse.g  = 1.0f;
d3dLight.Diffuse.b  = 1.0f;
d3dLight.Ambient.r  = 1.0f;
d3dLight.Ambient.g  = 1.0f;
d3dLight.Ambient.b  = 1.0f;
d3dLight.Specular.r = 1.0f;
d3dLight.Specular.g = 1.0f;
d3dLight.Specular.b = 1.0f;
    
// Position it high in the scene and behind the user.
// Remember, these coordinates are in world space, so
// the user could be anywhere in world space, too. 
// For the purposes of this example, assume the user
// is at the origin of world space.
d3dLight.Position.x = 0.0f;
d3dLight.Position.y = 1000.0f;
d3dLight.Position.z = -100.0f;
    
// Don't attenuate.
d3dLight.Attenuation0 = 1.0f; 
d3dLight.Range        = 1000.0f;
    
// Set the property information for the first light.
hr = d3dDevice->SetLight(0, &d3dLight);
*/
	
	/*
	// Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );
	*/

    // Set up a white, directional light, with an oscillating direction.
    // Note that many lights may be active at a time (but each one slows down
    // the rendering of our scene). However, here we are just using one. Also,
    // we need to set the D3DRS_LIGHTING renderstate to enable lighting
    // Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
    
	D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = 1.0f; mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = 1.0f; mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = 1.0f; mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = 1.0f; mtrl.Ambient.a = 1.0f;
    _d3d_dev.device->SetMaterial( &mtrl );
	
	D3DXVECTOR3 vecDir;

	vecDir = D3DXVECTOR3(0.0f,
                         0.0f,
                         1.0f);

    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );    
	
	light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Direction.x = 1.0f;
	light.Direction.y = 1.0f;
	light.Direction.z = 1.0f;
	light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Ambient.r  = 0.5f;
	light.Ambient.g  = 0.5f;
	light.Ambient.b  = 0.5f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;

    light.Position.x = -10000.0f;
	light.Position.y = 0.0f;
	light.Position.z = 0.0f;

	light.Attenuation0 = 0.8f;
	light.Attenuation1 = 0.5f;
	light.Attenuation2 = 0.5f;

	light.Range = 10000.0f;
//	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);

	//vecDir = D3DXVECTOR3(cosf(my_get_time()/350.0f),
    //                     1.0f,
     //                    sinf(my_get_time()/350.0f) );
    //D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    //light.Range       = 1000.0f;
    _d3d_dev.device->SetLight( 0, &light );
    _d3d_dev.device->LightEnable( 0, TRUE );
    //g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
    //g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	/*
	light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    vecDir = D3DXVECTOR3(cosf(my_get_time()/350.0f),
                         1.0f,
                         sinf(my_get_time()/350.0f) );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    */
	_d3d_dev.device->SetRenderState( D3DRS_LIGHTING, TRUE );
	
    // Finally, turn on some ambient light.
    //_d3d_dev.device->SetRenderState( D3DRS_AMBIENT, 0x00808080);
}


//prim_pool::resize_verts (int 

void calc_view_angles (double& axz, double& ayz)
{
	double txz = axz;
	double tyz = ayz;

	if (_build_env.writable.snap) {
		axz = round(txz, PI / 12.0);
		ayz = round(tyz, PI / 12.0);
	}
	else {
		axz = txz;
		ayz = tyz;
	}
}


ray_3d calc_view (ray_3d cursor)
{
	ray_3d result = cursor;
	double axz = cursor.axz, ayz = cursor.ayz;
	
	calc_view_angles (axz, ayz);
	result.axz = axz;
	result.ayz = ayz;
	result.pos = offset_3d(cursor.pos, -10.0, -axz, ayz, 0.0);
	
	return result;
}


void update_index_buffer (int& i_ct, index_buff_sect*& i_sect)
{
	if (i_ct >= i_sect->count) {
		i_ct = 0;
		i_sect = i_sect->next;
			
		verif(
			_d3d_dev.device->SetIndices(i_sect->buff),
			"Set index buffer"
		);
	}
}


void update_vert_buffer (int& v_ct, vert_buff_sect*& v_sect)
{
	if (v_ct >= v_sect->count) {
		v_ct = 0;
		v_sect = v_sect->next;
			
		verif (
			_d3d_dev.device->SetStreamSource (0, v_sect->buff, 0, sizeof(base_tex_vert)),
			"Set vertex buff"
		);
	}
}


void update_index_buffer (index_buff_sect*& i_sect)
{
	verif(
		_d3d_dev.device->SetIndices(i_sect->buff),
		"Set index buffer"
	);
}


void update_vert_buffer (vert_buff_sect*& v_sect)
{
	verif (
		_d3d_dev.device->SetStreamSource (0, v_sect->buff, 0, sizeof(base_tex_vert)),
		"Set vertex buff"
	);
}


void prim_pool::draw_prim (int& v_ct, int& i_ct, vert_buff_sect*& v_sect, index_buff_sect*& i_sect)
{
	verif (
		_d3d_dev.device->DrawIndexedPrimitive (D3DPT_TRIANGLELIST, 0, 0, v_sect->count, i_ct, 1),
		"quad: Draw prim"	
	);		

	i_ct += 3;
}


void prim_pool::draw_poly (poly_type* poly, int& v_ct, int& i_ct, vert_buff_sect*& v_sect, index_buff_sect*& i_sect)
{
	static tex_type* last_tex = NULL;

	if (last_tex != poly->tex) {
		_d3d_dev.device->SetTexture (0, poly->tex->tex);
		last_tex = poly->tex;
	}
	
	_d3d_dev.device->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	_d3d_dev.device->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	_d3d_dev.device->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		
	for (int i = 0; i < poly->verts - 2; i++) {		
		update_index_buffer (i_ct, i_sect);		
		update_vert_buffer (v_ct, v_sect);
		draw_prim (v_ct, i_ct, v_sect, i_sect);
	}

	//update_index_buffer (i_ct, i_sect);
	//draw_prim (v_ct, i_ct, v_sect, i_sect);
	
	v_ct += poly->verts;
}


int prim_pool::get_sel_verts (D3DXVECTOR3* result)
{
	int n = 0;

	for (user_vert_type* i = _user_vert_start; i != NULL; i = i->next)
		if (i->flags.selected)
			if (i->flags.visible) 
				if (!i->flags.deleted) {
					result[n] = i->pos;
					n++;
				}

	base_tex_vert* vert;
	
	for (vert_buff_sect* v = vert_start; v != NULL; v = v->next) {
		v->buff->Lock (0, 0, (void**)&vert, 0);
		for (int j = 0; j < v->count; j++)
			if (v->flags[j].selected) {
				result[n] = vert[j].position;				
				n++;
			}
		v->buff->Unlock ();
	}

	return n;
}


void prim_pool::select_marked ()
{
	for (vert_buff_sect* v = vert_start; v != NULL; v = v->next)
		for (int i = 0; i < v->count; i++)
			if (v->flags[i].marked)
				v->flags[i].selected = !v->flags[i].selected;

	for (user_vert_type* u = _user_vert_start; u != NULL; u = u->next)
		if (u->flags.marked)
			u->flags.selected = !u->flags.selected;
}


void show_text ()
{
	RECT r;
    DWORD color = 0xffffffff;

    for (int i = 0; i < 40; i++) {
		SetRect (&r, 0, i * 25, 0, 0);
		_d3d_dev.font->DrawText(NULL, _text_buff[i], -1, &r, DT_NOCLIP, color);
	}
}


void prim_pool::get_tri (
	vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0,
	point_3d& p1, point_3d& p2, point_3d& p3)
{
	plane_3d plane;

	base_tex_vert* vert;
	long* ind;

	v_sect->buff->Lock (0, 0, (void**)&vert, 0);
	i_sect->buff->Lock (0, 0, (void**)&ind, 0);	
	
	p1 = vert[ind[index0 + 0]].position;
	p2 = vert[ind[index0 + 1]].position;
	p3 = vert[ind[index0 + 2]].position;
		
	v_sect->buff->Unlock ();
	i_sect->buff->Unlock ();	
}


void prim_pool::get_tri (
	vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0,
	base_tex_vert& p1, base_tex_vert& p2, base_tex_vert& p3)
{
	plane_3d plane;

	base_tex_vert* vert;
	long* ind;

	v_sect->buff->Lock (0, 0, (void**)&vert, 0);
	i_sect->buff->Lock (0, 0, (void**)&ind, 0);	
	
	p1 = vert[ind[index0 + 0]];
	p2 = vert[ind[index0 + 1]];
	p3 = vert[ind[index0 + 2]];
		
	v_sect->buff->Unlock ();
	i_sect->buff->Unlock ();	
}


void prim_pool::get_poly (vert_buff_sect* v_sect, int vert0, base_tex_vert* vert, int vert_ct)
{
	base_tex_vert* v;
	
	v_sect->buff->Lock (0, 0, (void**)&v, 0);
	
	for (int i = 0; i < vert_ct; i++)
		vert[i] = v[vert0 + i];
			
	v_sect->buff->Unlock ();
}


void prim_pool::put_poly (vert_buff_sect* v_sect, int vert0, base_tex_vert* vert, int vert_ct)
{
	base_tex_vert* v;
	
	v_sect->buff->Lock (0, 0, (void**)&v, 0);
	
	for (int i = 0; i < vert_ct; i++)
		v[vert0 + i] = vert[i];
			
	v_sect->buff->Unlock ();
}


void prim_pool::put_tri (
	vert_buff_sect* v_sect, index_buff_sect* i_sect, int index0,
	base_tex_vert p1, base_tex_vert p2, base_tex_vert p3)
{
	plane_3d plane;

	base_tex_vert* vert;
	long* ind;

	v_sect->buff->Lock (0, 0, (void**)&vert, 0);
	i_sect->buff->Lock (0, 0, (void**)&ind, 0);	
	
	vert[ind[index0 + 0]] = p1;
	vert[ind[index0 + 1]] = p2;
	vert[ind[index0 + 2]] = p3;
		
	v_sect->buff->Unlock ();
	i_sect->buff->Unlock ();	
}


poly_type* prim_pool::create_poly (tex_type* tex, solid_type* sol, point_3d* p_list, int p_count, point_3d ref, bool side)
{
	vert_buff_sect* sect = get_vert_sect (p_count);
	int ind[512];
	point_3d n;
	float tu[512], tv[512];
	double ru[512], rv[512];
	int i;

	plane_3d plane = calc_plane_3d (p_list[0], p_list[1], p_list[2]);
	bool ref_rel = (side == (point_rel_plane (ref, plane) == -1));
	double x = atan(plane.mx);
	double y = 1.0 * PI / 2.0 + atan(plane.my);
	n = calc_normal (plane, ref_rel);
	
	if (plane.mx == PLANE_Y_PLANE) 
		for (i = 0; i < p_count; i++) {
			ru[i] = p_list[i].x / 10.0;
			if (ref_rel)
				rv[i] = p_list[i].z / 10.0;
			else
				rv[i] = -p_list[i].z / 10.0;
		}
	else
		for (i = 0; i < p_count; i++) {
			ru[i] = calc_tex_coord_xz (p_list[i], plane, ref_rel) / 10.0;
			rv[i] = calc_tex_coord (p_list[i], x, y) / 10.0;			
		}

	simp_tex_coord (p_count, ru, rv, tu, tv);
		
	for (i = 0; i < p_count; i++) 
		ind[i] = create_vert (p_list[i], sect, n, tu[i], tv[i]);			

	for (int v = 1; v < p_count - 1; v++)
		if (side == visible_side (ref, p_list[0], p_list[v], p_list[v + 1]))
			create_prim (NULL, ind[0], ind[v + 1], ind[v]);	
		else
			create_prim (NULL, ind[0], ind[v], ind[v + 1]);
	
	return log_poly (tex, sol, p_count);
}


void prim_pool::clear ()
{	
	// release solids
	solid_type* solid = solid_start;
	
	while (solid != NULL) {
		solid_type* next_solid = solid->next;
		delete solid;
		solid = next_solid;
	}

	solid_start = NULL;

	// release polygons
	poly_type* poly = poly_start;

	while (poly != NULL) {
		poly_type* next_poly = poly->next;
		delete poly;
		poly = next_poly;
	}

	poly_start = NULL;

	// release user verts
	user_vert_type* uv = _user_vert_start;

	while (uv != NULL) {
		user_vert_type* next_uv = uv->next;
		delete uv;
		uv = next_uv;
	}

	_user_vert_start = NULL;
	
	// release vertex and index buffers
	vert_buff_sect* v = vert_start;		
	index_buff_sect* i = index_start;
	vert_buff_sect* nv;
	index_buff_sect* ni;

	while (v != NULL) {
		delete v->flags;
		v->buff->Release();		
		nv = v->next;
		delete v;
		v = nv;
	}
	
	while (i != NULL) {
		i->buff->Release();
		ni = i->next;
		delete i;
		i = ni;
	}

	vert_start = NULL;
	index_start = NULL;
	prim_ct = 0;
}


int prim_pool::ray_int_prim (ray_3d& ray, point_3d& result)
{
	line_3d view_line = ray.cv_line_3d ();
	point_3d infront = offset_3d (ray.pos, 0.50f, -ray.axz, ray.ayz, 0.0);
	vert_buff_sect* v = vert_start;
	index_buff_sect* i = index_start;
	int vn = 0, in = 0;
	bool hit = false;
	bool side;
	int prim_n = -1;
	int n = 0;

	for (poly_type* poly = poly_start; poly != NULL; poly = poly->next) {
		for (int t = 0; t < poly->verts - 2; t++) {
			plane_3d plane;
			point_3d p3d;
			point_3d p1, p2, p3;
		
			get_tri (v, i, in, p1, p2, p3);
			plane = calc_plane_3d (p1, p2, p3);
			side = (point_rel_plane (ray.pos, plane) == PLANE_GREATER);
			
			//dprint ("mx ", plane.mx, 6);
			//dprint ("my ", plane.mx, 7);
			//dprint ("b ", plane.b, 8);

			if (line_3d_int_plane (view_line, plane, p3d)) 
				if (dist_3d (p3d, ray.pos) > dist_3d (p3d, infront)) 			
					if (point_in_tri (p3d, side, plane, p1, p2, p3))
						if (hit) 
							if (dist_3d (p3d, ray.pos) < dist_3d (result, ray.pos)) {
								result = p3d;
								prim_n = n;
							}
							else
								;
						else {
							result = p3d;
							prim_n = n;
							hit = true;
						}				

			if (in == i->count) {
				in = 0;
				i = i->next;
			}
			in += 3;					
			
			n++;
		}
		
		if (vn == v->count) {
			vn = 0;
			v = v->next;
		}
		vn += poly->verts;
	}

	return prim_n;	
}


void create_blank_world (ray_3d& cursor, prim_pool& pp, tex_pool& tp)
{
	pp.clear ();
	tp.clear ();
	cursor.pos = point_3d(0.0, 0.0, 0.0);
	cursor.axz = 0.0;
	cursor.ayz = 0.0;

	_build_env.curr_tex = NULL;
	for (int i = 0; i < 10; i++)
		_build_env.tex_pal[i] = NULL;
}


void save_world (char* filename, ray_3d& cursor, prim_pool& pp, tex_pool& tp)
{
	// write:
	// cursor
	// build environment
	// polygons
	// vertex buffer
	// user verts

	FILE* fp = fopen (filename, "wb");
	user_vert_type* uv;
	int ct, n;

	fwrite (&cursor, sizeof(ray_3d), 1, fp);
	fwrite (&_build_env.writable, sizeof(_build_env.writable), 1, fp);

	tp.save(fp);
	tp.update_nums();
	pp.number_solids ();

	if (_build_env.curr_tex == NULL)
		n = -1;
	else
		n = _build_env.curr_tex->n;	
	fwrite (&n, sizeof(int), 1, fp);

	for (int i = 0; i < 10; i++) {
		if (_build_env.tex_pal[i] == NULL)
			n = -1;
		else
			n = _build_env.tex_pal[i]->n;
		fwrite (&n, sizeof(int), 1, fp);
	}

	pp.save_polys (fp);
	pp.save_vert_buffers (fp);

	ct = 0;
	for (uv = _user_vert_start; uv != NULL; uv = uv->next)
		ct++;

	fwrite (&ct, sizeof(int), 1, fp);
	for (uv = _user_vert_start; uv != NULL; uv = uv->next)
		fwrite (uv, sizeof(user_vert_type), 1, fp);

	fclose(fp);
}


void load_world (char* filename, ray_3d& cursor, prim_pool& pp, tex_pool& tp)
{
	create_blank_world (cursor, pp, tp);

	FILE* fp = fopen (filename, "rb");
	int ct, i, n;

	fread (&cursor, sizeof(ray_3d), 1, fp);
	fread (&_build_env.writable, sizeof(_build_env.writable), 1, fp);

	tp.load (fp);
	tp.update_nums ();

	tex_type** ref = tp.create_tex_ref ();
		
	fread (&n, sizeof(int), 1, fp);
	if (n == -1)
		_build_env.curr_tex = NULL;
	else
		_build_env.curr_tex = ref[n];

	for (i = 0; i < 10; i++) {
		fread (&n, sizeof(int), 1, fp);
		if (n == -1)
			_build_env.tex_pal[i] = NULL;
		else
			_build_env.tex_pal[i] = ref[n];
	}

	pp.load_polys (ref, fp);
	pp.load_vert_buffers (fp);

	fread (&ct, sizeof(int), 1, fp);
	for (i = 0; i < ct; i++) {
		user_vert_type uv; 
		fread (&uv, sizeof(user_vert_type), 1, fp);
		create_user_vert (uv);
	}
	
	delete ref;	
	fclose(fp);
}


void prim_pool::create_floor (tex_type* tex, point_3d* p, int ct)
{
	point_3d top[256];
	point_3d c[2];	
	point_3d center;
	int i;

	for (i = 0; i < ct; i++) {
		top[i] = p[i];
		top[i].y = p[i].y + 1.0f;
	}

	c[0] = calc_center (p, ct);
	c[1] = calc_center (top, ct);
	center = calc_center (c, 2);
	
	solid_type* sol = create_solid (tex);
	create_poly (tex, sol, p, ct, center, false);
	create_poly (tex, sol, top, ct, center, false);
	point_3d corner[4];

	for (i = 0; i < ct - 1; i++) {
		corner[0] = top[i];
		corner[1] = p[i];
		corner[2] = p[i + 1];
		corner[3] = top[i + 1];
		create_poly (tex, sol, corner, 4, center, false);
	}

	corner[0] = top[ct - 1];
	corner[1] = p[ct - 1];
	corner[2] = p[0];
	corner[3] = top[0];	
	create_poly (tex, sol, corner, 4, center, false);
}


void tex_pool::save (FILE* fp)
{
	tex_type* t;
	int ct = 0;

	for (t = tex_start; t != NULL; t = t->next)
		ct++;		
	fwrite (&ct, sizeof(int), 1, fp);

	for (t = tex_start; t != NULL; t = t->next)
		fwrite (t->name, 40, 1, fp);		
}


void tex_pool::load (FILE* fp)
{
	int ct;

	fread (&ct, sizeof(int), 1, fp);
	for (int i = 0; i < ct; i++) {
		char name[40];
		fread (name, 40, 1, fp);
		//msg (name);
		load_3d (name);
	}

	//update_nums ();
}


tex_type** tex_pool::create_tex_ref ()
{
	int ct = 0;
	tex_type* curr;

	for (curr = tex_start; curr != NULL; curr = curr->next)
		ct++;
	
	tex_type** result = new tex_type*[ct];
		
	int i = 0;
	for (curr = tex_start; curr != NULL; curr = curr->next) {
		result[i] = curr;
		i++;
	}

	return result;
}


void tex_pool::clear ()
{
	tex_type* curr = tex_start;

	while (curr != NULL) {
		tex_type* next = curr->next;
		curr->tex->Release();
		delete curr;
		curr = next;
	}

	tex_start = NULL;
}


solid_type* prim_pool::create_solid (tex_type* base_tex)
{
	solid_type* nu = new solid_type;

	if (solid_start == NULL)
		solid_start = nu;
	else {
		solid_type* g;

		for (g = solid_start; g->next != NULL; g = g->next)
			;
		
		g->next = nu;
	}

	nu->base_tex = base_tex;
	nu->next = NULL;

	return nu;
}


void prim_pool::number_solids ()
{
	int n = 0;

	for (solid_type* s = solid_start; s != NULL; s = s->next, n++) 
		s->n = n;
}


solid_type** prim_pool::create_sol_ref ()
{
	solid_type* s;

	int n = 0;
	for (s = solid_start; s != NULL; s = s->next) 
		n++;
	
	solid_type** ref = new solid_type*[n];
	
	s = solid_start;
	for (int i = 0; i < n; i++) {		
		ref[i] = s;
		s = s->next;
	}
	
	return ref;
}


poly_type* prim_pool::poly_of_prim (int prim_n)
{
	int n = 0;

	for (poly_type* p = poly_start; p != NULL; p = p->next) 
		for (int i = 0; i < p->verts - 2; i++, n++)
			if (n == prim_n)
				return p;

	return NULL;
}


void prim_pool::set_texture (poly_type* poly, tex_type* tex)
{
	int i;
	int v = 0;
	base_tex_vert p1, p2, p3;

	vert_buff_sect* vs = vert_start;
	index_buff_sect* is = index_start;

	for (poly_type* p = poly_start; p != NULL; p = p->next) {
		if (p == poly) {
			base_tex_vert vert[256];
			point_3d p3d[256];
			float tu[256], tv[256];

			get_poly (vs, v, vert, p->verts);
			for (i = 0; i < p->verts; i++)
				p3d[i] = vert[i].position;

			calc_tex_coords (p3d, p->verts, tex, tu, tv);
		
			for (i = 0; i < p->verts; i++) {
				vert[i].tu = tu[i];
				vert[i].tv = tv[i];
			}

			put_poly (vs, v, vert, p->verts);
		}
			
		if (vs->count == v) {
			vs = vs->next;
			v = 0;
		}

		v += p->verts;
	}

	poly->tex = tex;
}


point_3d calc_normal (point_3d* p, point_3d ref, bool side)
{
	plane_3d plane = calc_plane_3d (p[0], p[1], p[2]);
	bool ref_rel = (side == (point_rel_plane (ref, plane) == -1));
	return calc_normal (plane, ref_rel);
}


bool calc_facing_side (plane_3d plane, point_3d ref, bool side)
{
	return side == (point_rel_plane (ref, plane) == -1);
}


void calc_tex_coords (point_3d* p_list, int p_count, tex_type* tex, float* tu, float* tv)
{
	plane_3d plane = calc_plane_3d (p_list[0], p_list[1], p_list[2]);
	double x = atan(plane.mx);
	double y = PI / 2.0 + atan(plane.my);
	double ru[512], rv[512];
	int i;

	bool side = visible_side (p_list);

	if (plane.mx == PLANE_Y_PLANE) 
		for (i = 0; i < p_count; i++) {
			ru[i] = p_list[i].x / tex->dim;
			if (side)
				rv[i] = p_list[i].z / tex->dim;
			else
				rv[i] = -p_list[i].z / tex->dim;
		}
	else
		for (i = 0; i < p_count; i++) {
			ru[i] = calc_tex_coord_xz (p_list[i], plane, side) / tex->dim;
			rv[i] = calc_tex_coord (p_list[i], x, y) / tex->dim;			
		}

	simp_tex_coord (p_count, ru, rv, tu, tv);
}


/*
solid_type* prim_pool::solid_of_prim (int prim)
{
	int n = 0;

	for (solid_type* s = solid_start; s != NULL; s = s->next) {
		poly_type* p = s->start;

		for (int i = 0; i < s->poly_ct (); i++, p = p->next) {
			for (int t = 0; t < p->verts - 2; t++, n++) 
				if (n == prim)
					return s;
		}
	}

	return NULL;
}


void set_texture (solid_type* sol, tex_type* tex)
{
	poly_type* p = sol->start;

	for (int i = 0; i < sol->poly_ct (); i++, p = p->next)
		p->tex = tex;
}


int prim_pool::solid_num (solid_type* sol)
{
	int n = 0;

	for (solid_type* s = solid_start; s != NULL; s = s->next, n++)
		if (s == sol)
			return n;

	return -1;
}
*/

#endif // !INC_DX90