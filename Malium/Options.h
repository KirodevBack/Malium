#pragma once
#include "Unreal.h"

enum Malium__URLSet
{
	Default,
	Hybrid, 
	Dev, 
	All, 
};

constexpr bool Console = true;
constexpr auto URLSet = Malium__URLSet::Default;
constexpr inline Malium::Unreal::FString Backend = L"http://127.0.0.1:3551"; // Your backend URL must include the port: https://yourbackend.xyz:443 for https http://yourbackend.xyz:80 for http or just adapt the port if it's localhost.
constexpr bool bHasPushWidget = false;
constexpr bool UseBackendParam = false; 
constexpr bool ManualMapping = false;
constexpr bool FixMemLeak = true;