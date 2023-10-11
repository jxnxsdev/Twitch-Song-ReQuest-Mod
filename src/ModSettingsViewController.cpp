#include "ModSettingsViewController.hpp"

#include "HMUI/Touchable.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "ModConfig.hpp"

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

void DidActivate(ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    if(firstActivation) {
        self->get_gameObject()->AddComponent<Touchable*>();
        GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        Transform* parent = container->get_transform();
        auto layout = BeatSaberUI::CreateHorizontalLayoutGroup(parent);
        layout->GetComponent<LayoutElement*>()->set_preferredWidth(90.0f);
        layout->set_childControlWidth(true);
        auto layoutParent = layout->get_transform();
        auto stringSetting = AddConfigValueStringSetting(layoutParent, getModConfig().Channel);
        auto underscoreButton = BeatSaberUI::CreateUIButton(layoutParent, "", UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(4.0f, 8.0f), [stringSetting] {
            stringSetting->KeyboardKeyPressed('_');
            stringSetting->UpdateClearButton();
        });
        BeatSaberUI::AddHoverHint(underscoreButton->get_gameObject(), "Adds a underscore");
        LayoutElement* layoutElement = underscoreButton->GetComponent<LayoutElement*>();
        layoutElement->set_minWidth(4.0f);

        auto layout2 = BeatSaberUI::CreateVerticalLayoutGroup(parent);
        layout2->GetComponent<LayoutElement*>()->set_preferredWidth(90.0f);
        layout2->set_childControlWidth(true);
        auto layoutParent2 = layout2->get_transform();
        auto text = BeatSaberUI::CreateText(layoutParent2, "Please enter your Twitch Name above\n\nNote: This Mod is still in a Beta state.\nPlease Report any Bugs you may encounter in the issues tab of the\ngithub repo");

    }
}