// ETH32 - an Enemy Territory cheat for Linux
// Copyright (c) 2007 eth32 team
// www.nixcoders.org

#pragma once

#include "CWindow.h"

class CStatusWindow : public CWindow
{
public:
	CStatusWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	int statbarSize;
};
