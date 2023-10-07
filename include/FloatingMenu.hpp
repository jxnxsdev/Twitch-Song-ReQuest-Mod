#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "HMUI/TableView_IDataSource.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "main.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "SongListObject.hpp"
#include "GlobalNamespace/SoloFreePlayFlowCoordinator.hpp"
#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "songloader/shared/API.hpp"

using namespace GlobalNamespace;

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif


#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

DECLARE_CLASS_CODEGEN_INTERFACES(TSRQ, FloatingMenu, UnityEngine::MonoBehaviour, classof(HMUI::TableView::IDataSource*),
    DECLARE_CTOR(ctor);
    DECLARE_SIMPLE_DTOR();
    DECLARE_INSTANCE_FIELD(bool, initialized);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, menu);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::SoloFreePlayFlowCoordinator*, soloFreePlayFlowCoordinator);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerLevelSelectionFlowCoordinator*, multiplayerLevelSelectionFlowCoordinator);
    DECLARE_INSTANCE_METHOD(void, SelectSong, HMUI::TableView* table, int id);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_FIELD(BSML::CustomListTableData*, songTableData);

    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);

    public:
        HMUI::TableView* songListTable() {
            if(songTableData) {
                getLogger().info("songTableData is not null");
                return songTableData->tableView;
            }
            else {
                getLogger().info("songTableData is null");
                return nullptr;
            }
        }
        void GoToSongSelect();
        void Initialize();
        void RefreshTable(bool fullReload = true);
        static void delete_instance();
        static SafePtrUnity<TSRQ::FloatingMenu> get_instance();
        void push(TSRQ::SongListObject* songListObject);
        void EnterSolo(IPreviewBeatmapLevel* level);
        // TODO: Figure out how to get this to work and not to have it being set in ctor
        float cellSize = 12.0f;
        std::vector<TSRQ::SongListObject*> songList;

private:
    static SafePtrUnity<TSRQ::FloatingMenu> instance;


)