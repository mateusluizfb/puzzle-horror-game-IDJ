#ifndef RECT_UTILS_H
#define RECT_UTILS_H

#include "../math/Rect.h"
#include <algorithm>

inline bool RectsOverlap(const Rect& a, const Rect& b) {
    return (a.x <= b.x + b.w &&
            a.x + a.w >= b.x &&
            a.y <= b.y + b.h &&
            a.y + a.h >= b.y);
}

inline Rect MergeRects(const Rect& a, const Rect& b) {
    float x = std::min(a.x, b.x);
    float y = std::min(a.y, b.y);
    float max_x = std::max(a.x + a.w, b.x + b.w);
    float max_y = std::max(a.y + a.h, b.y + b.h);
    return Rect(x, y, max_x - x, max_y - y);
}

#endif
