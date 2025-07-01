#pragma once

namespace dae {

    //Bottom > Top because +y is down
    struct Rect {
        int Top, Left;      // Bottom-left corner
        int Width, Height;

        int Right() const { return Left + Width; }
        int Bottom() const { return Top + Height; } 
    };

    inline bool IsColliding(const Rect& a, const Rect& b) {
        return (
            a.Right() > b.Left &&  
            a.Left < b.Right() &&  
            a.Top < b.Bottom() &&  
            a.Bottom() > b.Top);
    }
}

