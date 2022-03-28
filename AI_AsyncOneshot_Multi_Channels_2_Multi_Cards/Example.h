#pragma once

#ifdef __linux__ 
	#include <unistd.h>
	#include <iostream>
	#include <fstream>
	#include "corelib.h"

	#define APNAME "[AP] :"
	#define DELAY_10_MSEC usleep(10000);
#else
	#include "corelib.h"
	#include <iostream>
	#include <fstream>
	#include <conio.h>

	#ifdef _WIN64
	#pragma comment(lib,"corelib64.lib")
	#elif _WIN32
	#pragma comment(lib,"corelib.lib")
	#endif

	#define DELAY_10_MSEC Sleep(10);
#endif

using namespace std;

#pragma region  AI_CALLBACK_INTERFACE
class AI_OverrunEvent : I_AICallback
{
public:
    int doEventProc(I_AITask*);
};

class AI_DataReadyEvent : I_AICallback
{
public:
    int doEventProc(I_AITask*);
};

class AI_TaskFinishEvent_dev0: I_AICallback
{
public:
    int doEventProc(I_AITask*);
};
class AI_TaskFinishEvent_dev1 : I_AICallback
{
public:
	int doEventProc(I_AITask*);
};
#pragma endregion

#pragma region  AO_CALLBACK_INTERFACE
class AO_TaskFinishEvent : I_AOCallback
{
public:
    int doEventProc(I_AOTask*);
};

class AO_DataReadyEvent : I_AOCallback
{
public:
    int doEventProc(I_AOTask*);
};
#pragma endregion

#pragma region  DI_CALLBACK_INTERFACE
class DI_TaskFinishEvent : I_DICallback
{
public:
    int doEventProc(I_DITask*);
};

class DI_OverrunEvent : I_DICallback
{
public:
    int doEventProc(I_DITask*);
};

class DI_DataReadyEvent : I_DICallback
{
public:
    int doEventProc(I_DITask*);
};
#pragma endregion

#pragma region  DO_CALLBACK_INTERFACE
class DO_TaskFinishEvent : I_DOCallback
{
public:
    int doEventProc(I_DOTask*);
};

class DO_DataReadyEvent : I_DOCallback
{
public:
    int doEventProc(I_DOTask*);
};
#pragma endregion