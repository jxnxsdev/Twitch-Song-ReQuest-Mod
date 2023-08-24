#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "bsml/shared/BSML/MenuButtons/MenuButton.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "UI/FlowCoordinators/SettingsFlowCoordinator.hpp"

#define SETTINGSUI_INTERFACES { classof(::System::IDisposable *), classof(::Zenject::IInitializable *) }

#define GET_METHOD(method) il2cpp_utils::il2cpp_type_check::MetadataGetter<&method>::get()

___DECLARE_TYPE_WRAPPER_INHERITANCE(TwitchSongRequest::UI, SettingsUI, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject,
                                    "TwitchSongRequest::UI", SETTINGSUI_INTERFACES, 0, nullptr,
                                    DECLARE_PRIVATE_FIELD(::GlobalNamespace::MainFlowCoordinator *, _mainFlowCoordinator);
                                            DECLARE_PRIVATE_FIELD(::TwitchSongRequest::UI::FlowCoordinators::SettingsFlowCoordinator *, _settingsFlowCoordinator);
                                            DECLARE_PRIVATE_FIELD(::BSML::MenuButton *, _menuButton);

                                            DECLARE_CTOR(ctor, ::GlobalNamespace::MainFlowCoordinator *mainFlowCoordinator,
                                                         ::TwitchSongRequest::UI::FlowCoordinators::SettingsFlowCoordinator *settingsFlowCoordinator);

                                            DECLARE_INSTANCE_METHOD(void, ShowFlow);
                                            DECLARE_OVERRIDE_METHOD(void, Initialize, GET_METHOD(::Zenject::IInitializable::Initialize));
                                            DECLARE_OVERRIDE_METHOD(void, Dispose, GET_METHOD(::System::IDisposable::Dispose));
)

#undef SETTINGSUI_INTERFACES