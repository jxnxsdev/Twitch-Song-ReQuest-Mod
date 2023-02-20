#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(SongRequest, bool, "Song Request", true);
    CONFIG_VALUE(NowPlaying, bool, "Now Playing", true);
    CONFIG_VALUE(ServerAddress, std::string, "Server Address", "");
    CONFIG_VALUE(ServerPort, std::string, "Server Port", "");
);