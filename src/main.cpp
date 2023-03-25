#include "main.hpp"

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace TMPro;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
int connectionStatus = 2; // 0 = not connected, 1 = connected, 2 = not checked

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}



// Hooks
UnityEngine::GameObject* container = nullptr;

MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController
        *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    // Run the original method before our code.
    // Note, you can run the original method after our code if you want to change arguments.
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);


    if (firstActivation) {
        container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());



        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Welcome to Twitch Song Request!", true, UnityEngine::Vector2(0, 0));
    }

    if (container) {
        container->get_gameObject()->SetActive(true);
    }
}



void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(firstActivation) {
        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Welcome to Twitch Song Request!", true, UnityEngine::Vector2(0, 0));
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Check the GitHub repo for setup instructions.", true, UnityEngine::Vector2(0, 0));
    }

    // check if server port and server address are set in the mod config
    if (getModConfig().ServerPort.GetValue() == "" || getModConfig().ServerAddress.GetValue() == "") {
        QuestUI::BeatSaberUI::CreateText(self->get_transform(), "Please set the server port and address in the mod config.", true, UnityEngine::Vector2(0, 0));
    }
    else {
        // check if the server is connected
        if (connectionStatus == 2) {
            QuestUI::BeatSaberUI::CreateText(self->get_transform(), "Checking if server is connected...", true, UnityEngine::Vector2(0, 0));
        }
        else if (connectionStatus == 1) {
            QuestUI::BeatSaberUI::CreateText(self->get_transform(), "Server is connected.", true, UnityEngine::Vector2(0, 0));
        }
        else if (connectionStatus == 0) {
            QuestUI::BeatSaberUI::CreateText(self->get_transform(), "Server is not connected.", true, UnityEngine::Vector2(0, 0));
        }
    }
}

// websocket stuff
using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void handle_message(const std::string & message){
    // log the message to the console
    const char *logMessage;
    logMessage = message.c_str();
    getLogger().info("%s", logMessage);
}

int ConnectWebSocket() {
    std::string webSocketUrl = "ws://" + getModConfig().ServerAddress.GetValue() + ":" + getModConfig().ServerPort.GetValue();
    getLogger().info("Connecting to %s", webSocketUrl.c_str());
    ws = WebSocket::from_url(webSocketUrl);
    assert(ws);
    ws->send("Hello, world!");
    connectionStatus = 1;

    while (ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        ws->dispatch(handle_message);
    }

    connectionStatus = 0;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    QuestUI::Init();
    getModConfig().Init(modInfo);

    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), MainMenuUIHook);
    getLogger().info("Installed all hooks!");

    // start the websocket connection
    std::thread t1(ConnectWebSocket);
    t1.detach();
}