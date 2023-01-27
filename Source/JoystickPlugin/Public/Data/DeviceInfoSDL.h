// JoystickPlugin is licensed under the MIT License.
// Copyright Jayden Maalouf. All Rights Reserved.

#pragma once

THIRD_PARTY_INCLUDES_START

#include "SDL_haptic.h"
#include "SDL_joystick.h"

THIRD_PARTY_INCLUDES_END

struct _SDL_Joystick;
using SDL_Joystick = struct _SDL_Joystick;
struct _SDL_Haptic;
using SDL_Haptic = struct _SDL_Haptic;

struct FDeviceInfoSDL
{
	FDeviceInfoSDL()
		: DeviceIndex(0)
		  , DeviceId(0)
		  , InstanceId(0)
		  , DeviceName("Unknown Device")
		  , Haptic(nullptr)
		  , Joystick(nullptr)
	{
	}

	int DeviceIndex;

	int DeviceId;
	int InstanceId;

	FString DeviceName;
	FGuid ProductId;

	SDL_Haptic* Haptic;
	SDL_Joystick* Joystick;
};
