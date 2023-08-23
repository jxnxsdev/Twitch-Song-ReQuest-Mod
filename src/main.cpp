#include "main.hpp"

#include "ModConfig.hpp"
#include "ModSettingsViewController.hpp"
#include "Display.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "custom-types/shared/register.hpp"

#include "TwitchIRC/TwitchIRCClient.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "libcurl/shared/curl.h"

#include "song-details/shared/SongDetails.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ViewController.hpp"
#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/GameObject.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "UnityEngine/UI/RectMask2D.hpp"


#include <map>
#include <thread>
#include <iomanip>
#include <sstream>
#include <chrono>


bool threadRunning = false;
TwitchSongRequest::Display* display = nullptr;

void CreateUI() {
    getLogger().info("Creating UI");

    UnityEngine::GameObject* canvas = QuestUI::BeatSaberUI::CreateCanvas();
    display = canvas->AddComponent<TwitchSongRequest::Display*>();
    canvas->AddComponent<RectMask2D*>();
    canvas->AddComponent<Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 0.75f);
    RectTransform* transform = canvas->GetComponent<RectTransform*>();

    // make it interactive
    VRUIControls::VRGraphicRaycaster* raycaster = canvas->AddComponent<VRUIControls::VRGraphicRaycaster*>();

    // create a scrollable container
    UnityEngine::GameObject* layout = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(canvas->get_transform());

    // add a text to the container
    QuestUI::BeatSaberUI::CreateText(layout->get_transform(), "Twitch Song Requests");


    VerticalLayoutGroup* verticalLayout = layout->GetComponent<VerticalLayoutGroup*>();
    verticalLayout->set_childControlWidth(false);
    verticalLayout->set_childControlHeight(true);
    verticalLayout->set_childForceExpandWidth(true);
    verticalLayout->set_childForceExpandHeight(false);
    verticalLayout->set_childAlignment(TextAnchor::LowerLeft);
    VRUIControls::VRGraphicRaycaster* layoutRaycaster = layout->AddComponent<VRUIControls::VRGraphicRaycaster*>();

    GameObject* layoutGameObject = verticalLayout->get_gameObject();
    RectTransform* layoutTransform = verticalLayout->get_rectTransform();
    layoutTransform->set_pivot(UnityEngine::Vector2(0.0f, 0.0f));

    display->LayoutTransform = layoutTransform;
    display->Canvas = canvas;
}

void AddSongObject(std::string name, std::string artist, std::string id) {
    MapObject mapObject = {};
    mapObject.SongName = name;
    mapObject.SongArtist = artist;
    mapObject.SongID = id;
    mapObject.GameObject = nullptr;

    if(display) {
        display->AddMapObject(mapObject);
    }
}

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

#define BeatSaverAPILink "https://api.beatsaver.com/maps/id/"

void OnChatMessage(IRCMessage ircMessage, TwitchIRCClient* client) {
    std::string username = ircMessage.prefix.nick;
    std::string message = ircMessage.parameters.at(ircMessage.parameters.size() - 1);

    // check if the message begins with "!bsr"
    if(!message.starts_with("!bsr")) return;

    // check if there is a code after !bsr. The synctac is "!bsr <code>
    if(message.length() < 6) return;

    // get the code
    std::string code = message.substr(5);

    auto songdetails = songDetails.get();
    auto& song = songdetails->songs.FindByMapId(code);

    if(!song) {
        getLogger().info("Song not found!");
        return;
    }else {
        getLogger().info("Found song %s", songdetails->songs.FindByMapId(code).songName().c_str());
        AddSongObject(song.songName(), song.songAuthorName(), code);
    }

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
            if(!display) CreateUI();
        }
    }
}



// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    QuestUI::Init();

    songDetails = SongDetailsCache::SongDetails::Init();

    // Register settings menu
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    getLogger().info("Installed all hooks!");
}