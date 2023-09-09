#include "FloatingMenu.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "bsml/shared/BSML.hpp"

#include "assets.hpp"

DEFINE_TYPE(TSRQ, FloatingMenu);

SafePtrUnity<TSRQ::FloatingMenu> TSRQ::FloatingMenu::instance;

void TSRQ::FloatingMenu::Initialize() {
    if (initialized) return;
    menu = QuestUI::BeatSaberUI::CreateFloatingScreen(UnityEngine::Vector2(40.0f, 32.0f), UnityEngine::Vector3(0, 0, 0), UnityEngine::Vector3(0, 0, 0), 0.0f, true, true, 4);
    QuestUI::BeatSaberUI::AddHoverHint(menu->get_transform()->get_gameObject(), "Move by Pressing a trigger");


    BSML::parse_and_construct(IncludedAssets::menu_bsml, menu->get_transform(), this);

    initialized = true;
}

SafePtrUnity<TSRQ::FloatingMenu> TSRQ::FloatingMenu::get_instance() {
    if (instance) return instance;

    auto go = UnityEngine::GameObject::New_ctor(StringW("TSRQ Floating Menu"));
    Object::DontDestroyOnLoad(go);
    return go->AddComponent<TSRQ::FloatingMenu*>();
}

void TSRQ::FloatingMenu::delete_instance() {
    if(instance) {
        Object::DestroyImmediate(instance->get_gameObject());
        instance = nullptr;
    }
}