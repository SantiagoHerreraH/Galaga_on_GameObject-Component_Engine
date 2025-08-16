#pragma once

#include "MathTools.h"
#include <glm.hpp>
#include <algorithm>

#undef max
#undef min

namespace dae {

    //Bottom > Top because +y is down
    struct Rect {
        int Top, Left;      // Bottom-left corner
        int Width, Height;

        int Right() const { return Left + Width; }
        int Bottom() const { return Top + Height; } 

        bool IsInBounds(int x, int y) const{

            return
                !MathTools::IsOutOfBounds(x, Left, Right()) &&
                !MathTools::IsOutOfBounds(y, Top,  Bottom());
        }
        glm::vec2 KeepInBounds(const glm::vec2& p)const
        {
            const float minX = static_cast<float>(std::min(Left, Right()));
            const float maxX = static_cast<float>(std::max(Left, Right()));
            const float minY = static_cast<float>(std::min(Top,  Bottom()));
            const float maxY = static_cast<float>(std::max(Top,  Bottom()));

            return {
                std::clamp(p.x, minX, maxX),
                std::clamp(p.y, minY, maxY)
            };
        }

        bool IsColliding(const Rect& other) const{
            return (
                Right()     > other.Left &&
                Left        < other.Right() &&
                Top         < other.Bottom() &&
                Bottom()    > other.Top);
        }
    };

    
}

