#include "UI/FlowCoordinators/SettingsFlowCoordinator.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

DEFINE_TYPE(TwitchSongRequest::UI::FlowCoordinators, SettingsFlowCoordinator);

namespace TwitchSongRequest::UI::FlowCoordinators {
    void SettingsFlowCoordinator::Inject(::GlobalNamespace::MainFlowCoordinator *mainFlowCoordinator,
                                         ::TwitchSongRequest::UI::ViewControllers::SettingsViewController *settingsViewController) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _settingsViewController = settingsViewController;
    }

    void SettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (!firstActivation) return;
        set_showBackButton(true);
        SetTitle("TwitchSongRequest", ::HMUI::ViewController::AnimationType::In);
        ProvideInitialViewControllers(_settingsViewController, nullptr, nullptr, nullptr, nullptr);
    }

    void SettingsFlowCoordinator::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
    }

    void SettingsFlowCoordinator::BackButtonWasPressed() {
        parentFlowCoordinator->DismissFlowCoordinator(this, ::HMUI::ViewController::AnimationDirection::Horizontal,
                                                      nullptr, false);
    }
}
