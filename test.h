#ifndef INC_TEST
#define INC_TEST


#include <stdio.h>


void msg(const char* text)
{
	MessageBox(_main_win.handle, text, "Test Message", MB_OK);
}

void msg(const char* capt, const char* text)
{
	MessageBox(_main_win.handle, text, capt, MB_OK);
}


void msg(double n)
{
	char buff[100];
	sprintf(buff, "%f", n);
	MessageBox(_main_win.handle, buff, "", MB_OK);
}


void msg(double n1, double n2)
{
	char buff[100];
	sprintf(buff, "%f, %f", n1, n2);
	MessageBox(_main_win.handle, buff, "", MB_OK);
}


#endif //!INC_TEST
