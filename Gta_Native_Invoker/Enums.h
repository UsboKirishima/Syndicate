enum Buttons
{
	Button_A = 0xC1,
	Button_B = 0xC3,
	Button_X = 0xC2,
	Button_Y = 0xC0,
	Button_Back = 0xBF,
	Button_L1 = 0xC4,
	Button_L2 = 0xC6,
	Button_L3 = 0xC8,
	Button_R1 = 0xC5,
	Button_R2 = 0xC7,
	Button_R3 = 0xC9,
	Dpad_Up = 0xCA,
	Dpad_Down = 0xCB,
	Dpad_Left = 0xCC,
	Dpad_Right = 0xCD
};

enum ScaleformButtons
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_DOWN,
	BUTTON_DPAD_RIGHT,
	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_BLANK,
	BUTTON_DPAD_ALL,
	BUTTON_DPAD_UP_DOWN,
	BUTTON_DPAD_LEFT_RIGHT,
	BUTTON_LSTICK_UP,
	BUTTON_LSTICK_DOWN,
	BUTTON_LSTICK_LEFT,
	BUTTON_LSTICK_RIGHT,
	BUTTON_LSTICK,
	BUTTON_LSTICK_ALL,
	BUTTON_LSTICK_UP_DOWN,
	BUTTON_LSTICK_LEFT_RIGHT,
	BUTTON_LSTICK_ROTATE,
	BUTTON_RSTICK_UP,
	BUTTON_RSTICK_DOWN,
	BUTTON_RSTICK_LEFT,
	BUTTON_RSTICK_RIGHT,
	BUTTON_RSTICK,
	BUTTON_RSTICK_ALL,
	BUTTON_RSTICK_UP_DOWN,
	BUTTON_RSTICK_LEFT_RIGHT,
	BUTTON_RSTICK_ROTATE,
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_LB,
	BUTTON_LT,
	BUTTON_RB,
	BUTTON_RT,
	BUTTON_START,
	BUTTON_BACK,
	RED_BOX,
	RED_BOX_1,
	RED_BOX_2,
	RED_BOX_3,
	LOADING_HALF_CIRCLE_LEFT,
	ARROW_UP_DOWN,
	ARROW_LEFT_RIGHT,
	ARROW_ALL,
	LOADING_HALF_CIRCLE_LEFT_2,
	SAVE_HALF_CIRCLE_LEFT,
	LOADING_HALF_CIRCLE_RIGHT,
};

enum OptionType
{
	None,
	Checked,
	UnChecked,
	RadioChecked,
	RadioUnChecked,
};

struct Option
{
	char* Name;
	OptionType Type;
};
int CurrentCreatedMenuID = 0;

enum Submenus
{
	Closed,
	Main_Menu,
	Players_List,
	trolling,
	pvehcile,
	pattch,
	pweapons,
	pdrops,
	psvehicle,
	Player_Options,
	ANIMATIONS,
	Self_Options,
	funnyshit,
	editcunt,
	spawnv,
	allplayers,
	vebindsm,
	Settings,
	Settings_Banners,
	Settings_Body,
	Settings_Scroller,
	Settings_Indicators
};

enum Bones
{
	Head = 31086,
	Right_Wrist = 28422,
	Left_Wrist = 60309,
	Right_Hand = 57005,
	Chest = 24818,
	Mouth = 39317,
	Left_Hand = 18905,
	Left_Knee = 63931,
	Right_Knee = 36864,
	Left_Shoe = 14201,
	Right_Shoe = 52301,
	Crotch = 51826,
	Belly = 11816,
	Right_Shoulder = 40269,
	Right_Arm = 28252,
	Left_Shoulder = 45509,
	Left_Arm = 61163
};

unsigned int
DISABLE_CONTROL_ACTION = 0x1C2A3D0,
DISABLE_CONTROL_ACTION_DISABLE = 0x1591038,
DISABLE_CONTROL_ACTION_RPC = 0x400D04,

SET_CURRENT_PED_WEAPON = 0x1C2E9F8,
SET_CURRENT_PED_WEAPON_RPC = 0x4670AC,
SET_CURRENT_PED_WEAPON_DISABLE = 0x15B5BF0,
TunablePointer = 0x1E80774;