/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include "IOUI.h"
#include <intsafe.h>
#include <memory>
#include <map>
#include "Paths.hpp"
#include "Serial.hpp"
#include "Util.hpp"
#include "TUIO/TuioListener.h"
#include "TUIO/OscReceiver.h"
#include "TUIO//UdpReceiver.h"
#include "TUIO/TcpReceiver.h"
#include <thread>
#include <sstream>
#include <mutex>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace dh = DevelopHelper;

#pragma comment(lib,"libTUIO.lib")


using namespace TUIO;

class TuioDump :public TuioListener {
	std::mutex m_mutex;
private:
	//std::map<long, TuioObject> objects;
	std::map<long, TuioObject*> objects;
	std::map<long, TuioCursor*> cursors;
	std::map<long, TuioBlob*> blobs;

	std::vector<long> lastObjectIDs;
	std::vector<long> lastCursorIDs;
	std::vector<long> lastBlobIDs;
public:
	const json BuildAll() {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		json _tuioData = {
			{"objects",nullptr},
			{"cursors",nullptr},
			{"blobs",nullptr},
		};
		_tuioData["objects"] = buildObjects();
		_tuioData["cursors"] = buildCursors();
		_tuioData["blobs"] = buildBlobs();
		return _tuioData;
	}

	const json buildObjects() {
		json _objects = {
			{"add",json::array()},
			{"update",json::array()},
			{"remove",json::array()}
		};
		std::vector<long> _objectIDs;
		for (const auto& [k,v] : objects) {
			_objectIDs.push_back(k);
		}
		std::vector<long> _created;
		std::vector<long> _updated;
		std::vector<long> _deleted;
		getTagArray(_objectIDs, lastObjectIDs, _created, _updated, _deleted);
		lastObjectIDs = std::vector<long>(_objectIDs);

		for (const long& _id: _created)
		{
			auto& _obj = objects[_id];
			_objects["add"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"symbolID",_obj->getSymbolID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()},
				{"angle",_obj->getAngle()}
				});
		}
		for (const long& _id : _updated) {
			auto& _obj = objects[_id];
			_objects["update"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"symbolID",_obj->getSymbolID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()},
				{"angle",_obj->getAngle()}
				});
		}
		for (const long& _id : _deleted) {
			_objects["remove"].push_back(_id);
		}
		std::string _json = _objects.dump();
		return _objects;
	}

	const json buildCursors() {

		json _cursors = {
			{"add",json::array()},
			{"update",json::array()},
			{"remove",json::array()}
		};

		std::vector<long> _cursorIDs;
		for (auto& [k, v] : cursors) {
			_cursorIDs.push_back(k);
		}
		
		std::vector<long> _created;
		std::vector<long> _updated;
		std::vector<long> _deleted;
		getTagArray(_cursorIDs, lastCursorIDs, _created, _updated, _deleted);

		lastCursorIDs = std::vector<long>(_cursorIDs);
		for (const long& _id : _created)
		{
			auto& _obj = cursors[_id];
			_cursors["add"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()}
			});
		}
		
		for (const long& _id : _updated) {
			auto& _obj = cursors[_id];
			_cursors["update"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()}
			});
		}
		
		for (const long& _id : _deleted) {
			_cursors["remove"].push_back(_id);
		}
		return _cursors;
	}

	const json buildBlobs() {
		json _blobs = {
			{"add",json::array()},
			{"update",json::array()},
			{"remove",json::array()}
		};

		std::vector<long> _blobIDs;
		for (auto& [k, v] : blobs) {
			_blobIDs.push_back(k);
		}
		std::vector<long> _created;
		std::vector<long> _updated;
		std::vector<long> _deleted;
		getTagArray(_blobIDs, lastBlobIDs, _created, _updated, _deleted);

		lastBlobIDs = std::vector<long>(_blobIDs);
		
		for (const long& _id : _created)
		{
			auto& _obj = blobs[_id];
			_blobs["add"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()},
				{"angle",_obj->getAngle()},
				{"width",_obj->getWidth()},
				{"height",_obj->getHeight()},
				{"area",_obj->getArea()},
				{"motionSpeed",_obj->getMotionSpeed()},
				{"rotationSpeed",_obj->getRotationSpeed()},
				{"motionAccel",_obj->getMotionAccel()},
				{"rotationAccel",_obj->getRotationAccel()}
			});
		}
		
		for (const long& _id : _updated) {
			auto& _obj = blobs[_id];
			_blobs["update"].push_back({
				{"sessionID",_obj->getSessionID()},
				{"x",_obj->getX()},
				{"y",_obj->getY()},
				{"angle",_obj->getAngle()},
				{"width",_obj->getWidth()},
				{"height",_obj->getHeight()},
				{"area",_obj->getArea()},
				{"motionSpeed",_obj->getMotionSpeed()},
				{"rotationSpeed",_obj->getRotationSpeed()},
				{"motionAccel",_obj->getMotionAccel()},
				{"rotationAccel",_obj->getRotationAccel()}
			});
		}
		
		for (const long& _id : _deleted) {
			_blobs["remove"].push_back(_id);
		}
		return _blobs;
	}

	void getTagArray(const std::vector<long>& current, const std::vector<long>& last, std::vector<long>& created, std::vector<long>& updated,  std::vector<long>& deleted) {
		for (const long& _id : current)
		{
			if (std::find(last.begin(), last.end(), _id) != last.end()) {
				updated.push_back(_id);
				// 存在该ID
			}
			else {
				// 不存在该ID
				created.push_back(_id);
			}
		}
		for (const long& _id : last) {
			if (std::find(current.begin(), current.end(), _id) == current.end()) {
				// 该ID已被删除
				deleted.push_back(_id);
			}
		}
	}

public:
	void addTuioObject(TuioObject* tobj) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		objects.insert(std::pair<long, TuioObject*>(tobj->getSessionID(), tobj));
	}
	void updateTuioObject(TuioObject* tobj) {
		
		std::lock_guard<std::mutex> _mutex(m_mutex);
		objects[tobj->getSessionID()] = tobj;
	}
	void removeTuioObject(TuioObject* tobj) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		objects.erase(tobj->getSessionID());
	}

	void addTuioCursor(TuioCursor* tcur) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		cursors.insert(std::pair<long, TuioCursor*>(tcur->getSessionID(), tcur));
	}
	void updateTuioCursor(TuioCursor* tcur) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		cursors[tcur->getSessionID()] = tcur;
	}
	void removeTuioCursor(TuioCursor* tcur) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		cursors.erase(tcur->getSessionID());
	}

	void addTuioBlob(TuioBlob* tblb) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		blobs.insert(std::pair<long,TuioBlob*>(tblb->getSessionID(), tblb));
	}
	void updateTuioBlob(TuioBlob* tblb) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		blobs[tblb->getSessionID()] = tblb;
	}
	void removeTuioBlob(TuioBlob* tblb) {
		std::lock_guard<std::mutex> _mutex(m_mutex);
		blobs.erase(tblb->getSessionID());
	}

	void refresh(TuioTime frameTime) {}
};


DeviceInfo devInfo;

IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 0;
	devInfo.OutputCount = 0;
	devInfo.AxisCount = 0;
    return &devInfo;
}

std::map<uint8, std::shared_ptr<OscReceiver>> g_oscReceivers;
std::map<uint8, std::shared_ptr<TuioClient>> g_tuioClients;
std::map<uint8, std::shared_ptr<TuioDump>> g_tuioDumps;

std::map<uint8, std::string> g_drivers;
IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{
	std::string path = dh::Paths::Instance().GetModuleDir();
	std::string config_file_path = path + "Config\\TUIO\\config.ini";

	// DLL 延迟加载
	std::string _path = DevelopHelper::Paths::Instance().GetModuleDir() + "Core\\libTUIO.dll";
	auto _module = LoadLibraryA(_path.data());

	const char* app = "/Settings";
	char _driver[MAX_PATH];
	GetPrivateProfileStringA(app, BuildDeviceAttribute("DRIVER", deviceIndex).data(), "UDP", _driver, MAX_PATH,config_file_path.data());
	char _tcpAddress[MAX_PATH];
	GetPrivateProfileStringA(app, BuildDeviceAttribute("TCP_ADDRESS", deviceIndex).data(), "127.0.0.1", _tcpAddress, MAX_PATH, config_file_path.data());

	DWORD _udpPort = GetPrivateProfileIntA(app, BuildDeviceAttribute("UDP_PORT",deviceIndex).data(), 3333, config_file_path.data());
	DWORD _tcpPort = GetPrivateProfileIntA(app, BuildDeviceAttribute("TCP_PORT", deviceIndex).data(), 3333, config_file_path.data());

	OscReceiver* _oscReceiver = nullptr;
	if (std::string(_driver) == "TCP") {
		_oscReceiver = new TcpReceiver(_tcpAddress, _tcpPort);
	}
	else {
		_oscReceiver = new UdpReceiver(_udpPort);
	}
	TuioClient* _client = new TuioClient(_oscReceiver);

	g_oscReceivers.insert(std::pair<uint8, std::shared_ptr<OscReceiver>>(deviceIndex, _oscReceiver));
	g_tuioClients.insert(std::pair<uint8, std::shared_ptr<TuioClient >> (deviceIndex, _client));
	TuioDump* _dump = new TuioDump();
	g_tuioDumps.insert(std::pair<uint8, std::shared_ptr<TuioDump>>(deviceIndex, _dump));
	_client->addTuioListener(_dump);
	_client->connect(false);
	
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	g_tuioClients[deviceIndex]->disconnect();
	g_tuioClients[deviceIndex]->removeAllTuioListeners();
	
	g_tuioDumps.erase(deviceIndex);
	g_oscReceivers.erase(deviceIndex);
	g_tuioClients.erase(deviceIndex);
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{	
	return 0;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    return 0;
}


IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    return 0;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    return 0;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size) {
	if (g_tuioDumps.count(deviceIndex) <= 0) return 0;
	const json _data = g_tuioDumps[deviceIndex]->BuildAll();
	std::vector<BYTE> _bson = json::to_bson(_data);
	if (_bson.size() > Size) return 0;
	memcpy(Data, _bson.data(), _bson.size());
	OutputDebugStringA(std::to_string(_bson.size()).data());
	OutputDebugStringA("\r\n");
	return _bson.size();
}
