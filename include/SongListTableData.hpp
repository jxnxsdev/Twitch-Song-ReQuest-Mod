#pragma once


#include "SongListCell.hpp"
#include "HMUI/Touchable.hpp"
#include "bsml/shared/BSML.hpp"

#include "logging.hpp"
#include "main.hpp"
#include "assets.hpp"

const StringW ReuseIdentifier = "REUSECCustomSongListTableCell";

namespace TSRQ
{
    class SongListTableData
    {
    public:
        static CustomSongListTableCell *GetCell(HMUI::TableView *tableView)
        {
            auto tableCell = tableView->DequeueReusableCellForIdentifier(ReuseIdentifier);
            
            if (!tableCell)
            {   
                tableCell = UnityEngine::GameObject::New_ctor("CustomSongListTableCell")->AddComponent<CustomSongListTableCell *>();
                tableCell->set_interactable(true);
                tableCell->set_reuseIdentifier(ReuseIdentifier);
                BSML::parse_and_construct(IncludedAssets::songItem_bsml, tableCell->get_transform(), tableCell);
                
                // Weird hack cause HMUI touchable is not there for some reason
                tableCell->get_gameObject()->AddComponent<HMUI::Touchable *>();
                INFO("TSRQ: SongListCell Initialized");
            }

            return tableCell.cast<CustomSongListTableCell>();
        }
    };
}
