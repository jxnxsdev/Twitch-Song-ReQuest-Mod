#include "FloatingMenu.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/ScrollView.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "SongListTableData.hpp"
#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "assets.hpp"
#include "SongListObject.hpp"
#include "songloader/shared/API.hpp"
#define coro(coroutine) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(coroutine))
#include "main.hpp"
DEFINE_TYPE(TSRQ, FloatingMenu);

SafePtrUnity<TSRQ::FloatingMenu> TSRQ::FloatingMenu::instance;

// Runs on creation
void TSRQ::FloatingMenu::ctor() {
    this->cellSize = 8.05f;
}

void TSRQ::FloatingMenu::Initialize() {
    if (initialized) return;



    menu = QuestUI::BeatSaberUI::CreateFloatingScreen(UnityEngine::Vector2(80.0f, 72.0f), UnityEngine::Vector3(0, 4.0f, 0), UnityEngine::Vector3(0, 0, 0), 0.0f, true, true, 3);
    BSML::parse_and_construct(IncludedAssets::menu_bsml, menu->get_transform(), this);
    
    // QuestUI::BeatSaberUI::AddHoverHint(menu->get_transform()->get_gameObject(), "Move by Pressing a trigger");

    if (this->songTableData != nullptr && this->songTableData->m_CachedPtr.m_value != nullptr)
    {
        songTableData->tableView->SetDataSource(reinterpret_cast<HMUI::TableView::IDataSource *>(this), false);
        getLogger().info("TSRQ: SongTable Data Initialized");
    }
    
    this->PostParse();
    this->RefreshTable();
    initialized = true;
}

void TSRQ::FloatingMenu::PostParse() {
    // BSML has a bug that stops getting the correct platform helper and on game reset it dies and the scrollhelper stays invalid and scroll doesn't work
    auto platformHelper = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelCollectionTableView*>().First()->GetComponentInChildren<HMUI::ScrollView*>()->platformHelper;
    if (platformHelper == nullptr) {
    } else {
        for (auto x: this->GetComponentsInChildren<HMUI::ScrollView*>()){
            x->platformHelper=platformHelper;
        }
    }
}

SafePtrUnity<TSRQ::FloatingMenu> TSRQ::FloatingMenu::get_instance() {
    if (instance) return instance;

    auto go = UnityEngine::GameObject::New_ctor(StringW("TSRQ Floating Menu"));
    Object::DontDestroyOnLoad(go);
    instance = go->AddComponent<TSRQ::FloatingMenu*>();
    return instance;
}

void TSRQ::FloatingMenu::delete_instance() {
    if(instance) {
        Object::DestroyImmediate(instance->get_gameObject());
        instance = nullptr;
    }
}

float TSRQ::FloatingMenu::CellSize()
{
    getLogger().info("TSRQ: Cell size %f", this->cellSize);
    return this->cellSize;
}

int TSRQ::FloatingMenu::NumberOfCells()
{   
    getLogger().info("TSRQ: Number of cells %lu", songList.size());
    return songList.size();
}


HMUI::TableCell *TSRQ::FloatingMenu::CellForIdx(HMUI::TableView *tableView, int idx)
{
    return TSRQ::SongListTableData::GetCell(tableView)->PopulateWithSongData(songList[idx]);
}

void TSRQ::FloatingMenu::RefreshTable(bool fullReload)
{
    getLogger().info("TSRQ: RefreshTable");
    QuestUI::MainThreadScheduler::Schedule(
    [this]
    {
        // Sort entry list
        // std::stable_sort(downloadEntryList.begin(), downloadEntryList.end(),
        //     [] (DownloadHistoryEntry* entry1, DownloadHistoryEntry* entry2)
        //     {
        //         return (entry1->orderValue() < entry2->orderValue());
        //     }
        // );
        this->songListTable()->ReloadData();
        // coro(this->limitedFullTableReload->Call());
    });
}

void TSRQ::FloatingMenu::SelectSong(HMUI::TableView *table, int id)
{
    getLogger().info("TSRQ: Cell is clicked");

    if (NumberOfCells() <= id) {
        getLogger().info("How did you even click a non existent cell!!!???!!!??");
    }

    if(songList[id]->downloading == true || songList[id]->isDownloaded == true) {
        return;
    }

    songList[id]->downloading = true;
    this->RefreshTable();

    songList[id]->song->DownloadLatestBeatmapAsync([id, this](bool finished){
        if (finished) {
            songList[id]->setIsDownloaded(true);
            songList[id]->setIsDownloading(false);
            this->RefreshTable();
        }
    }, [id](float progress){

    });
}

void TSRQ::FloatingMenu::push(TSRQ::SongListObject* songListObject)
{
    std::optional<BeatSaver::Beatmap> song = songListObject->song;
    if (!song.has_value()) return;
    getLogger().info("TSRQ: Pushing song %s", song->GetMetadata().GetSongName().c_str());

    std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> local =  RuntimeSongLoader::API::GetLevelByHash(song->GetVersions().front().GetHash());

    if(local.has_value()) {
        songListObject->isDownloaded = true;
    }

    this->songList.push_back(songListObject);
    this->RefreshTable();
}