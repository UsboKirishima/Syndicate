bool Write_Global(int a_uiGlobalID, int a_uiValue)
{
	int Ptr = *(int*)((TunablePointer - 0x04) + (((a_uiGlobalID / 0x40000) & 0x3F) * 4));
	if (Ptr != 0)
	{
		*(int*)(Ptr + ((a_uiGlobalID % 0x40000) * 4)) = a_uiValue;
		return true;
	}
	return false;
}
bool vinval_ = true;
void HookNative(int native, int dest) {
	int FuncBytes[1];
	FuncBytes[0] = *(int*)dest;
	memcpy((void*)native, FuncBytes, 4);
}
bool CompareStrings(char* One, char* Two)
{
	if (strncmp(One, Two, 25) == 0)
		return true;
	else
		return false;
}
void HookDISABLECONTROL(struct NativeArg_s* a_pArgs)
{
	int funcadd = DISABLE_CONTROL_ACTION_RPC;
	if (!CompareStrings("am_mp_property_int", SCRIPT::GET_THIS_SCRIPT_NAME()))
		((int(*)(int a, int b))&funcadd)(a_pArgs->ArgValues[0], a_pArgs->ArgValues[1]);
}
void HookCURRENTPEDWEAPONRPC(struct NativeArg_s* a_pArgs)
{
	//int funcadd = SET_CURRENT_PED_WEAPON_RPC;
	//if (!CompareStrings("am_mp_property_int", SCRIPT::GET_THIS_SCRIPT_NAME()))
	//{
	//printf("Calling: Ped: %i Hash: %X EquipNow: %i\n", a_pArgs->ArgValues[0], a_pArgs->ArgValues[1], a_pArgs->ArgValues[2]);
	//((void(*)(Ped a, Hash b, BOOL c))&funcadd)(a_pArgs->ArgValues[0], a_pArgs->ArgValues[1], a_pArgs->ArgValues[2]);
	//}
}
void drawNotification(char* msg)
{
	UI::_0xF42C43C7("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(msg);
	UI::_0x38F82261(3000, 1);
}
bool PlayerGodmod;
void Godmod()
{
	if (!PlayerGodmod)
	{
		drawNotification("Godmode ~g~Enabled");
		PlayerGodmod = true;
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), true);
		PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), false);
		PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), false);
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), 1);
		PLAYER::GIVE_PLAYER_RAGDOLL_CONTROL(PLAYER::PLAYER_ID(), true);
		PED::SET_PED_RAGDOLL_ON_COLLISION(PLAYER::PLAYER_PED_ID(), false);
	}
	else
	{
		drawNotification("Godmode ~r~Disabled");
		PlayerGodmod = false;
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), false);
		PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), false);
		PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), false);
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), 0);
		PLAYER::GIVE_PLAYER_RAGDOLL_CONTROL(PLAYER::PLAYER_ID(), false);
		PED::SET_PED_RAGDOLL_ON_COLLISION(PLAYER::PLAYER_PED_ID(), false);
	}
}

bool Radioing;
void Radio()
{
	if (!Radioing)
	{
		Radioing = true;
		AUDIO::SET_MOBILE_PHONE_RADIO_STATE(PLAYER::PLAYER_PED_ID());
		AUDIO::SET_USER_RADIO_CONTROL_ENABLED(PLAYER::PLAYER_PED_ID());
		AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(PLAYER::PLAYER_PED_ID()); 
	}
	else
	{
		Radioing = false;
	}
}
bool revealPeople;
void RevealPlayers()
{
	if (!revealPeople)
	{
		revealPeople = true;
		Write_Global(2394218 + 56, 2147483647);
		Write_Global(2390201 + PLAYER::PLAYER_ID() * 223 + 172, 1);
	}
	else
	{
		revealPeople = false;
		Write_Global(2390201 + PLAYER::PLAYER_ID() * 223 + 172, 0);
	}
}
bool doAnimation(char* anim, char* animid)
{
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(PLAYER::PLAYER_PED_ID());
	STREAMING::REQUEST_ANIM_DICT(anim);
	if (STREAMING::HAS_ANIM_DICT_LOADED((anim)))
	{
		AI::TASK_PLAY_ANIM(PLAYER::PLAYER_PED_ID(), anim, animid, 8.0f, 0.0f, -1, 9, 0, 0, 0, 0);
		return true;
	}
	return false;
}
int ClonePed(int PedHandle)
{
	return PED::CLONE_PED(PedHandle, ENTITY::GET_ENTITY_HEADING(PedHandle), 1, 1);
}
bool AttachDetach;
void AttachToPlayer(int me, int cli)
{
	Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(cli), true);
	if (AttachDetach == false)
	{
		ENTITY::ATTACH_ENTITY_TO_ENTITY(me, cli, 0, MyCoords.x, MyCoords.y, MyCoords.z, 0, 0, 0, 1, 1, 0, 0, 2, 1);
		drawNotification("Attached To Player");
		AttachDetach = true;
	}
	else if (AttachDetach == true)
	{
		ENTITY::DETACH_ENTITY(PLAYER::PLAYER_PED_ID(), true, true);
		drawNotification("Detched From Player");
		AttachDetach = false;
	}
}
void stopTasks(int Client)
{
	AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED(Client));
}
void MaxUpgrades(int cli, bool type = false)
{
	Vehicle VehicleHandle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(cli), true);
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
	if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(VehicleHandle, 0);
		VEHICLE::SET_VEHICLE_COLOURS(VehicleHandle, 120, 120);
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(VehicleHandle, "AYELMAO");
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(VehicleHandle, 1);
		VEHICLE::TOGGLE_VEHICLE_MOD(VehicleHandle, 18, 1);
		VEHICLE::TOGGLE_VEHICLE_MOD(VehicleHandle, 22, 1);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 16, 5, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 12, 2, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 11, 3, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 14, 14, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 15, 3, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 13, 2, 0);
		VEHICLE::SET_VEHICLE_WHEEL_TYPE(VehicleHandle, 6);
		VEHICLE::SET_VEHICLE_WINDOW_TINT(VehicleHandle, 5);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 23, 19, 1);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 0, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 1, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 2, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 3, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 4, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 5, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 6, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 7, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 8, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 9, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 10, 1, 0);
	}
}
void MaxUpgradesMe(int cli, bool type = false)
{
	Vehicle VehicleHandle = cli;
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehicleHandle);
	if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehicleHandle))
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(VehicleHandle, 0);
		VEHICLE::SET_VEHICLE_COLOURS(VehicleHandle, 120, 120);
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(VehicleHandle, "AYELMAO");
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(VehicleHandle, 1);
		VEHICLE::TOGGLE_VEHICLE_MOD(VehicleHandle, 18, 1);
		VEHICLE::TOGGLE_VEHICLE_MOD(VehicleHandle, 22, 1);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 16, 5, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 12, 2, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 11, 3, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 14, 14, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 15, 3, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 13, 2, 0);
		VEHICLE::SET_VEHICLE_WHEEL_TYPE(VehicleHandle, 6);
		VEHICLE::SET_VEHICLE_WINDOW_TINT(VehicleHandle, 5);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 23, 19, 1);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 0, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 1, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 2, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 3, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 4, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 5, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 6, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 7, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 8, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 9, 1, 0);
		VEHICLE::SET_VEHICLE_MOD(VehicleHandle, 10, 1, 0);
	}
}
void TeleportToClient(int Client)
{
	Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(Client), 1);
	if (PED::IS_PED_SITTING_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID()))
		ENTITY::SET_ENTITY_COORDS(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), Coords.x, Coords.y, Coords.z, 1, 0, 0, 1);
	else
		ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), Coords.x, Coords.y, Coords.z, 1, 0, 0, 1);
}
bool garage;
void ShootGarage()
{
	if (!garage)
	{
		garage = true;
		HookNative(DISABLE_CONTROL_ACTION, (int)HookDISABLECONTROL);
		HookNative(SET_CURRENT_PED_WEAPON, (int)HookCURRENTPEDWEAPONRPC);
	}
	else
	{
		garage = false;
		*(int*)(DISABLE_CONTROL_ACTION) = DISABLE_CONTROL_ACTION_DISABLE;
		*(int*)(SET_CURRENT_PED_WEAPON) = SET_CURRENT_PED_WEAPON_DISABLE;
	}
}
void KickFromCar(int VehicleHandle)
{
	AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED(VehicleHandle));
}
bool RequestNetworkControl(uint vehID)
{
	int Tries = 0;
	bool
		hasControl = false,
		giveUp = false;
	do
	{
		hasControl = NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(vehID);
		Tries++;
		if (Tries > 300)
			giveUp = true;
	} while (!hasControl && !giveUp);

	if (giveUp)
		return false;
	else return true;
}

bool doUndriveableCar = false;
bool undriveablePlayersCar(int Client)
{
	int PedHandle = PLAYER::GET_PLAYER_PED(Client);
	if (PED::IS_PED_IN_ANY_VEHICLE(PedHandle, 0))
	{
		int VehHandle = PED::GET_VEHICLE_PED_IS_IN(PedHandle, 0);
		if (NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehHandle))
		{
			VEHICLE::SET_VEHICLE_UNDRIVEABLE(VehHandle, true);
			return true;
		}
		else return false;
	}
	else
	{
		doUndriveableCar = false;
		return false;
	}
}
void TakeWeaponsClient(int Client)
{
	WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::GET_PLAYER_PED(Client), true);
}
void PaintVehRan()
{
	int VehID = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(VehID);
	if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(VehID))
	{
		VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(VehID, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
		VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(VehID, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
	}
}
int FOVCu = 0;
int FPCam = 0;
bool fPerson;
void FirstPerson()
{
	if (!fPerson)
	{
		fPerson = true;
		FPCam = CAM::CREATE_CAMERA(26379945, false);
		CAM::ATTACH_CAM_TO_PED_BONE(FPCam, PLAYER::PLAYER_PED_ID(), Head, 0, 0, 0, false);
		CAM::SET_CAM_NEAR_CLIP(FPCam, 0.225f);
		CAM::SET_CAM_ACTIVE(FPCam, true);
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
	}
	else
	{
		fPerson = false;
		CAM::DESTROY_CAM(FPCam, 0);
		CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
		FPCam = 0;
	}
}
bool invinciblecar = false;
void VehicleGodMode()
{
	int car;
	car = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
	{
		if (!invinciblecar)
		{
			ENTITY::SET_ENTITY_INVINCIBLE(car, true);
			invinciblecar = true;
			drawNotification("Vehicle Godmode: ~g~Enabled");
		}
		else
		{
			invinciblecar = false;
			ENTITY::SET_ENTITY_INVINCIBLE(car, false);
			drawNotification("Vehicle Godmode: ~r~Disabled");
		}
	}
}
void OpenDoors(int VehicleHandle)
{
	for (int DoorIndex = 0; DoorIndex <= 6; DoorIndex++)
		VEHICLE::SET_VEHICLE_DOOR_OPEN(VehicleHandle, DoorIndex, 0, true);
}
void vehlower()
{
	int ped = PLAYER::PLAYER_PED_ID();
	int veh = PED::GET_VEHICLE_PED_IS_IN(ped, true);
	if (PED::IS_PED_IN_VEHICLE(ped, veh, true))
	{
		ENTITY::APPLY_FORCE_TO_ENTITY(veh, 1, 0, 0, -0.2, 0, 0, 0, 1, false, true, true, true, true);
	}
}
bool drift;
void Drif(uint vehicle)
{
	if (CONTROLS::IS_CONTROL_PRESSED(2, 44))
	{
		VEHICLE::SET_VEHICLE_REDUCE_GRIP(vehicle, 3);
	}
	if (CONTROLS::IS_CONTROL_JUST_RELEASED(2, 44))
	{
		VEHICLE::SET_VEHICLE_REDUCE_GRIP(vehicle, 100);
	}
}
bool Speed = false;
void playerMovementSpeed()
{
	if (!Speed)
	{
		PLAYER::_0x825423C2(PLAYER::PLAYER_ID(), 2.50f);
		drawNotification("Super Run: ~g~Enabled");
		Speed = true;
	}
	else
	{
		PLAYER::_0x825423C2(PLAYER::PLAYER_ID(), 1);
		drawNotification("Super Run: ~r~Disabled");
		Speed = false;
	}
}
bool usbo = false;
void usboSuperMod() 
{
	if(!usbo)
	{
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), true);
		PLAYER::SET_VEHICLE_MOD(PLAYER::PLAYER_ID(), true);
	}
	else
	{
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), true);
		PLAYER::SET_VEHICLE_MOD(PLAYER::PLAYER_ID(), true);
	}
}
bool suicide = false;
void SpeedSuicide()
{
	if(!suicide)
	{
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), false);
		PLAYER::_0x825423C2(PLAYER::PLAYER_ID(), 300);
		drawNotification("For suicide pre L!");
		suicide = false;
		drawNotification("SuicideMod: ~r~Disabled");
	}

}
bool ThaMessia = false;
void ThaMessiaMod()
{
	if (!ThaMessia)
	{
		fPerson = true;
		FPCam = CAM::CREATE_CAMERA(26379945, false);
		CAM::ATTACH_CAM_TO_PED_BONE(FPCam, PLAYER::PLAYER_PED_ID(), Head, 0, 0, 0, false);
		CAM::SET_CAM_NEAR_CLIP(FPCam, 0.225f);
		CAM::SET_CAM_ACTIVE(FPCam, true);
		CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
	}
	else
	{
		fPerson = false;
		CAM::DESTROY_CAM(FPCam, 0);
		CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 0);
		FPCam = 0;
	}
}