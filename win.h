#ifndef INC_WIN
#define INC_WIN

#include <windows.h>

LRESULT WINAPI window_class_msg_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Class for a Windows window
class window {  
public:
	HWND handle;	

	window(char* c_name, char* window_name, int x, int y, int w, int h)
	{
		// Register the window class
		WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, window_class_msg_proc, 0L, 0L,
						GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
						c_name, NULL};
		wclass = wc;
		class_name = c_name;
		RegisterClassEx(&wclass);
	
		// Create the window
		handle = CreateWindow(class_name, window_name, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, wclass.hInstance, NULL);	
	}

	~window()
	{
		UnregisterClass(class_name, wclass.hInstance);
	}

	void show() 
	{
		ShowWindow(handle, SW_SHOWDEFAULT);
		UpdateWindow(handle);
	}
            
private:
	bool full_screen;
	WNDCLASSEX wclass;
	char* class_name;
};


// Processes windows messages
class check_win_msg_class {
public:
	check_win_msg_class() {
		ZeroMemory(&msg, sizeof(msg));
	}
	bool quit() {
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
        }

		return msg.message == WM_QUIT;
	}
private:
	MSG msg;
} _check_win_msg;


//The window class message handler
LRESULT WINAPI window_class_msg_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	else if (msg == WM_SETCURSOR) {
		SetCursor(NULL);
		return 0;
	}

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


   
#endif // !INC_WIN

