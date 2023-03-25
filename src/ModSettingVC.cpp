#include "ModSettingVC.hpp"


void DidActivate(ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation) {
        self->get_gameObject()->AddComponent<Touchable*>();

        GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        Transform* parent = container->get_transform();

        auto layout = BeatSaberUI::CreateHorizontalLayoutGroup(parent);
        layout->GetComponent<LayoutElement*>()->set_preferredWidth(80.0f);
        layout->set_childControlWidth(true);

        auto title = BeatSaberUI::CreateText(parent, "Mod Settings", false);
        title->set_fontSize(6.0f);
        // display the title in the middle of the screen
        title->get_rectTransform()->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
        title->get_rectTransform()->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
        title->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0.0f, 0.0f));

        AddConfigValueIncrementVector3(parent, getModConfig().Position, 2, 0.05f);
        AddConfigValueIncrementVector3(parent, getModConfig().Rotation, 0, 1.0f);
    }
}