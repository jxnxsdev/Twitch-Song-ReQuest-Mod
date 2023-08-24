#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "UI/ViewControllers/SettingsViewController.hpp"

#define GET_METHOD(method) il2cpp_utils::il2cpp_type_check::MetadataGetter<&method>::get()

DECLARE_CLASS_CODEGEN(TwitchSongRequest::UI::FlowCoordinators, SettingsFlowCoordinator, ::HMUI::FlowCoordinator,
                      DECLARE_PRIVATE_FIELD(::GlobalNamespace::MainFlowCoordinator *, _mainFlowCoordinator);
                              DECLARE_PRIVATE_FIELD(::TwitchSongRequest::UI::ViewControllers::SettingsViewController *, _settingsViewController);

                              DECLARE_INJECT_METHOD(void, Inject, ::GlobalNamespace::MainFlowCoordinator *mainFlowCoordinator,
                                                    ::TwitchSongRequest::UI::ViewControllers::SettingsViewController *settingsViewController);

                              DECLARE_OVERRIDE_METHOD(void, DidActivate, GET_METHOD(::HMUI::FlowCoordinator::DidActivate), bool firstActivation,
                              bool addedToHierarchy, bool screenSystemEnabling);
                              DECLARE_OVERRIDE_METHOD(void, DidDeactivate, GET_METHOD(::HMUI::FlowCoordinator::DidDeactivate),
                              bool removedFromHierarchy, bool screenSystemDisabling);
                              DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, GET_METHOD(::HMUI::FlowCoordinator::BackButtonWasPressed));
)