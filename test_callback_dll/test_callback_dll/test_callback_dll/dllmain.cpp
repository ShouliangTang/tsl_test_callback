// dllmain.cpp : 定义 DLL 应用程序的入口点。
#pragma once
#include "pch.h"
//#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

