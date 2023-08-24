#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/Installer.hpp"

#define GET_METHOD(method) il2cpp_utils::il2cpp_type_check::MetadataGetter<&method>::get()

DECLARE_CLASS_CODEGEN(TwitchSongRequest::Installers, MenuInstaller, ::Zenject::Installer,
                      DECLARE_OVERRIDE_METHOD(void, InstallBindings, GET_METHOD(::Zenject::Installer::InstallBindings));
                              DECLARE_DEFAULT_CTOR();
)