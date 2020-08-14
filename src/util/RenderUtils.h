//
// Created by hendrik on 16.07.20.
//

#pragma once

#include <sstream>
#include <TUM_Draw.h>

/**
 * Draws information to the screen. Used for HUD drawing.
 *
 * Example:
 * With `text = "xyz: "` and `data = 10`, the function will draw "xyz: 10" to the screen at the specified position.
 *
 * @tparam T The type of the data to draw
 * @param text The text which is placed in front of the data
 * @param data The data to display alongsid the text
 * @param x The x position to draw
 * @param y The y position to draw
 */
template<class T>
void drawInfo(char* text, T data, int x, int y){
    std::stringstream ss;
    ss << std::noskipws << text;
    ss << data;
    std::string s;
    std::getline(ss, s);
    const char *chars = s.c_str();
    tumDrawText(const_cast<char *>(chars), x, y, 0xFFFFFF);
}

