/** Copyright (c) 2018 Hefei And Technology Co.,Ltd
 *  Author: MrBaoquan
 *  CreateTime: 2018-5-16 10:44
 *  Email: mrma617@gmail.com
 */
#include <algorithm>
#include "IOUI.h"
#include <thread>
#include <mutex>
#include "Paths.hpp"
#include "mIni/mini/ini.h"
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//#define GLFW_EXPOSE_NATIVE_WIN32
//#include "GLFW/glfw3native.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iomanip>


#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

 // [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
 // To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
 // Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


const char* glsl_version = "#version 130";

std::map<uint8,std::vector<BYTE>> g_inputStatuses;
//std::vector<BYTE> g_inputStatus;
std::map<uint8,std::vector<short>> g_outputStatuses;
//std::vector<short> g_outputStatus;
std::map<uint8,std::vector<int>> g_axisStatuses;
//std::vector<int> g_axisStatus;
std::mutex g_mutex;

std::map<uint8,std::shared_ptr<mINI::INIFile>> g_iniFiles;
//std::shared_ptr<mINI::INIFile> g_iniFile;
std::map<uint8, std::shared_ptr<mINI::INIStructure>> g_iniStructures;
//std::shared_ptr<mINI::INIStructure> g_iniStructure;

std::map<uint8,std::atomic<bool>> g_threadFinishers;
//std::atomic<bool> threadFinished(false);

std::map<uint8,std::string> g_imguiPaths;
//std::string g_imguiConfigPath = "imgui.ini";

std::map<uint8, GLFWwindow*> g_windows;
std::map<uint8, ImGuiContext*> g_imguiContexts;

std::vector<uint8> g_deviceIDs;

std::mutex initMutex;
std::mutex renderMutex;

std::condition_variable cv;

std::atomic<bool> g_createWindow(false);
bool createWindow(uint8 deviceIndex){
	std::unique_lock<std::mutex> lk(renderMutex);
	cv.wait(lk, [] {return !g_createWindow.load(); });
	g_createWindow.store(true);

	auto& ini = *g_iniStructures[deviceIndex];
	auto& g_iniFile = g_iniFiles[deviceIndex];
	int _posX = ini["Window"].has("PosX") ? std::stoi(ini["Window"]["PosX"]) : 200;
	int _posY = ini["Window"].has("PosY") ? std::stoi(ini["Window"]["PosY"]) : 200;
	int _width = ini["Window"].has("Width") ? std::stoi(ini["Window"]["Width"]) : 980;
	int _height = ini["Window"].has("Height") ? std::stoi(ini["Window"]["Height"]) : 550;
	int _minimized = ini["Window"].has("Minimized") ? std::stoi(ini["Window"]["Minimized"]) : 0;

	if (_width <= 0 || _height<=0 || _posX<=-10000||_posY<=-10000) {
		_width = 640;
		_height = 480;
		_posX = 200;
		_posY = 200;
	}

	OutputDebugStringA(std::string("creating window for device: " + std::to_string(deviceIndex) + "\n").c_str());
	std::string _windowTitle = std::string("IOToolkit Simulator - ") + std::to_string(deviceIndex);
	auto window = glfwCreateWindow(_width, _height, _windowTitle.c_str(), NULL, NULL);

	if (window == nullptr) {
		return false;
	}

	if(g_windows.count(deviceIndex) == 0)
		g_windows.insert(std::make_pair(deviceIndex, window));

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	// change window position
	glfwSetWindowPos(window, _posX, _posY);
	glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// keep top most window
	SetWindowPos(GetActiveWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if(_minimized)
		ShowWindow(GetActiveWindow(), SW_MINIMIZE);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto* imgui_context = ImGui::CreateContext();
	if(g_imguiContexts.count(deviceIndex)==0)
		g_imguiContexts.insert(std::make_pair(deviceIndex, imgui_context));
	ImGui::SetCurrentContext(imgui_context);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	std::string _configPath = io.IniFilename;
	io.IniFilename = g_imguiPaths[deviceIndex].c_str();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	//OutputDebugStringA(std::string("create imgui context for device: " + std::to_string(deviceIndex) + "\n").c_str());
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	//OutputDebugStringA(std::string("create window for device: " + std::to_string(deviceIndex) + " done\n").c_str());
	if(g_imguiContexts.count(deviceIndex)==0)
		g_imguiContexts.insert(std::make_pair(deviceIndex, imgui_context));

	g_deviceIDs.push_back(deviceIndex);
	OutputDebugStringA(std::string("create window for device: " + std::to_string(deviceIndex) + " done\n").c_str());
	g_createWindow.store(false);
	cv.notify_all();
	return true;
}

int g_deviceID = 0;

std::mutex g_deviceIDMutex;

std::atomic<bool> g_initGlfw(false);
std::mutex g_initMutex;
int init(uint8 deviceIndex)
{
	uint8 _deviceIndex = deviceIndex;
	{
		// 初始化glfw
		std::lock_guard<std::mutex> lk(g_initMutex);
		if(!g_initGlfw.load()){
			glfwSetErrorCallback(glfw_error_callback);
			OutputDebugStringA("prepare to init glfw\n");
			if (!glfwInit()){
				OutputDebugStringA("glfw init failed\n");
				return 0;
			}		
				// Decide GL+GLSL versions
			#if defined(IMGUI_IMPL_OPENGL_ES2)
				// GL ES 2.0 + GLSL 100
				const char* glsl_version = "#version 100";
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
			#elif defined(__APPLE__)
				// GL 3.2 + GLSL 150
				const char* glsl_version = "#version 150";
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
			#else
				// GL 3.0 + GLSL 130
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

				// 窗口自定义大小
				// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
				//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
				//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
			#endif
			OutputDebugStringA("init glfw done\n");
			g_initGlfw.store(true);
		}
	}
	
	auto& ini = *g_iniStructures[deviceIndex];
	auto& g_iniFile = g_iniFiles[deviceIndex];
	int _posX = ini["Window"].has("PosX") ? std::stoi(ini["Window"]["PosX"]) : 200;
	int _posY = ini["Window"].has("PosY") ? std::stoi(ini["Window"]["PosY"]) : 200;
	int _minimized = ini["Window"].has("Minimized") ? std::stoi(ini["Window"]["Minimized"]) : 0;
	
	
	bool show_demo_window = true;
	bool show_another_window = true;

	ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

	if (!createWindow(deviceIndex)) {
		return 0;
	}
	
	auto window = g_windows[deviceIndex];
	auto imgui_context = g_imguiContexts[deviceIndex];
	auto& io = ImGui::GetIO(); 

	auto _condition = [deviceIndex] {
		std::lock_guard<std::mutex> lock(g_deviceIDMutex);
		if(g_createWindow.load())
			return false;

		//OutputDebugStringA(std::string("check condition start: " + std::to_string(deviceIndex) + "\n").c_str());
		if (g_deviceIDs.size() > 1) {
			bool _result = g_deviceIDs[g_deviceID] == deviceIndex;
			//OutputDebugStringA(std::string("check condition for device: " + std::to_string(deviceIndex) + std::to_string(_result) + "\n").c_str());
			return _result;
		}
		//OutputDebugStringA(std::string("check condition for device: " + std::to_string(deviceIndex) + " true \n").c_str());
		return true;
	};

	while (!glfwWindowShouldClose(window))
	{
		{
			std::unique_lock<std::mutex> lk(renderMutex);
			cv.wait(lk, _condition);
			if (glfwWindowShouldClose(window)) {
				break;
			}
			//OutputDebugStringA(std::string("update window for device: " + std::to_string(deviceIndex) + "\n").c_str());

			ImGui::SetCurrentContext(imgui_context);
			glfwPollEvents();

			// check if window moved
			int _newPosX, _newPosY;
			glfwGetWindowPos(window, &_newPosX, &_newPosY);

			// check if window minimized
			int _newMinimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
			if (_newMinimized != _minimized) {
				_minimized = _newMinimized;
				ini["Window"]["Minimized"] = std::to_string(_minimized);
				g_iniFile->write(ini);
			}
			if(!_minimized)
			{
				if (_newPosX != _posX || _newPosY != _posY) {
					_posX = _newPosX;
					_posY = _newPosY;
					ini["Window"]["PosX"] = std::to_string(_posX);
					ini["Window"]["PosY"] = std::to_string(_posY);
					g_iniFile->write(ini);
				}

				// check if window resized
				int _newWidth, _newHeight;
				glfwGetWindowSize(window, &_newWidth, &_newHeight);
				if (_newWidth != io.DisplaySize.x || _newHeight != io.DisplaySize.y) {
					io.DisplaySize.x = _newWidth;
					io.DisplaySize.y = _newHeight;
					ini["Window"]["Width"] = std::to_string(io.DisplaySize.x);
					ini["Window"]["Height"] = std::to_string(io.DisplaySize.y);
					g_iniFile->write(ini);
				}	
			}


			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::SetNextWindowSizeConstraints(ImVec2(640, 480), ImVec2(-1, -1));
			
	#pragma region IOUI Core
			ImGui::Begin("IOToolkit Simulator", &show_another_window, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoSavedSettings );   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			
			auto& style = ImGui::GetStyle();
			style.WindowPadding = ImVec2(20.f, 20.f);
			static float defaultFramePaddingY = style.FramePadding.y;
			
			const char* items[] = { "8", "16", "32", "64", "128", "255"};
			static std::map<uint8,int> _diLengths;
			if(_diLengths.count(deviceIndex)==0){
				auto _currentIndex = ini["Window"].has("DIIndex") ? std::stoi(ini["Window"]["DIIndex"]) : 1;
				_diLengths.insert(std::pair<uint8,int>(deviceIndex, _currentIndex));
			}
			
			ImGui::PushItemWidth(80);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Device Settings:		"); ImGui::SameLine();
			// Digital input channels count
			ImGui::Text("Digital Input Count:"); ImGui::SameLine();
			// style.FramePadding.y = defaultFramePaddingY;
			if(ImGui::Combo("## Digital Input Length", &_diLengths[deviceIndex], items, IM_ARRAYSIZE(items))){
				auto _curIndex = _diLengths[deviceIndex];
				ini["Window"]["DIIndex"] = std::to_string(_curIndex);
				g_iniFile->write(ini);
			}
			auto _diLength =  std::stoi(items[_diLengths[deviceIndex]]);


			static std::map<uint8,int> _aiLengths;
			if(_aiLengths.count(deviceIndex)==0){
				auto _currentIndex = ini["Window"].has("AIIndex") ? std::stoi(ini["Window"]["AIIndex"]) : 0;
				_aiLengths.insert(std::pair<uint8,int>(deviceIndex, _currentIndex));
			}
			ImGui::SameLine();
			ImGui::Text(" Analog Input Count:"); ImGui::SameLine();
			if(ImGui::Combo("## Analog Input Length", &_aiLengths[deviceIndex], items, IM_ARRAYSIZE(items))){
				auto _curIndex = _aiLengths[deviceIndex];
				ini["Window"]["AIIndex"] = std::to_string(_curIndex);
				g_iniFile->write(ini);
			}
			auto _aiLength =  std::stoi(items[_aiLengths[deviceIndex]]);


			static std::map<uint8,int> _aoLengths;
			if(_aoLengths.count(deviceIndex)==0){
				auto _currentIndex = ini["Window"].has("AOIndex") ? std::stoi(ini["Window"]["AOIndex"]) : 1;
				_aoLengths.insert(std::pair<uint8,int>(deviceIndex, _currentIndex));
			}
			ImGui::SameLine();
			ImGui::Text(" Analog Output Count:"); ImGui::SameLine();
			if(ImGui::Combo("## Analog Output Length", &_aoLengths[deviceIndex], items, IM_ARRAYSIZE(items))){
				auto _curIndex = _aoLengths[deviceIndex];
				ini["Window"]["AOIndex"] = std::to_string(_curIndex);
				g_iniFile->write(ini);
			}
			auto _aoLength =  std::stoi(items[_aoLengths[deviceIndex]]);

			//style.FramePadding.y = defaultFramePaddingY;
			ImGui::PopItemWidth();
			ImGui::Separator();		
			

			ImGui::NewLine();
			float _currentRowX= 0;
			float _currentRowWidth = 0;
			float _btnWidth = 110;

			float itemSpacing = style.ItemSpacing.x;

			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.05, 0.05, 0.05, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			if(ImGui::CollapsingHeader("Digital Inputs", ImGuiTreeNodeFlags_DefaultOpen)){
				auto& g_inputStatus = g_inputStatuses[deviceIndex];
				for (int _idx = 0; _idx < _diLength; ++_idx) {
					// check if we need to start a new row
					if (_currentRowWidth + _btnWidth + itemSpacing > ImGui::GetWindowWidth()) {
						_currentRowX = 0;
						_currentRowWidth = 0;
						ImGui::NewLine();
					}

					_currentRowX += _btnWidth + itemSpacing;
					_currentRowWidth += _btnWidth + itemSpacing;

					std::string _btnStr = std::to_string(_idx);
					std::string _label = std::string("Button_")+ std::string((_diLength>99?3:2) - _btnStr.length(), '0') + _btnStr;
					
					auto _color = g_inputStatus[_idx] ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
					ImGui::PushStyleColor(ImGuiCol_Button, _color);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, _color);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _color);
					if (ImGui::Button(_label.c_str(), ImVec2(110, 25))) {
						g_inputStatus[_idx] = !g_inputStatus[_idx];
					}
					ImGui::PopStyleColor(3);
					ImGui::SameLine(); // 将下一个元素放在同一行
				}
			}
			ImGui::Spacing();

			ImGui::NewLine();
			if(ImGui::CollapsingHeader("Analog Inputs", ImGuiTreeNodeFlags_DefaultOpen)){
				auto& g_axisStatus = g_axisStatuses[deviceIndex];
				for (int _idx = 0; _idx < _aiLength; ++_idx) {				
					std::string _btnStr = std::to_string(_idx);
					std::string _label = std::string("Axis_") + std::string((_aiLength>99?3:2) - _btnStr.length(), '0') + _btnStr;
					ImGui::AlignTextToFramePadding();
					ImGui::Text(_label.c_str());ImGui::SameLine();
					// strength slider to window width
					ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 120);
					ImGui::SliderInt((std::string("##")+_label).c_str(), &g_axisStatus[_idx], -1000, 1000,"%d");
					ImGui::PopItemWidth();

					// 在Slider后面同一行添加Reset按钮
					ImGui::SameLine();

					// 设置按钮宽度，调整为适合大小
					float btn_width = 80.0f;
					if (ImGui::Button((std::string("Reset##") + _label).c_str(), ImVec2(btn_width, 0))) {
						g_axisStatus[_idx] = 0;  // 重置数值为0，您可根据需要设置默认值
					}
				}
			}
			
			ImGui::NewLine();
			// 输出相关值
			if(ImGui::CollapsingHeader("Analog Outputs", ImGuiTreeNodeFlags_DefaultOpen)){
				_btnWidth = 234;
				_currentRowWidth = 0;
				_currentRowX = 0;
				auto& g_outputStatus = g_outputStatuses[deviceIndex];
				for (int _idx=0; _idx<_aoLength;++_idx)
				{
					if (_currentRowWidth + _btnWidth + itemSpacing > ImGui::GetWindowWidth()) {
						_currentRowX = 0;
						_currentRowWidth = 0;
						ImGui::NewLine();
					}
					_currentRowX += _btnWidth + itemSpacing;
					_currentRowWidth += _btnWidth + itemSpacing;

					auto _btnColor = g_outputStatus[_idx]!=0? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
					ImGui::PushStyleColor(ImGuiCol_Button, _btnColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered,_btnColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, _btnColor);

					std::string _btnStr = std::to_string(_idx);
					std::string _label = std::string("OAxis_") + std::string((_aoLength>99?3:2) - _btnStr.length(), '0') + _btnStr;
					ImGui::AlignTextToFramePadding();
					ImGui::Text(_label.c_str()); ImGui::SameLine();

					auto _output = g_outputStatus[_idx];
					bool _isNegative = _output < 0;
					if (_isNegative) _output = -_output;
					std::string _outputString = std::to_string(std::abs(_output));
					_outputString = std::string(5 - _outputString.length(), '0') + _outputString;
					if (_isNegative) _outputString = "-" + _outputString;
					ImGui::Button(_outputString.c_str(), ImVec2(164, 25));
					ImGui::SameLine();
					ImGui::PopStyleColor(3);
				}
			}
			ImGui::PopStyleColor(3);
			ImGui::Spacing();
			ImGui::End();
	#pragma endregion
			
			//ImGui::ShowDemoWindow(&show_demo_window);

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);

			std::this_thread::sleep_for(std::chrono::milliseconds(4));
			if (g_deviceIDs.size() > 1) {
				if ((g_deviceID + 1) >= g_deviceIDs.size()) {
					g_deviceID = 0;
				}
				else {
					g_deviceID++;
				}
				//OutputDebugStringA(std::string("switch to device " + std::to_string(g_deviceID) + "\n").c_str());
				cv.notify_all();
			}else {
				cv.notify_all();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	std::unique_lock<std::mutex> lock(renderMutex);
	//cv.wait(lock, _condition);
	OutputDebugStringA(std::string("Thread " + std::to_string(deviceIndex) + " exiting\n").c_str());
	// Cleanup
	{
		std::lock_guard<std::mutex> lock(g_deviceIDMutex);
		OutputDebugStringA(std::string("remove device " + std::to_string(deviceIndex) + "\n").c_str());
		auto _newEnd = std::remove(g_deviceIDs.begin(), g_deviceIDs.end(), deviceIndex);
		g_deviceIDs.erase(_newEnd, g_deviceIDs.end());
		OutputDebugStringA(std::string("device " + std::to_string(deviceIndex) + " removed\n").c_str());
	}
	
	ImGui::SetCurrentContext(imgui_context);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext(imgui_context);
	glfwDestroyWindow(window);
	g_windows.erase(deviceIndex);
	g_imguiContexts.erase(deviceIndex);
	OutputDebugStringA(std::string("Thread " + std::to_string(deviceIndex) + " exited\n").c_str());
	g_threadFinishers[deviceIndex].store(true);

	if (g_deviceIDs.size() <= 0) {
		glfwTerminate();
	}else{
		g_deviceID = 0;
		OutputDebugStringA(std::string("|| switch to device " + std::to_string(g_deviceID) + "\n").c_str());
		cv.notify_all();
	}
	
	return 0;
}


DeviceInfo devInfo;
IOUI_API DeviceInfo* __stdcall Initialize()
{
	devInfo.InputCount = 255;
	devInfo.OutputCount = 255;
	devInfo.AxisCount = 255;
    return &devInfo;
}

IOUI_API int __stdcall OpenDevice(uint8 deviceIndex)
{	 
	//initGlfwIfNeed();

	std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
	auto _g_imguiConfigPath = path + std::string("Config\\IOUI\\imgui_") + std::to_string(deviceIndex) + std::string(".ini");
	if(g_imguiPaths.count(deviceIndex)==0)
	{
		g_imguiPaths.insert(std::make_pair(deviceIndex, _g_imguiConfigPath));
	}
	else {
		return 0;
	}


	if(g_inputStatuses.count(deviceIndex)==0)
	{
		g_inputStatuses.insert(std::make_pair(deviceIndex, std::vector<BYTE>(255, 0)));
	}
	if(g_outputStatuses.count(deviceIndex)==0)
	{
		g_outputStatuses.insert(std::make_pair(deviceIndex, std::vector<short>(255, 0)));
	}
	if(g_axisStatuses.count(deviceIndex)==0)
	{
		g_axisStatuses.insert(std::make_pair(deviceIndex, std::vector<int>(255, 0)));
	}

	std::string config_file_path = path + "Config\\IOUI\\window_"+std::to_string(deviceIndex)+".ini";
	if(g_iniFiles.count(deviceIndex)==0)
	{
		auto g_iniFile = std::make_shared<mINI::INIFile>(config_file_path);
		g_iniFiles.insert(std::make_pair(deviceIndex, g_iniFile));
		auto g_iniStructure = std::make_shared<mINI::INIStructure>();
		g_iniFile->read(*g_iniStructure);
		g_iniStructures.insert(std::make_pair(deviceIndex, g_iniStructure));
	}

	g_threadFinishers.insert(std::make_pair(deviceIndex, false));
	
		std::thread th([deviceIndex](){
			init(deviceIndex);
		});
		th.detach();
	
    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
	// close glfw window
	if (g_windows.count(deviceIndex) == 0) {
		return 1;
	}
	auto window = g_windows[deviceIndex];
	glfwSetWindowShouldClose(window, true);
	auto& threadFinished = g_threadFinishers[deviceIndex];
	while (!threadFinished.load()){
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		OutputDebugStringA(std::string("waiting for thread " + std::to_string(deviceIndex) + "\n").c_str());
	}
	OutputDebugStringA(std::string("closed device " + std::to_string(deviceIndex) + "\n").c_str());
    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		auto& g_outputStatus = g_outputStatuses[deviceIndex];
		for (int i = 0; i < 255; ++i)
		{
			g_outputStatus[i] = InDOStatus[i];
		}
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		auto& g_outputStatus = g_outputStatuses[deviceIndex];
		for (int i = 0; i < 255; ++i)
		{
			OutDOStatus[i] = g_outputStatus[i];
		}
	}

    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		auto& g_inputStatus = g_inputStatuses[deviceIndex];
		for (int i = 0; i < 255; ++i)
		{
			OutDIStatus[i] = g_inputStatus[i];
		}
	}
    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		auto& g_axisStatus = g_axisStatuses[deviceIndex];
		for (int i = 0; i < 8; ++i)
		{
			OutADStatus[i] = g_axisStatus[i];
		}
	}
    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 1;
}