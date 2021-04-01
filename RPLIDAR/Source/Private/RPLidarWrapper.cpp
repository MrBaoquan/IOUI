#include "RPLidarWrapper.h"
#include <math.h>
#include <algorithm>

#pragma  comment(lib,"rplidar_driver.lib")
#ifdef _DEBUG
#pragma comment(lib,"opencv_world440d.lib")
#else
#pragma comment(lib,"opencv_world440.lib")
#endif // WIN_64

#define DebugUI "调试窗口"
#define PreviewUI "预览窗口"

#define CV_SORT_EVERY_ROW    0
#define CV_SORT_EVERY_COLUMN 1
#define CV_SORT_ASCENDING    0
#define CV_SORT_DESCENDING   16

bool sortPointX_ASC(Point2f lhf, Point2f rhs) {
	return lhf.x < rhs.x;
}
bool sortPointX_DESC(Point2f lhf, Point2f rhs) {
	return lhf.x > rhs.x;
}

bool sortPointY_ASC(Point2f lhf, Point2f rhs) {
	return lhf.y < rhs.y;
}

bool sortPointY_DESC(Point2f lhf, Point2f rhs) {
	return lhf.y > rhs.y;
}

bool RPLidarWrapper::OpenLidar()
{
	this->initialize();
	rpDriver = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
	if (!rpDriver) {
		return false;
	}
	rplidar_response_device_info_t _deviceInfo;
	if (IS_OK(rpDriver->connect(rpConfig.config.PortName.data(), rpConfig.config.BaudRate))) {
		if (IS_OK(rpDriver->getDeviceInfo(_deviceInfo))) {
			rpDriver->startMotor();
			rpDriver->startScan(0, 1);
			return true;
		}
		else {
			delete rpDriver;
			rpDriver = nullptr;
		}
	}
	else {
		delete rpDriver;
		rpDriver = nullptr;
	};
	return false;
}

void onChangeTrackBar(int poi, void* usrdata)
{
	RPLidarWrapper* rpLidar = (RPLidarWrapper*)usrdata;
}
// 坐标系为 向右为正  向下为正
void RPLidarWrapper::initialize()
{
	rpConfig.Read();
	SetDebugMode(rpConfig.config.debugMode);
	loadConfig();
	syncCorners();
}

void RPLidarWrapper::syncCorners()
{
	SP1 = mapPhysical2ScreenCenter(P1);
	SP2 = mapPhysical2ScreenCenter(P2);
	SP3 = mapPhysical2ScreenCenter(P3);
	SP4 = mapPhysical2ScreenCenter(P4);
}

void RPLidarWrapper::Update()
{
	cv::waitKey(1);
	fetchScanPoints();
	buildTouchPoints();
	Render();
}


void RPLidarWrapper::Clear()
{
	this->screen.setTo(Scalar::all(0));
	this->preview.setTo(Scalar::all(0));
}

void RPLidarWrapper::Render()
{
	this->Clear();
	if (bEnableDebugUI) {
		renderPlots();
		// 雷达扫描点
		renderScanPoints();
		renderScreenCorners();
		cv::imshow(DebugUI, this->screen);
	}
	if (bEnablePreviewUI) {
		// 映射后的屏幕坐标点
		renderTouchPoints();
		cv::imshow(PreviewUI, this->preview);
	}
}

// 雷达点与屏幕坐标点转换
void RPLidarWrapper::buildTouchPoints()
{
	Point2f srcPoints[] = {
		SP1,SP2,SP3,SP4
	};

	Rect _rect = this->targetPerspectiveRect();
	std::vector<Point2f> _dstPoints = rect2Points(_rect);
	Mat _transform = getPerspectiveTransform(srcPoints, _dstPoints.data());

	std::vector<Point2f> _targets(lidarScreenPoints.size());
	if (lidarScreenPoints.size() > 0) {
		perspectiveTransform(lidarScreenPoints, _targets, _transform);
	}
	auto _originPoint = _dstPoints[0];

	// 最终的映射触摸点
	std::vector<Point2f> _touchPoints(lidarScreenPoints.size());
	auto _config = this->rpConfig;
	std::transform(_targets.begin(), _targets.end(), _touchPoints.begin(), [_rect](Point2f _value) {
		_value.x = ((_value.x - _rect.x) / _rect.width);
		_value.y = ((_value.y - _rect.y) / _rect.height);
		return _value;
	});
	
	// 过滤非触摸区域的点
	_touchPoints.erase(
		std::remove_if(
			_touchPoints.begin(),
			_touchPoints.end(),
			[](const Point2f& item) {return item.x < 0||item.x>1 || item.y < 0 || item.y>1; }),
		_touchPoints.end());

	if (simulatePoints.size() > 0) {
		_touchPoints.insert(_touchPoints.begin(), simulatePoints.begin(), simulatePoints.end());
	}
	
	touchPoints = _touchPoints;
}


void RPLidarWrapper::renderTouchPoints()
{
	std::vector<Point2f> _previewTouchPoints(touchPoints.size());
	//  绘制在模拟UI上
	std::transform(touchPoints.begin(), touchPoints.end(), _previewTouchPoints.begin(), [](Point2f _value) {
		_value.x = _value.x * 1280;
		_value.y = _value.y * 720;
		return _value;
	});

	int _index = 0;
	for each (auto& _point in _previewTouchPoints)
	{
		auto _new = _point;
		circle(this->preview, _new, _index==0?6:3, Scalar(0, 255, 0), -1);
		_index++;
	}
}

void RPLidarWrapper::fetchScanPoints()
{
	if (rpDriver != nullptr) {
		rplidar_response_measurement_node_t nodes[2048];
		size_t cnt = _countof(nodes);
		auto _result = rpDriver->grabScanData(nodes, cnt, 0);
		if (IS_OK(_result)) {
			lidarScanPoints.clear();
			lidarScreenPoints.clear();
			rpDriver->ascendScanData(nodes, cnt);
			for (int pos = 0; pos < (int)cnt; ++pos) {
				float _angle = (nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
				float _length = nodes[pos].distance_q2 / 4000.0f;	// 
				if (!nodes[pos].distance_q2) {
					continue;
				}
				LidarScanPoint p(_length, _angle);
				lidarScanPoints.push_back(p);

				float _x = 0;
				float _y = 0;
				polar2cartesian(p, _x, _y);
				auto _point = mapPhysical2ScreenCenter(Point2f(_x, _y));
				lidarScreenPoints.push_back(_point);
			}
		}
	}
}

// 绘制雷达坐标图
void RPLidarWrapper::renderPlots()
{
	auto& mat = this->screen;
	if (rpDriver) {
		putText(this->screen, "Lidar Status: Opened", Point2f(10, 30), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 0));
	}
	else {
		putText(this->screen, "Lidar Status: Not Open", Point2f(10, 30), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
	}
	
	line(mat, Point(0, DebugUISize.y / 2), Point(DebugUISize.x, DebugUISize.y / 2), Scalar(50, 50, 50), 1);
	line(mat, Point(DebugUISize.x / 2, 0), Point(DebugUISize.x / 2, DebugUISize.y), Scalar(50, 50, 50), 1);
	Point2f center(DebugUISize.x / 2, DebugUISize.y / 2);
	float _radius0 = (0.5f / debugRadius) * center.y;
	float _radius1 = (1 / debugRadius) * center.y;
	float _radius2 = (3 / debugRadius) * center.y;
	float _radius3 = (5 / debugRadius) * center.y;
	float _radius4 = (7 / debugRadius) * center.y;
	float _radius5 = (10 / debugRadius) * center.y;
	circle(mat, center, _radius0, Scalar(100, 100, 100));
	circle(mat, center, _radius1, Scalar(100, 100, 100));
	circle(mat, center, _radius2, Scalar(100, 100, 100));
	circle(mat, center, _radius3, Scalar(100, 100, 100));
	circle(mat, center, _radius4, Scalar(100, 100, 100));
	circle(mat, center, _radius5, Scalar(100, 100, 100));

	putText(this->screen, "0.5M", center + Point2f(-18, _radius0), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	putText(this->screen, "1M", center + Point2f(-10, _radius1), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	putText(this->screen, "3M", center + Point2f(-10, _radius2), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	putText(this->screen, "5M", center + Point2f(-10, _radius3), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	putText(this->screen, "7M", center + Point2f(-10, _radius4), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	putText(this->screen, "10M", center + Point2f(-18, _radius5), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));

}

void RPLidarWrapper::renderScanPoints()
{
	auto& mat = this->screen;
	for each (auto& _point in this->lidarScreenPoints)
	{
		circle(mat, _point, 3, Scalar(0, 255, 255), -1);
	}
}


void RPLidarWrapper::renderScreenCorners()
{
	auto& mat = this->screen;
	syncCorners();
	circle(mat, SP1, 5, Scalar(0, 255, 255), -1);
	circle(mat, SP2, 5, Scalar(0, 255, 255), -1);
	circle(mat, SP3, 5, Scalar(0, 255, 255), -1);
	circle(mat, SP4, 5, Scalar(0, 255, 255), -1);

	line(this->screen, SP1, SP2, Scalar(0, 255, 0));
	line(this->screen, SP2, SP3, Scalar(0, 255, 0));
	line(this->screen, SP3, SP4, Scalar(0, 255, 0));
	line(this->screen, SP4, SP1, Scalar(0, 255, 0));
	float _textOffset = 10;
	putText(this->screen, "P1", SP1 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
	putText(this->screen, "P2", SP2 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
	putText(this->screen, "P3", SP3 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
	putText(this->screen, "P4", SP4 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
}

// 极坐标转笛卡尔坐标
void RPLidarWrapper::polar2cartesian(const LidarScanPoint& laserPoint, float& x, float& y)
{
	float rad = (laserPoint.angle + this->angleOffset)  * PI / 180.0f;
	x = laserPoint.dist * -sin(rad);
	y = laserPoint.dist * cos(rad);
}


// 预览 鼠标回调
void RPLidarWrapper::PreviewMouseEventHandler(int evt, int x, int y, int flags, void* param) {
	RPLidarWrapper* rpLidar = (RPLidarWrapper*)param;	// 
	static bool bLeftMouseDown = false;
	if (evt == cv::EVENT_LBUTTONDOWN) {
		bLeftMouseDown = true;
		rpLidar->AddMouseSimulate(Point2f(x, y));
		SetCursor(LoadCursor(0, IDC_SIZE));
	}
	else if (evt == cv::EVENT_LBUTTONUP) {
		bLeftMouseDown = false;
		SetCursor(LoadCursor(0, IDC_ARROW));
		rpLidar->ClearSimulate();
	}
	else if (evt == cv::EVENT_MOUSEMOVE) {
		if (bLeftMouseDown) {
			rpLidar->AddMouseSimulate(Point2f(x, y));
		}
	}
}

void RPLidarWrapper::MouseEventHandler(int evt, int x, int y, int flags, void* param)
{
	static bool leftMouseDown = false;					// 鼠标左键是否按下
	static int hitCorner = -1;			// 选中的某个角
	static Point mouseDownOrigin = Point(-1000, -1000);
	
	static std::vector<Point2f> cornerOrigin;

	RPLidarWrapper* rpLidar = (RPLidarWrapper*)param;	// 
	if (evt == cv::EVENT_MOUSEWHEEL) {
		if (getMouseWheelDelta(flags) > 0) {
			rpLidar->debugRadius -= 0.2f;
			rpLidar->debugRadius = std::max(0.5f, std::min(rpLidar->debugRadius,10.0f));
		}
		else {
			rpLidar->debugRadius += 0.2f;
			rpLidar->debugRadius = std::max(0.5f, std::min(rpLidar->debugRadius, 10.0f));
		}

	}
	else if (evt==cv::EVENT_LBUTTONDOWN) {
		leftMouseDown = true;
		hitCorner = rpLidar->getHitCorner(Point2f(x, y));
		if (hitCorner!=-1) {
			SetCursor(LoadCursor(0, IDC_SIZEALL));
		}
		else if(rpLidar->hitPerspectiveArea(Point2f(x,y))){
			SetCursor(LoadCursor(0, IDC_SIZEALL));
			mouseDownOrigin = Point(x, y);
			cornerOrigin.clear();
			cornerOrigin.push_back(rpLidar->SP1);
			cornerOrigin.push_back(rpLidar->SP2);
			cornerOrigin.push_back(rpLidar->SP3);
			cornerOrigin.push_back(rpLidar->SP4);
		}
	}
	else if (evt == cv::EVENT_LBUTTONUP) {
		leftMouseDown = false;
		hitCorner = -1;
		mouseDownOrigin = Point(-1000, -1000);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}
	else if (evt == cv::EVENT_RBUTTONDOWN) {
		rpLidar->AdaptiveCorners();
	}
	else if (evt == cv::EVENT_MOUSEMOVE) {
		if (leftMouseDown&&hitCorner!=-1) {
			rpLidar->SetCorner(hitCorner, Point2f(x, y));
		}
		if (leftMouseDown&&mouseDownOrigin.x != -1000) {
			int _delta_x = x - mouseDownOrigin.x;
			int _delta_y = y - mouseDownOrigin.y;
			rpLidar->SetCorner(1,  cornerOrigin[0] + Point2f(_delta_x,_delta_y));
			rpLidar->SetCorner(2, cornerOrigin[1] + Point2f(_delta_x, _delta_y));
			rpLidar->SetCorner(3, cornerOrigin[2] + Point2f(_delta_x, _delta_y));
			rpLidar->SetCorner(4, cornerOrigin[3] + Point2f(_delta_x, _delta_y));
		}
		//OutputDebugStringA(_msg.data());
	}
	else if (evt==cv::EVENT_LBUTTONDBLCLK) {
		rpLidar->saveConfig();
	}
}


// 将坐标点映射回原始屏幕坐标
Point2f RPLidarWrapper::map2Screen(Point2f point)
{
	point.x -= DebugUISize.x / 2;
	point.y -= DebugUISize.y / 2;

	return point;
}


Point2f RPLidarWrapper::map2Center(Point2f point)
{
	point.x += DebugUISize.x / 2;
	point.y += DebugUISize.y / 2;
	return point;
}


Point2f RPLidarWrapper::mapPhysical2ScreenCenter(Point2f point)
{
	point.x = (point.x /this->debugRadius) * (DebugUISize.x/2);
	point.y = (point.y / this->debugRadius) * (DebugUISize.y/2);
	return map2Center(point);
}

cv::Point2f RPLidarWrapper::mapScreenCenter2Physical(Point2f point)
{
	point = map2Screen(point);
	point.x = (point.x / (DebugUISize.x / 2)) * this->debugRadius;
	point.y = (point.y / (DebugUISize.y / 2)) * this->debugRadius;
	return point;
}

// 获取四个角目标透视矩形
cv::Rect RPLidarWrapper::targetPerspectiveRect()
{
	Point2f _pts[] = { SP1,SP2,SP3,SP4 };
	std::sort(_pts, _pts + 4, sortPointX_ASC);
	std::sort(_pts, _pts + 2, sortPointY_ASC);

	auto _left_upper = _pts[0];

	std::sort(_pts, _pts + 4, sortPointX_DESC);
	std::sort(_pts, _pts + 2, sortPointY_DESC);
	auto _right_lowwer = _pts[0];

	return Rect(_left_upper,_right_lowwer);
}


std::vector<Point2f> RPLidarWrapper::rect2Points(Rect rect)
{
	std::vector<Point2f> _points;
	_points.push_back(Point2f(rect.x, rect.y));
	_points.push_back(Point2f(rect.x + rect.width, rect.y));
	_points.push_back(Point2f(rect.x + rect.width, rect.y + rect.height));
	_points.push_back(Point2f(rect.x, rect.y + rect.height));
	return _points;
}


void RPLidarWrapper::AddMouseSimulate(Point2f position)
{
	simulatePoints.clear();
	float _x = (position.x / 1280);
	float _y = (position.y / 720);
	simulatePoints.push_back(Point2f(_x,_y));
}


void RPLidarWrapper::ClearSimulate()
{
	simulatePoints.clear();
}

short* RPLidarWrapper::getTouchPoints()
{
	static std::vector<short> _cacheTouchPoints(64);
	std::vector<Point2f> _touchPoints(touchPoints.size());
	RPPoint _config = rpConfig.config.Screen;
	std::transform(touchPoints.begin(), touchPoints.end(), _touchPoints.begin(), [_config](Point2f point) {
		point.x *= _config.x;
		point.y *= _config.y;
		return point;
	});
	std::fill(_cacheTouchPoints.begin(), _cacheTouchPoints.end(), 0);
	int _maxCount = std::min(32, (int)_touchPoints.size());
	for (int _index = 0; _index < _maxCount; ++_index) {
		_cacheTouchPoints[_index * 2] = static_cast<int>(_touchPoints[_index].x);
		_cacheTouchPoints[_index * 2 + 1] = static_cast<int>(_touchPoints[_index].y);
	}
	return _cacheTouchPoints.data();
}

void RPLidarWrapper::CloseLidar()
{
	if (rpDriver) {
		rpDriver->stop();
		rpDriver->stopMotor();
		RPlidarDriver::DisposeDriver(rpDriver);
	}
}


void RPLidarWrapper::EnableDebugUI(bool bEnable)
{
	if (bEnable == bEnableDebugUI) return;
	bEnableDebugUI = bEnable;
	if (bEnable) {
		cv::imshow(DebugUI, this->screen);
		createTrackbar("角度偏移", DebugUI, &this->angleOffset, 360, onChangeTrackBar, this);
		setMouseCallback(DebugUI, MouseEventHandler, this);
	}
	else {
		setMouseCallback(DebugUI, NULL, this);
		waitKey(10);
		destroyWindow(DebugUI);
	}
}


void RPLidarWrapper::EnablePreviewUI(bool bEnable)
{
	if (bEnable == bEnablePreviewUI) return;
	bEnablePreviewUI = bEnable;
	if (bEnable) {
		cv::imshow(PreviewUI, this->preview);
		setMouseCallback(PreviewUI, PreviewMouseEventHandler, this);
	}
	else {
		waitKey(10);
		destroyWindow(PreviewUI);
	}
}


void RPLidarWrapper::EnableAll(bool bEnable)
{
	try
	{
		EnableDebugUI(bEnable);
		EnablePreviewUI(bEnable);
	}
	catch(...){
		OutputDebugStringA("Some error occur");
	}
	
}


void RPLidarWrapper::SetDebugMode(int mode)
{
	static int __mode = -1;
	if (__mode == mode) return;
	rpConfig.config.debugMode = mode;
	__mode = mode;
	if (mode == 0) {
		EnableAll(true);
	}
	else if (mode == 1) {
		EnableDebugUI(true);
		EnablePreviewUI(false);
	}
	else if (mode == 2) {
		EnableDebugUI(false);
		EnablePreviewUI(true);
	}
	else if (mode == -1) {
		EnableAll(false);
	}
}

bool RPLidarWrapper::hitPerspectiveArea(Point2f position)
{
	return targetPerspectiveRect().contains(position);
}

int RPLidarWrapper::getHitCorner(Point2f position)
{
	if (checkHitPoint(SP1, position)) {
		return 1;
	}
	if (checkHitPoint(SP2, position)) {
		return 2;
	}
	if (checkHitPoint(SP3, position)) {
		return 3;
	}
	if (checkHitPoint(SP4, position)) {
		return 4;
	}
	return -1;
}

float distance(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	float _x2 = pow(x2 - x1, 2);
	float _y2 = pow(y2 - y1, 2);
	return _x2 + _y2;
}
bool RPLidarWrapper::checkHitPoint(Point2f target, Point2f origin)
{
	float _dis = distance(target.x, target.y, origin.x,origin.y);
	/*OutputDebugStringA("\r\n");
	OutputDebugStringA(std::to_string(_dis).data());*/
	return _dis <=100;
}

void RPLidarWrapper::SetCorner(int index, Point2f newPosition)
{
	if (index == 1) {
		P1 = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 2) {
		P2 = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 3) {
		P3 = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 4) {
		P4 = mapScreenCenter2Physical(newPosition);
	}
}

void RPLidarWrapper::AdaptiveCorners()
{
	auto _rect = this->targetPerspectiveRect();
	auto _points = rect2Points(_rect);
	SetCorner(1, _points[0]);
	SetCorner(2, _points[1]);
	SetCorner(3, _points[2]);
	SetCorner(4, _points[3]);
}


/// 


// 加载配置
void RPLidarWrapper::loadConfig()
{
	P1 = Point2f(rpConfig.config.Corners[0].x, rpConfig.config.Corners[0].y);
	P2 = Point2f(rpConfig.config.Corners[1].x, rpConfig.config.Corners[1].y);
	P3 = Point2f(rpConfig.config.Corners[2].x, rpConfig.config.Corners[2].y);
	P4 = Point2f(rpConfig.config.Corners[3].x, rpConfig.config.Corners[3].y);
	debugRadius = rpConfig.config.DebugRadius;
	angleOffset = rpConfig.config.AngleOffset;
}

// 保存配置
void RPLidarWrapper::saveConfig()
{
	rpConfig.config.DebugRadius = debugRadius;
	rpConfig.config.AngleOffset = angleOffset;
	rpConfig.config.Corners[0] = RPPoint(P1.x, P1.y);
	rpConfig.config.Corners[1] = RPPoint(P2.x, P2.y);
	rpConfig.config.Corners[2] = RPPoint(P3.x, P3.y);
	rpConfig.config.Corners[3] = RPPoint(P4.x, P4.y);
	rpConfig.Save();
}




RPLidarWrapper::~RPLidarWrapper()
{
	this->screen.release();
	this->preview.release();
}
