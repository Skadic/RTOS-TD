//
// Created by skadic on 21.05.20.
//

#include "iostream"
extern "C" {
    #include "TUM_Draw.h"
    #include "TUM_Utils.h"
}

int main(int argc, char* argv[]) {

    std::cout << argv[0] << std::endl;

    auto path = tumUtilGetBinFolderPath(argv[0]);

    tumDrawInit(path);
    tumDrawBindThread();

    //tumDrawCircle(50, 50, 20, 0xFFFF00);
    tumDrawUpdateScreen();

    getchar();
}