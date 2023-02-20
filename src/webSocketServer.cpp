#include "webSocketServer.hpp"

const std::string SERVER_ADDRESS = getModConfig().ServerAddress.GetValue();
const std::string SERVER_PORT = getModConfig().ServerPort.GetValue();