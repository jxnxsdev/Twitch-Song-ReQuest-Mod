#include "main.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
int connectionStatus = 2; // 0 = not connected, 1 = connected, 2 = not checked
TwitchSongRequest::Handler* songRequestHandler = nullptr;

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

void CreateUI() {
    getLogger().info("Creating UI");

    UnityEngine::GameObject* canvas = QuestUI::BeatSaberUI::CreateCanvas();
    songRequestHandler = canvas->AddComponent<TwitchSongRequest::Handler*>();
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

    songRequestHandler->LayoutTransform = layoutTransform;
    songRequestHandler->Canvas = canvas;
}


// websocket stuff
using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void AddSongObject(std::string name, std::string artist, std::string id) {
    MapObject mapObject = {};
    mapObject.SongName = name;
    mapObject.SongArtist = artist;
    mapObject.SongID = id;
    mapObject.GameObject = nullptr;

    if(songRequestHandler) {
        songRequestHandler->AddMapObject(mapObject);
    }
}

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);

    if(nextScene.IsValid()) {
        std::string name = to_utf8(csstrtostr(nextScene.get_name()));
        if(name.find("Menu") != std::string::npos) {
            if (!songRequestHandler) {
                CreateUI();
            }
        }
    }
}

void handle_message(const std::string & message){
    // log the message to the console
    const char *logMessage;
    logMessage = message.c_str();
    getLogger().info("%s", logMessage);


    std::vector<std::string> parts;
    std::string part;
    std::istringstream f(message);
    while (std::getline(f, part, '`')) {
        parts.push_back(part);
    }

    AddSongObject(parts[0], parts[1], parts[2]);
}

void ConnectWebSocket() {
    std::string webSocketUrl = "ws://" + getModConfig().ServerAddress.GetValue() + ":" + getModConfig().ServerPort.GetValue();

    getLogger().info("Connecting to %s", webSocketUrl.c_str());
    try {
        ws = WebSocket::from_url(webSocketUrl);
    } catch (std::exception &e) {
        getLogger().error("Error connecting to websocket: %s", e.what());
        connectionStatus = 0;
        return;
    }
    assert(ws);
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
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    getLogger().info("Installed all hooks!");

    // start the websocket connection, dont crash if it fails
    std::thread (ConnectWebSocket).detach();
}