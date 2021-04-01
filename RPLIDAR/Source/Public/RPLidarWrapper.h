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

	// �Ƿ�����͸�Ӿ�����
	bool hitPerspectiveArea(Point2f position);
	
	int getHitCorner(Point2f position);
	// �ж�����Ƿ�����ĳ���ǵ�
	bool checkHitPoint(Point2f target, Point2f origin);
	// ���ýǵ��λ��
	void SetCorner(int index, Point2f newPosition);
	// �Զ������ǵ�
	void AdaptiveCorners();

	// Ŀ��͸�Ӿ���
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
	// ��ʾ�״���Դ���
	void EnableDebugUI(bool bEnable);
	// ��ʾԤ������
	void EnablePreviewUI(bool bEnable);
	// ��ʾ���е��Դ���
	void EnableAll(bool bEnable);
	void renderPlots();
	void renderScanPoints();
	void renderScreenCorners();
	void polar2cartesian(const LidarScanPoint& laserPoint, float& x, float& y);
	static void MouseEventHandler(int evt, int x, int y, int flags, void* param);
	static void PreviewMouseEventHandler(int evt, int x, int y, int flags, void* param);

	// ��opencv��Ļ����ӳ�䵽��������
	Point2f RPLidarWrapper::map2Screen(Point2f point);
	// ����������ӳ�䵽��Ļ����
	Point2f RPLidarWrapper::map2Center(Point2f point);

	Point2f RPLidarWrapper::mapPhysical2ScreenCenter(Point2f point);
	Point2f mapScreenCenter2Physical(Point2f point);

public:
	RPConfigMgr rpConfig;
	float debugRadius = 3;	// 2M
	int angleOffset = 0;
	RPlidarDriver* rpDriver = nullptr;
	Point DebugUISize = Point(800,800);
	// �״�ɨ��ԭʼ�������
	std::vector<LidarScanPoint> lidarScanPoints;
	// ����ģ�����Ļ��
	std::vector<Point2f> simulatePoints;
	
	// ���յĴ�����	����ֵ��Χ [0-1]
	std::vector<Point2f> touchPoints;
	// �״��ӳ�䵽��Ļ����ĵ�
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