#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/macros.hpp"

#include "bsml/shared/BSML/Components/Settings/SliderSetting.hpp"

#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"

using TextMeshProUGUI = TMPro::TextMeshProUGUI;
using SliderSetting = BSML::SliderSetting;
using Button = UnityEngine::UI::Button;

#define GET_METHOD(method) il2cpp_utils::il2cpp_type_check::MetadataGetter<&method>::get()

DECLARE_CLASS_CODEGEN(TwitchSongRequest::UI::ViewControllers, SettingsViewController,::HMUI::ViewController,


                      DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_METHOD(::HMUI::ViewController::DidActivate), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

DECLARE_BSML_PROPERTY(std::string, channelName);

)