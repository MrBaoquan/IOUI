#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <windows.h>
#include "Paths.hpp"

#include <fstream>

bool is_file_exist(const std::string& name) {
    std::ifstream file(name);
    return file.good();
}

static bool enable = false;

void EnableConsoleDebug(){
    std::string _path = DevelopHelper::Paths::Instance().GetModuleDir();

    // 判断是否存在debug.log文件，如果存在则删除
    std::string _logPath = _path + "\\debug.log";
    if(is_file_exist(_logPath)){
     	enable = true;
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    }
    else {
        enable = false;
    }
}

void DisableConsoleDebug(){
    if(!enable){
        return;
    }
    fclose(stdout);
    fclose(stderr);
    FreeConsole();
}

void ClearConsole(){
    if(!enable){
        return;
    }
    system("cls");
}

// 将输入的数字构造成Button_xxx字符, 当num小于100时, 前面补0
std::string ButtonName(std::string prefix, int num){
    if(num < 10){
        prefix += "0";
    }
    prefix += std::to_string(num);
    return prefix;
}

void PrintDIData( unsigned char* DIData, int size){
    if(!enable){
        return;
    }
    for(int i = 0; i < size; i++){
        auto _buttonName = ButtonName("Button_", i);
        if(i%8 == 0){
            printf("\n");
        }
        printf("%s:%d  ", _buttonName.c_str(), DIData[i]);
    } 
}

void PrintAIData(short* DIData, int size){
    if(!enable){
        return;
    }
    for(int i = 0; i < size; i++){
        auto _buttonName = ButtonName("Axis_", i);
        if(i%8 == 0){
            printf("\n");
        }
        printf("%s:%d  ", _buttonName.c_str(), DIData[i]);
    } 
}

void PrintLine(){
    if(!enable){
        return;
    }
    printf("\r\n");
}