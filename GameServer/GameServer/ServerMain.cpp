#pragma once
#include <winsock2.h>
#include <iostream>

#include "../Logic/Logic/GameServer.h"

using GameServer = FPLogic::GameServer;

int main(void)
{
	GameServer gameServer;
	gameServer.Init();

	//gameServer.Run();

	std::cout << "press any key to exit...";
	getchar();

	gameServer.Stop();

	return 0;
}