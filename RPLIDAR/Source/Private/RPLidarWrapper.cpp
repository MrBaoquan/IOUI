#include "RPLidarWrapper.h"
#include <math.h>
#include <algorithm>

// #pragma  comment(lib,"rplidar_driver.lib")

#ifdef _DEBUG
#pragma comment(lib,"opencv_world440d.lib")
#else
#pragma comment(lib,"opencv_world440.lib")
#endif // WIN_64

#define DebugUI "RPLidar Settings"
#define PreviewUI "Debug Window"

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

	rpDriver = *createLidarDriver();
	if (!rpDriver) {
		openSucceed = false;
		delete rpDriver;
		rpDriver = nullptr;
		return false;
	}
	
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	IChannel* _channel;

	if (_config->Driver == "SERIALPORT") {
		_channel = (*createSerialPortChannel(_config->PortName, _config->BaudRate));
	}
	else if (_config->Driver == "TCP") {
		_channel = (*createTcpChannel(_config->IP, _config->Port));
	}
	else if (_config->Driver == "UDP") {
		_channel = (*createUdpChannel(_config->IP, _config->Port));
	}
	else {
		delete rpDriver;
		rpDriver = nullptr;
		return false;
	}
	
	if (SL_IS_FAIL((rpDriver)->connect(_channel))) {
		delete rpDriver;
		rpDriver = nullptr;
		return false;
	}
	
	rplidar_response_device_info_t _deviceInfo;
	if (SL_IS_FAIL(rpDriver->getDeviceInfo(_deviceInfo))) {
		delete rpDriver;
		rpDriver = nullptr;
		return false;
	}
	rpDriver->setMotorSpeed();
	rpDriver->startScan(false, true);
	
	return true;
}

void onChangeTrackBar(int poi, void* usrdata)
{
	RPLidarWrapper* rpLidar = (RPLidarWrapper*)usrdata;
}
// 坐标系为 向右为正  向下为正
void RPLidarWrapper::initialize()
{
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	this->debugUIKey = "DebugUI-" + std::to_string(deviceID);
	this->previewUIKey = "PreviewUI-" + std::to_string(deviceID);
	
	SetDebugMode(_config->debugMode);
	loadConfig();
	syncPreviewUISize();
	syncCorners();
}

void RPLidarWrapper::syncCorners()
{
	for (int _index = 0; _index < this->Ps.size(); ++_index) {
		auto _points = Ps[_index];
		for (int _pIndex = 0; _pIndex < _points.size(); ++_pIndex) {
			SPs[_index][_pIndex] = mapPhysical2ScreenCenter(Ps[_index][_pIndex]);
		}
	}
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
		cv::imshow(this->debugUIKey, this->screen);
	}
	if (bEnablePreviewUI) {
		// 映射后的屏幕坐标点
		renderTouchPoints();
		cv::imshow(this->previewUIKey, this->preview);
	}
}

// 雷达点与屏幕坐标点转换
void RPLidarWrapper::buildTouchPoints()
{
	for (int _areaIndex = 0; _areaIndex < Ps.size(); ++_areaIndex) {
		auto _ps = Ps[_areaIndex];
		auto _sps = SPs[_areaIndex];
		Point2f srcPoints[] = {
			_sps[0],_sps[1],_sps[2],_sps[3]
		};
		Rect _rect = this->targetPerspectiveRect(_sps);
		std::vector<Point2f> _dstPoints = rect2Points(_rect);
		Mat _transform = getPerspectiveTransform(srcPoints, _dstPoints.data());

		std::vector<Point2f> _targets(lidarScreenPoints.size());
		if (lidarScreenPoints.size() > 0) {
			perspectiveTransform(lidarScreenPoints, _targets, _transform);
		}
		auto _originPoint = _dstPoints[0];

		// 最终的映射触摸点
		std::vector<Point2f> _touchPoints(lidarScreenPoints.size());
		
		auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
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
				[](const Point2f& item) {return item.x < 0 || item.x>1 || item.y < 0 || item.y>1; }),
			_touchPoints.end());

		if (_areaIndex == this->debugAreaIndex) {
			if (simulatePoints.size() > 0) {
				_touchPoints.insert(_touchPoints.begin(), simulatePoints.begin(), simulatePoints.end());
			}
		}

		this->allTouchPoints[_areaIndex] = _touchPoints;
	}
}


void RPLidarWrapper::renderTouchPoints()
{
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	auto _scale = _config->DebugUIScale;
	auto _screenSize = _config->Screens[this->debugAreaIndex];

	auto _touchPoints = allTouchPoints[this->debugAreaIndex];
	std::vector<Point2f> _previewTouchPoints(_touchPoints.size());
	//  绘制在模拟UI上
	std::transform(_touchPoints.begin(), _touchPoints.end(), _previewTouchPoints.begin(), [_screenSize, _scale](Point2f _value) {
		_value.x = _value.x * _screenSize.x * _scale;
		_value.y = _value.y * _screenSize.y * _scale;
		return _value;
	});

	int _index = 0;
	for each (auto& _point in _previewTouchPoints)
	{
		auto _new = _point;
		cv::circle(this->preview, _new, _index == 0 ? 6 : 3, Scalar(0, 255, 0), -1);
		_index++;
	}
	std::string _debugMode = "Area " + std::to_string(this->debugAreaIndex);
	cv::putText(this->preview, _debugMode, Point2f(10, 50), FONT_HERSHEY_SIMPLEX, 0.6f, Scalar(200, 200, 200));
}

void RPLidarWrapper::fetchScanPoints()
{
	if (rpDriver != nullptr) {
		rplidar_response_measurement_node_hq_t nodes[2048];
		size_t cnt = _countof(nodes);
		auto _result = rpDriver->grabScanDataHq(nodes, cnt, 0);
		if (IS_OK(_result)) {
			lidarScanPoints.clear();
			lidarScreenPoints.clear();
			rpDriver->ascendScanData(nodes, cnt);
			for (int pos = 0; pos < (int)cnt; ++pos) {
				float _angle = (nodes[pos].angle_z_q14 * 90.f) / (1 << 14);
				float _length = nodes[pos].dist_mm_q2 / 1000.f / (1 << 2);	// 
				if (!nodes[pos].dist_mm_q2) {
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
		cv::putText(this->screen, "Lidar Status: Opened", Point2f(10, 30), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 0));
	}
	else {
		cv::putText(this->screen, "Lidar Status: Not Open", Point2f(10, 30), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 0, 255));
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
	float _radius6 = (15 / debugRadius) * center.y;
	cv::circle(mat, center, _radius0, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius1, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius2, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius3, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius4, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius5, Scalar(100, 100, 100));
	cv::circle(mat, center, _radius6, Scalar(100, 100, 100));

	cv::putText(this->screen, "0.5M", center + Point2f(-18, _radius0), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "1M", center + Point2f(-10, _radius1), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "3M", center + Point2f(-10, _radius2), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "5M", center + Point2f(-10, _radius3), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "7M", center + Point2f(-10, _radius4), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "10M", center + Point2f(-18, _radius5), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));
	cv::putText(this->screen, "15M", center + Point2f(-18, _radius6), FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(150, 150, 150));

}

void RPLidarWrapper::renderScanPoints()
{
	auto& mat = this->screen;
	for each (auto& _point in this->lidarScreenPoints)
	{
		cv::circle(mat, _point, 3, Scalar(0, 255, 255), -1);
	}
}

float alpha = 100;
Scalar colors[6] = {
	Scalar(67,112,255),
	Scalar(244,169,3),
	Scalar(67,160,71),
	Scalar(53,216,253),
	Scalar(170,36,142),
	Scalar(122,110,84)
};
void RPLidarWrapper::renderScreenCorners()
{
	auto& mat = this->screen;
	syncCorners();
	for (int _index = 0;  _index< this->SPs.size(); ++_index) {
		auto _spts = this->SPs[_index];
		Point2f SP1 = _spts[0], SP2 = _spts[1], SP3 = _spts[2], SP4 = _spts[3];
		auto _circleColor = colors[_index];
		cv::circle(mat, SP1, 5, _circleColor, -1);
		cv::circle(mat, SP2, 5, _circleColor, -1);
		cv::circle(mat, SP3, 5, _circleColor, -1);
		cv::circle(mat, SP4, 5, _circleColor, -1);

		line(this->screen, SP1, SP2, _circleColor);
		line(this->screen, SP2, SP3, _circleColor);
		line(this->screen, SP3, SP4, _circleColor);
		line(this->screen, SP4, SP1, _circleColor);
		float _textOffset = 10;

		cv::putText(this->screen, "P1", SP1 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, _circleColor);
		cv::putText(this->screen, "P2", SP2 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, _circleColor);
		cv::putText(this->screen, "P3", SP3 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, _circleColor);
		cv::putText(this->screen, "P4", SP4 + Point2f(_textOffset, -_textOffset), FONT_HERSHEY_SIMPLEX, 0.5f, _circleColor);

		std::string _screenText = "Area" + std::to_string(_index);
		auto _rect = this->targetPerspectiveRect(_spts);
		cv::putText(this->screen, _screenText.data(),
			Point(_rect.x + _rect.width / 2,
				_rect.y + _rect.height / 2),
			FONT_HERSHEY_SIMPLEX,
			0.5f,
			Scalar(150, 150, 150));
	}
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
	else if (evt==cv::EVENT_LBUTTONDBLCLK) {
		//OutputDebugStringA("Double Click");
		rpLidar->nextDebugIndex();
	}
	else if (evt == cv::EVENT_MOUSEMOVE) {
		if (bLeftMouseDown) {
			rpLidar->AddMouseSimulate(Point2f(x, y));
		}
	}
}


void RPLidarWrapper::nextDebugIndex()
{
	int _next = debugAreaIndex + 1;
	this->debugAreaIndex = _next >= Ps.size() ? 0 : _next;
	syncPreviewUISize();

}

void RPLidarWrapper::MouseEventHandler(int evt, int x, int y, int flags, void* param)
{
	static bool leftMouseDown = false;					// 鼠标左键是否按下
	static int hitCorner = -1;			// 选中的某个角
	static int hitAreaIndex = -1;
	static Point mouseDownOrigin = Point(-1000, -1000);

	static std::vector<Point2f> cornerOrigin;

	RPLidarWrapper* rpLidar = (RPLidarWrapper*)param;	// 
	if (evt == cv::EVENT_MOUSEWHEEL) {
		if (getMouseWheelDelta(flags) > 0) {
			rpLidar->debugRadius -= 0.2f;
			rpLidar->debugRadius = std::max(0.5f, std::min(rpLidar->debugRadius, 15.0f));
		}
		else {
			rpLidar->debugRadius += 0.2f;
			rpLidar->debugRadius = std::max(0.5f, std::min(rpLidar->debugRadius, 15.0f));
		}

	}
	else if (evt == cv::EVENT_LBUTTONDOWN) {
		leftMouseDown = true;
		hitCorner = rpLidar->getHitCorner(Point2f(x, y),hitAreaIndex);
		//int _hitArea = -1;
		if (hitCorner != -1) {
			SetCursor(LoadCursor(0, IDC_SIZEALL));
		}
		else if (rpLidar->hitPerspectiveArea(Point2f(x, y), hitAreaIndex)) {
			SetCursor(LoadCursor(0, IDC_SIZEALL));
			mouseDownOrigin = Point(x, y);
			cornerOrigin.clear();
			auto _spts = rpLidar->SPs[hitAreaIndex];
			cornerOrigin.push_back(_spts[0]);
			cornerOrigin.push_back(_spts[1]);
			cornerOrigin.push_back(_spts[2]);
			cornerOrigin.push_back(_spts[3]);
		}
	}
	else if (evt == cv::EVENT_LBUTTONUP) {
		leftMouseDown = false;
		hitCorner = -1;
		hitAreaIndex = -1;
		mouseDownOrigin = Point(-1000, -1000);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}
	else if (evt == cv::EVENT_RBUTTONDOWN) {
		if (rpLidar->hitPerspectiveArea(Point2f(x, y), hitAreaIndex)) {
			rpLidar->AdaptiveCorners(hitAreaIndex);
		}
		
	}
	else if (evt == cv::EVENT_MOUSEMOVE) {
		if (leftMouseDown&&hitCorner != -1) {
			rpLidar->SetCorner(hitCorner, Point2f(x, y), hitAreaIndex);
		}
		if (leftMouseDown&&mouseDownOrigin.x != -1000) {
			int _delta_x = x - mouseDownOrigin.x;
			int _delta_y = y - mouseDownOrigin.y;
			rpLidar->SetCorner(1, cornerOrigin[0] + Point2f(_delta_x, _delta_y),hitAreaIndex);
			rpLidar->SetCorner(2, cornerOrigin[1] + Point2f(_delta_x, _delta_y), hitAreaIndex);
			rpLidar->SetCorner(3, cornerOrigin[2] + Point2f(_delta_x, _delta_y), hitAreaIndex);
			rpLidar->SetCorner(4, cornerOrigin[3] + Point2f(_delta_x, _delta_y), hitAreaIndex);
		}
		//OutputDebugStringA(_msg.data());
	}
	else if (evt == cv::EVENT_LBUTTONDBLCLK) {
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
	point.x = (point.x / this->debugRadius) * (DebugUISize.x / 2);
	point.y = (point.y / this->debugRadius) * (DebugUISize.y / 2);
	return map2Center(point);
}

cv::Point2f RPLidarWrapper::mapScreenCenter2Physical(Point2f point)
{
	point = map2Screen(point);
	point.x = (point.x / (DebugUISize.x / 2)) * this->debugRadius;
	point.y = (point.y / (DebugUISize.y / 2)) * this->debugRadius;
	return point;
}


void RPLidarWrapper::syncPreviewUISize()
{
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	float _scale = _config->DebugUIScale;
	auto _size = _config->Screens[this->debugAreaIndex];
	this->preview = Mat(_size.y*_scale, _size.x*_scale, CV_8UC3);
}

// 获取四个角目标透视矩形
cv::Rect RPLidarWrapper::targetPerspectiveRect(std::vector<Point2f> spts)
{
	Point2f _pts[] = { spts[0],spts[1],spts[2],spts[3] };
	std::sort(_pts, _pts + 4, sortPointX_ASC);
	std::sort(_pts, _pts + 2, sortPointY_ASC);

	auto _left_upper = _pts[0];

	std::sort(_pts, _pts + 4, sortPointX_DESC);
	std::sort(_pts, _pts + 2, sortPointY_DESC);
	auto _right_lowwer = _pts[0];

	return Rect(_left_upper, _right_lowwer);
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
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	auto _scale = _config->DebugUIScale;
	auto _screenSize = _config->Screens[this->debugAreaIndex];
	simulatePoints.clear();
	float _x = (position.x / (_screenSize.x*_scale));
	float _y = (position.y / (_screenSize.y*_scale));
	simulatePoints.push_back(Point2f(_x, _y));
}


void RPLidarWrapper::ClearSimulate()
{
	simulatePoints.clear();
}

short* RPLidarWrapper::getTouchPoints()
{
	auto _config_main = RPConfigMgr::Instance().GetConfig(0);
	static std::vector<short> _cacheTouchPoints(64*6);
	for (int _areaIndex = 0; _areaIndex < allTouchPoints.size(); ++_areaIndex) {
		auto _touchPoints = allTouchPoints[_areaIndex];

		RPPoint _config = _config_main->Screens[_areaIndex];
		std::transform(_touchPoints.begin(), _touchPoints.end(), _touchPoints.begin(), [_config](Point2f point) {
			point.x *= _config.x;
			point.y *= _config.y;
			return point;
		});

		std::fill(_cacheTouchPoints.begin()+64*_areaIndex, _cacheTouchPoints.begin()+64*(_areaIndex+1), -1);
		int _maxCount = std::min(16, (int)_touchPoints.size());
		for (int _index = 0; _index < _maxCount; ++_index) {
			_cacheTouchPoints[_areaIndex * 32 + _index * 2] = static_cast<int>(_touchPoints[_index].x);
			_cacheTouchPoints[_areaIndex * 32 + _index * 2 + 1] = static_cast<int>(_touchPoints[_index].y);
		}
	}
	return _cacheTouchPoints.data();
}

void RPLidarWrapper::CloseLidar()
{
	EnableAll(false);
	if (rpDriver) {
		rpDriver->stop();
		rpDriver->setMotorSpeed(0);
		rpDriver->disconnect();
		delete rpDriver;
		rpDriver = nullptr;
		// rpDriver->stopMotor();
		// RPlidarDriver::DisposeDriver(rpDriver);
	}
}


void RPLidarWrapper::EnableDebugUI(bool bEnable)
{
	if (bEnable == bEnableDebugUI) return;
	bEnableDebugUI = bEnable;
	if (bEnable) {
		cv::imshow(debugUIKey, this->screen);
		createTrackbar("Angle Offfset", debugUIKey, &this->angleOffset, 360, onChangeTrackBar, this);
		setMouseCallback(debugUIKey, MouseEventHandler, this);
	}
	else {
		setMouseCallback(debugUIKey, NULL, this);
		waitKey(10);
		destroyWindow(debugUIKey);
	}
}


void RPLidarWrapper::EnablePreviewUI(bool bEnable)
{
	if (bEnable == bEnablePreviewUI) return;
	bEnablePreviewUI = bEnable;
	if (bEnable) {
		cv::imshow(previewUIKey, this->preview);
		setMouseCallback(previewUIKey, PreviewMouseEventHandler, this);
	}
	else {
		waitKey(10);
		destroyWindow(previewUIKey);
	}
}


void RPLidarWrapper::EnableAll(bool bEnable)
{
	try
	{
		EnableDebugUI(bEnable);
		EnablePreviewUI(bEnable);
	}
	catch (...) {
		OutputDebugStringA("Some error occur");
	}

}


void RPLidarWrapper::SetDebugMode(int mode)
{
	// static int __mode = -1;
	if (__mode == mode) return;
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	_config->debugMode = mode;
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

bool RPLidarWrapper::hitPerspectiveArea(Point2f position, int& areaIndex)
{
	for (int _index = 0; _index < this->SPs.size(); ++_index) {
		if (targetPerspectiveRect(this->SPs[_index]).contains(position)) {
			areaIndex = _index;
			return true;
		}
	}
	areaIndex = -1;
	return false;
}

int RPLidarWrapper::getHitCorner(Point2f position, int& areaIndex)
{
	for (int _index = 0; _index < SPs.size(); ++_index) {
		auto _spts = SPs[_index];
		areaIndex = _index;
		if (checkHitPoint(_spts[0], position)) {
			return 1;
		}
		if (checkHitPoint(_spts[1], position)) {
			return 2;
		}
		if (checkHitPoint(_spts[2], position)) {
			return 3;
		}
		if (checkHitPoint(_spts[3], position)) {
			return 4;
		}
	}
	areaIndex = -1;
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
	float _dis = distance(target.x, target.y, origin.x, origin.y);
	/*OutputDebugStringA("\r\n");
	OutputDebugStringA(std::to_string(_dis).data());*/
	return _dis <= 100;
}

void RPLidarWrapper::SetCorner(int index, Point2f newPosition, int areaIndex)
{
	if (index == 1) {
		Ps[areaIndex][0] = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 2) {
		Ps[areaIndex][1] = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 3) {
		Ps[areaIndex][2] = mapScreenCenter2Physical(newPosition);
	}
	else if (index == 4) {
		Ps[areaIndex][3] = mapScreenCenter2Physical(newPosition);
	}
}

void RPLidarWrapper::AdaptiveCorners(int areaIndex)
{
	auto _rect = this->targetPerspectiveRect(this->SPs[areaIndex]);
	auto _points = rect2Points(_rect);
	SetCorner(1, _points[0], areaIndex);
	SetCorner(2, _points[1], areaIndex);
	SetCorner(3, _points[2],areaIndex);
	SetCorner(4, _points[3], areaIndex);
}


/// 


// 加载配置
void RPLidarWrapper::loadConfig()
{
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	for (int _index = 0; _index < _config->Areas.size(); ++_index) {

		allTouchPoints.push_back(std::vector<Point2f>());

		auto _rawPoints = _config->Areas[_index];
		std::vector<Point2f> _points;
		_points.push_back(Point2f(_rawPoints[0].x, _rawPoints[0].y));
		_points.push_back(Point2f(_rawPoints[1].x, _rawPoints[1].y));
		_points.push_back(Point2f(_rawPoints[2].x, _rawPoints[2].y));
		_points.push_back(Point2f(_rawPoints[3].x, _rawPoints[3].y));
		this->Ps.push_back(std::vector<Point2f>(_points.begin(),_points.end()));
		this->SPs.push_back(std::vector<Point2f>(_points.begin(), _points.end()));
	}
	debugRadius = _config->DebugRadius;
	angleOffset = _config->AngleOffset;
}

// 保存配置
void RPLidarWrapper::saveConfig()
{
	auto _config = RPConfigMgr::Instance().GetConfig(deviceID);
	_config->DebugRadius = debugRadius;
	_config->AngleOffset = angleOffset;

	for (int _index = 0; _index < this->Ps.size(); ++_index) {
		auto _points = this->Ps[_index];
		_config->Areas[_index][0] = RPPoint(_points[0].x, _points[0].y);
		_config->Areas[_index][1] = RPPoint(_points[1].x, _points[1].y);
		_config->Areas[_index][2] = RPPoint(_points[2].x, _points[2].y);
		_config->Areas[_index][3] = RPPoint(_points[3].x, _points[3].y);
	}
	RPConfigMgr::Instance().Save();
}

RPLidarWrapper::~RPLidarWrapper()
{
	this->screen.release();
	this->preview.release();
}
