#include "UI/ViewControllers/SettingsViewController.hpp"

#include "bsml/shared/BSML.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/ScriptableObject.hpp"
#include "UnityEngine/XR/XRNode.hpp"
#include "VRUIControls/VRInputModule.hpp"
#include "assets.hpp"
#include "ModConfig.hpp"

#define DEFINE_BSML_PROPERTY(type, name, get, set)          \
type SettingsViewController::get_##name() get               \
void SettingsViewController::set_##name(type value) set

DEFINE_TYPE(TwitchSongRequest::UI::ViewControllers, SettingsViewController);

using namespace BSML;
using namespace Libraries::HM::HMLib::VR;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::XR;
using namespace VRUIControls;

namespace TwitchSongRequest::UI::ViewControllers {
    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (!firstActivation) return;
        BSML::parse_and_construct(::IncludedAssets::settings_bsml, get_transform(), this);
    }

    DEFINE_BSML_PROPERTY(std::string, channelName, {
        return getModConfig().Channel.GetValue();
    },{
        return getModConfig().Channel.SetValue(value);
    });
}