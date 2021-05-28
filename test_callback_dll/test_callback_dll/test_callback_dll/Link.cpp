#pragma once
#include "pch.h"
#include "Link.h"
#include "ThreadPool.h"
#include "Base.h"
//#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"

void Link::Start()
{
	ThreadPool threadpool{ 5 };
	std::future<void> camera_program = threadpool.commit(Base::CameraProgram);
	std::future<void> accept_front_inf = threadpool.commit(Base::AcceptFrontInf, "S");
	std::future<float> get_dif = threadpool.commit(GetDiff);
}

float Link::GetDiff()
{
	float R = Base::LinkGetDiff();
	std::cout << "---------- GetDiff -----------   " << R << std::endl;
	return R;
}
