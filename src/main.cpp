#include "main.hpp"

#include "ModConfig.hpp"
#include "ModSettingsViewController.hpp"
#include "FloatingMenu.hpp"
#include "SongListTableData.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "TwitchIRC/TwitchIRCClient.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "song-details/shared/SongDetails.hpp"
#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "SongListObject.hpp"
#include "HMUI/ImageView.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "beatsaverplusplus/shared/BeatSaver.hpp"
#include <map>
#include <thread>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "web-utils/shared/WebUtils.hpp"
#include "logging.hpp"

using namespace HMUI;

bool threadRunning = false;
static UnityEngine::GameObject *menu;

bool menuInitialized = false;


static std::future<SongDetailsCache::SongDetails*> songDetails;

// Called at the early stages of game loading
extern "C" __attribute__((visibility("default"))) void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo.assign(info);

    getModConfig().Init(modInfo);

    INFO("Completed setup!");
}

std::vector<std::string> requestedSongs;

void OnChatMessage(IRCMessage ircMessage, TwitchIRCClient* client) {
    std::string username = ircMessage.prefix.nick;
    std::string message = ircMessage.parameters.at(ircMessage.parameters.size() - 1);
    if(!message.starts_with("!bsr")) return;
    if(message.length() < 6) return;
    std::string code = message.substr(5);

    if (!menuInitialized) return;

    if(std::find(requestedSongs.begin(), requestedSongs.end(), code) != requestedSongs.end()) return;

    std::thread([code] {
        auto response = WebUtils::Get<BeatSaver::API::BeatmapResponse>(BeatSaver::API::GetBeatmapByKeyURLOptions(code));
        
        if (!response.IsSuccessful()) {
            int responseCode = response.httpCode;
            int curlStatus = response.curlStatus;
            
            std::string message = "";
            ///.... Process error here
            return;
        }

        auto beatmap = response.GetParsedData();
        INFO("Found beatmap {} on BeatSaver", beatmap.GetName());

        // songlist object is created on the heap
        TSRQ::SongListObject* songListObject = new TSRQ::SongListObject;
        songListObject->setSong(beatmap);

        BSML::MainThreadScheduler::Schedule([songListObject]{
            TSRQ::FloatingMenu::get_instance()->push(songListObject);
        });
        
    }).detach();
}

#define JOIN_RETRY_DELAY 3000
#define CONNECT_RETRY_DELAY 15000

void TwitchIRCThread() {
    if(threadRunning)
        return;
    threadRunning = true;
    INFO("Thread Started!");
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
                        INFO("Twitch Chat: Joined Channel %s!", currentChannel.c_str());
                    }
                }
            }
            client.ReceiveData();
        } else {
            if(wasConnected) {
                wasConnected = false;
                INFO("Twitch Chat: Disconnected!");
            }
            if ((currentTime - lastConnectTry).count() > CONNECT_RETRY_DELAY) {
                INFO("Twitch Chat: Connecting...");
                lastConnectTry = currentTime;
                if (client.InitSocket()) {
                    if (client.Connect()) {
                        if (client.Login("justinfan" + std::to_string(1030307 + rand() % 1030307), "xxx")) {
                            wasConnected = true;
                            INFO("Twitch Chat: Logged In!");
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
        INFO("Twitch Chat: Disconnected!");
    }
    threadRunning = false;
    client.Disconnect();
    INFO("Thread Stopped!");
}

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged,
                &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged,
                void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    if(nextScene.IsValid()) {
        std::string sceneName = nextScene.get_name();
        if(sceneName.find("Menu") != std::string::npos) {
            BSML::MainThreadScheduler::Schedule(
                    [] {
                        if (!threadRunning)
                            std::thread (TwitchIRCThread).detach();
                    }
            );
        }
    }
}


MAKE_HOOK_MATCH(MainMenuViewControllerDidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuViewControllerDidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        TSRQ::FloatingMenu::get_instance()->Initialize();
        menuInitialized = true;
    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute__((visibility("default"))) void late_load() {
    il2cpp_functions::Init();
    BSML::Init();
    custom_types::Register::AutoRegister();

    songDetails = SongDetailsCache::SongDetails::Init();

    // Register settings menu
    // QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    // QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);

    INFO("Installing hooks...");
    INSTALL_HOOK(Logger, SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(Logger, MainMenuViewControllerDidActivate);
    INFO("Installed all hooks!");
}
