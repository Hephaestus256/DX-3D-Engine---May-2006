#include <\programs\win.h>


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	window w("a", "dude", 100, 100, 0, 0);

	while (!_check_win_msg.quit());
	
	return 0;
}