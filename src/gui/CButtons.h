// ETH32 - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#pragma once

#include "CControl.h"

class CButton : public CControl
{
public:
	CButton(const char *clabel, int cx, int cy, int cw, int ch, void (*cfunc)(void));
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 event, CControl **mhook, CControl **khook);
	void MouseMove(int mx, int my);
private:
	void (*func)(void);
};
