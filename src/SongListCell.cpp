#include "SongListCell.hpp"

#include "UnityEngine/RectTransform.hpp"

DEFINE_TYPE(TSRQ, CustomSongListTableCell)
namespace TSRQ {
    CustomSongListTableCell* CustomSongListTableCell::PopulateWithSongData(std::string entry) {
        songName->set_text(entry);
        // levelAuthorName->set_text(entry->levelAuthorName);
        // statusLabel->set_text(entry->statusMessage());
        // this->entry = entry;
        // entry->UpdateProgressHandler = [this]() {
        //     UpdateProgress();
        // };
        return this;
    }

    void CustomSongListTableCell::RefreshBgState() {
        bgContainer->set_color(UnityEngine::Color(0, 0, 0, highlighted ? 0.8f : 0.45f));
        // RefreshBar();
    }

    // void CustomSongListTableCell::RefreshBar() {
    //     if(!entry)
    //         return;
    //     auto clr = entry->status == DownloadHistoryEntry::Failed ? UnityEngine::Color::get_red() : entry->status != DownloadHistoryEntry::DownloadStatus::Queued ? UnityEngine::Color::get_green() : UnityEngine::Color::get_gray();
    //     clr.a = 0.5f + (entry->downloadProgress * 0.4f);
    //     bgProgress->set_color(clr);

    //     auto x = reinterpret_cast<UnityEngine::RectTransform*>(bgProgress->get_gameObject()->get_transform());
    //     if(!x)
    //         return;
    //     x->set_anchorMax(UnityEngine::Vector2(entry->downloadProgress, 1));
    //     static auto forceUpdate = reinterpret_cast<function_ptr_t<void, UnityEngine::RectTransform*>>(il2cpp_functions::resolve_icall("UnityEngine.RectTransform::ForceUpdateRectTransforms"));
    //     forceUpdate(x);
    // }

    // void CustomSongListTableCell::UpdateProgress() {
    //     statusLabel->set_text(entry->statusMessage());
    //     RefreshBar();
    // }

    void CustomSongListTableCell::SelectionDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        RefreshBgState();
    }

    void CustomSongListTableCell::HighlightDidChange(HMUI::SelectableCell::TransitionType transitionType) {
        RefreshBgState();
    }

    void CustomSongListTableCell::WasPreparedForReuse() {
        // entry->UpdateProgressHandler = nullptr;
    }
}

