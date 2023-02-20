#include "main.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup




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

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(firstActivation) {
        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Welcome to Twitch Song Request!", true, UnityEngine::Vector2(0, 0));
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Check the GitHub repo for setup instructions.", true, UnityEngine::Vector2(0, 0));
        // Check if Server Address and Port are set
        if (getModConfig().ServerAddress.GetValue().empty() || getModConfig().ServerPort.GetValue().empty()) {
            QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Server Address and Port are not set!", true, UnityEngine::Vector2(0, 0));
        }
        else {
            QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Server Address and Port are set!", true, UnityEngine::Vector2(0, 0));
        }
    }
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
    getLogger().info("Installed all hooks!");
}