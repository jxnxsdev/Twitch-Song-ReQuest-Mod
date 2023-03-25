#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(ServerAddress, std::string, "Server Address", "");
    CONFIG_VALUE(ServerPort, std::string, "Server Port", "");

    CONFIG_VALUE(Position, UnityEngine::Vector3, "Position", ConfigUtils::Vector3(0.0f, 4.4f, 4.5f));
    CONFIG_VALUE(Rotation, UnityEngine::Vector3, "Rotation", ConfigUtils::Vector3(-14.0f, 0.0f, 0.0f));
);