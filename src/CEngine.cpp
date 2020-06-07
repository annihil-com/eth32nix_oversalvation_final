// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#include "eth32.h"
#include <netinet/in.h>

CEngine Engine;
			//Name	    //Type   //Attribs	//OtherAkimbo	//Range	//HeadTraces //BodyTraces //Autofire
weapdef_t nullWeapon = { "Unknown", WT_NONE, WA_NONE, 	0, 		0, 	0, 	     0, 	  false };

typedef struct {
	int phase;
	vec4_t color[2];
} espPhase_t;
espPhase_t espPhase;

const char *pbssText[PB_SS_MAX] =
{
	"Normal",
 	"Custom",
	"Black",
  	"Clean",
};

const char *crosshairText[XHAIR_MAX] =
{
	"Off",
	"Sniper",
	"Open Cross",
	"Open Cross 2",
	"Closed Cross",
	"Threeway Cross",
	"Dot",
};

const char *hitsoundsText[HIT_MAX] =
{
	"Off",
 	"Head",
  	"HeadShot",
  	"Body",
};

#define	ICONS_SIZE 17					// for scaling
#define D_ICON_SIZE 24					// used without scaling

CArty::~CArty() {}
int CArty::Id() { return _id; }
bool CArty::Compare(gentity_t *bomb){ return (_id == bomb->currentState->pos.trTime); }
int CArty::Team() { return _team; }
bool CArty::GetBomb(vec3_t pos)
{
	if (_bombs) {
		VectorCopy(_bomb_origin, pos);
	} else
		return false;
}

bool CArty::Finished()
{
	if (_bombs == 4) {
		if ((eth32.cg.time - _bombtime) > 2500)
			return true;
	} else if (_bombs >= 8)
		return true;

	if ((eth32.cg.time - _start) > 33000)
		return true;

	return false;
}

float *CArty::Pos() { return &_origin[0]; }

// guess for delay between call and first bomb
int CArty::GetImpactTime()
{
	if (!_bombs) return ( 10800 - eth32.cg.time + _start );
	else return 0;
}

void CArty::AddBomb(gentity_t *bomb)
{
	if (!VectorCompare(bomb->currentState->pos.trBase, _bomb_origin)) {
		VectorCopy(bomb->currentState->pos.trBase, _bomb_origin);
		_bombs++;
		_bombtime = eth32.cg.time;
	}
}

CArty::CArty(int startTime, int id, int team, vec3_t pos)
{
	_bombs = 0;
	_id = id;
	_team = team;
	_start = startTime;
	VectorCopy(pos, _origin);
}

int CMortar::Id() { return _id; }

// calculate the trajectory only once to save FPS
CMortar::CMortar(gentity_t *shell)
{
	mshell = shell;
	_team = shell->currentState->teamNum;
	_id = shell->currentState->pos.trTime;

	trace_t tr;
	vec3_t shellImpact;
	int maxTime = 20000;
	int totalFly = 0;

	vec3_t v, p0, p1;
	float vz,vs, ft;

	// calculate forward trajectory to impact point
	VectorCopy(shell->currentState->pos.trDelta, v);
	vz = v[2];
	v[2] = 0;
	vs = VectorLength(v);
	VectorNormalizeFast(v);
	N = 0;
	VectorCopy(shell->currentState->pos.trBase, p0);

	VectorCopy(p0, trajectory[0]);
	ptvisible[0] = Engine.IsVisible(eth32.cg.refdef->vieworg, p0, -1);
	totalFly = 0;
	for (int t=50; t<maxTime; t+= 50) {
		ft = (float)t*0.001f;
		VectorMA(shell->currentState->pos.trBase, ft*vs, v, p1);
		p1[2] += vz*ft - 400.0*ft*ft;
		orig_CG_Trace(&tr, p0, 0, 0, p1, -1, MASK_MISSILESHOT);

		// check for hit (but not on sky)
		if (tr.fraction != 1 && !((tr.surfaceFlags & SURF_NODRAW) || (tr.surfaceFlags & SURF_NOIMPACT)) && (tr.contents != 0) ){
			VectorCopy(tr.endpos, impactPos);
			break;
		}

		N++;
		VectorCopy(p1, trajectory[N]);

		// check for hit (but not on sky)
		orig_CG_Trace(&tr, eth32.cg.refdef->vieworg, 0, 0, p1, -1, MASK_MISSILESHOT);
		if (tr.fraction != 1 && !((tr.surfaceFlags & SURF_NODRAW) || (tr.surfaceFlags & SURF_NOIMPACT)) && (tr.contents != 0) )
			ptvisible[N] = false;
		else
			ptvisible[N] = true;

		totalFly = t;
		VectorCopy(p1, p0);
	}

	VectorCopy(p0, impactPos);
	impactTime = eth32.cg.time + totalFly;
}

bool CMortar::Finished() { int cond = 500 + impactTime - eth32.cg.time; return cond < 0 ? true : false; }
bool CMortar::Compare(gentity_t *shell) { return (_id == shell->currentState->pos.trTime); }
float *CMortar::ImpactPos() { return impactPos; }
int CMortar::Team() { return _id; }
int CMortar::TimeRemaining() { int time = impactTime - eth32.cg.time; return time > 0 ? time : 0; }

void CMortar::DrawTrace()
{
	for (int i=2; i<N-1; i++)
		Engine.MakeRailTrail(trajectory[i], trajectory[i+1], false, ptvisible[i] ? colorGreen : colorRed, eth32.cg.frametime*3 );
}

CEngine::CEngine(void)
{
	forward = NULL;
	railtrailTime = NULL;
	pDrawPlayer = NULL;
	drawWeapon = false;
}

// actions to take when menu is closed
void CEngine::MenuCloseActions()
{
	SetConsoleShader();
}

void CEngine::SetConsoleShader()
{
	eth32.game.console[0] = eth32.game.console[0] ? eth32.game.console[0] : *(uint32*)(eth32.et->consoleshader);
	eth32.game.console[1] = eth32.game.console[1] ? eth32.game.console[1] : *(uint32*)(eth32.et->consoleshader+4);
	eth32.game.console[2] = eth32.game.console[2] ? eth32.game.console[2] : *(uint32*)(eth32.et->consoleshader+8);

	if (eth32.settings.transparantConsole && eth32.cg.media.nullShader){
		*(uint32*)(eth32.et->consoleshader) = eth32.cg.media.nullShader;
		*(uint32*)(eth32.et->consoleshader+4) = eth32.cg.media.nullShader;
		*(uint32*)(eth32.et->consoleshader+8) = eth32.cg.media.etlogoShader;
	} else {
		*(uint32*)(eth32.et->consoleshader) = eth32.game.console[0];
		*(uint32*)(eth32.et->consoleshader+4) = eth32.game.console[1];
		*(uint32*)(eth32.et->consoleshader+8) = eth32.game.console[2];
	}
}

void CEngine::RegisterMedia()
{
	// font(s)
	Syscall.R_RegisterFont("courbd", 21, &eth32.cg.media.fontArial);

	eth32.cg.media.etlogoShader = Syscall.R_RegisterShader("eth32logo");
	eth32.cg.media.nullShader = Syscall.R_RegisterShader("nullshader");
	eth32.cg.media.white = Syscall.R_RegisterShader("white");
	eth32.cg.media.cursor = Syscall.R_RegisterShader("ui/assets/3_cursor3");
	eth32.cg.media.binoc = Syscall.R_RegisterShader("gfx/misc/binocsimple");
	eth32.cg.media.smokePuff = Syscall.R_RegisterShader("smokepuff");
	eth32.cg.media.reticleSimple = Syscall.R_RegisterShader("gfx/misc/reticlesimple");
	eth32.cg.media.colorTable = Syscall.R_RegisterShader("gui/common/colortable");
	eth32.cg.media.statbar = Syscall.R_RegisterShader("statbar");
	eth32.cg.media.statbar2 = Syscall.R_RegisterShader("statbar2");
	
	// Custom Crosshairs
	eth32.cg.media.crosshair[XHAIR_SNIPER] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross2.tga");
	eth32.cg.media.crosshair[XHAIR_CROSS] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross3.tga");
	eth32.cg.media.crosshair[XHAIR_CROSS2] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross1.tga");
	eth32.cg.media.crosshair[XHAIR_CROSS3] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross4.tga");
	eth32.cg.media.crosshair[XHAIR_CROSS4] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross6.tga");
	eth32.cg.media.crosshair[XHAIR_DOT] = Syscall.R_RegisterShaderNoMip("gfx/crosshair/cross5.tga");

	// kobject: someone check this is also valid for TCE
	eth32.cg.media.railCore = Syscall.R_RegisterShader("railCore");

	// effects
	eth32.cg.media.disk = Syscall.R_RegisterShaderNoMip("gfx/effects/disk");
	eth32.cg.media.circle = Syscall.R_RegisterShaderNoMip("gfx/effects/circle");

	// common GUI stuff
	eth32.cg.media.combtnLeft = Syscall.R_RegisterShader("combtn_left");
	eth32.cg.media.combtnRight = Syscall.R_RegisterShader("combtn_right");
	eth32.cg.media.combtnCenter = Syscall.R_RegisterShader("combtn_center");
	eth32.cg.media.comselLeft = Syscall.R_RegisterShader("comsel_left");
	eth32.cg.media.comselRight = Syscall.R_RegisterShader("comsel_right");
	eth32.cg.media.comselCenter = Syscall.R_RegisterShader("comsel_center");

	if(eth32.cgMod->type == MOD_TCE)
	{
		//models
		eth32.cg.media.medpackTCE = Syscall.RegisterModel("models/multiplayer/medpack/medpack_pickup.md3");
		eth32.cg.media.flashGrenadeTCE = Syscall.RegisterModel("models/weapons2/grenade/m84.md3");
		eth32.cg.media.grenadeTCE = Syscall.RegisterModel("models/weapons2/mk32a/mk32a.md3");
		eth32.cg.media.dynamiteTCE = Syscall.RegisterModel("models/multiplayer/dynamite/dynamite.md3");
		eth32.cg.media.smokeGrenadeTCE = Syscall.RegisterModel("models/weapons2/grenade/m83.md3");
	}
	else
	{
		// world
		eth32.cg.media.medPack = Syscall.RegisterModel("models/multiplayer/medpack/medpack_pickup.md3");
		eth32.cg.media.ammoPack = Syscall.RegisterModel("models/multiplayer/ammopack/ammopack_pickup.md3");
		// projectiles/explosives
		eth32.cg.media.mortarModel = Syscall.RegisterModel("models/multiplayer/mortar/mortar_shell.md3");
		eth32.cg.media.grenadeAxisModel = Syscall.RegisterModel("models/ammo/grenade1.md3");
		eth32.cg.media.grenadeAlliedModel = Syscall.RegisterModel("models/weapons2/grenade/pineapple.md3");
		eth32.cg.media.satchelModel = Syscall.RegisterModel("models/multiplayer/satchel/satchel_world.md3");
		eth32.cg.media.dynamiteModel = Syscall.RegisterModel("models/multiplayer/dynamite/dynamite.md3");
		eth32.cg.media.rifleNadeAllied = Syscall.RegisterModel("models/multiplayer/m1_garand/m1_garand_prj.md3");
		eth32.cg.media.rifleNadeAxis = Syscall.RegisterModel("models/multiplayer/m1_garand/m1_garand_prj.md3"); /// haven't found it yet
		// weapon models
		eth32.cg.media.mp40Model = Syscall.RegisterModel("models/weapons2/mp40/mp40.md3");
		eth32.cg.media.thompsonModel = Syscall.RegisterModel("models/weapons2/thompson/thompson.md3");
		eth32.cg.media.stenModel = Syscall.RegisterModel("models/weapons2/sten/sten.md3");
		//Icons
		eth32.cg.media.disguisedIcon = Syscall.R_RegisterShaderNoMip("gfx/2d/friendlycross.tga");
	}
}

void CEngine::CG_Init_Pre(int serverMessageNum, int serverCommandSequence, int clientNum)
{
#ifdef ETH32_PRELIM_OFFSETS
	offset_cginit = true;
#endif
	cgHasFirst = false;
	Tools.RegisterThread(2, "Main");
	Tools.SetUserCvars();

	Syscall.CG_SendConsoleCommand("unbind f10\n");
	Syscall.CG_SendConsoleCommand("unbind f9\n");

	/*vsay binds*/
	Syscall.CG_SendConsoleCommand("bind KP_END vsay FTHealMe\n");
	Syscall.CG_SendConsoleCommand("bind KP_DOWNARROW vsay FTReviveTeamMate\n");
	Syscall.CG_SendConsoleCommand("bind KP_PGDN vsay FTReviveMe\n");
	Syscall.CG_SendConsoleCommand("bind KP_LEFTARROW vsay FTAttack\n");
	Syscall.CG_SendConsoleCommand("bind KP_INS vsay FTInfiltrate\n");
	Syscall.CG_SendConsoleCommand("bind KP_RIGHTARROW vsay FTExploreArea\n");
	Syscall.CG_SendConsoleCommand("bind KP_HOME vsay FTResupplyMe\n");
	Syscall.CG_SendConsoleCommand("bind KP_UPARROW vsay FTDeployLandmines\n");
	Syscall.CG_SendConsoleCommand("bind KP_PGUP vsay FTCoveringFire\n");
	Syscall.CG_SendConsoleCommand("bind KP_DEL vsay FTfollowme\n");
	Syscall.CG_SendConsoleCommand("bind 0 vsay Bye\n");
	Syscall.CG_SendConsoleCommand("bind + vsay Hi\n");

	// register all the medias we use
	RegisterMedia();

	 //register all sounds
	Sounds.RegisterSounds();

	eth32.cg.clientNum = clientNum;

	// reset snap to avoid trying to use an old one from prior map
	eth32.cg.snap = NULL;
	wantPBQuery = false;
	Engine.frameN = 0;

	eth32.cg.self = &eth32.cg.players[clientNum];

	// set rendering/drawing info
	eth32.cg.screenXScale = eth32.game.glconfig->vidWidth / 640.0f;
	eth32.cg.screenYScale = eth32.game.glconfig->vidHeight / 480.0f;

	// center of screen
	eth32.cg.centerX = (eth32.game.glconfig->vidWidth/2) / eth32.cg.screenXScale;
	eth32.cg.centerY = (eth32.game.glconfig->vidHeight/2) / eth32.cg.screenYScale;

	// must be called after glconfig, screenXScale, and screenYScale are valid
	Gui.Init();
	Sounds.Init();
	Tools.LoadSettings(true);
	Spectator.ClearSpectators();

	if (eth32.cgMod->type == MOD_NOQUARTER)
		eth32.cg.ws_max = WS_NQ_MAX;
	else if (eth32.cgMod->type == MOD_ETMAIN || eth32.cgMod->type == MOD_ETPUB || eth32.cgMod->type == MOD_ETPRO)
		eth32.cg.ws_max = WS_ETM_MAX;
	else if (eth32.cgMod->type == MOD_JAYMOD1 || eth32.cgMod->type == MOD_JAYMOD2)
		eth32.cg.ws_max = WS_JAY_MAX;
	else
		eth32.cg.ws_max = -1;

	entGrenades.clear();
}

void CEngine::CG_Init_Post(int serverMessageNum, int serverCommandSequence, int clientNum)
{
#ifdef ETH32_PRELIM_OFFSETS
	offset_cginit = false;
#endif

	memset(Tools.auxSpecList, 0, sizeof(Tools.auxSpecList));
	Spectator.LaunchThread();

	// place some critical checks here, and abort if they fail
	// so we dont have to continually check integrity elsewhere
	if (!eth32.et->inButtons) FATAL_ERROR("inButtons not defined")
	if (!eth32.game.cg_fov) FATAL_ERROR("cg_fov not found")

	eth32.cg.firstle = orig_CG_AllocLocalEntity();
	eth32.cg.highle = eth32.cg.firstle;
	eth32.cg.firstle->endTime = 0;

	//kobj: etpro workaround
	if (eth32.cgMod->type == MOD_ETPRO && eth32.cgMod->crc32 == 0xc10e7d8c){
		int i;

		// this value is &character->hudhead in CG_RegisterCharacter
		for (i=0; i<MAX_CLIENTS; i++)
			eth32.cg.players[i].headModel = *(qhandle_t *)((uint32)eth32.cg.module + 0x5ee0ac8);

#ifdef ETH32_DEBUG
		if (!eth32.cg.players[0].headModel)
			Debug.Log("Error: etpro head hModel not found!");
		else
			Debug.Log("Etpro head hModel is %i", eth32.cg.players[0].headModel);
#endif
	}

	Tools.SetUserCvars();

	// unreference our pk3 so we don't get kicked
	Hook.UnreferenceBadPk3();

	Aimbot.SetSelf(clientNum);
	Aimbot.SetUserCrouchStatus(false);
	Aimbot.SetAttackButton( (void *)eth32.et->inButtons );
	Aimbot.SetCrouchButton( (void *)((uint32)eth32.et->inButtons-sizeof(kbutton_t)) );

	eth32.cgameLoaded = true;
	sprintf(eth32.server, "%i.%i.%i.%i:%i", ((et_netadr_t *)eth32.et->server_ip)->ip[0], ((et_netadr_t *)eth32.et->server_ip)->ip[1], ((et_netadr_t *)eth32.et->server_ip)->ip[2], ((et_netadr_t *)eth32.et->server_ip)->ip[3], htons(((et_netadr_t *)eth32.et->server_ip)->port));

	const char *info = ConfigString(CS_SERVERINFO);

	// retrieve server name
	strcpy( eth32.cg.serverName, Tools.CleanString(Info_ValueForKey(info, "sv_hostname")));
	// retrieve map name
	strcpy( eth32.cg.serverMapName, Tools.CleanString(Info_ValueForKey(info, "mapname")));

#ifdef ETH32_DEBUG
	Debug.Log("Server: %s IP: %s Map: %s", eth32.cg.serverName, eth32.server, eth32.cg.serverMapName);
#endif

	eth32.cg.cgTime = 0;

	char *str = Info_ValueForKey( info, "sv_punkbuster" );
	if( str && *str && atoi( str ) )
		eth32.cg.pbserver = true;

	SetConsoleShader();

	if(!eth32.cgMod->predictedPS)
		eth32.cg.predictedps = &eth32.cg.snap->ps;
}

void CEngine::SetWeaponSpread()
{
	switch (eth32.cg.currentWeapon->type) {
		case WT_PISTOL:
			eth32.cg.spreadIndex *= eth32.settings.spread;
			break;
		case WT_SMG:
			if (eth32.cg.currentWeapon->attribs & WA_OVERHEAT)	// sten
				eth32.cg.spreadIndex *= eth32.settings.spread;
			else							// other smg
				eth32.cg.spreadIndex *= eth32.settings.spread;
			break;
		case WT_RIFLE:
			eth32.cg.spreadIndex = 250.0;
			break;
		case WT_SNIPER:
			eth32.cg.spreadIndex = 0.0;
			break;
		case WT_MG:
			eth32.cg.spreadIndex = 2500.0;
			break;
		case WT_SHOTGUN:
			eth32.cg.spreadIndex = 10000.0;		// guess
			break;
		default:
			eth32.cg.spreadIndex = 0.0;
	}

	// for simplicity's sake assume >lvl3 light weapons
	eth32.cg.spreadIndex *= 0.65;
}

int CEngine::CG_Shutdown(void)
{
	#ifdef ETH32_DEBUG
		Debug.Log("CG_Shutdown()");
	#endif

	vector<CArty*>::iterator itArty;
	for(itArty = Arty.begin(); itArty != Arty.end(); itArty++)
		delete *itArty;
	Arty.clear();

	vector<CMortar*>::iterator itMortar;
	for(itMortar = Mortar.begin(); itMortar != Mortar.end(); itMortar++)
		delete *itMortar;
	Mortar.clear();

	Hook.hookCgame(false);
	eth32.cgameLoaded = false;

	Tools.SaveSettings();
	Gui.Shutdown();
	Sounds.ShutDown();

	int r = orig_CG_vmMain(CG_SHUTDOWN);

	eth32.cg.hasInit = false;

	forward = NULL;
	pDrawPlayer = NULL;
	drawWeapon = false;

	Spectator.KillThread();

	memset(Tools.auxSpecList, 0, sizeof(Tools.auxSpecList));

	#ifdef ETH32_DEBUG
		Debug.Log("CG_Shutdown completed");
	#endif
	return r;
}

// generic obituary handler
void CEngine::Obituary(int attacker, int target)
{
	if (attacker >= 0 && attacker < MAX_CLIENTS)
		eth32.cg.players[target].stats.killer = attacker;
	else
		eth32.cg.players[target].stats.killer = -1;

	// reset the aimbot when we die
	if (target == eth32.cg.clientNum) {
		multiKill = 0;
		Aimbot.Respawn();

		if (eth32.settings.autoTapout)
			Syscall.SendClientCommand("forcetapout");
	}

	if (target == attacker) {
		eth32.cg.players[target].stats.spree = 0;
		eth32.cg.players[target].stats.suicides++;

	}
	else {
		eth32.cg.players[target].stats.spree = 0;
		eth32.cg.players[target].stats.deaths++;
		eth32.cg.players[target].stats.kdRatio = (float)eth32.cg.players[target].stats.kills / eth32.cg.players[target].stats.deaths;

		if (attacker >= 0 && attacker < MAX_CLIENTS) {	// make sure attacker wasn't world or tank etc
			eth32.cg.players[attacker].stats.spree++;
			if (eth32.cg.players[attacker].stats.spree > eth32.cg.players[attacker].stats.longestSpree)
				eth32.cg.players[attacker].stats.longestSpree = eth32.cg.players[attacker].stats.spree;
			eth32.cg.players[attacker].stats.kills++;
			eth32.cg.players[attacker].stats.kdRatio = (float)eth32.cg.players[attacker].stats.kills / (eth32.cg.players[attacker].stats.deaths > 0 ? eth32.cg.players[attacker].stats.deaths : 1);

			if (attacker == eth32.cg.clientNum) {
				eth32.cg.players[target].stats.my_deaths++;

				stats_t *targ = &eth32.cg.players[target].stats;
				targ->threat = targ->my_deaths == 0.f ? 999.0 : (float)targ->my_kills / ((float)targ->my_deaths);

				memcpy(&eth32.stats, &eth32.cg.players[eth32.cg.clientNum].stats, sizeof(stats_t));

				multiKill++;
				lastKill = eth32.cg.time;
			}
		}
	}
}

// attempt to parse etpub's server-side obit message generation
// when no EV_OBITUARY is send.
void CEngine::ParseServerSideObituary(char *msg)
{
	int i, j;
	char *l1, *l2;

	for (i=0; i<MAX_CLIENTS; i++) {
		if (!IS_INFOVALID(i))
			continue;
		l1 = strstr(msg, eth32.cg.players[i].name);
		if (l1) {
			for (j=0; j<MAX_CLIENTS; j++) {
				if (j == i || !IS_INFOVALID(j))
					continue;

				l2 = strstr(msg, eth32.cg.players[j].name);
				if (l2) {
					// assume victim comes before attack in msg
					if (l2 > l1)
						Obituary(j, i);
					else
						Obituary(i, j);
					return;
				}
			}

			// only 1 player mentioned
			if (j==MAX_CLIENTS)
				return;
			return;
		}
	}
}

void CEngine::ParseWeaponStats()
{
	if (eth32.cg.ws_max < 0)
		return;

	int i, iArg = 1;
	int nClientID = atoi(Syscall.CG_Argv(iArg++));
	int nRounds = atoi(Syscall.CG_Argv(iArg++));
	int weaponMask = atoi(Syscall.CG_Argv(iArg++));
	player_t *player = &eth32.cg.players[nClientID];
	weapStat_t *ws = &player->stats.weap[0];

	if(weaponMask != 0) {
		for(i=0; i<eth32.cg.ws_max; i++) {
			if(weaponMask & (1 << i)) {
				ws[i].hits = atoi(Syscall.CG_Argv(iArg++));
				ws[i].shots = atoi(Syscall.CG_Argv(iArg++));
				ws[i].kills = atoi(Syscall.CG_Argv(iArg++));
				ws[i].deaths = atoi(Syscall.CG_Argv(iArg++));
				ws[i].headshots = atoi(Syscall.CG_Argv(iArg++));
			}
		}
	}

	int kills;
	// dont count sniper weapons

	player->stats.bulletsFiredWS =
		ws[WS_LUGER].shots + ws[WS_COLT].shots + ws[WS_MP40].shots +
		ws[WS_THOMPSON].shots +	ws[WS_STEN].shots + ws[WS_FG42].shots;

	player->stats.bulletsHitWS =
		ws[WS_LUGER].hits + ws[WS_COLT].hits + ws[WS_MP40].hits +
		ws[WS_THOMPSON].hits +	ws[WS_STEN].hits + ws[WS_FG42].hits;

	player->stats.headshotsWS =
		ws[WS_LUGER].headshots + ws[WS_COLT].headshots + ws[WS_MP40].headshots +
		ws[WS_THOMPSON].headshots +	ws[WS_STEN].headshots + ws[WS_FG42].headshots;

	kills =
		ws[WS_LUGER].kills + ws[WS_COLT].kills + ws[WS_MP40].kills +
		ws[WS_THOMPSON].kills +	ws[WS_STEN].kills + ws[WS_FG42].kills;

	if (eth32.cgMod->type == MOD_NOQUARTER) {
		player->stats.bulletsFiredWS += ws[WS_STG44].shots + ws[WS_STEN_MKII].shots + ws[WS_BAR].shots + ws[WS_MP34].shots;
		player->stats.bulletsHitWS += ws[WS_STG44].hits + ws[WS_STEN_MKII].hits + ws[WS_BAR].hits + ws[WS_MP34].hits;
		player->stats.headshotsWS += ws[WS_STG44].headshots + ws[WS_STEN_MKII].headshots + ws[WS_BAR].headshots + ws[WS_MP34].headshots;
		kills += ws[WS_STG44].kills + ws[WS_STEN_MKII].kills + ws[WS_BAR].kills + ws[WS_MP34].kills;
	} else if (eth32.cgMod->type == MOD_JAYMOD1 || eth32.cgMod->type == MOD_JAYMOD2) {

	}

	if (player->stats.bulletsHitWS)
		player->stats.bulletAcc = 100.0*(float)player->stats.bulletsHitWS/(float)player->stats.bulletsFiredWS;
	if (kills)
		player->stats.hr_kills = (float)player->stats.headshotsWS / (float)kills;
	if (player->stats.bulletsFiredWS)
		player->stats.hr_shots = 100.0f*(float)player->stats.headshotsWS / (float)player->stats.bulletsFiredWS;
	if (player->stats.bulletsHitWS)
		player->stats.hr_hits = 100.0f*(float)player->stats.headshotsWS / (float)player->stats.bulletsHitWS;
}

float CEngine::FrameCorrectionTime(int n)
{
	int frTime = 0;

	if (n > 0){
		for (int i=0; i<n; i++)
			frTime += frameTimes[(frameN-i) % MAX_FRAME_TIMES];
		return 0.001f*(float)frTime;
	} else {
		float avg = 0;
		float std = 0;

		for(int i=0; i<MAX_FRAME_TIMES; i++)
			avg += frameTimes[i];
		avg = avg / (float)(MAX_FRAME_TIMES-1);

		for(int i=0; i<MAX_FRAME_TIMES; i++)
			std += pow(((float)frameTimes[i]-avg),2);
		std = sqrt(std/((float)(MAX_FRAME_TIMES-2)));

		return 0.002f*(avg+0.5*std);
	}
}

// special class of visuals that cannot be done preframe or postframe
void CEngine::GentityDecal(void)
{
	vector<gentity_t*>::iterator itGentity;
	gentity_t *gentity;
    vec4_t projection; 		// facing direction
    vec3_t end;
    vec4_t dlightCol; 		// color of decal
    vec3_t dlightOrigin; 	// origin - ground counted with cgtrace
    trace_t tr;

    VectorSet(projection, 0 , 0 , -1);

    dlightCol[3] = 0.4f;
    dlightCol[0] = 1.0;
    dlightCol[1] = dlightCol[2] = 0.0;

    if (eth32.settings.grenadeDlight && eth32.settings.drawHackVisuals) {
	for(itGentity = entGrenades.begin(); itGentity != entGrenades.end(); itGentity++)
	{
		gentity = *(itGentity);
		VectorCopy(gentity->lerpOrigin, end);
		end[2] -= 10000.0;
		projection[3] = 280.0f;
		orig_CG_Trace(&tr, gentity->lerpOrigin, NULL, NULL, end, -1, MASK_MISSILESHOT);

		VectorCopy(gentity->lerpOrigin, end);
		end[2] += 50.0f;

		if (gentity->currentState->teamNum == *eth32.cg.self->team)
			Syscall.R_AddLightToScene(end, 260.0f, 5.0, 0.0, 1.0, 0.0, 0, REF_FORCE_DLIGHT);
		else
			Syscall.R_AddLightToScene(end, 260.0f, 5.0, 1.0, 0.0, 0.0, 0, REF_FORCE_DLIGHT);
	}
    }

	vector<CArty*>::iterator itArty;
	itArty = Arty.begin();
	while (1) {
		if (itArty == Arty.end())
			break;

		CArty *arty = *itArty;
		if (arty->Finished()) {
			delete arty;
			Arty.erase(itArty);
			itArty = Arty.begin();
			continue;
		}

		ON_VISUALS {
			// draw main area of effect while arty is active
			vec3_t end;
			VectorCopy(arty->Pos(), end);
			end[2] += 50.0f;

			if (arty->Team() == *eth32.cg.self->team)
				Syscall.R_AddLightToScene(end, 610.0f, 5.0, 0.0, 1.0, 0.0, 0, REF_FORCE_DLIGHT);
			else
				Syscall.R_AddLightToScene(end, 610.0f, 5.0, 1.0, 0.0, 0.0, 0, REF_FORCE_DLIGHT);

			// draw bomb hotspots
			end[2] -= 50.0f;
			projection[3] = 40.0f;
	    		dlightCol[3] = 0.4f;
	    		dlightCol[0] = 1.0;
	    		dlightCol[1] = dlightCol[2] = 0.0;
			Syscall.R_ProjectDecal(eth32.cg.media.disk, 1, (vec3_t *)end, projection, dlightCol, 10, 1);
		}
		itArty++;
	}

	vector<CMortar*>::iterator itMortar;
	itMortar = Mortar.begin();
	while (1) {
		if (itMortar == Mortar.end())
			break;

		CMortar *mortar = *itMortar;

		if (mortar->Finished()) {
			delete mortar;
			Mortar.erase(itMortar);
			itMortar = Mortar.begin();
			continue;
		}

		ON_VISUALS {
			vec3_t end;
			VectorCopy(mortar->ImpactPos(), end);
			if (mortar->Team() == *eth32.cg.self->team)
				Syscall.R_AddLightToScene(end, 360.0f, 5.0, 0.0, 1.0, 0.0, 0, REF_FORCE_DLIGHT);
			else
				Syscall.R_AddLightToScene(end, 360.0f, 5.0, 1.0, 0.0, 0.0, 0, REF_FORCE_DLIGHT);

			// mark impact center
			projection[3] = 60.0f;
	  		dlightCol[3] = 1.0f;
	    		dlightCol[2] = 0.0;
	    		dlightCol[0] = dlightCol[1] = 1.0;
			Syscall.R_ProjectDecal(eth32.cg.media.circle, 1, (vec3_t *)end, projection, dlightCol, 10, 1);
		}
		itMortar++;
	}
}

void CEngine::PreFrame(int serverTime)
{
	// X    0 -> 180 -> -180 -> 0
	// Y	-90 -> 0 -> 90

	eth32.cg.ourViewAngleX = *eth32.game.viewAngleX + SHORT2ANGLE(eth32.cg.snap->ps.delta_angles[1]);
	eth32.cg.ourViewAngleY = *eth32.game.viewAngleY + SHORT2ANGLE(eth32.cg.snap->ps.delta_angles[0]);

	if (eth32.cg.ourViewAngleX > 180.0)
		eth32.cg.ourViewAngleX -= 360.0;
	if (eth32.cg.ourViewAngleX < -180.0)
		eth32.cg.ourViewAngleX += 360.0;


	eth32.cg.frametime = serverTime - eth32.cg.time;
	eth32.cg.time = serverTime;

	// if spec, set our team to team allies for chams and other team dependent funcs
	eth32.cg.team = *eth32.cg.players[eth32.cg.clientNum].team;
	if (eth32.cg.team == TEAM_SPECTATOR)
		eth32.cg.team = TEAM_ALLIES;

	frameN++;
	frameTimes[frameN % MAX_FRAME_TIMES] = eth32.cg.frametime;

	numFramePlayers = 0;
	pDrawPlayer = NULL;
	playerDrawn = false;
	
	// request weapon stats
	if (!eth32.cg.wantStats && (serverTime - eth32.cg.lastStatsReq > 2000)) {
		Syscall.SendClientCommand("sgstats %d", eth32.cg.clientNum);
		eth32.cg.lastStatsReq = serverTime;
	}

	this->GetSpawntimes();

	Gui.PreFrame();
	Aimbot.PreFrame();
	Engine.setMuzzle();
	Cam.resetInfo();
}

// i know it looks silly, but text esp must really be called in different place
void CEngine::ArtyEsp()
{
	vector<CArty*>::iterator itArty;
	// draw text esp
	for(itArty = Arty.begin(); itArty != Arty.end(); itArty++){
		CArty *arty = *itArty;

		int screenx, screeny;
		vec3_t end;

		VectorCopy(arty->Pos(), end);

		if(Draw.worldToScreen(end, &screenx, &screeny)) {
			char buf[32];
			vec4_t color;
			color[0] = color[1] = 1.0;
			color[2] = 0.0f;
			color[3] = 0.7f;
			sprintf(buf, "[A] - %.1f", (float)arty->GetImpactTime()/1000.0f);
			Draw.TextCenter(screenx, screeny + 2, 0.20, buf, color, qfalse, qfalse, &eth32.cg.media.fontArial);
		}
	}
}

void CEngine::RunArty(gentity_t *gentity)
{
	if (!eth32.settings.artyMarkers)
		return;

	if(gentity->currentState->eType == ET_MISSILE && gentity->currentState->weapon == WP_ARTY){
		bool found = false;
		vector<CArty*>::iterator itArty;

		for(itArty = Arty.begin(); itArty != Arty.end(); itArty++){
			if ((*itArty)->Compare(gentity)) {
				found = true;

				// bomb is not a spotter round
				if (gentity->currentState->otherEntityNum2 != 1)
					(*itArty)->AddBomb(gentity);
			}
		}

		// first or spotter bomb
		if (gentity->currentState->otherEntityNum2 == 1 && !found) {
			if (!found) {
				CArty *newArty = new CArty(eth32.cg.time, gentity->currentState->pos.trTime, gentity->currentState->teamNum, gentity->currentState->pos.trBase);
				Arty.push_back(newArty);
			}
		}
	}
}

void CEngine::PostFrame(void)
{
	int clientNum;
	player_t *player;

	if (!cgHasFirst) {
		cgHasFirst = true;
		eth32.cg.zerohour = eth32.cg.time;
		eth32.settings.doNamesteal = false;
	}

	eth32.cg.limbo = 0;
	for(int i=0; i<MAX_CLIENTS; i++) {
		if (!IS_INFOVALID(i))
			eth32.cg.players[i].playerType = PLAYER_NORMAL;

		if (IS_SAMETEAM(i, eth32.cg.clientNum) && *eth32.cg.players[i].health == -1)
			eth32.cg.limbo++;
	}

	if (eth32.cg.weaponList && eth32.cg.snap && eth32.cg.snap->ps.weapon < eth32.cg.numWeapons)
		eth32.cg.currentWeapon = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];
	else
		eth32.cg.currentWeapon = &nullWeapon;

	// traverse captured players list
	for (int i=0 ; i<numFramePlayers ; i++) {
		clientNum = framePlayers[i];
		player = &eth32.cg.players[clientNum];

		if (eth32.settings.drawHeadHitbox)
			ON_VISUALS Aimbot.DrawHeadBox( player->clientNum, eth32.settings.drawHeadAxes );
		if (eth32.settings.drawBodyHitbox)
			ON_VISUALS Aimbot.DrawBodyBox( player->clientNum );

		if ( eth32.settings.drawHackVisuals && !Gui.IsLimboPanelUp() )
			PlayerEsp(player);

		// added distance here, it might be used for aimbot
		player->distance = VectorDistance(eth32.cg.refdef->vieworg, player->lerpOrigin);

		Aimbot.AddTarget(player);
	}

	// traverse entity list
	entGrenades.clear();

	for (int i=0; i<eth32.cg.snap->numEntities; i++) {
		gentity_t *gentity = &eth32.cg.gentity[eth32.cg.snap->entities[i].number];
	}

	ON_VISUALS ArtyEsp();
	ON_VISUALS DrawMortarTrace();
	
	// Alexplay: Smoke marker blinking color.
	// This must be set here, otherwise it crashes on etpub!
	VectorCopy(colorWhite,espPhase.color[0]);
	VectorCopy(colorRed,espPhase.color[1]);
	VectorCopy(colorMdRed,espPhase.color[2]);
	for(int i=0; i<3; i++) espPhase.color[i][3] = 1.f;	
	espPhase.phase++;
	if (espPhase.phase == 2) espPhase.phase = 0;
	
	if (eth32.settings.drawHackVisuals && !Gui.IsLimboPanelUp() && eth32.cg.snap->ps.pm_type != PM_INTERMISSION) {	
		if (eth32.settings.customXhair) 
			DrawCustomCrosshairs();
	}
	memset(eth32.cg.refEntities, 0, sizeof(eth32.cg.refEntities));
	eth32.cg.refEntitiesCount = 0;
	
	Cam.numFramePlayers = this->numFramePlayers;

	Tools.NameSteal(0);

	ON_VISUALS Gui.PostFrame();

	Aimbot.DrawGrenadelauncherTrace();
	Aimbot.DrawGrenadeTrace();

	Aimbot.PostFrame();

	refEntities.clear(); //end of frame time to clear vector
}

/*
 Handle server-generated garbage in the viewangles
 Every CG_DrawActiveFrame (or CL_Frame) the engine generates a usercmd
 and sends this to the server. Usercmd contains the ANGLE2SHORT of
 cl.viewangles, or game.viewAngleX and game.viewAngleY.
 these values continously increase or decrease depending on mouse moventment
 (or other user input) only.

 Server then calculates our viewangles. To handle the input correctly,
 in case userinput != server viewangles, server also sends delta_angles
 which is the amount to be added to our viewangles to arrive at the server
 dictated ps.viewangles.

 mousemovent (viewAngleX,Y) -> ucmd -> server -> add delta_angles -> ps.viewangles -> send back -> refdef

*/

void CEngine::CorrectServerAngles()
{
	playerState_t *ps = &eth32.cg.snap->ps;

	// force roll to 0 to keep normal view orientation and send appropriate correction next frame :)
	// fixme: restore mouse roll input at appropriate time (i.e. respawn)
	if (ps->delta_angles[ROLL] != 0 && !(ps->eFlags & EF_MG42_ACTIVE || ps->eFlags & EF_AAGUN_ACTIVE || ps->eFlags & EF_MOUNTEDTANK) ){
		ps->delta_angles[ROLL] = 0;
		// curiously, there is no way in Q3 to undo a roll with user input without a hax
		*(float *)(eth32.game.viewAngleX+4) = -ps->viewangles[ROLL];
	}

}

int CEngine::CG_DrawActiveFrame(int serverTime, int stereoView, int demoPlayback)
{
	// No need to mess with anything until game is ready (i.e. we have snapshot)
	if (!eth32.cg.snap)
		return orig_CG_vmMain(CG_DRAW_ACTIVE_FRAME, serverTime, stereoView, demoPlayback);
#ifdef ETH32_PRELIM_OFFSETS
	if (serverTime - eth32.cg.zerohour > 12000) {
		Hook.GetPredictedPlayerState();
		FATAL_ERROR("finished finding offsets")
	}
	return orig_CG_vmMain(CG_DRAW_ACTIVE_FRAME, serverTime, stereoView, demoPlayback);
#endif

	eth32.cg.cgTime = serverTime;
	Engine.PreFrame(serverTime);
	int oldHealth = eth32.cg.snap->ps.stats[STAT_HEALTH];
	int ret = orig_CG_vmMain(CG_DRAW_ACTIVE_FRAME, serverTime, stereoView, demoPlayback);
	Engine.PostFrame();
	
	ON_VISUALS eth32.cg.snap->ps.powerups[PW_BLACKOUT] = 0; //Speclock removal
	
	return ret;
}

void CEngine::StartPlayerCapture(centity_t *cent)
{
	unsigned int clientNum = Tools.ClientNumForAddr((uint32)cent);

	playerDrawn = false;
	pDrawPlayer = NULL;

	if (clientNum < 0 || clientNum >= MAX_CLIENTS || IS_DEAD(clientNum) || !IS_INFOVALID(clientNum))
		return;

	framePlayers[numFramePlayers] = clientNum;
	Cam.framePlayers[numFramePlayers] = clientNum;
	pDrawPlayer = &eth32.cg.players[clientNum];

	if (pDrawPlayer->playerType == PLAYER_NORMAL) {
		// Anti team kill
		// Return fire to teambleeders
		if (eth32.settings.antiTk) {
			if (pDrawPlayer->clientNum == eth32.cg.snap->ps.persistant[PERS_ATTACKER]) 
				pDrawPlayer->friendly = false;
			else
				pDrawPlayer->friendly = IS_FRIENDLY(clientNum);
		} else
			pDrawPlayer->friendly = IS_FRIENDLY(clientNum);
	} else
		pDrawPlayer->friendly = pDrawPlayer->playerType & PLAYER_FRIEND;

	pDrawPlayer->invulnerable = IS_INVULNERABLE(clientNum);

	// sol: added visibility check here so that we can use it for chams
	pDrawPlayer->visible = IsVisible(eth32.cg.refdef->vieworg, pDrawPlayer->lerpOrigin, pDrawPlayer->clientNum);
}

void CEngine::EndPlayerCapture(void)
{
	// will use next index if player was actually drawn...
	// this was a fix for TCE, though if the nixCoders visibility solution works I will remove the drawn check
	if (playerDrawn)
		numFramePlayers++;

#ifdef ETH32_DEBUG
	if (pDrawPlayer && pDrawPlayer->name)
		if (VectorCompare(vec3_origin, pDrawPlayer->orHead.origin))
			Debug.Log("ERROR! ERROR! ERROR! %i %s did not receive a HEAD!", pDrawPlayer->clientNum, pDrawPlayer->name);
#endif

	pDrawPlayer = NULL;
}

void CEngine::CapturePlayerHead(refEntity_t *refent)
{
	// pDrawPlayer is only valid when we are dealing with a player that is not dead
	// kobj: etpro does not seem to set hModel for head here??? grab it directly from cgame instead!
	if (pDrawPlayer) {
		pDrawPlayer->headModel = refent->hModel;
	}
}

void CEngine::CmdActivate()
{
	kbutton_t *aBtn = (kbutton_t *) ((uint32)eth32.et->inButtons + 6*sizeof(kbutton_t));
	aBtn->wasPressed = 1;
}

// when launched, mortar speed is sqrt(a^2 + (b^2-a^2) z^2), with a = 3000, b = 1500, and z <= 1
// once launched, its normal unpowered trajectory
void CEngine::DrawMortarTrace()
{
	vector<CMortar*>::iterator itMortar;
	for(itMortar = Mortar.begin(); itMortar != Mortar.end(); itMortar++){
		CMortar *mortar = *itMortar;

		mortar->DrawTrace();

		int screenx, screeny;
		vec3_t end;

		VectorCopy(mortar->ImpactPos(), end);

		if(Draw.worldToScreen(end, &screenx, &screeny)) {
			char buf[32];
			vec4_t color;
			color[0] = color[1] = 1.0;
			color[2] = 0.0f;
			color[3] = 0.7f;
			sprintf(buf, "[M] - %.1f", (float)mortar->TimeRemaining()/1000.0f);
			Draw.TextCenter(screenx, screeny + 2, 0.20, buf, color, qfalse, qfalse, &eth32.cg.media.fontArial);
		}
	}
}

void CEngine::AddRefEntity(refEntity_t *re)
{
	if(eth32.cgMod->type == MOD_TCE
		&& re->hModel == eth32.cg.media.medpackTCE
		)
	{
		if(re->hModel == 0)
			return;
		if (eth32.settings.wallhack)
			ON_VISUALS re->renderfx |= RF_DEPTHHACK;

		if(re->hModel == 0)
			return;
		if (eth32.settings.wallhack)
			ON_VISUALS re->renderfx |= RF_DEPTHHACK;
	}

	if (re->hModel == eth32.cg.media.grenadeAxisModel)
	{
		vec3_t end;
		VectorCopy(re->origin, end);
		end[2] += 50.0f;
	}

	if(re->hModel == eth32.cg.media.medPack || re->hModel == eth32.cg.media.ammoPack) // maybe move?
	{
		if(re->hModel == 0)
			return;

		if (eth32.settings.wallhack)
			ON_VISUALS re->renderfx |= RF_DEPTHHACK;
	}

	if(re->hModel == eth32.cg.media.mortarModel)
	{
		vec3_t axis, view;
		AxisToAngles(re->axis, axis);
		AxisToAngles(eth32.cg.refdef->viewaxis, view);

		if((int)view[1] == (int)axis[1]) {
			VectorCopy( re->origin, Cam.gentityOrigin );
			memcpy( Cam.gentityAxis, re->axis , 3*sizeof(vec3_t) );
		}
	}

	if (pDrawPlayer && (re->reType != RT_SPRITE)) { // this is part of a player and isn't an icon above it's head
		if (re->hModel == pDrawPlayer->headModel) {
			VectorCopy( re->origin, pDrawPlayer->orHead.origin );
			memcpy( pDrawPlayer->orHead.axis,re->axis, 3*sizeof(vec3_t) );
		}

		playerDrawn = true;

		if (eth32.settings.wallhack) {

				ON_VISUALS re->renderfx |= RF_DEPTHHACK;

		}
	}

	if (drawingCam)
		refEntities.push_back(*re);
}

void CEngine::AddPolyToScene(qhandle_t shader, polyVert_t *polyVert)
{
	// check is already done in syscall, but checking again in case we use this func for something else
	if (shader == eth32.cg.media.smokePuff) {
		// apply some transparency to the smoke
		for (int c=0; c < 4; c++)
			polyVert[c].modulate[3] = eth32.settings.smoketrnsp;
	}
}

void CEngine::DrawCustomCrosshairs()
{
	float X = 320;	// Crosshair X Position
	float Y = 240;	// Crosshair Y Position
	float Size = eth32.settings.crossSize;	// Crosshair Scale
	vec4_t xhairColor;
	VectorCopy(eth32.settings.xhairColor, xhairColor);
	xhairColor[3] = eth32.settings.xhairOpacity;

	switch(eth32.settings.customXhair) {
		case XHAIR_OFF:
			break;
		case XHAIR_SNIPER: {
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_SNIPER]);
			Syscall.R_SetColor(NULL);
			break;
		}
		case XHAIR_CROSS: {
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_CROSS]);
			Syscall.R_SetColor(NULL);
			break;
		}
		case XHAIR_CROSS2: {
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_CROSS2]);
			Syscall.R_SetColor(NULL);
			break;
		}
		case XHAIR_CROSS3: {
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_CROSS3]);
			Syscall.R_SetColor(NULL);
			break;
		}
		case XHAIR_CROSS4: {
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_CROSS4]);
			Syscall.R_SetColor(NULL);
			break;	
		}
		case XHAIR_DOT: 
			Syscall.R_SetColor(xhairColor);
			Draw.Pic(X-Size/2, Y-Size/2, Size, Size, eth32.cg.media.crosshair[XHAIR_DOT]);
			Syscall.R_SetColor(NULL);
			break;
	}
}

void CEngine::PlayerEsp(player_t *player)
{
	static vec4_t colorHidden = { 0.5, 0.5, 0.5, 0.5 };
	vec3_t espOrigin;

	VectorCopy(player->orHead.origin, espOrigin);
	espOrigin[2] += 15.f;	// move to above head in world

	if (Draw.worldToScreen(espOrigin, &player->screenX, &player->screenY)) {
		float *textColor = (player->visible) ? colorWhite : colorHidden;
		player->screenY -= 13;

		if (eth32.settings.espName && !Gui.IsLimboPanelUp()) {
			Draw.TextCenter(player->screenX, player->screenY, 0.15, player->name, textColor, (player->visible) ? qfalse : qtrue, qtrue, GUI_FONT);
		}

		if (eth32.settings.drawDisguised && *player->team != *eth32.cg.self->team && (player->currentState->powerups & (1 << PW_OPS_DISGUISED))) {
			player->screenY -= 16;
			Draw.Pic(player->screenX-6, player->screenY, 12, 12, eth32.cg.media.disguisedIcon);
		}
	}
}

void CEngine::EV_Obituary(entityState_t *es)
{
	int target;
	int attacker;
	if (eth32.cgMod->type != MOD_ETPRO) {
		target = es->otherEntityNum;
		attacker = es->otherEntityNum2;
	}
	else {	// etpro's struct is modified. entitynums are in place of time and time2 members when using SDK struct
		target = es->time;
		attacker = es->time2;
	}

	eth32.cg.serverObits = true;

	Obituary(attacker, target);
}

void CEngine::EV_Bullet(entityState_t *es, bool hitFlesh)
{
	int source = es->otherEntityNum;
	int fleshtarget = es->eventParm;

	if (source >= 0 && source < MAX_CLIENTS && fleshtarget >= 0 && fleshtarget < MAX_CLIENTS)
	{
		player_t *player = &eth32.cg.players[source];

		player->stats.bulletsFired++;
		if (hitFlesh && !IS_SAMETEAM(source,fleshtarget))  // don't want to log hits on teammates
			player->stats.bulletsHit++;

		player->stats.accuracy = ((float)player->stats.bulletsHit / player->stats.bulletsFired) * 100.f;

		switch(eth32.settings.hitsounds) {
			case HIT_OFF:
				break;
			case HIT_HEAD: {
				if (hitFlesh && source == eth32.cg.snap->ps.clientNum) {
					sfxHandle_t sound = IS_SAMETEAM(source, fleshtarget) ? eth32.cg.media.sounds.hitsounds[HIT_TEAM] : eth32.cg.media.sounds.hitsounds[HIT_HEAD];
					Syscall.S_StartLocalSound(sound, CHAN_LOCAL_SOUND);
				}
				break;
			}
			case HIT_HEADSHOT: {
				if (hitFlesh && source == eth32.cg.snap->ps.clientNum) {
					sfxHandle_t sound = IS_SAMETEAM(source, fleshtarget) ? eth32.cg.media.sounds.hitsounds[HIT_TEAM] : eth32.cg.media.sounds.hitsounds[HIT_HEADSHOT];
					Syscall.S_StartLocalSound(sound, CHAN_LOCAL_SOUND);
				}
				break;
			}
			case HIT_BODY: {
				if (hitFlesh && source == eth32.cg.snap->ps.clientNum) {
					sfxHandle_t sound = IS_SAMETEAM(source, fleshtarget) ? eth32.cg.media.sounds.hitsounds[HIT_TEAM] : eth32.cg.media.sounds.hitsounds[HIT_BODY];
					Syscall.S_StartLocalSound(sound, CHAN_LOCAL_SOUND);
				}
				break;
			}
		}
	}
}

void CEngine::GetSpawntimes()
{
	if (!eth32.cg.bools.spawntimerReady && (orig_CG_CalculateReinfTime != NULL))
		return;

	int temp = *eth32.cg.players[eth32.cg.snap->ps.clientNum].team;

	eth32.cg.spawnTimes[TEAM_FRIEND] = orig_CG_CalculateReinfTime(qfalse);
	*eth32.cg.players[eth32.cg.snap->ps.clientNum].team = (temp == TEAM_AXIS) ? TEAM_ALLIES : TEAM_AXIS;
	eth32.cg.spawnTimes[TEAM_ENEMY] = orig_CG_CalculateReinfTime(qfalse);
	*eth32.cg.players[eth32.cg.snap->ps.clientNum].team = temp;
}

const char *CEngine::ConfigString(int index) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS || !eth32.cg.gameState)
		return NULL;

	return eth32.cg.gameState->stringData + eth32.cg.gameState->stringOffsets[index];
}

void CEngine::SetRailTrailTime( cvar_t *cv ){ railtrailTime = cv; }

void CEngine::_CG_RailTrail( vec3_t start, vec3_t end, vec3_t color, int lifeTime )
{
	localEntity_t	*le;
	refEntity_t		*re;

	le = orig_CG_AllocLocalEntity();

	// get highest entry for local ents
	if ((uint32)le > (uint32)eth32.cg.highle)
		eth32.cg.highle = le;

	re = &le->refEntity;

	le->leType = LE_FADE_RGB;
	le->startTime = eth32.cg.time;
	le->endTime = eth32.cg.time + lifeTime;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	re->shaderTime = eth32.cg.time / 1000.0f;
	re->reType = RT_RAIL_CORE;
	re->customShader = eth32.cg.media.railCore;

	VectorCopy( start, re->origin );
	VectorCopy( end, re->oldorigin );

	if (eth32.settings.railWallhack)
		re->renderfx |= RF_DEPTHHACK;

	le->color[0] = color[0]; //R
	le->color[1] = color[1]; //G
	le->color[2] = color[2]; //B
	le->color[3] = 1.0f;

	AxisClear( re->axis );
}

void CEngine::MakeRailTrail( vec3_t start, vec3_t end, bool box, vec3_t color, int lifeTime )
{
	if (!orig_CG_AllocLocalEntity || !eth32.cg.media.railCore || !eth32.settings.drawHackVisuals)
		return;

	// use cg_railTrailTime cvar
	if ( lifeTime < 0 )
		lifeTime = eth32.game.cg_railTrailTime->integer;

	// just a line
	if (!box) {
		_CG_RailTrail( start, end, color, lifeTime );
		return;
	}
	vec3_t	diff, v1, v2, v3, v4, v5, v6;

	VectorSubtract(start, end, diff);

	VectorCopy(start, v1);
	VectorCopy(start, v2);
	VectorCopy(start, v3);
	v1[0] -= diff[0];
	v2[1] -= diff[1];
	v3[2] -= diff[2];
	_CG_RailTrail( start, v1, color, lifeTime);
	_CG_RailTrail( start, v2, color, lifeTime);
	_CG_RailTrail( start, v3, color, lifeTime);

	VectorCopy(end, v4);
	VectorCopy(end, v5);
	VectorCopy(end, v6);
	v4[0] += diff[0];
	v5[1] += diff[1];
	v6[2] += diff[2];
	_CG_RailTrail( end, v4, color, lifeTime);
	_CG_RailTrail( end, v5, color, lifeTime);
	_CG_RailTrail( end, v6, color, lifeTime);

	_CG_RailTrail( v2, v6, color, lifeTime);
	_CG_RailTrail( v6, v1, color, lifeTime);
	_CG_RailTrail( v1, v5, color, lifeTime);

	_CG_RailTrail( v2, v4, color, lifeTime);
	_CG_RailTrail( v4, v3, color, lifeTime);
	_CG_RailTrail( v3, v5, color, lifeTime);
}

bool CEngine::IsVisible(vec3_t start, vec3_t pt, int skipEntity)
{
	trace_t t;

	orig_CG_Trace(&t, start, NULL, NULL, pt, skipEntity, MASK_SHOT);

	return (t.fraction == 1.f);
}

// kobject: fixed setMuzzle() check SDK, uses muzzleTrace, not muzzlePoint
void CEngine::setMuzzle(void)
{
	VectorCopy( eth32.cg.refdef->vieworg, eth32.cg.muzzle );

	if (eth32.cgMod->type == MOD_ETPUB || eth32.cgMod->type == MOD_ETMAIN) {	
		if(eth32.cg.snap->ps.viewangles[PITCH] > 30.0 && (eth32.cg.snap->ps.eFlags & EF_CROUCHING))
			eth32.cg.muzzle[2] += 30.0 - eth32.cg.snap->ps.viewheight;
	}

	SnapVector(eth32.cg.muzzle);
}

int CEngine::findSatchel(void)
{
	for (int entityNum = 0; entityNum < MAX_GENTITIES; entityNum++) {
		if ((eth32.cg.gentity[entityNum].currentState->weapon == WP_SATCHEL)
				&& (eth32.cg.gentity[entityNum].currentState->clientNum == eth32.cg.snap->ps.clientNum)
				&& (!VectorCompare(eth32.cg.gentity[entityNum].lerpOrigin, vec3_origin)))
			return entityNum;
	}

	return -1;
}

int CEngine::CalcViewValues()
{
#ifdef ETH32_PRELIM_OFFSETS
	return orig_CG_CalcViewValues();
#endif

	playerState_t *ps = eth32.settings.interpolatedPs ? eth32.cg.predictedps : &eth32.cg.snap->ps;

	if (ps->eFlags & EF_MG42_ACTIVE || ps->eFlags & EF_AAGUN_ACTIVE || ps->eFlags & EF_MOUNTEDTANK ||
		(eth32.cg.snap->ps.pm_flags & PMF_LIMBO) || eth32.cg.snap->ps.stats[STAT_HEALTH] <= 0 )
		return orig_CG_CalcViewValues();

	// set refdef size
	eth32.cg.refdef->width = eth32.game.glconfig->vidWidth;
	eth32.cg.refdef->width &= ~1;

	eth32.cg.refdef->height = eth32.game.glconfig->vidHeight;
	eth32.cg.refdef->height &= ~1;

	eth32.cg.refdef->x = (eth32.game.glconfig->vidWidth - eth32.cg.refdef->width)/2;
	eth32.cg.refdef->y = (eth32.game.glconfig->vidHeight - eth32.cg.refdef->height)/2;

	if (eth32.cg.snap->ps.pm_type == PM_INTERMISSION){
		VectorCopy( ps->origin, eth32.cg.refdef->vieworg );
		VectorCopy( ps->viewangles, eth32.cg.refdefViewAngles );
		AnglesToAxis( eth32.cg.refdefViewAngles, eth32.cg.refdef->viewaxis );

		eth32.cg.refdef->fov_x = eth32.game.cg_fov->value;
		float x = eth32.cg.refdef->width / tan( eth32.cg.refdef->fov_x / 360 * M_PI );
		eth32.cg.refdef->fov_y = atan2( eth32.cg.refdef->height, x );
		eth32.cg.refdef->fov_y = eth32.cg.refdef->fov_y * 360 / M_PI;
		return 0;
	}

	VectorCopy( ps->origin, eth32.cg.refdef->vieworg );
	VectorCopy( ps->viewangles, eth32.cg.refdefViewAngles );

	eth32.cg.refdef->vieworg[2] += ps->viewheight;

	AnglesToAxis( eth32.cg.refdefViewAngles, eth32.cg.refdef->viewaxis );

	eth32.cg.refdef->fov_x = eth32.game.cg_fov->value;
	float x = eth32.cg.refdef->width / tan( eth32.cg.refdef->fov_x / 360 * M_PI );
	eth32.cg.refdef->fov_y = atan2( eth32.cg.refdef->height, x );
	eth32.cg.refdef->fov_y = eth32.cg.refdef->fov_y * 360 / M_PI;

	return 0;
}
