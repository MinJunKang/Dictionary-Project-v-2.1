#include <Windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Timer Program");
HFONT m_font;

unsigned int Read_Num() {
	FILE *fp;
	fp = fopen("time_limit.txt", "rt");
	unsigned int time;

	if (fp) {
		fscanf(fp, "%d", &time);
		fclose(fp);
	}
	return time;
}

void Write_Num(unsigned int num) {
	FILE *fp;
	fp = fopen("time_limit.txt", "wt");

	if (fp) {
		fprintf(fp, "%d", num);
		fclose(fp);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	m_font = CreateFont(40, // nHeight 
		20, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		1, // nWeight 
		0, // bItalic 
		0, // bUnderline 
		0, // cStrikeOut 
		0, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		0,                              // nClipPrecision 
		DEFAULT_QUALITY,       // nQuality
		DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily 
		"±¼¸²"); // lpszFacename

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = TEXT("MESSAGE");
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClass(&WndClass);
	HDC hdc; PAINTSTRUCT ps;
	string message;
	hWnd = CreateWindow(lpszClass, lpszClass, WS_VISIBLE, 930, 520, 300, 150, nullptr, (HMENU)nullptr, hInstance, nullptr);
	
	hdc = BeginPaint(hWnd, &ps);
	SelectObject(hdc, m_font);

	clock_t start_time;
	clock_t diff_time;

	unsigned int hour, min, sec;
	//Write_Num(15);
	unsigned int limit = Read_Num();

	start_time = clock();

	MSG msg;
	ShowWindow(hWnd, nCmdShow);

	string hours, mins, secs;
	
	while (1) {
		diff_time = limit * 1000 - (clock() - start_time);

		hour = diff_time / 3600000;
		min = (diff_time / 60000) % 60;
		sec = (diff_time / 1000) % 60;

		hours = to_string(hour);
		mins = to_string(min);
		secs = to_string(sec);

		if (hour < 10)
			hours = "0" + hours;
		if (min < 10)
			mins = "0" + mins;
		if (sec < 10)
			secs = "0" + secs;

		message = hours + " : " + mins + " : " + secs;

		TextOut(hdc, 42, 35, message.c_str(), message.length());
		if (diff_time / 1000 <= 0)
			break;
		GetMessage(&msg, nullptr, 0, 0);
	}
	message = " Time Over ";
	TextOut(hdc, 42, 35, message.c_str(), message.length());
	Sleep(1000);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}