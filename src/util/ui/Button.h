#pragma once

#include <functional>
#include <string>

struct Button {
    std::string name;
    short x;
    short y;
    short w;
    short h;
    const std::function <void ()> onClick;

public:
    Button(std::string name, short x, short y, short w, short h, std::function<void()> onClick);
};

