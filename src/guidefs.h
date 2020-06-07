// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#pragma once

#include "CAimbot.h"

#define SETFLOAT(x)			((int)(x * 1000))		// only keeping up to 0.001
#define GETFLOAT(x)			(x / 1000.f)

// Control definition reference: (update this if you add new control types)

// Type					Arg0			Arg1			Arg2			Arg3			Arg4
// CTRL_CHECKBOX		bool *target	N/A				N/A				N/A				N/A
// CTRL_INTDISPLAY		int *target		N/A				N/A				N/A				N/A
// CTRL_INTSLIDER		int	min			int	max			int *target		N/A				N/A
// CTRL_FLOATSLIDER		float min		float max		float *target	N/A				N/A
// CTRL_COLORPICKER		BYTE *color		N/A				N/A				N/A				N/A

// NOTE: cast arg0-arg4 to int if used, use SETFLOAT/GETFLOAT for floats

static const windef_t windows[] =
{
	// MAINVIEW
	{
		"Status",			// title
		WIN_STATUS,			// type
		GUI_MAINVIEW,			// view
		5, 422, 120, 60,			// x, y, w, h
		0,				// num controls
	},
	{
		"Weapon",			// title
		WIN_WEAPON,			// type
		GUI_MAINVIEW,			// view
		95, 422, 120, 60,		// x, y, w, h
		0,				// num controls
	},
	{
		"Spectators",			// title
  		WIN_SPECTATOR,			// type
		GUI_MAINVIEW,			// view
		50, 193, 100, 160,		// x, y, w, h
		0,				// num controls
	},
	{
		"Respawn",		 	// title
		WIN_RESPAWN,			// type
		GUI_MAINVIEW,			// view
		280, 5, 38, 20,			// x, y, w, h
		0,				// num controls
	},
	{
		"Cameras",			// title
		WIN_CAMERA,			// type
		GUI_MAINVIEW,			// view
		5, 5, 5, 5,			// x, y, w, h
		0,				// num controls
	},
	{
		"Weapon Config",		// title
		WIN_WEAPCONFIG,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 225,		// x, y, w, h
		0,				// num controls
	},
	{
		"Hitbox",		// title
		WIN_HITBOX,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 225,		// x, y, w, h
		0,				// num controls
	},
	{
		"Camera Settings",
		WIN_CAMCONFIG,
		GUI_MENUVIEW,
		500, 55, 150, 150,
		0,
	},
	{
		"Banner",
		WIN_BANNER,			// type
		GUI_MAINVIEW,			// view
		20, 20, 1, 1,			// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					AIMBOT
	******************************************************************* **/
	{
		"Aimbot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 325,		// x, y, w, h
		14,				// num controls
		{
			// Type			Label					X		Y		W		H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Aimbot Mode", 		5, 		5,		240,	23,		0, AIMMODE_MAX-1, (int)&eth32.settings.aimMode, (int)aimModeText },
			{ CTRL_DROPBOX,		"Aim Type",			5,		34,		240,	23,		AIM_OFF, AIM_MAX-1, (int)&eth32.settings.aimType, (int)aimTypeText },
			{ CTRL_CHECKBOX,	"Autofire",			5,		63,		240,	8,		(int)&eth32.settings.autofire },
			{ CTRL_CHECKBOX,	"Validate Attack",	5,		76,		240,	8,		(int)&eth32.settings.atkValidate },
			{ CTRL_CHECKBOX,	"Target Lock",		5,		89,		240,	8,		(int)&eth32.settings.lockTarget },
			{ CTRL_FLOATSLIDER, 	"FOV",			5,		102,	240,	20,		SETFLOAT(0), SETFLOAT(360), (int)&eth32.settings.fov },
			{ CTRL_DROPBOX,		"Target Sort",		5,		127,	240,	23,		SORT_OFF, SORT_MAX-1, (int)&eth32.settings.aimSort, (int)sortTypeText },
			{ CTRL_DROPBOX,		"Aim Priority",		5,		159,	240,	23,		0, AP_MAX-1, (int)&eth32.settings.headbody, (int)priorityTypeText },
			{ CTRL_DROPBOX,		"Head Trace Style",	5,		185,	240,	23,		HEAD_CENTER, HEAD_MAX-1, (int)&eth32.settings.headTraceType, (int)headTraceTypeText },
			{ CTRL_DROPBOX,		"Body Trace Style",	5,		210,	240,	23,		BODY_CENTER, BODY_MAX-1, (int)&eth32.settings.bodyTraceType, (int)bodyTraceTypeText },
			{ CTRL_FLOATSLIDER,	"Dynamic Hitbox",   5,		235, 	240,	20,		SETFLOAT(0), SETFLOAT(3), (int)&eth32.settings.dynamicHitboxScale },
			{ CTRL_FLOATSLIDER, "Anim. Correction", 5,		260,	240,	20,		SETFLOAT(-10), SETFLOAT(10), (int)&eth32.settings.animCorrection },
			{ CTRL_CHECKBOX,	"Auto Crouch",		5,		285,	240,	8,		(int)&eth32.settings.autoCrouch },
			{ CTRL_CHECKBOX,	"Lock Sensitivity",	5,		298,	240,	8,		(int)&eth32.settings.lockSensitivity },
		}
	},
	{
		"Human Aim",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		325, 60, 200, 190,		// x, y, w, h
		6,				// num controls
		{
			// Type				Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_FLOATSLIDER,	"Humanaim Value",	5,	5,	190,	20,		SETFLOAT(0.0),	SETFLOAT(1.0),	(int)&eth32.settings.humanValue },
			{ CTRL_FLOATSLIDER,	"Divmax",			5,	30,	190,	20,		SETFLOAT(0),	SETFLOAT(10), 	(int)&eth32.settings.divMax },
			{ CTRL_FLOATSLIDER,	"Divmin",			5,	55,	190,	20,		SETFLOAT(0),	SETFLOAT(10), 	(int)&eth32.settings.divMin },
			{ CTRL_DROPBOX,		"Human Aim", 		5,	80,	190,	23,		0, 	HUMAN_AIM_MAX-1,	(int)&eth32.settings.humanAimType,(int)humanAimTypeText },
            { CTRL_CHECKBOX,	"Lock Mouse",		5,	110,190,	8,				(int)&eth32.settings.lockMouse },
			{ CTRL_DROPBOX,		"Aim Protect", 		5, 	125,190,	23,		0, PROTECT_MAX-1, (int)&eth32.settings.aimprotect, (int)aimprotectText },
		},
	},
	/** *******************************************************************
					AIMBOT EXTRA
	******************************************************************* **/
	{
		"Corrections",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 170,		// x, y, w, h
		6,				// num controls
		{
			{ CTRL_FLOATSLIDER,	"Head Hitbox Size",			5,	5, 		240,	20,		SETFLOAT(1), SETFLOAT(15), (int)&eth32.settings.headBoxSize },
			{ CTRL_FLOATSLIDER,	"Body Hitbox Size",	    	5,	30, 	240,	20,		SETFLOAT(1), SETFLOAT(40), (int)&eth32.settings.bodybox },
			{ CTRL_CHECKBOX,	"Enable Auto Corrections",	5,	55,		240,	8,										(int)&eth32.settings.autocorrections },
			{ CTRL_FLOATSLIDER,	"Auto Corrections ",		5,	80,		240,	20,		SETFLOAT(0), SETFLOAT(10000), 	(int)&eth32.settings.RangE },
			{ CTRL_DROPBOX,		"Hitbox Style",		        5,	105,	240,    23,		HITBOX_OFF, HITBOX_MAX-1, (int)&eth32.settings.hitboxType, (int)hitboxText },
			{ CTRL_FLOATSLIDER,	"Weapon Spread",			5,	130,	240,	20,		SETFLOAT(0), SETFLOAT(1000),	(int)&eth32.settings.spread },
		}
	},
	{
		"Predictions",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 300, 250, 140,		// x, y, w, h
		6,				// num controls
		{
			// Type			Label				X	Y	W	H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Preshoot",		       		5,	5,      240,	8,		(int)&eth32.settings.preShoot },
			{ CTRL_CHECKBOX,	"Preaim",		       		5,	18,     240,	8,		(int)&eth32.settings.preAim },
			{ CTRL_FLOATSLIDER,	"Preshoot Time",	        5,	31,		240,	20,		SETFLOAT(0), SETFLOAT(300), (int)&eth32.settings.preShootTime },
			{ CTRL_FLOATSLIDER,	"Preaim Time",		        5,	54, 	240,	20,		SETFLOAT(0), SETFLOAT(300), (int)&eth32.settings.preAimTime },
			{ CTRL_CHECKBOX,	"Apply Target Predict",		5,	79,		240,	8,										(int)&eth32.settings.predTarget },
			{ CTRL_FLOATSLIDER,	"Self Predict",				5,	92,		240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (int)&eth32.settings.predSelf },
		}
	},
	/** *******************************************************************
					VISUALS
	******************************************************************* **/
	{
		"Visuals",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 234,		// x, y, w, h
		12,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Original HUD",			5,		5,		140,		8,		(int)&eth32.settings.guiOriginal },
			{ CTRL_CHECKBOX,	"Draw Hackvisuals",		5,		18,		140,		8,		(int)&eth32.settings.drawHackVisuals },
			{ CTRL_CHECKBOX,	"Wallhack",				5,		31,		140,		8,		(int)&eth32.settings.wallhack },
			{ CTRL_CHECKBOX,	"Draw Scope Blackout",	5,		44,		140,		8,		(int)&eth32.settings.blackout },
			{ CTRL_CHECKBOX,	"Weapon Zoom",			5,		57,		140,		8,		(int)&eth32.settings.weaponZoom },
			{ CTRL_FLOATSLIDER,	"Scoped Turning",		5,		70,		140,		20,		SETFLOAT(0.1), SETFLOAT(1.0), (int)&eth32.settings.scopedTurnSpeed },
			{ CTRL_INTSLIDER,	"Smoke Visibility",		5,		95,		140,		20,		0, 100, (int)&eth32.settings.smoketrnsp },

			{ CTRL_CHECKBOX,	"Banner",				5,		145,	140,		8,		(int)&eth32.settings.guiBanner },
			{ CTRL_FLOATSLIDER,	"Banner size",			5,		158,	140,		20,		SETFLOAT(0), SETFLOAT(3), (int)&eth32.settings.BannerScale },
			{ CTRL_CHECKBOX,	"Remove Foliage",		5,		183,	140,		8,		(int)&eth32.settings.removeFoliage },
			{ CTRL_CHECKBOX,	"Remove Hit Particles",	5,		196,	140,		8,		(int)&eth32.settings.removeParticles },
			{ CTRL_CHECKBOX,	"Players Names",		5,		209,	140,		8,			(int)&eth32.settings.espName },
		},
	},
	{
		"Hitbox Display",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 180,		// x, y, w, h
		8,				// num controls
		{
			// Type			Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Head",			5,		5,		140,		8,		(int)&eth32.settings.drawHeadHitbox },
			{ CTRL_CHECKBOX,	"Head Axes",		5,		18,		140,		8,		(int)&eth32.settings.drawHeadAxes },
			{ CTRL_CHECKBOX,	"Body",			5,		31,		140,		8,		(int)&eth32.settings.drawBodyHitbox },
			{ CTRL_CHECKBOX,	"Show Aimpoints",	5,		44,		140,		8,		(int)&eth32.settings.debugPoints},
			{ CTRL_CHECKBOX,	"Bullet Rail",		5,		57,		140,		8,		(int)&eth32.settings.drawBulletRail },
			{ CTRL_CHECKBOX,	"Rail Wallhack",	5,		70,		140,		8,		(int)&eth32.settings.railWallhack },
			{ CTRL_INTSLIDER,	"Head Trail Time",	5,		83,		140,		20,		0, 300, (int)&eth32.settings.headRailTime },
			{ CTRL_INTSLIDER,	"Body Trail Time",	5,		108,		140,		20,		0, 300, (int)&eth32.settings.bodyRailTime },
		},
	},
	/** *******************************************************************
					VISUAL EXTRA
	******************************************************************* **/
	{
		"Crosshair",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 200, 95,		// x, y, w, h
		3,				// num controls
		{
			{ CTRL_DROPBOX,		"Custom Crosshair",	5,		5,		190,	23,		0, XHAIR_MAX-1, (int)&eth32.settings.customXhair, (int)crosshairText },
			{ CTRL_FLOATSLIDER,	"Size",			5,		30,		190,	20,		SETFLOAT(0), SETFLOAT(500), (int)&eth32.settings.crossSize },
			{ CTRL_FLOATSLIDER,	"Opacity",		5,		55,		190,	20,		SETFLOAT(0), SETFLOAT(1), (int)&eth32.settings.xhairOpacity },
		},
	},
	/** *******************************************************************
					COLOR PICKER
	******************************************************************* **/
	{
		"Colors",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		5, 20, 630, 176,		// x, y, w, h
		1,				// num controls
		{
			// Type		    Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_COLORPICKER, "Picker",			5,		5,		620,		156 },
		},
	},
	/** *******************************************************************
					MISC
	******************************************************************* **/
	{
		"Misc",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		6, 55, 150, 202,		// x, y, w, h
		13,				// num controls
		{
			{ CTRL_CHECKBOX,	"Spec list", 		5, 		5, 		140, 	8, 		(int)&eth32.settings.getSpeclist },
			{ CTRL_CHECKBOX,	"Transparent Console",	5,		18,		140,	8,		(int)&eth32.settings.transparantConsole },
			{ CTRL_CHECKBOX,	"Respawn Timers",	5,		31,		140,	8,		(int)&eth32.settings.respawnTimers },
			{ CTRL_CHECKBOX,	"Auto Tapout",		5,		44,		140,	8,		(int)&eth32.settings.autoTapout },
			{ CTRL_DROPBOX,		"PB Screenshot",	5,		57,		140,	23,		0, PB_SS_MAX-1, (int)&eth32.settings.pbScreenShot, (int)pbssText },
			{ CTRL_CHECKBOX,	"Original Viewvalues",	5,	86,		140,	8,		(int)&eth32.settings.origViewValues },
			{ CTRL_CHECKBOX,	"Interpolated PS",	5,		99,		140,	8,		(int)&eth32.settings.interpolatedPs },
			{ CTRL_CHECKBOX,	"Damage Feedback",	5,		112,		140,	8,		(int)&eth32.settings.dmgFeedback },
			{ CTRL_CHECKBOX,	"Auto Vote",		5,		125,		140,	8,		(int)&eth32.settings.autoVote },
			{ CTRL_CHECKBOX,	"Auto Complaint",	5,		138,		140,	8,		(int)&eth32.settings.autoComplaint },
			{ CTRL_CHECKBOX,	"Anti Teamkill",	5,		151,		140,	8,		(int)&eth32.settings.antiTk },
			{ CTRL_CHECKBOX,	"Spoof OS",			5,		164,		140,	8,		(int)&eth32.settings.etproOs },
			{ CTRL_CHECKBOX,	"Timenudge Hack",	5,		177,		140,	8,		(int)&eth32.settings.nudgeHack },
		},
	},
	{
		"Sound",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 250, 150, 72,		// x, y, w, h
		3,				// num controls
		{
			{ CTRL_DROPBOX,		"Hitsound Type",		5,		5,		140,	23,		0, HIT_MAX-2, (int)&eth32.settings.hitsounds, (int)hitsoundsText },
			{ CTRL_CHECKBOX,	"Pure Only",		    5,		34,		140,	8,		(int)&eth32.settings.pureSounds },
			{ CTRL_CHECKBOX,	"HQ sounds",		    5,		47,		140,	8,		(int)&eth32.settings.hqSounds },
		},
	},
	{
		"Name Stealer",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 370, 150, 118,		// x, y, w, h
		5,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Name Steal",		5,		5,		140,		8,		(int)&eth32.settings.doNamesteal },
			{ CTRL_INTSLIDER,	"Delay",			5,		18,		140,		20,		0, 20000, (int)&eth32.settings.NamestealDelay },
			{ CTRL_INTSLIDER,	"Init Grace",		5,		41,		140,		20,		0, 20000, (int)&eth32.settings.NamestealGrace },
			{ CTRL_DROPBOX,		"Steal type",		5,		64,		140,		23,		0, NAMESTEAL_MAX-1, (int)&eth32.settings.NamestealMode, (int)namestealText },
			{ CTRL_CHECKBOX,	"PB Exact Namesteal",5,		93,		140,		8,		(int)&eth32.settings.nsSmartMode },
		}
	},
};

static const assetdef_t assetDefs[] =
{
//	  Key			Type				Target
	{ "titlecolor",		ASSET_VEC4,				(void*)eth32.guiAssets.titleColor },
	{ "textcolor1",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor1 },
	{ "textcolor2",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor2 },
	{ "titleleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleLeft },
	{ "titlecenter",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleCenter },
	{ "titleright",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleRight },
	{ "winleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winLeft },
	{ "wintop",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTop },
	{ "wintopl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTopLeft },
	{ "wincenter",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winCenter },
	{ "txtinputl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputLeft },
	{ "txtinputc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputCenter },
	{ "txtinputr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputRight },
	{ "btnl",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnLeft },
	{ "btnc",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnCenter },
	{ "btnr",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnRight },
	{ "btnsell",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselLeft },
	{ "btnselc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselCenter },
	{ "btnselr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselRight },
	{ "check",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.check },
	{ "checkbox",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.checkBox },
	{ "mouse",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.mousePtr },
	{ "dropboxarrow",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.dropboxArrow },
	{ "sliderbtn",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderBtn },
	{ "slidertrack",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderTrack },
	{ "camcorner",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.camCorner },
};
