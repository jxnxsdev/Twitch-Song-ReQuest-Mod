#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,

                CONFIG_VALUE(Channel, std::string, "Channel Name", "hdgamer1404Jonas");

                CONFIG_VALUE(PositionMenu, UnityEngine::Vector3, "Menu Position", ConfigUtils::Vector3(2.0f, 3.6f, 3.5f));
                CONFIG_VALUE(RotationMenu, UnityEngine::Vector3, "Menu Rotation", ConfigUtils::Vector3(-36.0f, 34.0f, 0.0f));
                CONFIG_VALUE(SizeMenu, UnityEngine::Vector2, "Menu Size", ConfigUtils::Vector2(58.0f, 72.0f));

                )