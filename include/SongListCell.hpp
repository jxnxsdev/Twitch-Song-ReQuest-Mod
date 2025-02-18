#pragma once

#include "FloatingMenu.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"
#include "System/Object.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "song-details/shared/Data/Song.hpp"
#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "song-details/shared/SongDetails.hpp"
#include "SongListObject.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "beatsaverplusplus/shared/Models/Beatmap.hpp"

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

namespace TSRQ {
    class SongListObject;
}

DECLARE_CLASS_CODEGEN(TSRQ, CustomSongListTableCell, HMUI::TableCell,
    DECLARE_OVERRIDE_METHOD_MATCH(void, SelectionDidChange, &HMUI::SelectableCell::SelectionDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, HighlightDidChange, &HMUI::SelectableCell::HighlightDidChange, HMUI::SelectableCell::TransitionType transitionType);
    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, bgContainer);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, bgProgress);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, songName);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, levelAuthorName);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, statusLabel);

public:
    // Song entry to have a reference to the song data
    TSRQ::SongListObject* entry;
    CustomSongListTableCell* PopulateWithSongData(TSRQ::SongListObject* songListObject);
    // void UpdateProgress();
    // void RefreshBar();
    void RefreshBgState();
)