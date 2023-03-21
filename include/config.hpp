#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(ServerAddress, std::string, "Server Address", "");
    CONFIG_VALUE(ServerPort, std::string, "Server Port", "");
);