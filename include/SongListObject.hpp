#pragma once


#include "SongListCell.hpp"
#include "HMUI/Touchable.hpp"
#include "bsml/shared/BSML.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

#include "main.hpp"
#include "assets.hpp"

namespace TSRQ
{
    class SongListObject {
    public:
        std::optional<BeatSaver::Beatmap> song;
        bool downloading = false;
        bool isDownloaded = false;

        void setSong(std::optional<BeatSaver::Beatmap> song) {
            this->song = song;
        }

        void setIsDownloading(bool downloading) {
            this->downloading = downloading;
        }

        void setIsDownloaded(bool isDownloaded) {
            this->isDownloaded = isDownloaded;
        }
    };
}