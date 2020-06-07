// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

/*
	The main header file we might say, contains all the info that is used while playing.
	When you're adding something in here, make sure it's being added to the right struct!

	Also, please comment the members you're adding so other developers know what's the function of each part.
*/
#pragma once

#include "unixtypes.h"
#include "eth32shared.h"

// stuff that we don't get from sdk
#include "q3_types.h"

// **************************
// Macros
// **************************
#define GET_PAGE(addr) ((void *)(((uint32)addr) & ~((uint32)(getpagesize() - 1))))
#define unprotect(addr) (mprotect(GET_PAGE(addr), getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC))
#define reprotect(addr) (mprotect(GET_PAGE(addr), getpagesize(), PROT_READ | PROT_EXEC))
#define BOUND_VALUE(var,min,max)	if((var)>(max)){(var)=(max);};if((var)<(min)){(var)=(min);}

#ifndef uint32
typedef unsigned int uint32;
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

#define CG_VM   *eth32.game.vm == eth32.game.cgvm
#define UI_VM   *eth32.game.vm == eth32.game.uivm

typedef struct weapStat_s {
	int kills;
	int deaths;
	int shots;
	int hits;
	int headshots;
} weapStat_t;

#define WS_ABS_MAX	32
#define WS_ETM_MAX	22
#define WS_JAY_MAX	24 // ?
#define WS_NQ_MAX	32

typedef struct stats_s {
	int kills;
	int deaths;
	int spree;
	int longestSpree;
	float kdRatio;
	float threat;
	float accuracy;			// EV_BULLET based
	int suicides;
	int bulletsFired;		// EV_BULLET based
	int bulletsHit;			// EV_BULLET based

	int my_kills;
	int	my_deaths;
	int my_kdRatio;

	int killer;

	// noquarter seems to have most weapons - make big enough to fit all modz
	weapStat_t weap[WS_ABS_MAX];
	int bulletsFiredWS;
	int bulletsHitWS;
	float bulletAcc;
	int headshotsWS;
	float hr_kills;			// headshot ratio -> headshots/kills
	float hr_hits;			// headshot ratio -> headshots/hits
	float hr_shots;			// headshot ratio -> headshots/shots

}stats_t;

#define MAX_MULTI	48
#define MAX_HISTORY	100

typedef struct history_s {
	vec3_t	p;
	float	t;
} history_t;

typedef enum playerType_s {
	PLAYER_NORMAL,
	PLAYER_FRIEND,
	PLAYER_ENEMY,
} playerType_t;

typedef struct player_s {					// where we can access all the info we need on a player
	bool			*infoValid;		// we can add more data here later (aimpoints, visible, invunerable, etc.)
	centity_t		*cent;			// will be all pointers to the information we need in cg_entities and clientinfo
	entityState_t	*currentState;
	float			*lerpOrigin;
	char			*name;
	int				*team;
	int				*health;
	int				*cls;			// class
	int				Class;			// Used to draw class icons
	lerpFrame_t		*lf;			// for animation correction

	orientation_t	orHead;			// head orientation (position & rotation)
	bool			friendly;		// true for teammates
	bool			invulnerable;	// to disable aiming at them, and possibly for use in chams
	bool			visible;		// is this player visible
	bool			omniBot;		// is this player a bot?
	float			distance;		// distance from view origin to player
	int				screenX;		// x coord on screen
	int				screenY;		// y coord on screen
	qhandle_t		headModel;

	stats_t			stats;
	vec3_t			headPt;			// visible point on the head
	vec3_t			bodyPt;			// visible point on body box
	vec3_t			aimPt;			// if aimbot is used, this will need to be set
	vec3_t			preAimPt;		// for early aim to reduce spread in the first shots
	int				clientNum;		// so we can use player_t* easier
	playerType_t	playerType;		// friend 'o f00
} player_t;

typedef struct gentity_s {
	bool			*infoValid;
	centity_t		*cent;			// will be all pointers to the information we need in cg_entities and clientinfo
	entityState_t	*currentState;
	float			*lerpOrigin;	// for greater portability
	bool		visible;		// is this gentity drawn inside screen coords?
	int			screenX;		// x coord on screen
	int			screenY;		// y coord on screen
	int			clientNum;
} gentity_t;

typedef enum weapType_s {
	WT_NONE,
	WT_KNIFE,
	WT_PISTOL,
	WT_SMG,
	WT_STEN,
	WT_MG,
	WT_RIFLE,
	WT_SNIPER,
	WT_SHOTGUN,
	WT_HEAVY,
	WT_EXPLOSIVE,
	WT_MAX,
} weapType_t;

typedef enum aimProtect_s {
	PROTECT_OFF,
	PROTECT_SPECS,
	PROTECT_ALL,
	PROTECT_MAX
} aimProtect_t;

// sol: weapon attribs will be used for figuring out what kind of weapon we are dealing with
typedef enum weapAttrib_s {
	WA_NONE	=			0,
	WA_USER_DEFINED =	(1 << 0),	// allowed to configure through weapon window when enabled
	WA_OVERHEAT =		(1 << 1),	// this weapon can overheat
	WA_BLOCK_FIRE =		(1 << 2),	// this weapon should block fire in certain autofire modes
	WA_AKIMBO =			(1 << 3),	// dual weapons, need to know for drawing ammo properly
	WA_SCOPED =			(1 << 4),	// this weapon is in scoped mode
	WA_UNSCOPED =		(1 << 5),	// this weapon is in unscoped mode
	WA_RIFLE_GRENADE =	(1 << 6),	// camera use, grenade bot
	WA_MORTAR =			(1 << 7),	// camera use
	WA_PANZER =			(1 << 8),	// camera use
	WA_SATCHEL =		(1 << 9),	// camera use
	WA_NO_AMMO =		(1 << 10),	// this weapon doesn't use ammo
	WA_ONLY_CLIP =		(1 << 11),	// this weapon only uses clip (disregard ammo)
	WA_BALLISTIC =		(1 << 12),	// this weapon can be used for the ballistics aimbot
	WA_GRENADE =		(1 << 13),	// grenade bot (different code then riflenade)
} weapAttrib_t;

typedef struct weapdef_s {
	const char	*name;
	// non user defined
	weapType_t	type;		// type of weapon, user can set a preference to all weapons of the same type
	unsigned int 	attribs;	// a mix of weapAttrib_t for filtering weapons
	int		otherAkimbo;	// if this weapon is akimbo, this is the matching weapon to use for ammo

	// things user can set per weapon if userDefined
	int		range;			// max range aimbot will search for targets
	int		headTraces;		// number of head traces to check
	int		bodyTraces;		// number of body traces to check
	int		delay;			// time in msec between firing
	int		spread;
	float	Sdelay;
	bool	autofire;		// use autofire for this weap
	bool	antiOverheat;	// if weap can overheat, enable anti-overheat

} weapdef_t;

typedef enum namestealMode_s {
	NAMESTEAL_TEAM,
	NAMESTEAL_ENEMY,
	NAMESTEAL_ALL,
	NAMESTEAL_FILE,
	NAMESTEAL_MAX
} namestealMode_t;

typedef enum pbss_s {
	PB_SS_NORMAL,
 	PB_SS_CUSTOM,
	PB_SS_BLACK,
  	PB_SS_CLEAN,
   	PB_SS_MAX
} pbss_t;

typedef enum customxhair_s {
	XHAIR_OFF,
 	XHAIR_SNIPER,
  	XHAIR_CROSS,
   	XHAIR_CROSS2,
   	XHAIR_CROSS3,
	XHAIR_CROSS4,
	XHAIR_DOT,
	XHAIR_MAX
} customxhair_t;

typedef enum hitSounds_s {
	HIT_OFF,
	HIT_HEAD,
	HIT_HEADSHOT,
	HIT_BODY, 
	HIT_TEAM,
	HIT_MAX
} hitSounds_t;

typedef struct eth32sounds_s {
	sfxHandle_t		hitsounds[HIT_MAX];
} eth32sounds_t;

typedef struct media_s {	// all the medias (pictures, sounds, models, etc)
	qhandle_t		white;
	qhandle_t		cursor;
	qhandle_t		binoc;
	qhandle_t		smokePuff;
	qhandle_t		reticleSimple;
	qhandle_t 		weaponIcons[WP_NUM_WEAPONS];

	fontInfo_t		fontArial;


	// effects
	qhandle_t		disk;
	qhandle_t		circle;

	eth32sounds_t   	sounds;
	// menu stuff
	qhandle_t		statbar;
	qhandle_t		statbar2;
	qhandle_t		colorTable;
	qhandle_t		crosshair[XHAIR_MAX];

	// TCE model
	qhandle_t		grenadeTCE;			// TCE grenade Model
	qhandle_t		dynamiteTCE;		// TCE dynamite Model
	qhandle_t		flashGrenadeTCE;	// TCE flash Grenade Model
	qhandle_t		smokeGrenadeTCE;	// TCE smoke Grenade Model
	qhandle_t		medpackTCE;			// TCE med pack model
	// TCE icons
	qhandle_t		grenadeTCEIcon;		// TCE grenade Icon
	qhandle_t		dynamiteTCEIcon;	// TCE dynamite Icon
	qhandle_t		smokeGrenadeTCEIcon;// TCE flash Grenade Icon
	qhandle_t		flashGrenadeTCEIcon;// TCE smoke Grenade Icon

	// Regular Models
	qhandle_t		disguisedIcon;		// for disguised players
	qhandle_t 		medPack;			// Health pack pickup Model
	qhandle_t		ammoPack;			// Ammo pack pickup Model
	qhandle_t		grenadeAxisModel;	// Axis grenade
	qhandle_t		grenadeAlliedModel;	// Allied grenade
	qhandle_t		dynamiteModel;		// Dynamite Model
	qhandle_t		satchelModel;		// Satchel Model
	qhandle_t		mortarModel;		// Mortar shell Model
	qhandle_t		thompsonModel;		// Thompson Model
	qhandle_t		mp40Model;			// Mp40 Model
	qhandle_t		stenModel;			// Sten Model
	qhandle_t		rifleNadeAxis;		// axis grenade model for riflelauncher
	qhandle_t		rifleNadeAllied;	// allied grenade model for riflelauncher

	qhandle_t		railCore;			// shader for railtrails
	qhandle_t		combtnLeft, combtnRight, combtnCenter;
	qhandle_t		comselLeft, comselRight, comselCenter;

	qhandle_t		nullShader;
	qhandle_t		etlogoShader;

} media_t;

typedef struct bools_s {
	bool spawntimerReady;					// we can't use spawntimer until cgame gathers some info
} bools_t;

typedef enum teamState_s {
	TEAM_ENEMY,
	TEAM_FRIEND,
	MAX_TEAMSTATES,
} teamState_t;

typedef struct chargeTimes_s {
	int soldier[2];
	int medic[2];
	int engineer[2];
	int lt[2];
	int covops[2];
} chargeTimes_t;

typedef struct cgame_s {
	void 		*module;			// module (base) of cgame.dll in memory
	void		*handle;			// (unix) dlopen handle
	bool		automod;			// did we redirect unsupported mod to etmain?
	int			supported;			// used by VM_Create, if supported then modify vm->entryPoint
	int			clientNum;			// our ID on a server
	int			team;				// set once per frame to allow viewing team/enemy chams even while spec
	int			time;				// current server time
	int			zerohour;			// time of real first active frame
	int			frametime;			// time needed to draw this frame - can be used for prediction later
	int			xhairClientNum;			// client num at crosshair (-1 if none)
	player_t	players[MAX_CLIENTS];		// our player data , NOTE: we must know mod cent base/size and clientinfo base/size
	gentity_t	gentity[MAX_GENTITIES];		// gentity data
	char		spectators[MAX_CLIENTS][MAX_NAME_LENGTH];	// holds who is spectating us
	int			nspectators;		// no. of spectators
	player_t	*self;				// me!
	bool		pbserver;			// sv_punkbuster 1
	bool		serverObits;		// server-side obits (some etpub servers)
	
	refEntity_t 	refEntities[MAX_GENTITIES];
	int 			refEntitiesCount;		// Number of refEntities, used by items ESP
	
	bool		SafeVote;

	snapshot_t	*snap;
	gameState_t	*gameState;
	pmoveExt_t	*pmext;
	playerState_t	*predictedps;

	weapdef_t   *currentWeapon;			// current weapon out of weaponList
	weapdef_t	*weaponList;			// current mod list of weapons
	weapdef_t	*modifyWeapon;			// used by weapon window
	int			modifyWeapNum;			// used by weapon window
	int 		numWeapons;			// mod dependent

	// rendering info
	float		screenXScale;
	float		screenYScale;
	refdef_t	*refdef;
	float		*refdefViewAngles;
	int		centerX, centerY;
	char		serverName[50];
	char		serverMapName[25];

	vec3_t		lerpOrigin;
	vec3_t		lerpAngles;

	// muzzle, viewangles
	vec3_t		muzzle;
	float		ourViewAngleX;
	float		ourViewAngleY;

	media_t		media;				// all our medias

	bools_t		bools;				// various checks

	int		spawnTimes[MAX_TEAMSTATES];	// spawntimes
	chargeTimes_t	chargeTimes;			// for use by weap charge

	int			cgTime;				// cgame time
	bool		hasInit;			// finished CG_Init()
	float		spreadIndex;		// spread index for current weapon
	float		ping;
	int 		limbo;				// ammount of limbo players, used for aimbot and banner

	uint32		defaultConsoleShader;		// name says it all

	vec3_t		tempRenderVector;
	
	vmCvar_t	*cg_teamchatheight;
	bool	hooked;
	int		pbss;				// number of PB screenshots taken
	int		lastStatsReq;		// time of last stats request
	bool	wantStats;			// +stats down
	int		ws_max;				// mod dependant WS_MAX
	int 	numScores;

	localEntity_t	*firstle;		// first entry of le's in cgame (needed for clean ss)
	localEntity_t	*highle;		// highest entry of le's in cgame found
} cgame_t;

typedef struct ui_s {
	void		*module;			// module (base) of ui.dll in memory
	void		*handle;			// (unix) dlopen handle
} ui_t;

typedef struct game_s {
	void		*module;				// et.exe module
	char		path[MAX_PATH];		// C:\path\to\ET\

	float		*viewAngleX;		// view angle x
	float		*viewAngleY;		// view angle y
	glconfig_t	*glconfig;			// et's glconfig
	int		*com_frameTime;		// continous, cgame independent msec counter

	qhandle_t	console[3];			// the three shares used for the console

	// ptrs to some cvars so we don't have to do Cvar_VariableStringBuffer
	cvar_t		*cg_fov;
	cvar_t		*sensitivity;
	cvar_t		*cg_railTrailTime;

	// linux only
	uint32		textSectionStart;
	uint32		textSectionEnd;

        uint32  	cgvm;                                   // location of et's cg vm
        uint32  	uivm;                                   // location of et's ui vm
        uint32  	*vm;                                    // ptr to et's current vm
} game_t;

// MOD/ET defines - these values must never change
// Every new mod/et version must be added to the portal defines as well
#define MOD_ETMAIN		0
#define MOD_ETPUB		1
#define MOD_JAYMOD1		2
#define MOD_JAYMOD2		3
#define MOD_NOQUARTER	4
#define MOD_ETPRO		5
#define MOD_TCE			6

#define ET_255			0
#define	ET_256			1
#define ET_260			2
#define	ET_260b			3

typedef struct et_s {
	char			version[64];	// ET.exe version
	uint32			crc32;		// crc32 checksum of ET.exe
	int			ver;		// for portal communication

	uint32			viewAngleX;
	uint32			viewAngleY;
	uint32			inButtons;
	uint32			glConfig;
	uint32			com_frameTime;
	uint32			consoleshader;

	uint32			etroq_video;
	char			etroq_safe[5];

	uint32			syscall;
	uint32			Cvar_Get;
	uint32			Cvar_Set2;
	uint32			fs_searchpaths;
	uint32			FS_PureServerSetLoadedPaks;
	uint32			FS_AddGameDirectory;
	uint32			FS_AddGameDirectory2;
	uint32			FS_Startup;
	uint32			FS_Startup2;
	uint32			Cmd_AddCommand;
	uint32			VM_Create;
	uint32			Sys_LoadDll;
	uint32			LoadTGA;
	uint32			server_ip;
	uint32			SCR_UpdateScreen;
	uint32			Con_DrawConsole;
	uint32			currentVm;
	uint32			Cvar_VariableString;

	uint32			aimHackSize;
	uint32			aimHackOffs;
	char			aimHack[50];
} et_t;

typedef enum camType_s {
	CAM_MORTAR,
	CAM_FOLLOW,
	CAM_MAX
} camType_t;

typedef struct camInfo_s {
	int x1, y1, x2, y2;
	bool	display;
	float	distance;
	float	angle;
	float	fov;

} camInfo_t;

typedef struct gentityInfo_s {
	bool	chams;
	bool	text;
	bool	icon;
	int	distance;
} gentityInfo_t;

typedef struct settings_s {
	// Aimbot
	int		aimMode;	// type of aimbot
	int		aimType;	// global setting to set aim style (always, on key, etc.) or disable aim globally
	bool	autofire;	// global autofire settings (each weapon can be set for autofire as well)
	bool	atkValidate;
	bool	lockTarget;
	float	fov;
	int		aimSort;	// global target sort
	int		headbody;
	int		hitboxType;
	int		headTraceType;
	int		bodyTraceType;
	float	dynamicHitboxScale;	// to scale dynamic hitboxes (or not)
	float	animCorrection;
	bool	autoCrouch;
	bool 	autocorrections;
	bool	valGrenTrajectory;
	bool	valRifleTrajectory;
	bool	grenadeTracer;
	bool	rifleTracer;

	// Human Vars
	int		humanAimType;	// type of human aim to do
    float	humanValue;
	float	divMin;
	float	divMax;
	bool 	lockMouse;
	bool	lockSensitivity;
	int		aimprotect;		// to disable aimbot on certain conditions

	// Aimbot Extra
	float	headBoxSize;
	float 	bodybox;
	int   	pointshead;
    int  	pointsbody;
    bool 	autoDelay;
    int 	delayClose;
    int 	delayMed;
    int 	delayFar;
    bool	autoVecZ;

    bool	preShoot;
    bool	preAim;
    float	preShootTime;
	float	preAimTime;

    float	predSelf;
    float	predTarget;
    float   pred;

	float 	spread;
    float   farBoxSize;
	float   closeBoxSize;
    float   RangE;
    float   vecstandcloseX;
    float   vecstandfarX;
    float   veccrouchcloseX;
    float   veccrouchfarX;
    float   vecstandcloseY;
    float   vecstandfarY;
    float   veccrouchcloseY;
    float   veccrouchfarY;
    float   vecstandcloseZ;
    float   vecstandfarZ;
    float   veccrouchcloseZ;
    float   veccrouchfarZ;
    float   vecpronecloseX;
    float   vecpronefarX;
    float   vecpronecloseY;
    float   vecpronefarY;
    float   vecpronecloseZ;
    float   vecpronefarZ;    
    float   delayclose;
    float   delayfar;
	float   pointsclose;
    float   pointsfar;
    	// Visuals
    bool	guiOriginal;
	bool	drawHackVisuals;
	bool	wallhack;
	bool	blackout;	// draw sniper/binocs blackout
	bool	weaponZoom;	// draw scoped weapon zoom
	float	scopedTurnSpeed;	// scoped turning speed
	int		smoketrnsp;	// smoke transparency changer

	bool	guiBanner;
	float	BannerScale;
	char	BannerFmt[MAX_STRING_CHARS];
	bool 	removeFoliage;
	bool	removeParticles;

	int			camType;
	camInfo_t 	cams[CAM_MAX];

	bool	drawHeadHitbox;
	bool	drawHeadAxes;
	bool	drawBodyHitbox;
	bool	debugPoints;
	bool	drawBulletRail;
	bool	railWallhack;		// see the rails trough walls
	int		headRailTime;
	int		bodyRailTime;
	int		bulletRailTime;

	bool	espName;		// name esp
	bool	grenadeDlight;	// extra grenade visuals
	bool	mortarDlight;	// extra mortar visuals
	bool	drawDisguised;	// draw disguised marker
	bool	mortarTrace;	// extra mortar trace + esp
	bool	artyMarkers;	// extra arty markers
	
	
	// Visuals Extra

	vec3_t	xhairColor;
	vec3_t	colorHeadHitbox;
	vec3_t	colorBodyHitbox;
	vec3_t	colorBulletRail;
	vec3_t	colorXAxis;
	vec3_t	colorYAxis;
	vec3_t	colorZAxis;
	vec4_t gui_stamina;
	vec4_t gui_charge;
	vec4_t gui_overheat;
	vec4_t gui_reload;
	
	int 	customXhair;
	float 	crossSize;
	float	xhairOpacity;

	// Misc
	uchar	colorEnemy[3];
	bool	getSpeclist;	// get spectator list
	bool	transparantConsole;
	bool	respawnTimers; // draw respawn timers window
	bool	autoTapout;	// tapout on death right to reinforce queue
	int		pbScreenShot;
	bool	origViewValues;
	bool	interpolatedPs;
	bool	dmgFeedback;
	bool	autoVote;
	bool	autoComplaint;
	bool 	antiTk;
	bool	etproOs;
	bool	nudgeHack;

	int		hitsounds;
	bool	pureSounds;			// only play 'pure' sounds - usefull against annoying voicespam
	bool	hqSounds;			// if HQ sounds are to be played

	bool	doNamesteal;		// do namesteal - forced to off on connect!
	int		NamestealDelay;		// delay between steals
	int		NamestealGrace;		// time to wait after joining
	int		NamestealMode;		// type of namestealing
	bool	nsSmartMode;

	// Extra stuff
	bool	loaded;		// not for normal use, to prevent saving settings when they are not rdy		
	char	pk3file[MAX_STRING_CHARS];
	char	etproGuid[MAX_STRING_CHARS];
	char	jayMac[MAX_STRING_CHARS];
	bool	defaultSoundsLoaded;
	char	cvarPrefix[MAX_STRING_CHARS];	
} settings_t;

typedef struct cvarInfo_s {
	char name[MAX_STRING_CHARS];
	char ourValue[MAX_STRING_CHARS];
	char restoreValue[MAX_STRING_CHARS];
	bool isSet;
	cvarInfo_s *next;
} cvarInfo_t;

typedef struct guiassets_s {				// barebones of style management
	float 	fontScale;
	int	fontHeight;

	vec4_t titleColor;
	vec4_t textColor1;
	vec4_t textColor2;

	qhandle_t titleLeft, titleCenter, titleRight;

	qhandle_t winLeft;
	qhandle_t winTop;
	qhandle_t winTopLeft;
	qhandle_t winCenter;

	qhandle_t txtinputLeft, txtinputCenter, txtinputRight;
	qhandle_t btnLeft, btnCenter, btnRight;
	qhandle_t btnselLeft, btnselCenter, btnselRight;

	qhandle_t dropboxArrow;
	qhandle_t check, checkBox;
	qhandle_t scrollbarArrow, scrollbarBtn, scrollbarTrack;

	qhandle_t sliderTrack, sliderBtn;
	qhandle_t camCorner;

	qhandle_t mousePtr;
} guiassets_t;

#define PB_CVAR_IN			0x1
#define PB_CVAR_OUT			0x2
#define PB_CVAR_INCLUDE		0x15
#define PB_CVAR_EXCLUDE		0x16

// from 1.738
typedef struct pb_cvar_s {
	char name[0x80];			// restricted cvar name
	char type;					// type of restriction
	char string[0x80];			// string value
	char padding[3];
	double low;					// lower bound
	double high;				// higher bound
	int unknown;
} pb_cvar_t;

typedef struct pb_s {
	bool 		loaded;
	bool		statusOK;
	void 		*handle;
	void 		*module;
	char 		file[MAX_PATH];

	uint32 		text;					// start of .text
	uint32		text_size;
	uint32		bss;					// start of .bss
	uint32		bss_size;

	uint32 		*nCvars;				// total PB restricted cvars
	pb_cvar_t 	**cvarlist;

	char		spoofIp[16];
	char		realIp[16];
} pb_t;

typedef struct eth32_s {
	cgame_t			cg;					// info from cgame lib
	ui_t			ui;					// info from ui lib
	game_t			game;				// info from et.exe
	cg_t			ocg;

	const et_t		*et;				// needed info about et.exe
	const cgMod_t	*cgMod;				// needed info about cgame.dll
	pb_t			pb;					// infos about PB module

	settings_t		settings;			// our user settings

	stats_t			stats;				// EntitiyEvent info

	guiassets_t		guiAssets;			// used for CGui for rendering

	char			path[MAX_PATH];		// C:\path\to\eth32\

	bool			cgameLoaded;		// tracker
	char			server[64];			// tracker

	char			pk3File[MAX_STRING_CHARS];	// pk3file
	uint32			eth32CRC;

	Display 		*x11disp;			// for restoring X params on crash
	Window 			*x11win;			// for restoring X params on crash
	bool			eth32mod;			// dump eth32's client offsets
	uint32			eth32modcrc;
} eth32_t;

extern eth32_t eth32;

// kobject: imported this from Q3 SDK
// downtime & msec only have effect for movement buttons, so for attack we're in the clear
typedef struct kbutton_s {
	int		down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	int		active;			// current state
	int		wasPressed;		// set when down, not cleared when up
} kbutton_t;

