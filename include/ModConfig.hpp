#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,

                CONFIG_VALUE(Channel, std::string, "Channel Name", "hdgamer1404Jonas");
                CONFIG_INIT_FUNCTION(
                    CONFIG_INIT_VALUE(Channel);

                    );
                );