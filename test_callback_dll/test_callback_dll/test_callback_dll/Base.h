#pragma once
#include "pch.h"
#include <iostream>
//#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"
#include <opencv2/opencv.hpp>
#include "TYApi.h"
#include "TYImageProc.h"
#include "common.hpp"

static float ree = 100.0F;

static int depth_H = 250;
static int depth_W = 250;
static float actual_distance = 520;//MM毫米 现在的这个距离这个就是差不多52公分长250个像素点 X横向上的距离 本身depth的那个就是MM的,这样差不多一个像素点就代表了2MM
static float pixel_distance = actual_distance / depth_W;
static float pixel_distance_square = pixel_distance * pixel_distance;
static int conversion_rate = 1000 * 1000;////立方毫秒 --》 立方分米
static int pox_dir_num = 0;

typedef std::vector<std::vector<float>> type_array;
static std::mutex some_mutex;
static std::mutex any_mutex;
static std::shared_ptr<std::atomic<bool>> flag_standard_img = std::make_shared<std::atomic<bool>>(false);
static std::vector<std::vector<float>> standard_img(depth_W, std::vector<float>(depth_H));
static std::vector<std::vector<float>> now_img(depth_W, std::vector<float>(depth_H));

static std::vector<cv::Point2i> NeihborPos;////8邻域
static std::map<int, std::vector<cv::Point2i>> MaskNum;////中心点的附近的几个像素邻域点

namespace Base
{
	struct CallbackData {
		int             index;
		TY_DEV_HANDLE   hDevice;
		TY_ISP_HANDLE   hIspHandle;
		bool            exit;
		cv::Mat         depth;
		cv::Mat         leftIR;
		cv::Mat         rightIR;
		cv::Mat         color;
		DepthViewer*    dViewer;
	};
	enum MaskNumDir
	{
		Left = 0,
		LeftTop = 1,
		Top = 2,
		RightTop = 3,
		Right = 4,
		RightLower = 5,
		Lower = 6,
		LeftLower = 7,
	};

	void Get8NeihborPos(std::vector<cv::Point2i>& NeihborPos);
	void GetMaskNum(std::map<int, std::vector<cv::Point2i>>& MaskNum, int num);
	int Dir(int& i, int& j);
	void frameCallback(TY_FRAME_DATA* frame, void* userdata);
	void eventCallback(TY_EVENT_INFO *event_info, void *userdata);
	float GetDepthInf(std::map<std::pair<int, int>, float>& Src, const std::pair<int, int>& point);
	std::map<std::pair<int, int>, float> Check8NeihborPos(std::map<std::pair<int, int>, float>& map_src, const std::pair<int, int>& point);
	std::map<int, std::map<std::pair<int, int>, float>> Check8MaskNum(std::map<std::pair<int, int>, float>& map_src, const std::pair<int, int>& point);
	float Get8AvgValue(std::map < std::pair<int, int>, float>& value);
	float GetlinePox(std::map<std::pair<int, int>, float>& P, int dir, std::pair<int, int>& Point);
	void MatToArray(cv::Mat & Src, std::vector<std::vector<float>>& Dst);
	bool LackDepthMapTwo(std::map<std::pair<int, int>, float>& map_src, std::set<std::pair<int, int>>& map_dst);
	void PushDepthDate(type_array& Src);
	float DValueImg(type_array& Ori, type_array& Tar);
	float LinkGetDiff();

	void AcceptFrontInf(std::string str);
	void CameraProgram();
}
