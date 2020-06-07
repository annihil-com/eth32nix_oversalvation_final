// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

// the nature of member function pointers and C function pointers
// prevent this from being really C++. Oh well, so long it works, right?

class CXCommands
{
public:
	CXCommands(void);

	void RegisterCommands();

private:
	bool cmdsRegistered;
};

extern CXCommands CCommands;
