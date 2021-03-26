#pragma once

#include <windows.h>
#include "opencv2/opencv.hpp"
#include "RPConfig.h"
#include "rplidar.h"

using namespace cv;
using namespace rp::standalone::rplidar;

#define PI       3.14159265358979323846

struct LidarScanPoint
{
	LidarScanPoint() {
		dist = 1.5f;
		angle = 0.0f;
		valid = false;
	}
	LidarScanPoint(float _dist, float _angle) {
		dist = _dist;
		angle = _angle;
	}
	float dist;     // in meter
	float angle;    // in degree, 0 expected to be the front of LIDAR, and increase by rotate in counter-clockwise (left-hand system)
	bool valid;     // if the lidar scan point is valid or not (for eg. no obstacle detected)
};

class RPLidarWrapper {
public:
	RPLidarWrapper(){}
	~RPLidarWrapper();
	bool OpenLidar();
	void Update();
	void Clear();
	void Render();
	void CloseLidar();

	void SetDebugMode(int mode);

	// 是否点击到透视矩形区
	bool hitPerspectiveArea(Point2f position);
	
	int getHitCorner(Point2f position);
	// 判断鼠标是否点击到某个角点
	bool checkHitPoint(Point2f target, Point2f origin);
	// 设置角点的位置
	void SetCorner(int index, Point2f newPosition);
	// 自动调整角点
	void AdaptiveCorners();

	// 目标透视矩形
	Rect targetPerspectiveRect();
	std::vector<Point2f> rect2Points(Rect rect);

	void AddMouseSimulate(Point2f);
	void ClearSimulate();

	short* getTouchPoints();

private:
	void initialize();
	void loadConfig();
	void saveConfig();
	void buildTouchPoints();
	void renderTouchPoints();
	void syncCorners();
	void fetchScanPoints();
	// 显示雷达调试窗口
	void EnableDebugUI(bool bEnable);
	// 显示预览窗口
	void EnablePreviewUI(bool bEnable);
	// 显示所有调试窗口
	void EnableAll(bool bEnable);
	void renderPlots();
	void renderScanPoints();
	void renderScreenCorners();
	void polar2cartesian(const LidarScanPoint& laserPoint, float& x, float& y);
	static void MouseEventHandler(int evt, int x, int y, int flags, void* param);
	static void PreviewMouseEventHandler(int evt, int x, int y, int flags, void* param);

	// 将opencv屏幕坐标映射到中心坐标
	Point2f RPLidarWrapper::map2Screen(Point2f point);
	// 将中心坐标映射到屏幕坐标
	Point2f RPLidarWrapper::map2Center(Point2f point);

	Point2f RPLidarWrapper::mapPhysical2ScreenCenter(Point2f point);
	Point2f mapScreenCenter2Physical(Point2f point);

public:
	RPConfigMgr rpConfig;
	float debugRadius = 3;	// 2M
	int angleOffset = 0;
	RPlidarDriver* rpDriver = nullptr;
	Point DebugUISize = Point(800,800);
	// 雷达扫描原始极坐标点
	std::vector<LidarScanPoint> lidarScanPoints;
	// 用于模拟的屏幕点
	std::vector<Point2f> simulatePoints;
	
	// 最终的触摸点	坐标值范围 [0-1]
	std::vector<Point2f> touchPoints;
	// 雷达点映射到屏幕坐标的点
	std::vector<Point2f> lidarScreenPoints;
	//std::vector<Point2f> lidarPhysicalPoints;
	Mat screen = Mat::zeros(800,800,CV_8UC3);
	Mat preview = Mat::zeros(720, 1280, CV_8UC3);

	Point2f P1, P2, P3, P4;
	Point2f SP1, SP2, SP3, SP4;

	Point2f perspactiveOrigin[4];

	bool bEnablePreviewUI = false;
	bool bEnableDebugUI = false;
};