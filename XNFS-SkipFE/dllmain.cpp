// SkipFE for Most Wanted and Carbon.
// Support for other games coming soon.

#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include "..\includes\IniReader.h"

#ifdef GAME_MW
#include "MW_JoylogAddresses.h"
#endif

#ifdef GAME_CARBON
#include "Carbon_JoylogAddresses.h"
#endif

unsigned int SkipFE = 0;
unsigned int SkipFETrackNumber = DEFAULT_WORLD_TRACKNUM;
unsigned int SkipFENumPlayerCars = 1;
unsigned int SkipFENumAICars = 0;
unsigned int SkipFENumLaps = 2;
unsigned int SkipFERaceType = 0;
unsigned int SkipFEMaxCops = 0;
unsigned int SkipFEHelicopter = 0;
float SkipFETrafficDensity = 0;
float SkipFETrafficOncoming = 1.0;
unsigned int SkipFEDamageEnabled = 1;
unsigned int SkipFEDifficulty = 1;
unsigned int SkipFEDisableCops = 1;
unsigned int SkipFESplitScreen = 0;
unsigned int SkipFEPoint2Point = 0;
unsigned int SkipFELanguage = 0;
unsigned int SkipFETrackDirection = 0;
unsigned int SkipFEPovType1 = 2;
unsigned int SkipFEDisableTraffic = 0;

unsigned int IsSoundEnabled = 0;
unsigned int IsAudioStreamingEnabled = 0;
unsigned int IsSpeechEnabled = 0;
unsigned int IsNISAudioEnabled = 0;
unsigned int SkipMovies = 0;
unsigned int SkipNISs = 0;
unsigned int SkipCareerIntro = 0;
unsigned int SkipDDayRaces = 0;
unsigned int IsMemcardEnabled = 0;
unsigned int IsAutoSaveEnabled = 0;
unsigned int EmergencySaveMemory = 0;
unsigned int EnableJoylog = 0;

float SkipFEPlayerPerformance;
char* SkipFEPlayerCar = PLAYER1CAR;
char* SkipFEPlayer2Car = PLAYER2CAR;

#ifdef GAME_MW
char* SkipFEOpponentPresetRide;
#elif GAME_CARBON
char* SkipFEPlayerPresetRide;
char* SkipFEWingmanPresetRide;
char* SkipFEOpponentPresetRide0;
char* SkipFEOpponentPresetRide1;
char* SkipFEOpponentPresetRide2;
char* SkipFEOpponentPresetRide3;
char* SkipFEOpponentPresetRide4;
char* SkipFEOpponentPresetRide5;
char* SkipFEOpponentPresetRide6;
char* SkipFEOpponentPresetRide7;
#endif

bool UnlockAllThings = 0;
char* SkipFERaceIDRead;
float DebugStartLocationX = 0.0;
float DebugStartLocationY = 0.0;
float DebugStartLocationZ = 0.0;

bool bReplayingFlag = 0;
const char* JoylogReplayFile;
const char* JoylogCaptureFile;

char*(__cdecl* bStrCpy)(char* to, const char* from) = (char*(__cdecl*)(char*, const char*))BSTRPCY_ADDRESS;

void __declspec(naked) JoylogBufferCreator() // might not be right for Carbon... snipped code from MW
{
	_asm
	{
		mov eax, [esp+8]
		push esi
		mov esi, ecx
		mov ecx, [esp+8]
		push edi
		push ecx
		lea edx, [esi+0x14]
		push edx
		mov [esi+0x10], eax
		call bStrCpy
		xor eax, eax
		mov [esi], eax
		mov [esi+4], eax
		mov [esi+8], eax
		mov [esi+0xC], eax
		lea edi, [esi+0x118]
		add esp, 8
		mov ecx, 0x1000
		rep stosd
		pop edi
		mov eax, esi
		pop esi
		retn 8
	}
}

void*(__cdecl* bOpen)(const char* filename, int open_mode, int warn_if_cant_open) = (void*(__cdecl*)(const char*, int, int))BOPEN_ADDRESS;
int(__cdecl* GetQueuedFileSize)(const char* filename) = (int(__cdecl*)(const char*))GETQUEUEDFILESIZE_ADDRESS;
void(__thiscall* JoylogBuffer_JoylogBuffer)(void* JoylogBuffer, const char* filename, int top_position) = (void(__thiscall*)(void*, const char*, int))&JoylogBufferCreator;
void(__cdecl* bClose)(void *bFile) = (void(__cdecl*)(void*))BCLOSE_ADDRESS;



int bFileExists(const char* filename)
{
	FILE* fin = fopen(filename, "rb");
	if (!fin)
	{
		printf("ERROR: Can't find file for replaying: %s\n", filename);
		perror("ERROR");
		return 0;
	}
	else
	{
		fclose(fin);
		return 1;
	}
}

void __cdecl Joylog_Init()
{
	int v1;
	void* v2;
	void *v5;

	if (EnableJoylog)
	{
		if (bFileExists(JoylogReplayFile) && bReplayingFlag)
		{
			bOpen(JoylogReplayFile, 1, 1);
			v1 = GetQueuedFileSize(JoylogReplayFile);
			v2 = malloc(0x4118);
			if (v2)
				JoylogBuffer_JoylogBuffer(v2, JoylogReplayFile, v1);
			*(void**)JOYLOGREPLAYINGBUFFER_ADDRESS = v2;
			*(int*)JOYLOGREPLAYINGFLAG_ADDRESS = 1;
		}
		else
		{
			v2 = malloc(0x4118);
			if (v2)
				JoylogBuffer_JoylogBuffer(v2, JoylogCaptureFile, 0);
			*(void**)JOYLOGCAPTURINGBUFFER_ADDRESS = v2;
			*(int*)JOYLOGCAPTURINGFLAG_ADDRESS = 1;
			v5 = bOpen(JoylogCaptureFile, 6, 1);
			if (v5)
				bClose(v5);
			else
				*(int*)JOYLOGCAPTURINGFLAG_ADDRESS = 0;
		}
	}
}

int Init()
{
	CIniReader inireader("");

	EnableJoylog = inireader.ReadInteger("Joylog", "EnableJoylog", 0);
	bReplayingFlag = inireader.ReadInteger("Joylog", "ReplayingFlag", 0);
	JoylogReplayFile = inireader.ReadString("Joylog", "JoylogReplayFile", "ReplayJoylog.jlg");
	JoylogCaptureFile = inireader.ReadString("Joylog", "JoylogCaptureFile", "CaptureJoylog.jlg");

	SkipFE = inireader.ReadInteger("SkipFE", "SkipFE", 0);
	SkipFESplitScreen = inireader.ReadInteger("SkipFE", "SkipFESplitScreen", 0);
	SkipFEPovType1 = inireader.ReadInteger("SkipFE", "SkipFEPovType1", 2);
	SkipFEDamageEnabled = inireader.ReadInteger("SkipFE", "SkipFEDamageEnabled", 1);
	SkipFEDifficulty = inireader.ReadInteger("SkipFE", "SkipFEDifficulty", 1);
	SkipFELanguage = inireader.ReadInteger("SkipFE", "SkipFELanguage", 0);

	SkipFETrackNumber = inireader.ReadInteger("TrackSettings", "SkipFETrackNumber", DEFAULT_WORLD_TRACKNUM);
	SkipFERaceIDRead = inireader.ReadString("TrackSettings", "SkipFERaceID", NULL);
	SkipFENumLaps = inireader.ReadInteger("TrackSettings", "SkipFENumLaps", 2);
	SkipFETrackDirection = inireader.ReadInteger("TrackSettings", "SkipFETrackDirection", 0);
	SkipFERaceType = inireader.ReadInteger("TrackSettings", "SkipFERaceType", 0);
	SkipFEPoint2Point = inireader.ReadInteger("TrackSettings", "SkipFEPoint2Point", 0);

	SkipFEPlayerCar = inireader.ReadString("CarSettings", "SkipFEPlayerCar", PLAYER1CAR);
	SkipFEPlayer2Car = inireader.ReadString("CarSettings", "SkipFEPlayer2Car", PLAYER2CAR);
	SkipFEPlayerPerformance = inireader.ReadFloat("CarSettings", "SkipFEPlayerPerformance", 0.0);

#ifdef GAME_MW
	SkipFEOpponentPresetRide = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide", NULL);
#elif GAME_CARBON
	SkipFEPlayerPresetRide = inireader.ReadString("CarSettings", "SkipFEPlayerPresetRide", NULL);
	SkipFEWingmanPresetRide = inireader.ReadString("CarSettings", "SkipFEWingmanPresetRide", NULL);
	SkipFEOpponentPresetRide0 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide0", NULL);
	SkipFEOpponentPresetRide1 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide1", NULL);
	SkipFEOpponentPresetRide2 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide2", NULL);
	SkipFEOpponentPresetRide3 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide3", NULL);
	SkipFEOpponentPresetRide4 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide4", NULL);
	SkipFEOpponentPresetRide5 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide5", NULL);
	SkipFEOpponentPresetRide6 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide6", NULL);
	SkipFEOpponentPresetRide7 = inireader.ReadString("CarSettings", "SkipFEOpponentPresetRide7", NULL);
#endif
	SkipFENumPlayerCars = inireader.ReadInteger("CarSettings", "SkipFENumPlayerCars", 1);
	SkipFENumAICars = inireader.ReadInteger("CarSettings", "SkipFENumAICars", 0);
	
	SkipFEDisableTraffic = inireader.ReadInteger("TrafficSettings", "SkipFEDisableTraffic", 0);
	SkipFETrafficDensity = inireader.ReadFloat("TrafficSettings", "SkipFETrafficDensity", 0.0);
	SkipFETrafficOncoming = inireader.ReadFloat("TrafficSettings", "SkipFETrafficOncoming", 0.5);
	
	SkipFEMaxCops = inireader.ReadInteger("CopSettings", "SkipFEMaxCops", 0);
	SkipFEDisableCops = inireader.ReadInteger("CopSettings", "SkipFEDisableCops", 1);
	SkipFEHelicopter = inireader.ReadInteger("CopSettings", "SkipFEHelicopter", 0);
	

	IsSoundEnabled = inireader.ReadInteger("GameSettings", "IsSoundEnabled", 1);
	IsAudioStreamingEnabled = inireader.ReadInteger("GameSettings", "IsAudioStreamingEnabled", 1);
	IsSpeechEnabled = inireader.ReadInteger("GameSettings", "IsSpeechEnabled", 1);
	IsNISAudioEnabled = inireader.ReadInteger("GameSettings", "IsNISAudioEnabled", 1);
	SkipMovies = inireader.ReadInteger("GameSettings", "SkipMovies", 0);
	SkipCareerIntro = inireader.ReadInteger("GameSettings", "SkipCareerIntro", 0);
	SkipDDayRaces = inireader.ReadInteger("GameSettings", "SkipDDayRaces", 0);
	IsMemcardEnabled = inireader.ReadInteger("GameSettings", "IsMemcardEnabled", 1);
	IsAutoSaveEnabled = inireader.ReadInteger("GameSettings", "IsAutoSaveEnabled", 1);
	EmergencySaveMemory = inireader.ReadInteger("GameSettings", "EmergencySaveMemory", 0);
	UnlockAllThings = inireader.ReadInteger("GameSettings", "UnlockAllThings", 0);

	DebugStartLocationX = inireader.ReadFloat("WorldDebugLocation", "DebugStartLocationX", 0.0);
	DebugStartLocationY = inireader.ReadFloat("WorldDebugLocation", "DebugStartLocationY", 0.0);
	DebugStartLocationZ = inireader.ReadFloat("WorldDebugLocation", "DebugStartLocationZ", 0.0);

	*(unsigned int*)ENABLEJOYLOG_ADDRESS = EnableJoylog;

	*(unsigned int*)SKIPFE_ADDRESS = SkipFE;
	*(unsigned int*)SKIPFETRACKNUM_ADDRESS = SkipFETrackNumber;

	if (SkipFERaceIDRead[strlen(SkipFERaceIDRead) - 1] == '\n')
		SkipFERaceIDRead[strlen(SkipFERaceIDRead) - 1] = '\0';

	strncpy((char*)SKIPFERACEID_ADDRESS, SkipFERaceIDRead, 11);

	*(char**)SKIPFEPLAYERCAR_ADDRESS = SkipFEPlayerCar;
	*(char**)SKIPFEPLAYER2CAR_ADDRESS = SkipFEPlayer2Car;
	*(float*)SKIPFEPLAYERPERFORMANCE_ADDRESS = SkipFEPlayerPerformance;

#ifdef GAME_MW
	*(char**)SKIPFEOPPONENTPRESETRIDE_ADDRESS = SkipFEOpponentPresetRide;
#elif GAME_CARBON
	*(char**)SKIPFEOPPONENTPRESETRIDE0_ADDRESS = SkipFEOpponentPresetRide0;
	*(char**)SKIPFEOPPONENTPRESETRIDE1_ADDRESS = SkipFEOpponentPresetRide1;
	*(char**)SKIPFEOPPONENTPRESETRIDE2_ADDRESS = SkipFEOpponentPresetRide2;
	*(char**)SKIPFEOPPONENTPRESETRIDE3_ADDRESS = SkipFEOpponentPresetRide3;
	*(char**)SKIPFEOPPONENTPRESETRIDE4_ADDRESS = SkipFEOpponentPresetRide4;
	*(char**)SKIPFEOPPONENTPRESETRIDE5_ADDRESS = SkipFEOpponentPresetRide5;
	*(char**)SKIPFEOPPONENTPRESETRIDE6_ADDRESS = SkipFEOpponentPresetRide6;
	*(char**)SKIPFEOPPONENTPRESETRIDE7_ADDRESS = SkipFEOpponentPresetRide7;
#endif

	*(unsigned int*)SKIPFESPLITSCREEN_ADDRESS = SkipFESplitScreen;
	*(unsigned int*)SKIPFENUMPLAYERCARS_ADDRESS = SkipFENumPlayerCars;
	*(unsigned int*)SKIPFENUMAICARS_ADDRESS = SkipFENumAICars;
	*(unsigned int*)SKIPFENUMLAPS_ADDRESS = SkipFENumLaps;
	*(unsigned int*)SKIPFETRACKDIRECTION_ADDRESS = SkipFETrackDirection;
	*(unsigned int*)SKIPFEMAXCOPS_ADDRESS = SkipFEMaxCops;
	*(unsigned int*)SKIPFEDISABLECOPS_ADDRESS = SkipFEDisableCops;
	*(unsigned int*)SKIPFEHELICOPTER_ADDRESS = SkipFEHelicopter;
	*(unsigned int*)SKIPFEPOVTYPE1_ADDRESS = SkipFEPovType1;
	*(float*)SKIPFETRAFFICDENSITY_ADDRESS = SkipFETrafficDensity;
	*(unsigned int*)SKIPFEDISABLETRAFFIC_ADDRESS = SkipFEDisableTraffic;
	*(float*)SKIPFETRAFFICONCOMING_ADDRESS = SkipFETrafficOncoming;
	*(unsigned int*)SKIPFERACETYPE_ADDRESS = SkipFERaceType;
	*(unsigned int*)SKIPFEPOINT2POINT_ADDRESS = SkipFEPoint2Point;
	*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = SkipFEDifficulty;
	*(unsigned int*)SKIPFEDAMAGEENABLED_ADDRESS = SkipFEDamageEnabled;
	*(unsigned int*)SKIPFELANGUAGE_ADDRESS = SkipFELanguage;

	*(unsigned int*)ISSOUNDENABLED_ADDRESS = IsSoundEnabled;
	*(unsigned int*)ISAUDIOSTREAMINGENABLED_ADDRESS = IsAudioStreamingEnabled;
	*(unsigned int*)ISSPEECHENABLED_ADDRESS = IsSpeechEnabled;
	*(unsigned int*)ISNISAUDIOENABLED_ADDRESS = IsNISAudioEnabled;
	*(unsigned int*)ISMEMCARDENABLED_ADDRESS = IsMemcardEnabled;
	*(unsigned int*)ISAUTOSAVEENABLED_ADDRESS = IsAutoSaveEnabled;
	*(unsigned int*)EMERGENCYSAVEMEMORY_ADDRESS = EmergencySaveMemory;
	*(unsigned char*)UNLOCKALLTHINGS_ADDRESS = UnlockAllThings;
	*(unsigned int*)SKIPMOVIES_ADDRESS = SkipMovies;
	*(unsigned int*)SKIPCAREERINTRO_ADDRESS = SkipCareerIntro;

#ifdef GAME_MW
	*(unsigned int*)SKIPDDAYRACES_ADDRESS = SkipDDayRaces;
#endif

	*(float*)DEBUGX_ADDRESS = DebugStartLocationX;
	*(float*)DEBUGY_ADDRESS = DebugStartLocationY;
	*(float*)DEBUGZ_ADDRESS = DebugStartLocationZ;

	injector::MakeCALL(JOYLOGINITCALL_ADDRESS, Joylog_Init, true);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
		Init();
	}
	return TRUE;
}

