#include "UI/SettingsUI.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"

#include "bsml/shared/BSML.hpp"


DEFINE_TYPE(TwitchSongRequest::UI, SettingsUI);

namespace TwitchSongRequest::UI {
    void SettingsUI::ctor(::GlobalNamespace::MainFlowCoordinator *mainFlowCoordinator,
                          ::TwitchSongRequest::UI::FlowCoordinators::SettingsFlowCoordinator *settingsFlowCoordinator) {

        _mainFlowCoordinator = mainFlowCoordinator;
        _settingsFlowCoordinator = settingsFlowCoordinator;
        _menuButton = BSML::MenuButton::Make_new("TwitchSongRequest", "Configure the Settings for TwitchSongRequest",
                                                 std::bind(&SettingsUI::ShowFlow, this));
    }

    void SettingsUI::ShowFlow() {
        _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf()
                ->PresentFlowCoordinator(_settingsFlowCoordinator, nullptr,
                                         ::HMUI::ViewController::AnimationDirection::Horizontal, false, false);
    }

    void SettingsUI::Initialize() {
        ::BSML::Register::RegisterMenuButton(_menuButton);
    }

    void SettingsUI::Dispose() {
        ::BSML::Register::UnRegisterMenuButton(_menuButton);
    }
}