#pragma once
#include "pch.h"
//#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"
#include "TYApi.h"
#include "TYImageProc.h"
#include "common.hpp"
#include "Base.h"


namespace Link
{
	//extern "C"{
		//struct img_inf
		//{
		//	int rows;
		//	int clos;
		//};
		//_declspec(dllexport) img_inf GetImgInf(const char* str);
	float *f = 0;
	extern "C" _declspec(dllexport) void Start();
	extern "C" _declspec(dllexport) float GetDiff();


	//}
}
