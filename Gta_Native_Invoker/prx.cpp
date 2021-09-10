#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <string.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>
#include <ppu_intrinsics.h>
#include <stdarg.h>
#include <stdio.h>
#include <cstdlib>
#include "Enums.h"
#include "Natives.h"
#include "Dialog.h"
#include "Menu.h"
#include "Mods.h"

SYS_MODULE_INFO("GTAV_Native_Invoker", 0, 1, 1);
SYS_MODULE_START(PRX_ENTRY);

#pragma region Variables
Native_s** g_Natives;
int submenu = 0;
int submenuLevel;
int lastSubmenu[20];
int lastOption[20];
int currentOption;
int optionCount;
bool optionPress = false;
bool rightPress = false;
bool leftPress = false;
bool fastRightPress = false;
bool fastLeftPress = false;
bool squarePress = false;
int selectedPlayer;
bool menuSounds = true;
bool keyboardActive = false;
int keyboardAction;
int *keyboardVar = 0;
char *infoText;

//BOOLS
bool loop40K_;
bool NoCops = false;
int wantedlevel = 0;
bool SuperJump = false;
bool SuperRun = false;
bool slowmotion = false;
int spawnedped;
int SpawnedVeh;
uint32_t hash_veh;
bool SpawnVehicle;
bool PedSpawnVehicle;
bool SlingShot;
bool lel = false;
bool carinvstext;
bool TeleportToMe = false;
int TeleportVehicle = 0;
bool spawnsetswat;
bool frezzeplayer;
bool toogleesp_;
bool looprainbow_;
bool _lower;
bool SlingShot2;
bool hasNoCollision;
bool r3boost;
bool l3break;
bool l2bboost;
bool leftfix;

bool newTimerTick = true;
int maxTimerCount;
bool newTimerTick2 = true;
int maxTimerCount2;

int instructCount;
int mov;
bool instructionsSetupThisFrame;
bool squareInstruction;
bool lrInstruction;

int bannerTextRed = 255;
int bannerTextGreen = 255;
int bannerTextBlue = 255;
int bannerTextOpacity = 255;
int bannerTextFont = 1;
int bannerRectRed = 38;
int bannerRectGreen = 153;
int bannerRectBlue = 191;
int bannerRectOpacity = 255;
int backgroundRed = 0;
int backgroundGreen = 0;
int backgroundBlue = 0;
int backgroundOpacity = 10;
int bannerTextFont2 = 0;
int subr = 0;
int subb = 0;
int subg = 0;
int suba = 20;
int optionsRed = 255;
int optionsGreen = 255;
int optionsBlue = 255;
int optionsOpacity = 255;
int optionsFont = 4;
int scrollerRed = 38;
int scrollerGreen = 153;
int scrollerBlue = 191;
int scrollerOpacity = 255;
float Cunt = 0.080;
float Cunt2 = 0.225;
float Checkbox = 0.225;
float ScrollX = 0.0525f;
float menuXCoord = 0.145f;
float textXCoord = 0.06f;
float titlebox3 = 0.928f;
int maxOptions = 15;
#pragma endregion

#pragma region Internals
int write_process(void* destination, const void* source, size_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);
	return_to_user_prog(int);
}
void PatchInJump(uint64_t Address, int Destination, bool Linked)
{
	int FuncBytes[4];													// Use this data to copy over the address.
	Destination = *(int*)Destination;									// Get the actual destination address (pointer to void).
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);			// lis %r11, dest>>16
	if (Destination & 0x8000)											// if bit 16 is 1...
		FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF);					// addi %r11, %r11, dest&OxFFFF
	FuncBytes[2] = 0x7D6903A6;											// mtctr %r11
	FuncBytes[3] = 0x4E800420;											// bctr
	if (Linked)
		FuncBytes[3] += 1;												// bctrl
	write_process((void*)Address, FuncBytes, 4 * 4);
	//memcpy((void*)Address, FuncBytes, 4 * 4);
}
unsigned int FindNativeTableAddress()
{
	unsigned int  l_uiNativeTableAddress = 0;
	unsigned int l_uiStartAddress = 0x390000;
	unsigned int l_uiAddress = 0;
	for (unsigned int i = 0; i < 0x10000; i++)
	{
		if (*(unsigned int*)(l_uiStartAddress + i) == 0x3C6072BD)
			if (*(unsigned int*)(l_uiStartAddress + i + 8) == 0x6063E002)
			{
				l_uiAddress = *(unsigned int*)(l_uiStartAddress + i + 0x10);
				l_uiAddress &= 0xFFFFFF;
				l_uiAddress += (l_uiStartAddress + i + 0x10) - 1;
				printf("address: 0x%08x\n", l_uiAddress);
				break;
			}
	}
	l_uiNativeTableAddress = (*(unsigned int*)(l_uiAddress + 0x24) << 16) + (*(unsigned int*)(l_uiAddress + 0x2C) & 0xFFFF);
	l_uiNativeTableAddress -= 0x10000;
	return l_uiNativeTableAddress;
}
int NativeAddress(int Native, bool PatchInJump = true)
{
	int Hash = Native & 0xFF;
	int Table = *(int*)(FindNativeTableAddress() + (Hash * 4));
	while (Table != 0)
	{
		int NativeCount = *(int*)(Table + 0x20);
		for (int i = 0; i < NativeCount; i++)
		{
			if (*(int*)((Table + 0x24) + (i * 4)) == Native)
			{
				if (PatchInJump)
				{
					int NativeLocation = *(int*)(*(int*)((Table + 4) + (i * 4)));
					for (int i = 0; i < 50; ++i)
					{
						short CurrentPlace = *(short*)(NativeLocation + (i * 4));
						if (CurrentPlace == 0x4AE6 || CurrentPlace == 0x4AA8 || CurrentPlace == 0x4AE4 || CurrentPlace == 0x4AE5)
						{
							return (((*(int*)(NativeLocation + (i * 4))) - 0x48000001) + (NativeLocation + (i * 4))) - 0x4000000;
						}
					}
				}
				else
				{
					return *(int*)((Table + 4) + (i * 4));
				}

			}
		}
		Table = *(int*)(Table);
	}
}
void sleep(usecond_t time)  //1 second = 1000
{
	sys_timer_usleep(time * 1000);
}
void set_text_component(char *text)
{
	GRAPHICS::_0x3AC9CB55("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	GRAPHICS::_0x386CE0B8();
}
void DrawSprite(char* Streamedtexture, char* textureName, float X, float Y, float Width, float Height, float Rotation, int R = 255, int G = 255, int B = 255, int A = 255)
{
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(Streamedtexture, false);
	GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(Streamedtexture);
	GRAPHICS::DRAW_SPRITE(Streamedtexture, textureName, X, Y, Width, Height, Rotation, R, G, B, A);
}
void DRAW_SPRITE(char* TextOne, char* TextTwo, float X, float Y, float Width, float Height, float Rotation, int R = 255, int G = 255, int B = 255, int A = 255)
{
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(TextOne, false);
	if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(TextOne))
		GRAPHICS::DRAW_SPRITE(TextOne, TextTwo, X, Y, Width, Height, Rotation, R, G, B, A);
}
void DrawSprite1(char * Streamedtexture, char * textureName, float x, float y, float width, float height, float rotation, int r, int g, int b, int a)
{
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(Streamedtexture, false);
	if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(Streamedtexture));
	GRAPHICS::DRAW_SPRITE(Streamedtexture, textureName, x, y, width, height, rotation, r, g, b, a);
}
void drawText(char * text, int font, float x, float y, float scalex, float scaley, int r, int b, int g, int a, bool center)
{
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(scalex, scaley);
	UI::SET_TEXT_COLOUR(r, g, b, a);
	UI::SET_TEXT_WRAP(0.0f, 1.0f);
	UI::SET_TEXT_CENTRE(center);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_OUTLINE();
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_TEXT(x, y);
}
void drawText2(char * text, int font, float x, float y, float scalex, float scaley, int r, int b, int g, int a, bool center)
{
 UI::SET_TEXT_FONT(4);
 UI::SET_TEXT_SCALE(scalex, scaley);
 UI::SET_TEXT_COLOUR(r, g, b, a);
 UI::SET_TEXT_WRAP(0.0f, 1.0f);
 UI::SET_TEXT_CENTRE(center);
 //UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
 UI::SET_TEXT_OUTLINE();
 UI::_SET_TEXT_ENTRY("STRING");
 UI::_ADD_TEXT_COMPONENT_STRING(text);
 UI::_DRAW_TEXT(x, y);
}
void playSound(char* sound)
{
	if (menuSounds)
		AUDIO::PLAY_SOUND_FRONTEND(-1, sound, "HUD_FRONTEND_DEFAULT_SOUNDSET");
}
bool isNumericChar(char x)
{
	return (x >= '0' && x <= '9') ? true : false;
}
int StoI(char *str)
{
	if (*str == NULL)
		return 0;

	int res = 0,
		sign = 1,
		i = 0;

	if (str[0] == '-')
	{
		sign = -1;
		i++;
	}
	for (; str[i] != '\0'; ++i)
	{
		if (isNumericChar(str[i]) == false)
			return 0;
		res = res * 10 + str[i] - '0';
	}
	return sign*res;
}
char *ItoS(int num)
{
	char buf[30];
	snprintf(buf, sizeof(buf), "%i", num);
	return buf;
}
char* FtoS(float input)
{
	char returnvalue[64];
	int wholenumber = (int)input;
	input -= wholenumber;
	input *= 100;
	sprintf(returnvalue, "%d.%d", wholenumber, (int)input);
	return returnvalue;
}
void startKeyboard(int action, char *defaultText, int maxLength)
{
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(0, "FMMC_KEY_TIP8", "", defaultText, "", "", "", maxLength);
	keyboardAction = action;
	keyboardActive = true;
}
void changeSubmenu(int newSubmenu)
{
	lastSubmenu[submenuLevel] = submenu;
	lastOption[submenuLevel] = currentOption;
	currentOption = 1;
	submenu = newSubmenu;
	submenuLevel++;
}
void SetupTimer(int tickTime)
{
	if (newTimerTick) {
		maxTimerCount = GAMEPLAY::GET_GAME_TIMER() + tickTime;
		newTimerTick = false;
	}
}
bool IsTimerReady()
{
	return GAMEPLAY::GET_GAME_TIMER() > maxTimerCount;
}
void ResetTimer()
{
	newTimerTick = true;
}
void SetupTimer2(int tickTime)
{
	if (newTimerTick2) {
		maxTimerCount2 = GAMEPLAY::GET_GAME_TIMER() + tickTime;
		newTimerTick2 = false;
	}
}
bool IsTimerReady2()
{
	return GAMEPLAY::GET_GAME_TIMER() > maxTimerCount2;
}
void ResetTimer2()
{
	newTimerTick2 = true;
}
void addTitle(char *title)
{
	drawText(title, bannerTextFont, menuXCoord, 0.070f, 0.75f, 0.75f, bannerTextRed, bannerTextGreen, bannerTextBlue, bannerTextOpacity, true);
	GRAPHICS::DRAW_RECT(menuXCoord, 0.0850f, 0.19f, 0.080f, bannerRectRed, bannerRectGreen, bannerRectBlue, bannerRectOpacity);
}
void addSubTitle(char *texting)
{
	drawText(texting, 6, Cunt, 0.125, 0.40, 0.40f, bannerTextRed, bannerTextGreen, bannerTextBlue, bannerTextOpacity, true);
	GRAPHICS::DRAW_RECT(menuXCoord, 0.1410f, 0.190f, 0.034f, subr, subg, subb, suba);
}
#pragma region Option Helpers
void addOption(char *option,  char *info = NULL)
{
	optionCount++;
	if (currentOption == optionCount)
		infoText = info;
	if (currentOption <= maxOptions && optionCount <= maxOptions)
		drawText(option, optionsFont, textXCoord, (optionCount * 0.035f + 0.125f), 0.4f, 0.4f, optionsRed, optionsGreen, optionsBlue, optionsOpacity, false);
	else if ((optionCount > (currentOption - maxOptions)) && optionCount <= currentOption)
		drawText(option, optionsFont, textXCoord, ((optionCount - (currentOption - maxOptions)) * 0.035f + 0.125f), 0.4f, 0.4f, optionsRed, optionsGreen, optionsBlue, optionsOpacity, false);
}
void addSubmenuOption(char *option, int newSubmenu, char *info = NULL)
{
	addOption(option, info);
	if (currentOption == optionCount)
	{
		if (optionPress)
			changeSubmenu(newSubmenu);
	}
}
void addBoolOption(char *option, bool b00l, char *info = NULL)
{
	char buf[30];
	if (b00l)
	{
		snprintf(buf, sizeof(buf), "%s: On", option);
		addOption(buf, info);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%s: Off", option);
		addOption(buf, info);
	}
}
void addCheckBool(char *option, bool b00l1, char *info = NULL)
{
	char buf[30];
	if (b00l1)
	{
		snprintf(buf, sizeof(buf), "%s", option);
		DrawSprite1("commonmenu", "shop_box_tick", Checkbox, (optionCount * 0.035f + 0.175f), 0.03, 0.04, 0, 255, 255, 255, 255);
		addOption(buf, info);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%s", option);
		DrawSprite1("commonmenu", "shop_box_blank", Checkbox, (optionCount * 0.035f + 0.175f), 0.03, 0.04, 0, 255, 255, 255, 255);
		addOption(buf, info);
		
	}
}
template<typename TYPE>
void addCustomBoolOption(char *option, TYPE value, TYPE match, char *info = NULL)
{
	char buf[30];
	if (value == match)
	{
		snprintf(buf, sizeof(buf), "%s: On", option);
		addOption(buf, info);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%s: Off", option);
		addOption(buf, info);
	}
}
void addIntOption(char *option, int *var, int min, int max, bool keyboard, char *info = NULL)
{
	char buf[30];
	snprintf(buf, sizeof(buf), "%s: < %i >", option, *var);
	addOption(buf, info);
	if (currentOption == optionCount)
	{
		lrInstruction = true;
		if (rightPress)
		{
			playSound("NAV_UP_DOWN");
			if (*var >= max)
				*var = min;
			else
				*var = *var + 1;
		}
		else if (leftPress)
		{
			playSound("NAV_UP_DOWN");
			if (*var <= min)
				*var = max;
			else
				*var = *var - 1;
		}
		if (keyboard)
		{
			squareInstruction = true;
			if (squarePress)
			{
				startKeyboard(0, "", (strlen(ItoS(max)) + 1));
				keyboardVar = var;
			}
		}
	}
}
void addFloatOption(char *option, float *var, float min, float max, bool holdPress, char *info = NULL)
{
	char buf[30];
	snprintf(buf, sizeof(buf), "%s: < %s >", option, FtoS(*var));
	addOption(buf, info);
	if (currentOption == optionCount)
	{
		lrInstruction = true;
		if (holdPress)
		{
			if (fastRightPress)
			{
				playSound("NAV_UP_DOWN");
				if (*var >= max)
					*var = min;
				else
					*var = *var + 0.01f;
			}
			else if (fastLeftPress)
			{
				playSound("NAV_UP_DOWN");
				if (*var <= min)
					*var = max;
				else
					*var = *var - 0.01f;
			}
		}
		else
		{
			if (rightPress)
			{
				playSound("NAV_UP_DOWN");
				if (*var >= max)
					*var = min;
				else
					*var = *var + 0.01f;
			}
			else if (leftPress)
			{
				playSound("NAV_UP_DOWN");
				if (*var <= min)
					*var = max;
				else
					*var = *var - 0.01f;
			}
		}
	}
}
void addKeyboardOption(char *option, int action, char *defaultText, int maxLength, char *info = NULL)
{
	addOption(option, info);
	if (currentOption == optionCount)
	{
		if (optionPress)
			startKeyboard(action, defaultText, maxLength);
	}
}
#pragma endregion
bool Is_Player_Friend(Player player)
{
	bool BplayerFriend = false;
	bool bplayerFriend = false;
	int handle[76];
	NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &handle[0], 13);
	if (NETWORK::NETWORK_IS_HANDLE_VALID(&handle[0], 13))
	{
		BplayerFriend = NETWORK::NETWORK_IS_FRIEND(&handle[0]);
	}
	if (BplayerFriend)
		bplayerFriend = true;
	else
		bplayerFriend = false;

	return bplayerFriend;
}
int getOption()
{
	if (optionPress)
		return currentOption;
	else return 0;
}
void addVehOption(char *option, char *model, char* notifcation, char *info = NULL)
{
	addOption(option, info);
	if (currentOption == optionCount && optionPress)
	{
		//CREATE_VEHL(model);
		hash_veh = GAMEPLAY::GET_HASH_KEY(model);
		SpawnVehicle = true;
		drawNotification(notifcation);
	}
}
void monitorButtons()
{
	SetupTimer(20);
	if (submenu == Closed)
	{
		if (IsTimerReady())
		{
			if (CONTROLS::IS_CONTROL_PRESSED(0, Button_R1) && CONTROLS::IS_CONTROL_PRESSED(0, Dpad_Left))
			{
				submenu = Main_Menu;
				submenuLevel = 0;
				currentOption = 1;
				playSound("YES");
			}
			ResetTimer();
		}
	}
	else {
		if (IsTimerReady())
		{
			if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_B))
			{
				if (submenu == Main_Menu)
				{
					submenu = Closed;
				}
				else {
					submenu = lastSubmenu[submenuLevel - 1];
					currentOption = lastOption[submenuLevel - 1];
					submenuLevel--;
				}
				playSound("Back");
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_A))
			{
				optionPress = true;
				playSound("SELECT");
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Dpad_Up))
			{
				currentOption--;
				if (currentOption < 1)
				{
					currentOption = optionCount;
				}
				playSound("NAV_UP_DOWN");
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Dpad_Down))
			{
				currentOption++;
				if (currentOption > optionCount)
				{
					currentOption = 1;
				}
				playSound("NAV_UP_DOWN");
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Dpad_Right))
			{
				rightPress = true;
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Dpad_Left))
			{
				leftPress = true;
			}
			else if (CONTROLS::IS_CONTROL_PRESSED(0, Dpad_Right))
			{
				fastRightPress = true;
			}
			else if (CONTROLS::IS_CONTROL_PRESSED(0, Dpad_Left))
			{
				fastLeftPress = true;
			}
			else if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_X))
			{
				squarePress = true;
			}
			ResetTimer();
		}
	}
}
void normalMenuActions()
{
	UI::HIDE_HELP_TEXT_THIS_FRAME();
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(0);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(10);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(6);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(7);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(9);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(8);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 19);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 20);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 21);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 27);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 54);
	CONTROLS::DISABLE_CONTROL_ACTION(0, 123);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 124);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 125);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 126);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 138);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 139);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 140);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 177);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 178);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 179);
    CONTROLS::DISABLE_CONTROL_ACTION(0, 207);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 166);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 167);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 177);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 178);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 193);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 194);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 195);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 202);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 203);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 204);
    CONTROLS::SET_INPUT_EXCLUSIVE(2, 205);
    if (optionCount > maxOptions)
{
	GRAPHICS::DRAW_RECT(menuXCoord, (((maxOptions * 0.035f) / 2) + 0.159f), 0.19f, (maxOptions * 0.035f), backgroundRed, backgroundGreen, backgroundBlue, backgroundOpacity); //Background
	if (currentOption > maxOptions)
	{
		GRAPHICS::DRAW_RECT(ScrollX, ((maxOptions * 0.035f) + 0.1415f), 0.005f, 0.035f, scrollerRed, scrollerGreen, scrollerBlue, scrollerOpacity); //Scroller
	}
	else
	{
		GRAPHICS::DRAW_RECT(ScrollX, ((currentOption * 0.035f) + 0.1415f), 0.005f, 0.035f, scrollerRed, scrollerGreen, scrollerBlue, scrollerOpacity); //Scroller
	}
	if (currentOption != optionCount)
	{
	}
}
    else
    {
	    GRAPHICS::DRAW_RECT(menuXCoord, (((optionCount * 0.035f) / 2) + 0.159f), 0.19f, (optionCount * 0.035f), backgroundRed, backgroundGreen, backgroundBlue, backgroundOpacity); //Background
	    GRAPHICS::DRAW_RECT(ScrollX, ((currentOption * 0.035f) + 0.1415f), 0.005f, 0.035f, scrollerRed, scrollerGreen, scrollerBlue, scrollerOpacity); //Scroller
    }
    if (!GAMEPLAY::IS_STRING_NULL_OR_EMPTY(infoText))
{
	if (optionCount > maxOptions)
	{
		drawText(infoText, optionsFont, menuXCoord, ((maxOptions + 1) * 0.035f + 0.125f), 0.4f, 0.4f, bannerTextRed, bannerTextGreen, bannerTextBlue, bannerTextOpacity, true);
		GRAPHICS::DRAW_RECT(menuXCoord, (((maxOptions + 1) * 0.035f) + 0.1415f), 0.19f, 0.035f, subr, subg, subb, suba); //Info Box
	}
	else
	{
		drawText(infoText, optionsFont, menuXCoord, ((optionCount + 1) * 0.035f + 0.125f), 0.4f, 0.4f, bannerTextRed, bannerTextGreen, bannerTextBlue, bannerTextOpacity, true);
		GRAPHICS::DRAW_RECT(menuXCoord, (((optionCount + 1) * 0.035f) + 0.1415f), 0.19f, 0.035f, subr, subg, subb, suba); //Info Box
	}
}
	UI::DISPLAY_RADAR(false);
}
void resetVars()
{
	if (submenu != Closed)
		normalMenuActions();
	optionPress = false;
	rightPress = false;
	leftPress = false;
	fastRightPress = false;
	fastLeftPress = false;
	squarePress = false;
	infoText = NULL;
	instructionsSetupThisFrame = false;
	squareInstruction = false;
	lrInstruction = false;
}
#pragma endregion

void addPlayerVeh(char *option, char *model, char* notifcation, char *info = NULL)
{
	addOption(option, info);
	if (currentOption == optionCount && optionPress)
	{
		//CREATE_VEHL(model);
		hash_veh = GAMEPLAY::GET_HASH_KEY(model);
		PedSpawnVehicle = true;
		drawNotification(notifcation);
	}
}

#pragma region Other Loop
void otherLoop()
{
	if (loop40K_) { Vector3 mycoords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(selectedPlayer), true); STREAMING::REQUEST_MODEL(0x113FD533); if (STREAMING::HAS_MODEL_LOADED(0x113FD533) == 1) { OBJECT::CREATE_AMBIENT_PICKUP(0xCE6FDD6B, mycoords.x, mycoords.y, mycoords.z, 0, 40000, 0x113FD533, 0, 1);  STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(0x113FD533); } }
	if (NoCops) { PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()); }
	else { NoCops = false; }
	if (SuperJump) { GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID()); }
	if(SuperRun){ PLAYER::_0x825423C2(PLAYER::PLAYER_ID(), 10.0f); }
	if (slowmotion) { GAMEPLAY::SET_TIME_SCALE(0.2); }
	else { GAMEPLAY::SET_TIME_SCALE(1); }
	if (looprainbow_ == true) { PaintVehRan(); }
	if (_lower) { vehlower(); }
	if (l2bboost) {
		if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_L2)) VEHICLE::SET_VEHICLE_FORWARD_SPEED(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), -100);
	}
	if (r3boost) { if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_R2)) VEHICLE::SET_VEHICLE_FORWARD_SPEED(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), 100); }
	if (l3break) { if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Button_R3)) VEHICLE::SET_VEHICLE_FORWARD_SPEED(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), 0); }
	if (leftfix) { if (CONTROLS::IS_CONTROL_JUST_PRESSED(0, Dpad_Left)) VEHICLE::SET_VEHICLE_FIXED(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID())); }
	if (drift) { Drif(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0)); }
	else { drift = false; }
	if (SlingShot2)
	{
		NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true));
		if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true)));
		{
			ENTITY::APPLY_FORCE_TO_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true), 1, 0, 0, 20, 0, 0, 0, 1, false, true, true, true, true);
		}
		SlingShot2 = false;
	}
	if (PedSpawnVehicle)
	{
		Ped pedHandle = PLAYER::GET_PLAYER_PED(selectedPlayer);
		Vehicle vehHandle = PED::GET_VEHICLE_PED_IS_USING(pedHandle);

		if (!STREAMING::HAS_MODEL_LOADED(hash_veh))
			STREAMING::REQUEST_MODEL(hash_veh);
		else
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(pedHandle, true))
			{
				VEHICLE::DELETE_VEHICLE(&vehHandle);
			}
			Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(selectedPlayer), true);
			SpawnedVeh = VEHICLE::CREATE_VEHICLE(hash_veh, coords.x, coords.y, coords.z, 0, 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(SpawnedVeh))
			{
				ENTITY::SET_ENTITY_HEADING(SpawnedVeh, ENTITY::GET_ENTITY_HEADING(pedHandle));
				PED::SET_PED_INTO_VEHICLE(pedHandle, SpawnedVeh, -1);
				//MaxVeh();
				ENTITY::SET_ENTITY_INVINCIBLE(SpawnedVeh, true);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(SpawnedVeh, false);
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(SpawnedVeh, "AYELMAO");
				//if (spawncolor)
				//{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				//	}
				SpawnVehicle = false;
				//drawNotification("Vehicle: Spawned");
			}
		}
	}
	if (SpawnVehicle)
	{
		Ped pedHandle = PLAYER::PLAYER_PED_ID();
		Vehicle vehHandle = PED::GET_VEHICLE_PED_IS_USING(pedHandle);

		if (!STREAMING::HAS_MODEL_LOADED(hash_veh))
			STREAMING::REQUEST_MODEL(hash_veh);
		else
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(pedHandle, true))
			{
				VEHICLE::DELETE_VEHICLE(&vehHandle);
			}
			Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
			SpawnedVeh = VEHICLE::CREATE_VEHICLE(hash_veh, coords.x, coords.y, coords.z, 0, 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(SpawnedVeh))
			{
				ENTITY::SET_ENTITY_HEADING(SpawnedVeh, ENTITY::GET_ENTITY_HEADING(pedHandle));
				PED::SET_PED_INTO_VEHICLE(pedHandle, SpawnedVeh, -1);
				//MaxVeh();
				ENTITY::SET_ENTITY_INVINCIBLE(SpawnedVeh, true);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(SpawnedVeh, false);
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(SpawnedVeh, "AYELMAO");
				//if (spawncolor)
				//{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				//	}
				SpawnVehicle = false;
				//drawNotification("Vehicle: Spawned");
			}
		}
		if (TeleportToMe)
		{
			TeleportVehicle = PED::GET_VEHICLE_PED_IS_IN(selectedPlayer, true);
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(TeleportVehicle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(TeleportVehicle))
			{
				Vector3 MyLoc = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
				ENTITY::SET_ENTITY_COORDS(TeleportVehicle, MyLoc.x, MyLoc.y, MyLoc.z, true, false, false, true);
				TeleportToMe = false;
			}
		}
		if(keyboardActive)
		{
			if (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 1)
			{
				keyboardActive = false;
				switch (keyboardAction)
				{
				case 0: //addIntOption
					*keyboardVar = StoI(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
					break;
				case 1:
					NETWORKCASH::NETWORK_EARN_FROM_ROCKSTAR(StoI(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()));
					break;
				case 2:
					hash_veh = GAMEPLAY::GET_HASH_KEY((GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()));
					SpawnVehicle = true;
					break;
				case 9:
					strncpy((char*)0x41143344, GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT(), strlen(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()));
					*(char*)(0x41143344 + strlen(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) = 0;
					strncpy((char*)0x01FF248C, GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT(), strlen(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()));
					*(char*)(0x01FF248C + strlen(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) = 0;
					break;
				}
			}
			else if (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 2 || GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 3)
			{
				keyboardActive = false;
			}
		}
	}
}
namespace Mods
{
	void slingshot()
	{
		NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true));
		if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true)));
		{
			ENTITY::APPLY_FORCE_TO_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true), 1, 0, 0, 20, 0, 0, 0, 1, false, true, true, true, true);
		}
	}
	void DeleteVehicle(int VehicleHandle)
	{
		
	}
};
#pragma endregion
int Hook()
{
	UI::DISPLAY_RADAR(true);
	monitorButtons();
	otherLoop();
	optionCount = 0;
	switch (submenu)
	{
#pragma region Main Menu
	case Main_Menu:
		addTitle("Pandemonium");
		addSubTitle("Main Menu");
		addSubmenuOption("Player Options", Self_Options, " ");
		addSubmenuOption("Community Mods", funnyshit, " ");
		addSubmenuOption("Spawn Vehicle", spawnv, " ");
		addSubmenuOption("Vehicle Options", editcunt, " ");
		addOption("Weapon Options", " ");
		addSubmenuOption("Network Options", Players_List, " ");
		addOption("Model Changer", " ");
		addOption("Teleport Options", " ");
		addOption("Misc Options", " ");
		addOption("Object Options", " ");
		addOption("Animations Options", " ");
		addOption("Ped Options", " ");
		addOption("World Options", " ");
		break;
#pragma endregion
#pragma region Players List
	case Players_List:
		addTitle("Pandemonium");
		addSubTitle("Players List");
		for (int i = 0; i < 18; i++)
		{
			addOption(PLAYER::GET_PLAYER_NAME(i));
		}
		if (optionPress && PLAYER::IS_PLAYER_PLAYING(currentOption - 1))
		{
			selectedPlayer = currentOption - 1;
			changeSubmenu(Player_Options);
		}
		break;
#pragma endregion
#pragma region Player Options
	case Player_Options:
		addTitle("Pandemonium");
		addSubTitle(PLAYER::GET_PLAYER_NAME(selectedPlayer));
		addSubmenuOption("Trolling", trolling, " ");
		addSubmenuOption("Vehicle", pvehcile, " ");
		addSubmenuOption("Attachments", pattch, " ");
		addSubmenuOption("Weapons", pweapons, " ");
		addSubmenuOption("Drops", pdrops, " ");
		addSubmenuOption("Spawn Vehicle", psvehicle, " ");
		addOption("Teleport To Player", " ");
		switch (getOption())
		{
		case 7: TeleportToClient(selectedPlayer); break;
		}
		break;
	case trolling:
		addTitle("Pandemonium");
		addSubTitle("Trolling Options");
		addOption("Clone", " ");//done
		addOption("Attach to Player", " ");//done
		addOption("Explode", " ");//done
		addOption("SWAT Player", " ");//not working
		addCheckBool("Freeze Player", frezzeplayer, " ");//not working
		addOption("Kick Player", " ");//done
		addOption("Clear Tasks", " ");//done
		addCheckBool("Draw Arrow On Player", toogleesp_, " ");//not working
		switch (getOption())
		{
		case 1: ClonePed(PLAYER::GET_PLAYER_PED(selectedPlayer)); break;
		case 2: AttachToPlayer(PLAYER::PLAYER_PED_ID(), PLAYER::GET_PLAYER_PED(selectedPlayer)); break;
		case 3: Vector3 Pos1 = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(selectedPlayer), true); FIRE::ADD_EXPLOSION(Pos1.x, Pos1.y, Pos1.z, 29, 0.5f, true, false, 5.0f); break;
		case 4: break;
		case 5: break;
		case 6: NETWORK::NETWORK_SESSION_KICK_PLAYER(selectedPlayer); break;
		case 7: stopTasks(selectedPlayer); break;
		case 8: break;
		}
		break;
	case pvehcile:
		Vehicle VehicleHandle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true);
		addTitle("Pandemonium");
		addSubTitle("Vehicle Options");
		addOption("Teleport Car To Me", " ");
		addOption("Max Upgrade", " ");//done
		addOption("Fix Vehicle", " ");//done
		addOption("Delete Vehicle", " ");//done
		addOption("Stop Car", " ");//done
		addOption("Flip Vehicle Upright", " ");//done
		addOption("Paint Random", " ");//done
		addOption("Car Jump", " ");//done
		addOption("Kick Out Of Vehicle", " ");//done
		addCheckBool("Undriveable Car", doUndriveableCar, " ");//done
		switch (getOption())
		{
		case 1: if (PED::IS_PED_IN_ANY_VEHICLE(selectedPlayer, true))
		{
			TeleportToMe = true;
		}
				else
					drawNotification("~w~Player is not in a vehicle."); break;
		case 2: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
			{
				MaxUpgrades(selectedPlayer);
			}
			break;
		case 3: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
			{
				VEHICLE::SET_VEHICLE_FIXED(VehicleHandle);
			}
			break;
		case 4: if (ENTITY::DOES_ENTITY_EXIST(VehicleHandle))
		{
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(VehicleHandle, 1, 1);
			VEHICLE::DELETE_VEHICLE(&VehicleHandle);
		} break;
		case 5: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true), -0);
			}
			break;
		case 6: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
			{
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(selectedPlayer), true));
			}
			break;
		case 7: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
			{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(VehicleHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(VehicleHandle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
			}
			break;
		case 8: Mods::slingshot(); break;
		case 9: KickFromCar(selectedPlayer); break;
		case 10: undriveablePlayersCar(selectedPlayer); break;
		}
		break;
	case pattch:
		addTitle("Pandemonium");
		addSubTitle("Attachments");
		addOption("");
		switch (getOption())
		{

		}
		break;
	case pweapons:
		addTitle("Pandemonium");
		addSubTitle("Weapon Options");
		addOption("Take all Weapons", " ");
		switch (getOption())
		{
		case 1: TakeWeaponsClient(selectedPlayer); break;
		}
		break;
	case pdrops:
		addTitle("Pandemonium");
		addSubTitle("Drop Options");
		addCheckBool("Money Drop", loop40K_, " ");
		switch (getOption())
		{
		case 1: loop40K_ = !loop40K_; break;
		}
		break;
	case psvehicle:
		addTitle("Pandemonium");
		addSubTitle("Spawn Vehicle");
		addPlayerVeh("T20", "t20", "T20 Spawned", " ");//weird
		switch (getOption())
		{

		}
		break;
#pragma endregion
#pragma region Self Options
	case Self_Options:
		addTitle("Pandemonium");
		addSubTitle("Self Options");
		addCheckBool("Godmode", PlayerGodmod, " ");//done
		addCheckBool("Never Wanted", NoCops, " ");;//done
		addCheckBool("Reveal Players", revealPeople, " ");
		addOption("Change Model", " ");//submenu
		addOption("Animations", " ");//submenu
		addOption("Scenarios", " ");//submenu
		addCheckBool("Invisible", !ENTITY::IS_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID()), " ");//done
		addOption("Change Name", " ");//done
		addCheckBool("Super Jump", SuperJump, " ");//done
		addCheckBool("Super Run", Speed, " ");
		addCheckBool("Slow Motion", slowmotion, " ");//done
		addOption("Clone Player", " ");//done
		addOption("Not Targeted", " ");
		addOption("Explode", " ");//done
		addCheckBool("Mobile Radio", Radioing, " ");//done
		switch (getOption())
		{
		case 1: Godmod(); break;
		case 2: NoCops = !NoCops; break;
		case 3: RevealPlayers();
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), !ENTITY::IS_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID())); break;
		case 8: startKeyboard(9, "Change Name", 15); GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(0, "PM_NAME_CHALL", "", "", "", "", "", 40); keyboardActive = true; break;
		case 9: SuperJump = !SuperJump; break;
		case 10: playerMovementSpeed(); break;
		case 11: slowmotion = !slowmotion; break;
		case 12: ClonePed(PLAYER::PLAYER_PED_ID()); break;
		case 13: break;
		case 14: Vector3 Pos1 = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(PLAYER::PLAYER_PED_ID()), true); FIRE::ADD_EXPLOSION(Pos1.x, Pos1.y, Pos1.z, 29, 0.5f, true, false, 5.0f); break;
		case 15: Radio(); break;
		}
		break;
	case ANIMATIONS:
		addTitle("Pandemonium");
		addSubTitle("Animations");
		addOption("Nothing", " ");
		switch (getOption())
		{
		case 1: break;
		}
		break;
#pragma endregion
#pragma region Fun Shit
	case funnyshit:
		addTitle("Pandemonium");
		addSubTitle("Community Mods");
		addCheckBool("Shoot in Garage", garage, " ");
		addCheckBool("First Person", fPerson, " ");
		switch (getOption())
		{
		case 1: ShootGarage(); break;
		case 2: FirstPerson(); break;
		}
		break;
#pragma endregion
#pragma region Edit Carz
	case editcunt:
		addTitle("Pandemonium");
		addSubTitle("Edit Vehicle");
		addCheckBool("Godmode", invinciblecar, " ");
		addCheckBool("Invisibility", carinvstext, " ");
		addOption("Max Upgrade", " ");
		addOption("Fix Vehicle", " ");
		addOption("Paint Random", " ");
		addCheckBool("Rainbow Car", looprainbow_, " ");
		addOption("Flip Vehicle", " ");
		addOption("Open/Close Doors", " ");
		addCheckBool("Lower Vehicle", _lower, " ");
		addOption("Jump Car", " ");
		addSubmenuOption("Vehicle Binds", vebindsm, " ");
		addCheckBool("No Collision", hasNoCollision, " ");
		int VehID = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
		switch (getOption())
		{
		case 1: invinciblecar = !invinciblecar; break;
		case 2: carinvstext = !carinvstext; vinval_ = !vinval_; NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID)) {
				ENTITY::SET_ENTITY_VISIBLE(VehID, vinval_);
			}
			break;
		case 3: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID))
			{
				MaxUpgradesMe(VehID, true);
			}
			break;
		case 4: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID)) {
				VEHICLE::SET_VEHICLE_FIXED(VehID);
			}
			break;
		case 5: PaintVehRan(); break;
		case 6: looprainbow_ = !looprainbow_; break;
		case 7: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID)) {
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), true));
			}
			break;
		case 8: NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID))
			{
				OpenDoors(VehID);
			}
			break;
		case 9: _lower = !_lower; break;
		case 10: SlingShot2 = !SlingShot2; break;
		case 11: break;
		case 12: if (hasNoCollision)
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0));
			ENTITY::SET_ENTITY_COLLISION1(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), true);
			drawNotification("Collision: ~r~Disabled");
			hasNoCollision = false;
		}
				 else
				 {
					 NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0));
					 ENTITY::SET_ENTITY_COLLISION1(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), false);
					 drawNotification("Collision: ~g~Enabled");
					 hasNoCollision = true;
				 }
				 break;
		}
		break;
#pragma endregion
#pragma region Vehicle Binds
	case vebindsm:
		addTitle("Pandemonium");
		addSubTitle("Vehicle Binds");
		addCheckBool("Boost(R2)", r3boost);
		addCheckBool("Back Boost(L2)", l2bboost);
		addCheckBool("Insta Stop (R3)", l3break);
		addCheckBool("Fix (D-Left)", leftfix);
		addCheckBool("Drift (R1)", drift);
		switch (getOption())
		{
		case 1: r3boost = !r3boost; break;
		case 2: l2bboost = !l2bboost; break;
		case 3: l3break = !l3break; break;
		case 4: leftfix = !leftfix; break;
		case 5: drift = !drift; break;
		}
		break;
#pragma endregion
#pragma region Spawn Vehicle
	case spawnv:
		addTitle("Pandemonium");
		addSubTitle("Spawn Vehicle");
		addVehOption("T20", "t20", "T20 Spawned", " ");
		switch (getOption())
		{

		}
		break;
	}
#pragma endregion
	resetVars();
	return true;
}
void Thread(uint64_t nothing)
{
	sleep(30000); //Wait until native table initialization
	g_Natives = (Native_s**)FindNativeTableAddress();
	PatchInJump(NativeAddress(0x9FAB6729, true), (int)Hook, false); //IS_PLAYER_ONLINE
	sys_ppu_thread_exit(nothing);
}
extern "C" int PRX_ENTRY()
{
	sys_ppu_thread_t ThreadModuleID;
	sys_ppu_thread_create(&ThreadModuleID, Thread, 0, 10, 0x10, 0, "Pandemonium 1.27");
	Dialog::msgdialog_mode = Dialog::MODE_STRING_OK;
	Dialog::Show("Welcome to Pandemonium 1.27");
	return SYS_PRX_RESIDENT;
}