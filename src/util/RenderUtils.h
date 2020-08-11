//
// Created by hendrik on 16.07.20.
//

#pragma once

#include <sstream>
#include <TUM_Draw.h>

template<class T>
void drawInfo(char* text, T number, int x, int y){
    std::stringstream ss;
    ss << std::noskipws << text;
    ss << number;
    std::string s;
    std::getline(ss, s);
    const char *chars = s.c_str();
    tumDrawText(const_cast<char *>(chars), x, y, 0xFFFFFF);
}

