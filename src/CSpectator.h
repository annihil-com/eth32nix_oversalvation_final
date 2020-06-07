// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#pragma once
#include <pthread.h>

class CSpectator
{
	public:
		CSpectator();
		~CSpectator();
		void LaunchThread();
		void Init();
		void ClearSpectators();
		void AddSpectator(const char *name);
		void ParseServerMessage(char *buf);
		void KillThread();
		void UpdateStats(int bs, int rq, char *sv);
		void GetStats(int *bs, int *rq, char *sv);
		bool active;

		bool wantThreadExit;
	private:
		pthread_mutex_t *mutex;
		pthread_t sthread;
		
		int bytes, reqs;
		char server[64];
};

extern CSpectator Spectator;
