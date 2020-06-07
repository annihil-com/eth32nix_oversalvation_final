// ETH32 - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#include "../eth32.h"

CCameraWindow::CCameraWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	cameraActive = false;
	type = -1;
}

void CCameraWindow::Display(void)
{
	if (!eth32.cg.snap || !Engine.forward || !eth32.cg.snap->ps.weapon || eth32.cg.snap->ps.weapon > eth32.cg.numWeapons) {
		cameraActive = false;
		type = -1;
		return;
	}

	int weapnum = eth32.cg.snap->ps.weapon;
	
	if(eth32.settings.cams[CAM_FOLLOW].display && !VectorCompare(Cam.gentityOrigin, vec3_origin) ) {
		type = CAM_FOLLOW;
		Cam.followCam(&eth32.settings.cams[CAM_FOLLOW]);
	} else if (IS_WEAPATTRIB(weapnum, WA_MORTAR) && eth32.settings.cams[CAM_MORTAR].display) {
		type = CAM_MORTAR;
		Cam.mortarCam(&eth32.settings.cams[CAM_MORTAR]);
		return;
	}

	cameraActive = true;
	
	if (Gui.InputActive()) {
		camInfo_t *cinfo = &eth32.settings.cams[type];
		// draw corners
		Draw.RawPicST(cinfo->x1 - 4, cinfo->y1 - 4, 16, 16, 0, 0, 1, 1, eth32.guiAssets.camCorner);
		Draw.RawPicST(cinfo->x2 - 12, cinfo->y1 - 4, 16, 16, 1, 0, 0, 1, eth32.guiAssets.camCorner);
		Draw.RawPicST(cinfo->x1 - 4, cinfo->y2 - 12, 16, 16, 0, 1, 1, 0, eth32.guiAssets.camCorner);
		Draw.RawPicST(cinfo->x2 - 12, cinfo->y2 - 12, 16, 16, 1, 1, 0, 0, eth32.guiAssets.camCorner);
	}
}

int CCameraWindow::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (!cameraActive) {
		if (*mhook == this) {
			*mhook = NULL;
			return 0;	// we had focus, but no longer camera... release mouse focus
		}
		return 1; // no camera active, let GUI search for a valid window
	}

	camInfo_t *cinfo = &eth32.settings.cams[type];

	if (INBOUNDS2(mx, my, cinfo->x1 - 4, cinfo->y1 - 4, cinfo->x2 + 4, cinfo->y2 + 4)) {
		if (mevent == WM_LBUTTONDOWN) {
			if (INBOUNDS3(mx, my, cinfo->x1 + 4, cinfo->y1 + 4, 8, 8))
				cornerActive = CORNER_TL;
			else if (INBOUNDS3(mx, my, cinfo->x2 - 4, cinfo->y1 + 4, 8, 8))
				cornerActive = CORNER_TR;
			else if (INBOUNDS3(mx, my, cinfo->x1 + 4, cinfo->y2 - 4, 8, 8))
				cornerActive = CORNER_BL;
			else if (INBOUNDS3(mx, my, cinfo->x2 - 4, cinfo->y2 - 4, 8, 8))
				cornerActive = CORNER_BR;
			else
				cornerActive = -1; // move whole window

			prevx = mx;
			prevy = my;
			*mhook = this;
		}
		return 0;
	}
	return 1;
}

int CCameraWindow::MouseWheel(int mx, int my, int wheel)
{
	if (!cameraActive)
		return 1;

	camInfo_t *cinfo = &eth32.settings.cams[type];

	if (INBOUNDS2(mx,my,cinfo->x1,cinfo->y1,cinfo->x2,cinfo->y2)) {
		cinfo->distance += -wheel * 100.f;
		
		if (cinfo->distance > 2000.f) 
			cinfo->distance = 2000.f;
		if (cinfo->distance < 0.f)
			cinfo->distance = 0.f;

		return 0;
	}
	return 1;
}
		
void CCameraWindow::MouseMove(int mx, int my)
{
	if (!cameraActive)
		return;

	camInfo_t *cinfo = &eth32.settings.cams[type];

	int deltax = mx - prevx;
	int deltay = my - prevy;

	prevx = mx;
	prevy = my;

	switch (cornerActive)
	{
	case CORNER_TL:
		cinfo->x1 += deltax;
		cinfo->y1 += deltay;
		
		if (cinfo->x1 < 4)
			cinfo->x1 = 4;
		else if ((cinfo->x2 - cinfo->x1) < CAM_MIN_SIZE)
			cinfo->x1 = cinfo->x2 - CAM_MIN_SIZE;

		if (cinfo->y1 < 4)
			cinfo->y1 = 4;
		else if ((cinfo->y2 - cinfo->y1) < CAM_MIN_SIZE)
			cinfo->y1 = cinfo->y2 - CAM_MIN_SIZE;
		break;
	case CORNER_TR:
		cinfo->x2 += deltax;
		cinfo->y1 += deltay;
		
		if (cinfo->x2 > (eth32.game.glconfig->vidWidth - 4))
			cinfo->x2 = eth32.game.glconfig->vidWidth - 4;
		else if ((cinfo->x2 - cinfo->x1) < CAM_MIN_SIZE)
			cinfo->x2 = cinfo->x1 + CAM_MIN_SIZE;

		if (cinfo->y1 < 4)
			cinfo->y1 = 4;
		else if ((cinfo->y2 - cinfo->y1) < CAM_MIN_SIZE)
			cinfo->y1 = cinfo->y2 - CAM_MIN_SIZE;
		break;
	case CORNER_BL:
		cinfo->x1 += deltax;
		cinfo->y2 += deltay;
		
		if (cinfo->x1 < 4)
			cinfo->x1 = 4;
		else if ((cinfo->x2 - cinfo->x1) < CAM_MIN_SIZE)
			cinfo->x1 = cinfo->x2 - CAM_MIN_SIZE;

		if (cinfo->y2 > (eth32.game.glconfig->vidHeight - 4))
			cinfo->y2 = eth32.game.glconfig->vidHeight - 4;
		else if ((cinfo->y2 - cinfo->y1) < CAM_MIN_SIZE)
			cinfo->y2 = cinfo->y1 + CAM_MIN_SIZE;
		break;
	case CORNER_BR:
		cinfo->x2 += deltax;
		cinfo->y2 += deltay;
		
		if (cinfo->x2 > (eth32.game.glconfig->vidWidth - 4))
			cinfo->x2 = eth32.game.glconfig->vidWidth - 4;
		else if ((cinfo->x2 - cinfo->x1) < CAM_MIN_SIZE)
			cinfo->x2 = cinfo->x1 + CAM_MIN_SIZE;

		if (cinfo->y2 > (eth32.game.glconfig->vidHeight - 4))
			cinfo->y2 = eth32.game.glconfig->vidHeight - 4;
		else if ((cinfo->y2 - cinfo->y1) < CAM_MIN_SIZE)
			cinfo->y2 = cinfo->y1 + CAM_MIN_SIZE;
		break;
	default:
		cinfo->x1 += deltax;
		cinfo->y1 += deltay;
		cinfo->x2 += deltax;
		cinfo->y2 += deltay;

		if (cinfo->x1 < 4) {
			int dx = 4 - cinfo->x1;
			cinfo->x1 += dx;
			cinfo->x2 += dx;
		}
		
		if (cinfo->y1 < 4) {
			int dy = 4 - cinfo->y1;
			cinfo->y1 += dy;
			cinfo->y2 += dy;
		}

		if (cinfo->x2 > (eth32.game.glconfig->vidWidth - 4)) {
			int dx = eth32.game.glconfig->vidWidth - 4 - cinfo->x2;
			cinfo->x1 += dx;
			cinfo->x2 += dx;
		}

		if (cinfo->y2 > (eth32.game.glconfig->vidHeight - 4)) {
			int dy = eth32.game.glconfig->vidHeight - 4 - cinfo->y2;
			cinfo->y1 += dy;
			cinfo->y2 += dy;
		}
		break;
	}
}

/*********************
* CCameraConfigWindow
**********************/

CCameraConfigWindow::CCameraConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
		
	AddControl(new CDropbox("Camera Type", 5, 5, ww-10, 23, 0, CAM_MAX-1, &eth32.settings.camType, camTypeText));

	cam = &eth32.settings.cams[eth32.settings.camType];

	show = new CCheckBox("Show", 5, 33, ww-10, 8, &cam->display);
	AddControl(show);
	fov = new CFloatSlider("FOV", 5, 51, ww-10, 20, 30, 150, &cam->fov);
	AddControl(fov);
	distance = new CFloatSlider("Distance", 5, 76, ww-10, 20, 10, 2000, &cam->distance);
	AddControl(distance);
	angle = new CFloatSlider("Angle", 5, 101, ww-10, 20, 0, 90, &cam->angle);
	AddControl(angle);
	
}

void CCameraConfigWindow::Display(void)
{
	UpdateOffsets();

	cam = &eth32.settings.cams[eth32.settings.camType];

	switch(eth32.settings.camType)
	{
		case CAM_MORTAR:
		{
			fov->Show(true);
			distance->Show(true);
			angle->Show(true);
			break;
		}
		case CAM_FOLLOW:
		{
			fov->Show(true);
			distance->Show(true);
			angle->Show(false);
			break;
		}
		
		
		
		
		default:
			break;
	}

	CWindow::Display();
}

void CCameraConfigWindow::UpdateOffsets(void)
{
	cam = &eth32.settings.cams[eth32.settings.camType];

	show->SetTarget(&cam->display);
	distance->SetTarget(&cam->distance);
	angle->SetTarget(&cam->angle);
	fov->SetTarget(&cam->fov);
}
