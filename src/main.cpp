#include "main.hpp"

#include "ModConfig.hpp"
#include "ModSettingsViewController.hpp"
#include "FloatingMenu.hpp"
#include "SongListTableData.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "custom-types/shared/register.hpp"

#include "TwitchIRC/TwitchIRCClient.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "song-details/shared/SongDetails.hpp"

#include "bsml/shared/BSML.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "lapiz/shared/AttributeRegistration.hpp"

#include "GlobalNamespace/LevelSelectionNavigationController.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

#include "SongListObject.hpp"
#include "songloader/shared/API.hpp"

#include <map>
#include <thread>
#include <iomanip>
#include <sstream>
#include <chrono>


bool threadRunning = false;
static UnityEngine::GameObject *menu;


static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
static std::future<SongDetailsCache::SongDetails*> songDetails;

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

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();

    getModConfig().Init(modInfo);

    getLogger().info("Completed setup!");
}

std::vector<std::string> requestedSongs;

void OnChatMessage(IRCMessage ircMessage, TwitchIRCClient* client) {
    std::string username = ircMessage.prefix.nick;
    std::string message = ircMessage.parameters.at(ircMessage.parameters.size() - 1);
    if(!message.starts_with("!bsr")) return;
    if(message.length() < 6) return;
    std::string code = message.substr(5);

    if(std::find(requestedSongs.begin(), requestedSongs.end(), code) != requestedSongs.end()) return;

    QuestUI::MainThreadScheduler::Schedule([code]{
        BeatSaver::API::GetBeatmapByKeyAsync(code, [code](std::optional<BeatSaver::Beatmap> beatmap) {
            if (beatmap.has_value()) {
                getLogger().info("Found beatmap %s on BeatSaver", beatmap.value().GetName().c_str());

                TSRQ::SongListObject* songListObject = new TSRQ::SongListObject;
                songListObject->setSong(beatmap);

                TSRQ::FloatingMenu::get_instance()->push(songListObject);

                requestedSongs.push_back(code);

                getLogger().info("pushed song %s", code.c_str());
            }else {
                getLogger().info("Song with key %s not found", code.c_str());
            }
        });

    });


}

#define JOIN_RETRY_DELAY 3000
#define CONNECT_RETRY_DELAY 15000

void TwitchIRCThread() {
    if(threadRunning)
        return;
    threadRunning = true;
    getLogger().info("Thread Started!");
    TwitchIRCClient client = TwitchIRCClient();
    std::string currentChannel = "";
    using namespace std::chrono;
    milliseconds lastJoinTry = 0ms;
    milliseconds lastConnectTry = 0ms;
    bool wasConnected = false;
    while(threadRunning) {
        auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
        if(client.Connected()) {
            std::string targetChannel = getModConfig().Channel.GetValue();
            if(currentChannel != targetChannel) {
                if ((currentTime - lastJoinTry).count() > JOIN_RETRY_DELAY) {
                    lastJoinTry = currentTime;
                    if(client.JoinChannel(targetChannel)) {
                        currentChannel = targetChannel;
                        getLogger().info("Twitch Chat: Joined Channel %s!", currentChannel.c_str());
                    }
                }
            }
            client.ReceiveData();
        } else {
            if(wasConnected) {
                wasConnected = false;
                getLogger().info("Twitch Chat: Disconnected!");
            }
            if ((currentTime - lastConnectTry).count() > CONNECT_RETRY_DELAY) {
                getLogger().info("Twitch Chat: Connecting...");
                lastConnectTry = currentTime;
                if (client.InitSocket()) {
                    if (client.Connect()) {
                        if (client.Login("justinfan" + std::to_string(1030307 + rand() % 1030307), "xxx")) {
                            wasConnected = true;
                            getLogger().info("Twitch Chat: Logged In!");
                            client.HookIRCCommand("PRIVMSG", OnChatMessage);
                            currentChannel = "";
                        }
                    }
                }
            }
        }
        std::this_thread::yield();
    }
    if(wasConnected) {
        wasConnected = false;
        getLogger().info("Twitch Chat: Disconnected!");
    }
    threadRunning = false;
    client.Disconnect();
    getLogger().info("Thread Stopped!");
}

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged,
                &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged,
                void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    if(nextScene.IsValid()) {
        std::string sceneName = to_utf8(csstrtostr(nextScene.get_name()));
        if(sceneName.find("Menu") != std::string::npos) {
            QuestUI::MainThreadScheduler::Schedule(
                    [] {
                        if (!threadRunning)
                            std::thread (TwitchIRCThread).detach();
                    }
            );
        }
    }
}

MAKE_HOOK_MATCH(LevelSelectionNavigationControllerDidActivate, &GlobalNamespace::LevelSelectionNavigationController::DidActivate, void, GlobalNamespace::LevelSelectionNavigationController *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    LevelSelectionNavigationControllerDidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    getLogger().info("Creating Menu");

    if (firstActivation) TSRQ::FloatingMenu::get_instance()->Initialize();



    /*if (firstActivation) {
        menu = QuestUI::BeatSaberUI::CreateFloatingScreen(UnityEngine::Vector2(60, 150), UnityEngine::Vector3(0, 0, 0), UnityEngine::Vector3(0, 0, 0), 1.0f, true, true, 4);

        QuestUI::BeatSaberUI::AddHoverHint(menu->get_transform()->get_gameObject(), "Move by Pressing a trigger");




        getLogger().info("Created menu");
        return;
    }

    if (menu) {
        menu->get_gameObject()->set_active(true);
    }*/
}

MAKE_HOOK_MATCH(LevelSelectionNavigationControllerDidDeactivate, &GlobalNamespace::LevelSelectionNavigationController::DidDeactivate, void, GlobalNamespace::LevelSelectionNavigationController *self, bool removedFromHierarchy, bool screenSystemDisabling) {

    LevelSelectionNavigationControllerDidDeactivate(self, removedFromHierarchy, screenSystemDisabling);

    if (menu) {
        menu->get_gameObject()->set_active(false);
    }
}



// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    QuestUI::Init();
    ::BSML::Init();
    ::custom_types::Register::AutoRegister();
    ::Lapiz::Attributes::AutoRegister();

    songDetails = SongDetailsCache::SongDetails::Init();

    // Register settings menu
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(getLogger(), LevelSelectionNavigationControllerDidActivate);
    INSTALL_HOOK(getLogger(), LevelSelectionNavigationControllerDidDeactivate);
    getLogger().info("Installed all hooks!");
}