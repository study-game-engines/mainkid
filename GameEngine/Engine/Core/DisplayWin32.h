#pragma once
#include <windows.h>
#include <WinUser.h>
#include <iostream>



class EngineCore;

class DisplayWin32
{
public:
	DisplayWin32() {};
	DisplayWin32( LPCWSTR applicationName, HINSTANCE hInstance,
		const int& screenHeight = 800, const int& screenWidth = 800);

	~DisplayWin32();
	HWND GetHWND();
	float GetHeight();
	float GetWidth();

private:
	float clientHeight;
	float clientWidth;
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASSEX wc;

};

