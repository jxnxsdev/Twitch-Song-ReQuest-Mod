#pragma once

#include "MapObject.hpp"
#include "ModConfig.hpp"
#include "main.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "custom-types/shared/macros.hpp"

#include <vector>
#include <mutex>
#include <sstream>
#include <string>
#include <map>

#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

using namespace QuestUI;
using namespace UnityEngine::SceneManagement;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace TMPro;

DECLARE_CLASS_CODEGEN(TwitchSongRequest, Display, UnityEngine::MonoBehaviour,

                      private:
                              std::vector<MapObject> mapObjects;
                              std::mutex mapObjectsMutex;

                              public:
                              void AddMapObject(MapObject object);
                              void SetPosition(UnityEngine::Vector3 position);
                              void SetRotation(UnityEngine::Vector3 rotation);
                              void DownloadSong(std::string id);

                              DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, Canvas);
                              DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, LayoutTransform);
                              DECLARE_INSTANCE_METHOD(void, Update);
                              DECLARE_OVERRIDE_METHOD(void, Finalize, il2cpp_utils::FindMethod("System", "Object", "Finalize"));

)