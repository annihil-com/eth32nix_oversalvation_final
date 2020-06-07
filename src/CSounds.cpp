// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#include "eth32.h"
#include "puresounds.h"

CSounds Sounds;

int csnd_hash[8192];

void CSounds::Init()
{
	gameSounds = NULL;
	nGameSounds = 0;
	memset(csnd_hash, 0, sizeof(csnd_hash));
}

void CSounds::ShutDown()
{
	if (gameSounds) free(gameSounds);
	nGameSounds = 0;
}

// all legit sounds pass trough here, therefore,
// sounds that dont exist csnd_hash are eth32 sounds
int CSounds::HookSounds(char *name, int compressed)
{
	if (!name)
		return 0;

	gameSounds = (eth32Sounds_t *)realloc(gameSounds, (nGameSounds+1)*sizeof(eth32Sounds_t));

	sfxHandle_t snd = Syscall.S_RegisterSound(name, (qboolean)compressed);

	int j = 0;
	gameSounds[nGameSounds].pure = false;
	while (pureSounds[j]) {
		if (!strcmp(pureSounds[j], name)){
			gameSounds[nGameSounds].pure = true;
			break;
		}
		j++;
	}

	csnd_hash[(int)snd] = nGameSounds;
	nGameSounds++;
	return snd;
}

bool CSounds::Process(int type, int *args)
{
	if (!gameSounds)
		return true;

	int sndHnd = (type == CG_S_STARTLOCALSOUND) ? args[0] : args[3];
	eth32Sounds_t *snd = &gameSounds[csnd_hash[sndHnd]];

	if (eth32.settings.pureSounds && !snd->pure)
		return false;

	return true;
}

void CSounds::RegisterSounds( void )
{
	// Hitsounds
	eth32.cg.media.sounds.hitsounds[HIT_HEAD] = Syscall.S_RegisterSound("sounds/hitsound/head.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_HEADSHOT] = Syscall.S_RegisterSound("sounds/hitsound/headshot.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_BODY] = Syscall.S_RegisterSound("sounds/hitsound/body.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_TEAM] = Syscall.S_RegisterSound("sounds/hitsound/team.wav", qtrue);

#ifdef ETH32_DEBUG
	Debug.Log("All Game Sounds Registered...");
#endif
}
