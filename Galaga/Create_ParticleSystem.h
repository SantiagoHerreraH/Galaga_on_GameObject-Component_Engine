#pragma once

#include "GameObject.h"
#include "CParticleSystem.h"
#include "Settings.h"

namespace dae {
    inline GameObjectHandle Create_ParticleSystem() {
        GameObjectHandle particleSystemObj = std::make_shared<GameObject>();

        CParticleSystem particleSystem{};

        particleSystem.SetNumber(50);
        particleSystem.SetColorRange({ 0,0,0 }, { 255, 255, 255 });
        particleSystem.SetMovementDirectionRange({ 0,1 }, { 0,1 });
        particleSystem.SetPositionRange({ 0,0 }, { g_WindowWidth, g_WindowHeight });
        particleSystem.SetMovementSpeedRange(50, 300);
        particleSystem.SetBounds(true, Rect{ 0, 0, g_WindowWidth , g_WindowHeight });

        particleSystemObj->AddComponent(particleSystem);

        return particleSystemObj;
    }

}