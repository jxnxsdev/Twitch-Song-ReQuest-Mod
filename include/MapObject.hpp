#pragma once
#include <string>
#include "UnityEngine/GameObject.hpp"

struct MapObject {
    std::string SongName;
    std::string SongArtist;
    std::string SongID;
    UnityEngine::GameObject* GameObject;

};