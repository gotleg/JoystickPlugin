
/*
*
* Copyright (C) <2014> samiljan <Sam Persson>, tsky <thomas.kollakowksy@w-hs.de>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

#include "JoystickInputDevice.h"

#include "DeviceSDL.h"
#include "JoystickFunctionLibrary.h"


FJoystickInputDevice::FJoystickInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) : MessageHandler(InMessageHandler)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::StartupModule() creating Device SDL"));

	DeviceSDL = MakeShareable(new FDeviceSDL(this));
	DeviceSDL->Init();
}

FJoystickInputDevice::~FJoystickInputDevice()
{
	DeviceSDL = nullptr;
}

void FJoystickInputDevice::Tick(float DeltaTime)
{
}

bool FJoystickInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FJoystickInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
}

void FJoystickInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values)
{
}

void FJoystickInputDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

#define LOCTEXT_NAMESPACE "JoystickNamespace"

void FJoystickInputDevice::InitInputDevice(const FDeviceInfoSDL &Device)
{
	int32 DeviceId = Device.DeviceId;
	FJoystickInfo DeviceInfo;

	DeviceInfo.Connected = true;
	DeviceInfo.DeviceId = DeviceId;
	DeviceInfo.Player = 0;

	DeviceInfo.ProductId = FDeviceSDL::DeviceIndexToGUID(Device.DeviceIndex);
	DeviceInfo.ProductName = Device.DeviceName.Replace(TEXT(" "), TEXT("")).Replace(TEXT("."), TEXT(""));
	DeviceInfo.DeviceName = DeviceInfo.ProductName;

	UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *DeviceInfo.DeviceName, DeviceId);
	JoystickDeviceInfo.Emplace(DeviceId, DeviceInfo);

	FJoystickDeviceData InitialState = DeviceSDL->InitialDeviceState(DeviceId);
	JoystickDeviceData.Emplace(DeviceId, InitialState);

	// create FKeyDetails for axis
	DeviceAxisKeys.Emplace(DeviceId);
	for (int32 iAxis = 0; iAxis < InitialState.Axes.Num(); iAxis++)
	{
		FString strName = FString::Printf(TEXT("Joystick_%s_%d_Axis%d"), *DeviceInfo.DeviceName, DeviceInfo.DeviceId, iAxis);
		UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId);
		DeviceAxisKeys[DeviceId].Add(FKey(FName(*strName)));

		if (!EKeys::GetKeyDetails(DeviceAxisKeys[DeviceId][iAxis]).IsValid())
		{
			FText textValue = FText::Format(LOCTEXT("Joystick_{0}_{1}_Axis{2}", "{0} {1} Axis {2}"), FText::FromString(DeviceInfo.ProductName), DeviceInfo.DeviceId, FText::AsNumber(iAxis));
			EKeys::AddKey(FKeyDetails(DeviceAxisKeys[DeviceId][iAxis], textValue, FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
		}
	}

	// create FKeyDetails for buttons
	DeviceButtonKeys.Emplace(DeviceId);
	for (int32 iButton = 0; iButton < InitialState.Buttons.Num(); iButton++)
	{
		FString strName = FString::Printf(TEXT("Joystick_%s_%d_Button%d"), *DeviceInfo.DeviceName, DeviceInfo.DeviceId, iButton);
		UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId);
		DeviceButtonKeys[DeviceId].Add(FKey(FName(*strName)));

		if (!EKeys::GetKeyDetails(DeviceButtonKeys[DeviceId][iButton]).IsValid())
		{
			FText textValue = FText::Format(LOCTEXT("Joystick_{0}_{1}_Button{2}", "{0} {1} Button {2}"), FText::FromString(DeviceInfo.ProductName), DeviceInfo.DeviceId, FText::AsNumber(iButton));
			EKeys::AddKey(FKeyDetails(DeviceButtonKeys[DeviceId][iButton], textValue, FKeyDetails::GamepadKey));
		}
	}

	FString _2DaxisNames[] = { TEXT("X"), TEXT("Y") };

	// create FKeyDetails for hats
	for (int32 iAxis = 0; iAxis < 2; iAxis++)
	{
		DeviceHatKeys[iAxis].Emplace(DeviceId);
		for (int32 iHat = 0; iHat < InitialState.Hats.Num(); iHat++)
		{
			FString strName = FString::Printf(TEXT("Joystick_%s_%d_Hat%d_%s"), *DeviceInfo.DeviceName, DeviceInfo.DeviceId, iHat, *_2DaxisNames[iAxis]);
			UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId);
			FKey key{ *strName };
			DeviceHatKeys[iAxis][DeviceId].Add(key);

			if (!EKeys::GetKeyDetails(key).IsValid())
			{
				FText textValue = FText::Format(LOCTEXT("Joystick_{0}_{1}_Hat{2}_{3}", "{0} {1} Hat {2} {3}"), FText::FromString(DeviceInfo.ProductName), DeviceInfo.DeviceId, FText::AsNumber(iHat), FText::FromString(_2DaxisNames[iAxis]));
				EKeys::AddKey(FKeyDetails(key, textValue, FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
			}
		}
	}

	// create FKeyDetails for balls
	for (int32 iAxis = 0; iAxis < 2; iAxis++)
	{
		DeviceBallKeys[iAxis].Emplace(DeviceId);
		for (int32 iBall = 0; iBall < InitialState.Balls.Num(); iBall++)
		{
			FString strName = FString::Printf(TEXT("Joystick_%s_%d_Ball%d_%s"), *DeviceInfo.DeviceName, DeviceInfo.DeviceId, iBall, *_2DaxisNames[iAxis]);
			UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId);
			FKey key{ *strName };
			DeviceBallKeys[iAxis][DeviceId].Add(key);

			if (!EKeys::GetKeyDetails(key).IsValid())
			{
				FText textValue = FText::Format(LOCTEXT("Joystick_{0}_{1}_Ball{2}_{3}", "{0} {1} Ball {2} {3}"), FText::FromString(DeviceInfo.ProductName), DeviceInfo.DeviceId, FText::AsNumber(iBall), FText::FromString(_2DaxisNames[iAxis]));
				EKeys::AddKey(FKeyDetails(key, textValue, FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

//Public API Implementation

void FJoystickInputDevice::JoystickPluggedIn(const FDeviceInfoSDL &DeviceInfoSDL)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), DeviceInfoSDL.DeviceId);

	InitInputDevice(DeviceInfoSDL);
}

void FJoystickInputDevice::JoystickUnplugged(int32 DeviceId)
{
	FJoystickInfo& InputDevice = JoystickDeviceInfo[DeviceId];
	InputDevice.Connected = false;
}

void FJoystickInputDevice::JoystickButton(int32 DeviceId, int32 Button, bool Pressed)
{
	FButtonData& state = JoystickDeviceData[DeviceId].Buttons[Button];
	state.PreviousButtonState = state.ButtonState;
	state.ButtonState = Pressed;
}

void FJoystickInputDevice::JoystickAxis(int32 DeviceId, int32 Axis, float Value)
{
	FAnalogData& state = JoystickDeviceData[DeviceId].Axes[Axis];
	state.PreviousValue = state.Value;
	state.Value = Value;
}

void FJoystickInputDevice::JoystickHat(int32 DeviceId, int32 Hat, EJoystickPOVDirection Value)
{
	FHatData& state = JoystickDeviceData[DeviceId].Hats[Hat];
	state.PreviousDirection = state.Direction;
	state.Direction = Value;
}

void FJoystickInputDevice::JoystickBall(int32 DeviceId, int32 Ball, FVector2D Value)
{
	FBallData& state = JoystickDeviceData[DeviceId].Balls[Ball];
	state.PreviousDirection = state.Direction;
	state.Direction = Value;
}

FJoystickDeviceData FJoystickInputDevice::GetDeviceData(int32 DeviceId)
{
	if (!JoystickDeviceData.Contains(DeviceId))
	{
		return FJoystickDeviceData();
	}

	return JoystickDeviceData[DeviceId];
}

FJoystickInfo FJoystickInputDevice::GetDeviceInfo(int32 DeviceId)
{
	if (!JoystickDeviceInfo.Contains(DeviceId))
	{
		return FJoystickInfo();
	}

	return JoystickDeviceInfo[DeviceId];	
}

int32 FJoystickInputDevice::GetDeviceCount() const
{
	return JoystickDeviceInfo.Num();
}

static FName JoystickInputInterfaceName = FName("JoystickPluginInput");
void FJoystickInputDevice::SendControllerEvents()
{
	//UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickDevice::SendControllerEvents()"));
	
	for (const TPair<int32, FJoystickInfo>& Device : JoystickDeviceInfo)
	{
		const int32 DeviceId = Device.Key;
		if (JoystickDeviceInfo.Contains(DeviceId)) 
		{
			const FJoystickInfo& CurrentDevice = JoystickDeviceInfo[DeviceId];
			const FJoystickDeviceData& CurrentState = JoystickDeviceData[DeviceId];
			
			if (JoystickDeviceInfo[DeviceId].Connected) 
			{
				const int32 playerId = CurrentDevice.Player;
				FInputDeviceScope InputScope(this, JoystickInputInterfaceName, DeviceId, CurrentDevice.DeviceName);

				//Axis
				for (int32 AxisIndex = 0; AxisIndex < CurrentState.Axes.Num(); AxisIndex++)
				{
					const FKey& AxisKey = DeviceAxisKeys[DeviceId][AxisIndex];
					MessageHandler->OnControllerAnalog(AxisKey.GetFName(), playerId, CurrentState.Axes[AxisIndex].GetValue());
				}

				//Hats
				for (int32 HatIndex = 0; HatIndex < CurrentState.Hats.Num(); HatIndex++)
				{
					const FVector2D& POVAxis = UJoystickFunctionLibrary::POVAxis(CurrentState.Hats[HatIndex].Direction);
					MessageHandler->OnControllerAnalog(DeviceHatKeys[0][DeviceId][HatIndex].GetFName(), playerId, POVAxis.X);
					MessageHandler->OnControllerAnalog(DeviceHatKeys[1][DeviceId][HatIndex].GetFName(), playerId, POVAxis.Y);
				}

				//Balls
				for (int32 BallIndex = 0; BallIndex < CurrentState.Balls.Num(); BallIndex++)
				{
					const FVector2D& BallAxis = CurrentState.Balls[BallIndex].Direction;
					MessageHandler->OnControllerAnalog(DeviceBallKeys[0][DeviceId][BallIndex].GetFName(), playerId, BallAxis.X);
					MessageHandler->OnControllerAnalog(DeviceBallKeys[1][DeviceId][BallIndex].GetFName(), playerId, BallAxis.Y);
				}

				//Buttons
				for (int32 ButtonIndex = 0; ButtonIndex < CurrentState.Buttons.Num(); ButtonIndex++)
				{
					FButtonData& ButtonData = JoystickDeviceData[DeviceId].Buttons[ButtonIndex];

					const FKey& ButtonKey = DeviceButtonKeys[DeviceId][ButtonIndex];
					if (ButtonData.ButtonState != ButtonData.PreviousButtonState)
					{
						if (ButtonData.ButtonState)
						{
							MessageHandler->OnControllerButtonPressed(ButtonKey.GetFName(), playerId, false);
						}
						else
						{
							MessageHandler->OnControllerButtonReleased(ButtonKey.GetFName(), playerId, false);
						}
						
						ButtonData.PreviousButtonState = ButtonData.ButtonState;
					}
				}
			}
		}
	}

	DeviceSDL->Update();
}

void FJoystickInputDevice::IgnoreGameControllers(const bool bIgnore) const
{
	if (DeviceSDL == nullptr)
	{
		return;
	}
	
	DeviceSDL->IgnoreGameControllers(bIgnore);
}

FDeviceSDL* FJoystickInputDevice::GetDeviceSDL() const
{
	if (!DeviceSDL.IsValid())
	{
		return nullptr;
	}
	
	return DeviceSDL.Get();
}

void FJoystickInputDevice::ReinitialiseJoystickData(const int32 DeviceId)
{	
	FJoystickDeviceData& JoystickState = JoystickDeviceData[DeviceId];
	JoystickState = DeviceSDL->InitialDeviceState(DeviceId);
}

void FJoystickInputDevice::GetDeviceIds(TArray<int32>& DeviceIds) const
{
	JoystickDeviceInfo.GenerateKeyArray(DeviceIds);
}
