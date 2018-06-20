#include <\dx90\include\ddraw.h>
#include <stdio.h>

char* dd_err(HRESULT hr);
bool dd_failed_check(HRESULT hr, char *szMessage);
bool create_d_buff();

class window_type {
public:
	LPDIRECTDRAW        pDD;         // DirectDraw object
	LPDIRECTDRAWSURFACE pDDSPrimary; // DirectDraw primary surface
	LPDIRECTDRAWSURFACE pDDSBack;    // DirectDraw back surface
	LPDIRECTDRAWCLIPPER pClipper;    // Clipper for windowed mode
	HWND                hWnd;        // Handle of window
	bool                bFullScreen; // are we in fullscreen mode?
};

 
window_type _g;

char* dd_err(HRESULT hr)
{
	switch (hr)
	{
		case DDERR_ALREADYINITIALIZED:           return "DDERR_ALREADYINITIALIZED";
		case DDERR_CANNOTATTACHSURFACE:          return "DDERR_CANNOTATTACHSURFACE";
		case DDERR_CANNOTDETACHSURFACE:          return "DDERR_CANNOTDETACHSURFACE";
		case DDERR_CURRENTLYNOTAVAIL:            return "DDERR_CURRENTLYNOTAVAIL";
		case DDERR_EXCEPTION:                    return "DDERR_EXCEPTION";
		case DDERR_GENERIC:                      return "DDERR_GENERIC";
		case DDERR_HEIGHTALIGN:                  return "DDERR_HEIGHTALIGN";
		case DDERR_INCOMPATIBLEPRIMARY:          return "DDERR_INCOMPATIBLEPRIMARY";
		case DDERR_INVALIDCAPS:                  return "DDERR_INVALIDCAPS";
		case DDERR_INVALIDCLIPLIST:              return "DDERR_INVALIDCLIPLIST";
		case DDERR_INVALIDMODE:                  return "DDERR_INVALIDMODE";
		case DDERR_INVALIDOBJECT:                return "DDERR_INVALIDOBJECT";
		case DDERR_INVALIDPARAMS:                return "DDERR_INVALIDPARAMS";
		case DDERR_INVALIDPIXELFORMAT:           return "DDERR_INVALIDPIXELFORMAT";
		case DDERR_INVALIDRECT:                  return "DDERR_INVALIDRECT";
		case DDERR_LOCKEDSURFACES:               return "DDERR_LOCKEDSURFACES";
		case DDERR_NO3D:                         return "DDERR_NO3D";
		case DDERR_NOALPHAHW:                    return "DDERR_NOALPHAHW";
		case DDERR_NOCLIPLIST:                   return "DDERR_NOCLIPLIST";
		case DDERR_NOCOLORCONVHW:                return "DDERR_NOCOLORCONVHW";
		case DDERR_NOCOOPERATIVELEVELSET:        return "DDERR_NOCOOPERATIVELEVELSET";
		case DDERR_NOCOLORKEY:                   return "DDERR_NOCOLORKEY";
		case DDERR_NOCOLORKEYHW:                 return "DDERR_NOCOLORKEYHW";
		case DDERR_NODIRECTDRAWSUPPORT:          return "DDERR_NODIRECTDRAWSUPPORT";
		case DDERR_NOEXCLUSIVEMODE:              return "DDERR_NOEXCLUSIVEMODE";
		case DDERR_NOFLIPHW:                     return "DDERR_NOFLIPHW";
		case DDERR_NOGDI:                        return "DDERR_NOGDI";
		case DDERR_NOMIRRORHW:                   return "DDERR_NOMIRRORHW";
		case DDERR_NOTFOUND:                     return "DDERR_NOTFOUND";
		case DDERR_NOOVERLAYHW:                  return "DDERR_NOOVERLAYHW";
		case DDERR_NORASTEROPHW:                 return "DDERR_NORASTEROPHW";
		case DDERR_NOROTATIONHW:                 return "DDERR_NOROTATIONHW";
		case DDERR_NOSTRETCHHW:                  return "DDERR_NOSTRETCHHW";
		case DDERR_NOT4BITCOLOR:                 return "DDERR_NOT4BITCOLOR";
		case DDERR_NOT4BITCOLORINDEX:            return "DDERR_NOT4BITCOLORINDEX";
		case DDERR_NOT8BITCOLOR:                 return "DDERR_NOT8BITCOLOR";
		case DDERR_NOTEXTUREHW:                  return "DDERR_NOTEXTUREHW";
		case DDERR_NOVSYNCHW:                    return "DDERR_NOVSYNCHW";
		case DDERR_NOZBUFFERHW:                  return "DDERR_NOZBUFFERHW";
		case DDERR_NOZOVERLAYHW:                 return "DDERR_NOZOVERLAYHW";
		case DDERR_OUTOFCAPS:                    return "DDERR_OUTOFCAPS";
		case DDERR_OUTOFMEMORY:                  return "DDERR_OUTOFMEMORY";
		case DDERR_OUTOFVIDEOMEMORY:             return "DDERR_OUTOFVIDEOMEMORY";
		case DDERR_OVERLAYCANTCLIP:              return "DDERR_OVERLAYCANTCLIP";
		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		case DDERR_PALETTEBUSY:                  return "DDERR_PALETTEBUSY";
		case DDERR_COLORKEYNOTSET:               return "DDERR_COLORKEYNOTSET";
		case DDERR_SURFACEALREADYATTACHED:       return "DDERR_SURFACEALREADYATTACHED";
		case DDERR_SURFACEALREADYDEPENDENT:      return "DDERR_SURFACEALREADYDEPENDENT";
		case DDERR_SURFACEBUSY:                  return "DDERR_SURFACEBUSY";
		case DDERR_CANTLOCKSURFACE:              return "DDERR_CANTLOCKSURFACE";
		case DDERR_SURFACEISOBSCURED:            return "DDERR_SURFACEISOBSCURED";
		case DDERR_SURFACELOST:                  return "DDERR_SURFACELOST";
		case DDERR_SURFACENOTATTACHED:           return "DDERR_SURFACENOTATTACHED";
		case DDERR_TOOBIGHEIGHT:                 return "DDERR_TOOBIGHEIGHT";
		case DDERR_TOOBIGSIZE:                   return "DDERR_TOOBIGSIZE";
		case DDERR_TOOBIGWIDTH:                  return "DDERR_TOOBIGWIDTH";
		case DDERR_UNSUPPORTED:                  return "DDERR_UNSUPPORTED";
		case DDERR_UNSUPPORTEDFORMAT:            return "DDERR_UNSUPPORTEDFORMAT";
		case DDERR_UNSUPPORTEDMASK:              return "DDERR_UNSUPPORTEDMASK";
		case DDERR_VERTICALBLANKINPROGRESS:      return "DDERR_VERTICALBLANKINPROGRESS";
		case DDERR_WASSTILLDRAWING:              return "DDERR_WASSTILLDRAWING";
		case DDERR_XALIGN:                       return "DDERR_XALIGN";
		case DDERR_INVALIDDIRECTDRAWGUID:        return "DDERR_INVALIDDIRECTDRAWGUID";
		case DDERR_DIRECTDRAWALREADYCREATED:     return "DDERR_DIRECTDRAWALREADYCREATED";
		case DDERR_NODIRECTDRAWHW:               return "DDERR_NODIRECTDRAWHW";
		case DDERR_PRIMARYSURFACEALREADYEXISTS:  return "DDERR_PRIMARYSURFACEALREADYEXISTS";
		case DDERR_NOEMULATION:                  return "DDERR_NOEMULATION";
		case DDERR_REGIONTOOSMALL:               return "DDERR_REGIONTOOSMALL";
		case DDERR_CLIPPERISUSINGHWND:           return "DDERR_CLIPPERISUSINGHWND";
		case DDERR_NOCLIPPERATTACHED:            return "DDERR_NOCLIPPERATTACHED";
		case DDERR_NOHWND:                       return "DDERR_NOHWND";
		case DDERR_HWNDSUBCLASSED:               return "DDERR_HWNDSUBCLASSED";
		case DDERR_HWNDALREADYSET:               return "DDERR_HWNDALREADYSET";
		case DDERR_NOPALETTEATTACHED:            return "DDERR_NOPALETTEATTACHED";
		case DDERR_NOPALETTEHW:                  return "DDERR_NOPALETTEHW";
		case DDERR_BLTFASTCANTCLIP:              return "DDERR_BLTFASTCANTCLIP";
		case DDERR_NOBLTHW:                      return "DDERR_NOBLTHW";
		case DDERR_NODDROPSHW:                   return "DDERR_NODDROPSHW";
		case DDERR_OVERLAYNOTVISIBLE:            return "DDERR_OVERLAYNOTVISIBLE";
		case DDERR_NOOVERLAYDEST:                return "DDERR_NOOVERLAYDEST";
		case DDERR_INVALIDPOSITION:              return "DDERR_INVALIDPOSITION";
		case DDERR_NOTAOVERLAYSURFACE:           return "DDERR_NOTAOVERLAYSURFACE";
		case DDERR_EXCLUSIVEMODEALREADYSET:      return "DDERR_EXCLUSIVEMODEALREADYSET";
		case DDERR_NOTFLIPPABLE:                 return "DDERR_NOTFLIPPABLE";
		case DDERR_CANTDUPLICATE:                return "DDERR_CANTDUPLICATE";
		case DDERR_NOTLOCKED:                    return "DDERR_NOTLOCKED";
		case DDERR_CANTCREATEDC:                 return "DDERR_CANTCREATEDC";
		case DDERR_NODC:                         return "DDERR_NODC";
		case DDERR_WRONGMODE:                    return "DDERR_WRONGMODE";
		case DDERR_IMPLICITLYCREATED:            return "DDERR_IMPLICITLYCREATED";
		case DDERR_NOTPALETTIZED:                return "DDERR_NOTPALETTIZED";
		case DDERR_UNSUPPORTEDMODE:              return "DDERR_UNSUPPORTEDMODE";
		case DDERR_NOMIPMAPHW:                   return "DDERR_NOMIPMAPHW";
		case DDERR_INVALIDSURFACETYPE:           return "DDERR_INVALIDSURFACETYPE";
		case DDERR_DCALREADYCREATED:             return "DDERR_DCALREADYCREATED";
		case DDERR_CANTPAGELOCK:                 return "DDERR_CANTPAGELOCK";
		case DDERR_CANTPAGEUNLOCK:               return "DDERR_CANTPAGEUNLOCK";
		case DDERR_NOTPAGELOCKED:                return "DDERR_NOTPAGELOCKED";
		case DDERR_NOTINITIALIZED:               return "DDERR_NOTINITIALIZED";
	}
	return "Unknown Error";
}



bool dd_failed_check(HRESULT hr, char *szMessage)
{
	if (FAILED(hr))
	{
		char buf[1024];
		sprintf( buf, "%s (%s)\n", szMessage, dd_err(hr) );
		OutputDebugString( buf );
		return true;
	}
	return false;
}



bool dd_init( HWND hWnd )
{
	HRESULT hr;

	_g.hWnd = hWnd;

	// Initialize DirectDraw
	hr = DirectDrawCreate( NULL, &_g.pDD, NULL );
	if (dd_failed_check(hr, "DirectDrawCreate failed" ))
		return false;

	return true;
}



bool dd_create_surfaces (bool bFullScreen)
{
	HRESULT hr; // Holds return values for DirectX function calls

	_g.bFullScreen = bFullScreen;

	// If we want to be in full-screen mode
	if (_g.bFullScreen)
	{
		// Set the "cooperative level" so we can use full-screen mode
		hr = _g.pDD->SetCooperativeLevel(_g.hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_NOWINDOWCHANGES);
		if (dd_failed_check (hr, "SetCooperativeLevel"))
			return false;

		// Set 640x480x256 full-screen mode
		hr = _g.pDD->SetDisplayMode(640, 480, 8);
		if (dd_failed_check (hr, "SetDisplayMode" ))
			return false;
	}
	else
	{
		// Set DDSCL_NORMAL to use windowed mode
		hr = _g.pDD->SetCooperativeLevel(_g.hWnd, DDSCL_NORMAL);
		if (dd_failed_check (hr, "SetCooperativeLevel windowed" ))
			return false;
	}
	

	DDSURFACEDESC ddsd; // A structure to describe the surfaces we want
	// Clear all members of the structure to 0
	memset(&ddsd, 0, sizeof(ddsd));
	// The first parameter of the structure must contain the size of the structure
	ddsd.dwSize = sizeof(ddsd);

	if (_g.bFullScreen)
	{
		// Screw the full-screen mode (for now) (FIXME)
	}
	else
	{

		//-- Create the primary surface

		// The dwFlags paramater tell DirectDraw which DDSURFACEDESC
		// fields will contain valid values
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		hr = _g.pDD->CreateSurface(&ddsd, &_g.pDDSPrimary, NULL);
		if (dd_failed_check(hr, "Create primary surface"))
			return false;

		//-- Create the back buffer

		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		// Make our off-screen surface 320x240
		ddsd.dwWidth = 320;
		ddsd.dwHeight = 240;
		// Create an offscreen surface
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		hr = _g.pDD->CreateSurface(&ddsd, &_g.pDDSBack, NULL);
		if (dd_failed_check(hr, "Create back surface"))
			return false;

	}

	
	//-- Create a clipper for the primary surface in windowed mode
	if (!_g.bFullScreen) {

		// Create the clipper using the DirectDraw object
		hr = _g.pDD->CreateClipper(0, &_g.pClipper, NULL);
		if (dd_failed_check(hr, "Create clipper"))
			return false;

		// Assign your window's HWND to the clipper
		hr = _g.pClipper->SetHWnd(0, _g.hWnd);
		if (dd_failed_check(hr, "Assign hWnd to clipper"))
			return false;

		// Attach the clipper to the primary surface
		hr = _g.pDDSPrimary->SetClipper(_g.pClipper);
		if (dd_failed_check(hr, "Set clipper"))
			return false;
	}


	return true;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG  Msg;

	HINSTANCE g_hInstance = hInstance;
	HWND g_hwndMain;

//	if (!hPrevInstance) {
//		if (!Register( g_hInstance ))
//			return FALSE;
//	}

	// Create the main window
	//g_hwndMain = Create( nCmdShow, 320, 240 );
	//if (!g_hwndMain)
	//	return FALSE;

	// Initialize DirectDraw
	if (!dd_init( g_hwndMain ))
	{
		MessageBox( g_hwndMain, "Failed to initialize DirectDraw", "Error", MB_OK );
		return 0;
	}

	// Create DirectDraw surfaces
	if (!dd_create_surfaces( false ))
	{
		MessageBox( g_hwndMain, "Failed to create surfaces", "Error", MB_OK );
		return 0;
	}

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

