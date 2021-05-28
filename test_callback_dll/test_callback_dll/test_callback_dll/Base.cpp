#pragma once
#include"Base.h"
//#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"

namespace Base
{
	void Get8NeihborPos(std::vector<cv::Point2i>& NeihborPos)
	{
		NeihborPos.push_back(cv::Point2i(-1, 0));
		NeihborPos.push_back(cv::Point2i(1, 0));
		NeihborPos.push_back(cv::Point2i(0, -1));
		NeihborPos.push_back(cv::Point2i(0, 1));

		NeihborPos.push_back(cv::Point2i(-1, -1));
		NeihborPos.push_back(cv::Point2i(-1, 1));
		NeihborPos.push_back(cv::Point2i(1, -1));
		NeihborPos.push_back(cv::Point2i(1, 1));

		////LOGD("Get8NeihborPos size %d", NeihborPos.size());
	}

	void GetMaskNum(std::map<int, std::vector<cv::Point2i>>& MaskNum, int num)
	{
		pox_dir_num = num;
		for (int i = -num; i <= num; i++)
		{
			for (int j = -num; j <= num; j++)
			{
				if (i == 0 || j == 0 || std::abs(i) == std::abs(j))
				{
					if (i == 0 && j == 0)
					{
						continue;
					}
					else
					{
						auto n = Dir(i, j);
						//MaskNum.emplace(n, cv::Point2i(i, j));
						MaskNum[n].push_back(cv::Point2i(i, j));
					}
				}
			}
		}

		////LOGD("GetMaskNum size %d", MaskNum.size());
	}

	int Dir(int & i, int & j)
	{
		if (i == 0 || j == 0)////要去除当前0，0的这个位置的点
		{
			if (i < 0)
			{
				return MaskNumDir::Left;
			}
			else if (i > 0)
			{
				return MaskNumDir::Right;
			}
			if (j < 0)
			{
				return MaskNumDir::Top;
			}
			else if (j > 0)
			{
				return MaskNumDir::Lower;
			}
		}
		else
		{
			if (i < 0)
			{
				if (j < 0)
				{
					return MaskNumDir::LeftTop;
				}
				else
				{
					return MaskNumDir::LeftLower;
				}
			}
			else
			{
				if (j < 0)
				{
					return MaskNumDir::RightTop;
				}
				else
				{
					return MaskNumDir::RightLower;
				}
			}
		}
		return MaskNumDir::Left;
	}

	void frameCallback(TY_FRAME_DATA * frame, void * userdata)
	{
		//LOGD("1_1 *********************");
		CallbackData* pData = (CallbackData*)userdata;
		LOGD("=== Get frame %d", ++pData->index);

		int fps = get_fps();
		if (fps > 0) {
			LOGI("fps: %d", fps);
		}

		parseFrame(*frame, &pData->depth, &pData->leftIR, &pData->rightIR, &pData->color, pData->hIspHandle);

		float preset_threshold_max = 500;//500？？是不是有点小了
		float left_num = 0;
		float right_num = 0;

		int depth_cols_cen = pData->depth.cols / 2;
		int depth_rows_cen = pData->depth.rows / 2;
		cv::Mat test_vec_color = pData->color;

		cv::Mat test_resu_color(depth_H, depth_W, CV_16UC3);

		cv::Rect rect_copy(depth_cols_cen - depth_H / 2, depth_rows_cen - depth_W / 2, depth_H, depth_W);
		cv::Mat rect_color;
		pData->color(rect_copy).copyTo(rect_color);
		imshow("截取的color", rect_color);
		cv::waitKey(1);
		if (!pData->color.empty()) {
			cv::imshow("Color", pData->color); cv::waitKey(1);}

		//LOGD("2_1 *********************");

		cv::Rect rect_copy_(depth_cols_cen - depth_H / 2, depth_rows_cen - depth_W / 2, depth_H, depth_W);
		cv::Mat rect_depth;
		pData->depth(rect_copy_).copyTo(rect_depth);
		//LOGD("2_2 *********************");
		type_array rect_depth_array(depth_W, std::vector<float>(depth_H));
		MatToArray(rect_depth, rect_depth_array);
		//LOGD("2_3 *********************");
		PushDepthDate(/*rect_depth*/rect_depth_array);

		//LOGD("3_1 *********************");
		if (!flag_standard_img->load())
		{
			some_mutex.lock();
			now_img.assign(rect_depth_array.begin(), rect_depth_array.end());
			some_mutex.unlock();
		}

		if (flag_standard_img->load())
		{
			////float dif_value = DValueImg(standard_img, now_img);
			float dif_value = DValueImg(standard_img, rect_depth_array) / conversion_rate;
			ree = dif_value;

			//////auto retuen2 = TYDepthSpeckleFilter(image_data, de);
			std::cout << std::fixed << "two img different value is " << dif_value << "立方分米" << std::endl;
			if (!rect_depth.empty())
			{
				pData->dViewer->show(rect_depth);
			}
		}
		else
		{
			ree = 1.0F;
		}


		std::cout << "REE *********************    " << ree << std::endl;

		TYISPUpdateDevice(pData->hIspHandle);

		if (!pData->color.empty()) {
			LOGI("Color format is %s", colorFormatName(TYImageInFrame(*frame, TY_COMPONENT_RGB_CAM)->pixelFormat));
		}
		int key = cv::waitKey(1);
		switch (key & 0xff) {
		case 0xff:
			break;
		case 'q':
			pData->exit = true;
			// have to call TYUnregisterCallback to release thread
			break;
		default:
			LOGD("Unmapped key %d", key);
		}
		//LOGD("5_1 *********************");
		LOGD("=== Callback: Re-enqueue buffer(%p, %d)", frame->userBuffer, frame->bufferSize);
		ASSERT_OK(TYEnqueueBuffer(pData->hDevice, frame->userBuffer, frame->bufferSize));
	}

	void eventCallback(TY_EVENT_INFO * event_info, void * userdata)
	{
		if (event_info->eventId == TY_EVENT_DEVICE_OFFLINE) {
			LOGD("=== Event Callback: Device Offline!");
			// Note: 
			//Please set TY_BOOL_KEEP_ALIVE_ONOFF feature to false if you need to debug with breakpoint!
		}
		else if (event_info->eventId == TY_EVENT_LICENSE_ERROR) {
			LOGD("=== Event Callback: License Error!");
		}
	}

	float GetDepthInf(std::map<std::pair<int, int>, float>& Src, const std::pair<int, int>& point)
	{
		uint16_t resu = 0;
		int num = 0;
		float re_num = 0.0;
		int CurrX = 0;
		int CurrY = 0;
		uint16_t uu = 0;
		for (int q = 0; q < NeihborPos.size(); q++)
		{
			CurrX = point.first + NeihborPos.at(q).x;
			CurrY = point.second + NeihborPos.at(q).y;
			if ((CurrX >= 0 && CurrX < depth_H) && (CurrY >= 0 && CurrY < depth_W)) //防止越界
			{
				uu = Src[std::make_pair(CurrY, CurrX)];
				if (uu > 0 && uu <= 4000)
				{
					resu = resu + uu;
					++num;
				}
			}
		}
		if (num != 0 && resu != 0)
		{
			re_num = (resu / num)*1.0;
		}
		return re_num;
	}

	std::map<std::pair<int, int>, float> Check8NeihborPos(std::map<std::pair<int, int>, float>& map_src, const std::pair<int, int>& point)
	{
		std::map<std::pair<int, int>, float> re;
		int CurrX = 0;
		int CurrY = 0;
		//LOGD("Check8NeihborPos NeihborPos size %d", NeihborPos.size());
		for (int i = 0; i < NeihborPos.size(); i++)
		{
			CurrX = point.first + NeihborPos.at(i).x;
			CurrY = point.second + NeihborPos.at(i).y;
			if ((CurrX >= 0 && CurrX < depth_H) && (CurrY >= 0 && CurrY < depth_W)) //防止越界
			{
				if (map_src[std::make_pair(CurrY, CurrX)] > 0)
				{
					re.emplace(std::make_pair(CurrY, CurrX), map_src[std::make_pair(CurrY, CurrX)]);
				}
			}
		}
		return re;
	}

	std::map<int, std::map<std::pair<int, int>, float>> Check8MaskNum(std::map<std::pair<int, int>, float>& map_src, const std::pair<int, int>& point)
	{
		std::map<int, std::map<std::pair<int, int>, float>> re;
		int CurrX = 0;
		int CurrY = 0;
		//LOGD("Check8MaskNum MaskNum size %d", MaskNum.size());
		for (auto m : MaskNum)
		{
			auto dir = m.first;
			for (auto n : m.second)
			{
				CurrX = point.first + n.x;
				CurrY = point.second + n.y;
				if ((CurrX >= 0 && CurrX < depth_H) && (CurrY >= 0 && CurrY < depth_W)) //防止越界
				{
					if (map_src[std::make_pair(CurrY, CurrX)] > 0)
					{
						auto t = Dir(n.x, n.y);
						std::map<std::pair<int, int>, float> s;
						s.emplace(std::make_pair(std::make_pair(CurrY, CurrX), map_src[std::make_pair(CurrY, CurrX)]));
						re.emplace(t, s);
					}
				}
			}
		}
		return re;
	}

	float Get8AvgValue(std::map<std::pair<int, int>, float>& value)
	{
		float num = 0.0;
		float avg = 0.0;
		for (auto& m : value)
		{
			num += m.second;
		}
		avg = num / (value.size());
		return avg;
	}

	float GetlinePox(std::map<std::pair<int, int>, float>& P, int dir, std::pair<int, int>& Point)
	{
		float num = 0.0;
		int size_ = P.size();
		if (size_ == pox_dir_num || size_ >= 2)
		{
			////计算一下这个line的，一般情况下这个可以看成线性的，一般是平的或者向上或者向下，中间的话看看一个差值吧，基本上就照着这个方向走，一般就是三个像素了，也不做其他的考虑的吧
			////看看这个的差值
			if (dir == MaskNumDir::Top || dir == MaskNumDir::Lower)
			{
				int max_ = P.rbegin()->first.second;
				int min_ = P.begin()->first.second;
				int dif1 = max_ - min_;
				float dif2 = P.rbegin()->second - P.begin()->second;
				float dif3 = dif2 / dif1;////从小到大的时候的一个每一个像素点的差值的均值，可能是 + -
				return (max_ - Point.second)*dif3 + P.rbegin()->second;
			}
			else
			{
				int max_ = P.rbegin()->first.first;
				int min_ = P.begin()->first.first;
				int dif1 = max_ - min_;
				float dif2 = P.rbegin()->second - P.begin()->second;
				float dif3 = dif2 / dif1;////从小到大的时候的一个每一个像素点的差值的均值，可能是 + -
				return (max_ - Point.second)*dif3 + P.rbegin()->second;
			}
		}
		return num;
	}

	void MatToArray(cv::Mat & Src, std::vector<std::vector<float>>& Dst)
	{
		for (size_t i = 0; i < Src.cols; i++)
		{
			for (size_t j = 0; j < Src.rows; j++)
			{
				uint16_t a = Src.at<uint16_t>(i, j);
				Dst[i][j] = a;
				if (Dst[i][j] > 4000 /*|| Dst[i][j] == 0*/)
				{
					std::cout << a << " , " << i << " , " << j << std::endl;
				}
			}
		}
	}

	bool LackDepthMapTwo(std::map<std::pair<int, int>, float>& map_src, std::set<std::pair<int, int>>& map_dst)
	{
		//int hah = 0;
		//auto hah1 = map_dst.begin();
		//auto hah2 = map_src.begin();
		//LOGD("map_src --------********* %d ", map_src.size());
		//LOGD("map_dst --------********* %d ", map_dst.size());
		//while (++hah != 10)
		//{
		//	LOGD("2_5 map_src --------********* %d ", hah1->second);
		//	LOGD("2_5 map_dst --------********* %d ", hah2->second);
		//	++hah1;
		//	++hah2;
		//}
		while (true)
		{
			std::set<std::pair<int, int>> map_;
			std::map<std::pair<int, int>, std::map<std::pair<int, int>, float>> fill_point;
			std::map<std::pair<int, int>, float> fill_point_;
			std::map<std::pair<int, int>, float> fill_point8;
			for (auto m : map_dst)
			{
				std::map<int, float> number;
				auto re1 = Check8MaskNum(map_src, m);//dir points floats
				//LOGD("2_5 re1--------********* %d", re1.size());
				for (auto& n : re1)
				{
					float num = GetlinePox(n.second, n.first, m);
					if (num >= 1)
					{
						number.emplace(n.first, num);
					}
				}
				//LOGD("2_5 number-----********* %d", number.size());
				float a = 0;
				float b = 0;
				for (auto& n : number)
				{
					a += n.second;
				}
				if (!number.empty())
				{
					b = a / number.size();
					map_.emplace(m);
					//LOGD("2_6 ----------********* %d", map_.size());
					fill_point_.emplace(m, b);
				}

				auto re = Check8NeihborPos(map_src, m);
				if (!re.empty())
				{
					fill_point.emplace(m, re);
					map_.emplace(m);
					//LOGD("2_6 ----------********* %d", map_.size());
				}
			}
			for (auto& m : fill_point)
			{
				fill_point8.emplace(m.first, Get8AvgValue(m.second));
			}

			//LOGD("2_6 ****************** %d", map_.size());
			for (auto& m : map_)
			{
				int num = 0;
				float sum = 0;
				auto it1 = fill_point_.find(m);
				if (it1 != fill_point_.end())
				{
					sum += it1->second;
					++num;
				}
				auto it2 = fill_point8.find(m);
				if (it2 != fill_point8.end())
				{
					sum += it2->second;
					++num;
				}
				map_src[m] = sum / num;
				map_dst.erase(m);
			}
			//LOGD("2_7 ****************** %d", map_dst.size());

			if (map_dst.empty())
			{
				return false;
			}
		}
		return true;
	}

	void PushDepthDate(type_array & Src)
	{
		//LOGD("2_4 *********************");
		std::map<std::pair<int, int>, float> map_src;
		std::set<std::pair<int, int>> map_dst;
		for (int i = 0; i < Src.size(); i++)
		{
			for (int j = 0; j < Src[i].size(); j++)
			{
				map_src[std::make_pair(i, j)] = Src[i][j];
				if (Src[i][j] == 0 || Src[i][j] >= 4001)
				{
					map_dst.emplace(std::make_pair(i, j));
				}
			}
		}
		//LOGD("2_5 *********************");
		while (LackDepthMapTwo(map_src, map_dst))
		{

		}
		//LOGD("2_9 *********************");
		for (auto& it_map : map_src)
		{
			Src[it_map.first.first][it_map.first.second] = it_map.second;
		}
		//LOGD("2_10 *********************");
	}

	float DValueImg(type_array & Ori, type_array & Tar)
	{
		float diff_value = 0.0;
		float fab = 0.0;
		for (size_t i = 0; i < Tar.size(); i++)
		{
			for (size_t j = 0; j < Tar.size(); j++)
			{
				auto OR = Ori[i][j];
				auto TA = Tar[i][j];
				fab = 0.0;
				fab = OR -TA;
				if (fabs(fab) > 2)//由于float的数据可能会有小数点后面的小点点，因此换一种方法做一下一个约束，还有一个就是同一个话可能会有一个就是不动的情况下也会有小的差值的  
				{
					diff_value += (fab * pixel_distance_square);
				}
			}
		}
		return diff_value;
	}

	float LinkGetDiff()
	{
		return ree;
	}


	void AcceptFrontInf(std::string str)
	{
		//LOGD("AcceptFrontInf Start");
		if (str == "S" || str == "s")
		{
			LOGD("AcceptFrontInf Start Start");
			std::this_thread::sleep_for(std::chrono::seconds(10));
			LOGD("AcceptFrontInf Start Start ------------ ");
			some_mutex.lock();
			standard_img.assign(now_img.begin(), now_img.end());
			some_mutex.unlock();
			flag_standard_img->store(true);
			LOGD("AcceptFrontInf Start End");
		}
		else if (str == "Q" || str == "q")
		{
			LOGD("AcceptFrontInf Quert ------------ ");
			some_mutex.lock();
			standard_img.clear();
			some_mutex.unlock();
			flag_standard_img->store(false);
		}
		else
		{
			LOGE("AcceptFrontInf Is Error");
		}
	}

	void CameraProgram()
	{
		Base::Get8NeihborPos(NeihborPos);
		Base::GetMaskNum(MaskNum, 3);

		std::string ID, IP;
		TY_INTERFACE_HANDLE hIface = NULL;
		TY_DEV_HANDLE hDevice = NULL;
		int32_t color, ir, depth;
		color = ir = depth = 1;

		if (!color && !depth && !ir) {
			LOGD("At least one component need to be on");
			//return -1;
		}

		LOGD("Init lib");
		ASSERT_OK(TYInitLib());
		TY_VERSION_INFO ver;
		ASSERT_OK(TYLibVersion(&ver));
		LOGD("     - lib version: %d.%d.%d", ver.major, ver.minor, ver.patch);

		std::vector<TY_DEVICE_BASE_INFO> selected;
		ASSERT_OK(selectDevice(TY_INTERFACE_ALL, ID, IP, 1, selected));
		ASSERT(selected.size() > 0);
		TY_DEVICE_BASE_INFO& selectedDev = selected[0];

		ASSERT_OK(TYOpenInterface(selectedDev.iface.id, &hIface));
		ASSERT_OK(TYOpenDevice(hIface, selectedDev.id, &hDevice));

		int32_t allComps;
		ASSERT_OK(TYGetComponentIDs(hDevice, &allComps));
		if (allComps & TY_COMPONENT_RGB_CAM  && color) {
			LOGD("Has RGB camera, open RGB cam");
			ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_RGB_CAM));
		}

		if (allComps & TY_COMPONENT_IR_CAM_LEFT && ir) {
			LOGD("Has IR left camera, open IR left cam");
			ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_IR_CAM_LEFT));
		}

		if (allComps & TY_COMPONENT_IR_CAM_RIGHT && ir) {
			LOGD("Has IR right camera, open IR right cam");
			ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_IR_CAM_RIGHT));
		}

		//try to enable depth map
		LOGD("Configure components, open depth cam");
		DepthViewer depthViewer("Depth");
		if (allComps & TY_COMPONENT_DEPTH_CAM && depth) {
			int32_t image_mode;
			ASSERT_OK(get_default_image_mode(hDevice, TY_COMPONENT_DEPTH_CAM, image_mode));
			LOGD("Select Depth Image Mode: %dx%d", TYImageWidth(image_mode), TYImageHeight(image_mode));
			ASSERT_OK(TYSetEnum(hDevice, TY_COMPONENT_DEPTH_CAM, TY_ENUM_IMAGE_MODE, image_mode));
			ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_DEPTH_CAM));

			//depth map pixel format is uint16_t ,which default unit is  1 mm
			//the acutal depth (mm)= PixelValue * ScaleUnit 
			float scale_unit = 1.;
			TYGetFloat(hDevice, TY_COMPONENT_DEPTH_CAM, TY_FLOAT_SCALE_UNIT, &scale_unit);
			depthViewer.depth_scale_unit = scale_unit;
		}

		LOGD("Prepare image buffer");
		uint32_t frameSize;
		ASSERT_OK(TYGetFrameBufferSize(hDevice, &frameSize));
		LOGD("     - Get size of framebuffer, %d", frameSize);

		LOGD("     - Allocate & enqueue buffers");
		char* frameBuffer[2];
		frameBuffer[0] = new char[frameSize];
		frameBuffer[1] = new char[frameSize];
		LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[0], frameSize);
		ASSERT_OK(TYEnqueueBuffer(hDevice, frameBuffer[0], frameSize));
		LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[1], frameSize);
		ASSERT_OK(TYEnqueueBuffer(hDevice, frameBuffer[1], frameSize));

		LOGD("Register event callback");
		ASSERT_OK(TYRegisterEventCallback(hDevice, eventCallback, NULL));

		bool hasTrigger;
		ASSERT_OK(TYHasFeature(hDevice, TY_COMPONENT_DEVICE, TY_STRUCT_TRIGGER_PARAM, &hasTrigger));
		if (hasTrigger) {
			LOGD("Disable trigger mode");
			TY_TRIGGER_PARAM trigger;
			trigger.mode = TY_TRIGGER_MODE_M_PER;
			trigger.fps = 1;
			ASSERT_OK(TYSetStruct(hDevice, TY_COMPONENT_DEVICE, TY_STRUCT_TRIGGER_PARAM, &trigger, sizeof(trigger)));
		}

		// Create callback thread
		CallbackData cb_data;
		cb_data.index = 0;
		cb_data.hDevice = hDevice;
		cb_data.exit = false;
		cb_data.hIspHandle = NULL;
		cb_data.dViewer = &depthViewer;
		// Register Callback
		CallbackWrapper cbWrapper;
		cbWrapper.TYRegisterCallback(hDevice, frameCallback, &cb_data);////这个里面包含了去获取帧的那个

		int32_t componentIDs = 0;
		ASSERT_OK(TYGetEnabledComponents(hDevice, &componentIDs));
		if (allComps & TY_COMPONENT_RGB_CAM & componentIDs) {
			ASSERT_OK(TYISPCreate(&cb_data.hIspHandle));//create a isp handle to convert raw image(color bayer format) to rgb image
			ASSERT_OK(ColorIspInitSetting(cb_data.hIspHandle, hDevice));//Init code can be modified in common.hpp
			//You can  call follow function to show  color isp supported features
#if 0
			ColorIspShowSupportedFeatures(hColorIspHandle);
#endif
			//You can turn on auto exposure function as follow ,but frame rate may reduce .
			//Device may be casually stucked  1~2 seconds while software is trying to adjust device exposure time value
#if 0
			ASSERT_OK(ColorIspInitAutoExposure(cb_data.hIspHandle, hDevice));
#endif
		}

		LOGD("Start capture");
		ASSERT_OK(TYStartCapture(hDevice));

		LOGD("While loop to fetch frame");
		while (!cb_data.exit) {
			MSLEEP(1000 * 10);
		}

		cbWrapper.TYUnregisterCallback();
		ASSERT_OK(TYStopCapture(hDevice));
		ASSERT_OK(TYCloseDevice(hDevice));
		ASSERT_OK(TYCloseInterface(hIface));
		ASSERT_OK(TYISPRelease(&cb_data.hIspHandle));
		ASSERT_OK(TYDeinitLib());
		delete frameBuffer[0];
		delete frameBuffer[1];

		LOGD("Main done!");
	}


}