//
// Created by skadic on 31.07.20.
//

#include "Button.h"

#include <utility>

Button::Button(std::string name, short x, short y, short w, short h, std::function<void ()> onClick) : name{name}, x(x), y(y), w(w), h(h),
                                                                                              onClick(std::move(onClick)) {}
