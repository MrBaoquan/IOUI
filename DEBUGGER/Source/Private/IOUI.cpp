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

 // #define GLFW_EXPOSE_NATIVE_WIN32
 // #include "GLFW/glfw3native.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iomanip>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

const char* glsl_version = "#version 130";

struct DeviceContext
{
    std::vector<BYTE> inputStatus;
    std::vector<short> outputStatus;
    std::vector<int> axisStatus;

    std::shared_ptr<mINI::INIFile> iniFile;
    std::shared_ptr<mINI::INIStructure> iniStructure;

    std::string imguiConfigPath;

    GLFWwindow* window;
    ImGuiContext* imguiContext;

    std::mutex mutex;

    std::thread workerThread;

    DeviceContext()
        : inputStatus(255, 0), outputStatus(255, 0), axisStatus(255, 0), window(nullptr), imguiContext(nullptr)
    {
    }
};

std::map<uint8, std::shared_ptr<DeviceContext>> g_deviceContexts;
std::mutex g_deviceContextsMutex;

std::mutex initMutex;
std::mutex renderMutex;

std::condition_variable cv;

std::atomic<bool> g_createWindow(false);

int g_deviceID = 0;
std::mutex g_deviceIDMutex;

std::atomic<bool> g_initGlfw(false);
std::mutex g_initMutex;

std::vector<uint8> g_deviceIDs;

std::shared_ptr<DeviceContext> GetDeviceContext(uint8 deviceIndex)
{
    std::lock_guard<std::mutex> lock(g_deviceContextsMutex);
    auto it = g_deviceContexts.find(deviceIndex);
    if (it != g_deviceContexts.end())
        return it->second;
    return nullptr;
}

bool createWindow(uint8 deviceIndex)
{
    std::unique_lock<std::mutex> lk(renderMutex);
    cv.wait(lk, []
        { return !g_createWindow.load(); });
    g_createWindow.store(true);

    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
    {
        g_createWindow.store(false);
        cv.notify_all();
        return false;
    }

    auto& ini = *ctx->iniStructure;
    auto& g_iniFile = ctx->iniFile;
    int _posX = ini["Window"].has("PosX") ? std::stoi(ini["Window"]["PosX"]) : 200;
    int _posY = ini["Window"].has("PosY") ? std::stoi(ini["Window"]["PosY"]) : 200;
    int _width = ini["Window"].has("Width") ? std::stoi(ini["Window"]["Width"]) : 980;
    int _height = ini["Window"].has("Height") ? std::stoi(ini["Window"]["Height"]) : 550;
    int _minimized = ini["Window"].has("Minimized") ? std::stoi(ini["Window"]["Minimized"]) : 0;

    if (_width <= 0 || _height <= 0 || _posX <= -10000 || _posY <= -10000)
    {
        _width = 640;
        _height = 480;
        _posX = 200;
        _posY = 200;
    }

    std::string _windowTitle = std::string("IOToolkit Simulator - ") + std::to_string(deviceIndex);
    auto window = glfwCreateWindow(_width, _height, _windowTitle.c_str(), NULL, NULL);

    if (window == nullptr)
    {
        g_createWindow.store(false);
        cv.notify_all();
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        ctx->window = window;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowPos(window, _posX, _posY);
    glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

    SetWindowPos(GetActiveWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    if (_minimized)
        ShowWindow(GetActiveWindow(), SW_MINIMIZE);

    IMGUI_CHECKVERSION();
    auto* imgui_context = ImGui::CreateContext();
    {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        ctx->imguiContext = imgui_context;
    }
    ImGui::SetCurrentContext(imgui_context);

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = ctx->imguiConfigPath.c_str();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    g_deviceIDs.push_back(deviceIndex);

    g_createWindow.store(false);
    cv.notify_all();
    return true;
}

int init(uint8 deviceIndex)
{
    uint8 _deviceIndex = deviceIndex;

    {
        std::lock_guard<std::mutex> lk(g_initMutex);
        if (!g_initGlfw.load())
        {
            glfwSetErrorCallback(glfw_error_callback);

            if (!glfwInit())
            {
                return 0;
            }

#if defined(IMGUI_IMPL_OPENGL_ES2)
            const char* glsl_version = "#version 100";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
            const char* glsl_version = "#version 150";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

            g_initGlfw.store(true);
        }
    }

    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
        return 0;

    auto& ini = *ctx->iniStructure;
    auto& g_iniFile = ctx->iniFile;

    int _posX = ini["Window"].has("PosX") ? std::stoi(ini["Window"]["PosX"]) : 200;
    int _posY = ini["Window"].has("PosY") ? std::stoi(ini["Window"]["PosY"]) : 200;
    int _minimized = ini["Window"].has("Minimized") ? std::stoi(ini["Window"]["Minimized"]) : 0;

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

    if (!createWindow(deviceIndex))
    {
        return 0;
    }

    GLFWwindow* window;
    ImGuiContext* imgui_context;

    {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        window = ctx->window;
        imgui_context = ctx->imguiContext;
    }

    auto& io = ImGui::GetIO();

    auto _condition = [deviceIndex]
        {
            std::lock_guard<std::mutex> lock(g_deviceIDMutex);
            if (g_createWindow.load())
                return false;
            if (g_deviceIDs.size() > 1)
            {
                bool _result = g_deviceIDs[g_deviceID] == deviceIndex;
                return _result;
            }
            return true;
        };

        while (!glfwWindowShouldClose(window))
        {
            {
                std::unique_lock<std::mutex> lk(renderMutex);
                cv.wait(lk, _condition);
                if (glfwWindowShouldClose(window))
                {
                    break;
                }

                ImGui::SetCurrentContext(imgui_context);
                glfwPollEvents();

                int _newPosX, _newPosY;
                glfwGetWindowPos(window, &_newPosX, &_newPosY);

                int _newMinimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
                if (_newMinimized != _minimized)
                {
                    _minimized = _newMinimized;
                    ini["Window"]["Minimized"] = std::to_string(_minimized);
                    g_iniFile->write(ini);
                }
                if (!_minimized)
                {
                    if (_newPosX != _posX || _newPosY != _posY)
                    {
                        _posX = _newPosX;
                        _posY = _newPosY;
                        ini["Window"]["PosX"] = std::to_string(_posX);
                        ini["Window"]["PosY"] = std::to_string(_posY);
                        g_iniFile->write(ini);
                    }

                    int _newWidth, _newHeight;
                    glfwGetWindowSize(window, &_newWidth, &_newHeight);
                    if (_newWidth != io.DisplaySize.x || _newHeight != io.DisplaySize.y)
                    {
                        io.DisplaySize.x = _newWidth;
                        io.DisplaySize.y = _newHeight;
                        ini["Window"]["Width"] = std::to_string(io.DisplaySize.x);
                        ini["Window"]["Height"] = std::to_string(io.DisplaySize.y);
                        g_iniFile->write(ini);
                    }
                }

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(io.DisplaySize);
                ImGui::SetNextWindowSizeConstraints(ImVec2(640, 480), ImVec2(-1, -1));

#pragma region IOUI Core
                ImGui::Begin("IOToolkit Simulator", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

                auto& style = ImGui::GetStyle();
                style.WindowPadding = ImVec2(20.f, 20.f);
                static float defaultFramePaddingY = style.FramePadding.y;

                const char* items[] = { "8", "16", "32", "64", "128", "255" };

                static std::map<uint8, int> _diLengths;
                if (_diLengths.count(deviceIndex) == 0)
                {
                    auto _currentIndex = ini["Window"].has("DIIndex") ? std::stoi(ini["Window"]["DIIndex"]) : 1;
                    _diLengths.insert(std::make_pair(deviceIndex, _currentIndex));
                }

                ImGui::PushItemWidth(80);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Device Settings:		");
                ImGui::SameLine();
                ImGui::Text("Digital Input Count:");
                ImGui::SameLine();

                if (ImGui::Combo("## Digital Input Length", &_diLengths[deviceIndex], items, IM_ARRAYSIZE(items)))
                {
                    auto _curIndex = _diLengths[deviceIndex];
                    ini["Window"]["DIIndex"] = std::to_string(_curIndex);
                    g_iniFile->write(ini);
                }
                auto _diLength = std::stoi(items[_diLengths[deviceIndex]]);

                static std::map<uint8, int> _aiLengths;
                if (_aiLengths.count(deviceIndex) == 0)
                {
                    auto _currentIndex = ini["Window"].has("AIIndex") ? std::stoi(ini["Window"]["AIIndex"]) : 0;
                    _aiLengths.insert(std::make_pair(deviceIndex, _currentIndex));
                }
                ImGui::SameLine();
                ImGui::Text(" Analog Input Count:");
                ImGui::SameLine();
                if (ImGui::Combo("## Analog Input Length", &_aiLengths[deviceIndex], items, IM_ARRAYSIZE(items)))
                {
                    auto _curIndex = _aiLengths[deviceIndex];
                    ini["Window"]["AIIndex"] = std::to_string(_curIndex);
                    g_iniFile->write(ini);
                }
                auto _aiLength = std::stoi(items[_aiLengths[deviceIndex]]);

                static std::map<uint8, int> _aoLengths;
                if (_aoLengths.count(deviceIndex) == 0)
                {
                    auto _currentIndex = ini["Window"].has("AOIndex") ? std::stoi(ini["Window"]["AOIndex"]) : 1;
                    _aoLengths.insert(std::make_pair(deviceIndex, _currentIndex));
                }
                ImGui::SameLine();
                ImGui::Text(" Analog Output Count:");
                ImGui::SameLine();
                if (ImGui::Combo("## Analog Output Length", &_aoLengths[deviceIndex], items, IM_ARRAYSIZE(items)))
                {
                    auto _curIndex = _aoLengths[deviceIndex];
                    ini["Window"]["AOIndex"] = std::to_string(_curIndex);
                    g_iniFile->write(ini);
                }
                auto _aoLength = std::stoi(items[_aoLengths[deviceIndex]]);

                ImGui::PopItemWidth();
                ImGui::Separator();

                ImGui::NewLine();
                float _currentRowX = 0;
                float _currentRowWidth = 0;
                float _btnWidth = 110;
                float itemSpacing = style.ItemSpacing.x;

                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.05, 0.05, 0.05, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

                auto& inputStatus = ctx->inputStatus;
                auto& axisStatus = ctx->axisStatus;
                auto& outputStatus = ctx->outputStatus;

                if (ImGui::CollapsingHeader("Digital Inputs", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    std::lock_guard<std::mutex> lock(ctx->mutex);
                    for (int _idx = 0; _idx < _diLength; ++_idx)
                    {
                        if (_currentRowWidth + _btnWidth + itemSpacing > ImGui::GetWindowWidth())
                        {
                            _currentRowX = 0;
                            _currentRowWidth = 0;
                            ImGui::NewLine();
                        }
                        _currentRowX += _btnWidth + itemSpacing;
                        _currentRowWidth += _btnWidth + itemSpacing;

                        std::string _btnStr = std::to_string(_idx);
                        std::string _label = std::string("Button_") + std::string((_diLength > 99 ? 3 : 2) - _btnStr.length(), '0') + _btnStr;

                        auto _color = inputStatus[_idx] ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
                        ImGui::PushStyleColor(ImGuiCol_Button, _color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, _color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _color);
                        if (ImGui::Button(_label.c_str(), ImVec2(110, 25)))
                        {
                            inputStatus[_idx] = !inputStatus[_idx];
                        }
                        ImGui::PopStyleColor(3);
                        ImGui::SameLine();
                    }
                }
                ImGui::Spacing();

                ImGui::NewLine();
                if (ImGui::CollapsingHeader("Analog Inputs", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    std::lock_guard<std::mutex> lock(ctx->mutex);
                    for (int _idx = 0; _idx < _aiLength; ++_idx)
                    {
                        std::string _btnStr = std::to_string(_idx);
                        std::string _label = std::string("Axis_") + std::string((_aiLength > 99 ? 3 : 2) - _btnStr.length(), '0') + _btnStr;
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text(_label.c_str());
                        ImGui::SameLine();

                        ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 120);
                        ImGui::SliderInt((std::string("##") + _label).c_str(), &axisStatus[_idx], -1000, 1000, "%d");
                        ImGui::PopItemWidth();

                        ImGui::SameLine();

                        float btn_width = 80.0f;
                        if (ImGui::Button((std::string("Reset##") + _label).c_str(), ImVec2(btn_width, 0)))
                        {
                            axisStatus[_idx] = 0;
                        }
                    }
                }

                ImGui::NewLine();
                if (ImGui::CollapsingHeader("Analog Outputs", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    _btnWidth = 234;
                    _currentRowWidth = 0;
                    _currentRowX = 0;
                    std::lock_guard<std::mutex> lock(ctx->mutex);
                    for (int _idx = 0; _idx < _aoLength; ++_idx)
                    {
                        if (_currentRowWidth + _btnWidth + itemSpacing > ImGui::GetWindowWidth())
                        {
                            _currentRowX = 0;
                            _currentRowWidth = 0;
                            ImGui::NewLine();
                        }
                        _currentRowX += _btnWidth + itemSpacing;
                        _currentRowWidth += _btnWidth + itemSpacing;

                        auto _btnColor = outputStatus[_idx] != 0 ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
                        ImGui::PushStyleColor(ImGuiCol_Button, _btnColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _btnColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, _btnColor);

                        std::string _btnStr = std::to_string(_idx);
                        std::string _label = std::string("OAxis_") + std::string((_aoLength > 99 ? 3 : 2) - _btnStr.length(), '0') + _btnStr;
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text(_label.c_str());
                        ImGui::SameLine();

                        auto _output = outputStatus[_idx];
                        bool _isNegative = _output < 0;
                        if (_isNegative)
                            _output = -_output;
                        std::string _outputString = std::to_string(std::abs(_output));
                        _outputString = std::string(5 - _outputString.length(), '0') + _outputString;
                        if (_isNegative)
                            _outputString = "-" + _outputString;
                        ImGui::Button(_outputString.c_str(), ImVec2(164, 25));
                        ImGui::SameLine();
                        ImGui::PopStyleColor(3);
                    }
                }

                ImGui::PopStyleColor(3);
                ImGui::Spacing();
                ImGui::End();

#pragma endregion

                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);

                std::this_thread::sleep_for(std::chrono::milliseconds(4));
                if (g_deviceIDs.size() > 1)
                {
                    if ((g_deviceID + 1) >= g_deviceIDs.size())
                    {
                        g_deviceID = 0;
                    }
                    else
                    {
                        g_deviceID++;
                    }
                    cv.notify_all();
                }
                else
                {
                    cv.notify_all();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        std::unique_lock<std::mutex> lock(renderMutex);

        {
            std::lock_guard<std::mutex> lock(g_deviceIDMutex);

            auto _newEnd = std::remove(g_deviceIDs.begin(), g_deviceIDs.end(), deviceIndex);
            g_deviceIDs.erase(_newEnd, g_deviceIDs.end());
        }

        {
            std::lock_guard<std::mutex> lock(ctx->mutex);
            ImGui::SetCurrentContext(ctx->imguiContext);
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext(ctx->imguiContext);

            glfwDestroyWindow(ctx->window);
            ctx->window = nullptr;
            ctx->imguiContext = nullptr;
        }

        if (g_deviceIDs.size() <= 0)
        {
            glfwTerminate();
        }
        else
        {
            g_deviceID = 0;
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
    std::lock_guard<std::mutex> lock(g_deviceContextsMutex);

    if (g_deviceContexts.count(deviceIndex))
    {
        return 0;
    }

    std::string path = DevelopHelper::Paths::Instance().GetModuleDir();
    auto ctx = std::make_shared<DeviceContext>();
    ctx->imguiConfigPath = path + std::string("Config\\IOUI\\imgui_") + std::to_string(deviceIndex) + std::string(".ini");

    std::string config_file_path = path + "Config\\Debugger\\window_" + std::to_string(deviceIndex) + ".ini";
    ctx->iniFile = std::make_shared<mINI::INIFile>(config_file_path);
    ctx->iniStructure = std::make_shared<mINI::INIStructure>();
    ctx->iniFile->read(*ctx->iniStructure);

    g_deviceContexts[deviceIndex] = ctx;

    ctx->workerThread = std::thread([deviceIndex]()
        { init(deviceIndex); });

    return 1;
}

IOUI_API int __stdcall CloseDevice(uint8 deviceIndex)
{
    std::shared_ptr<DeviceContext> ctx;
    {
        std::lock_guard<std::mutex> lock(g_deviceContextsMutex);
        auto it = g_deviceContexts.find(deviceIndex);
        if (it == g_deviceContexts.end())
            return 1;
        ctx = it->second;
    }
    GLFWwindow* window = nullptr;
    {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        window = ctx->window;
        if (window && !glfwWindowShouldClose(window))
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    if (ctx->workerThread.joinable())
        ctx->workerThread.join();

    {
        std::lock_guard<std::mutex> lock(ctx->mutex);
        ctx->window = nullptr;
        ctx->imguiContext = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_deviceContextsMutex);
        g_deviceContexts.erase(deviceIndex);
    }

    return 1;
}

IOUI_API int __stdcall SetDeviceDO(uint8 deviceIndex, short* InDOStatus)
{
    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
        return 0;

    std::lock_guard<std::mutex> lock(ctx->mutex);
    for (int i = 0; i < 255; ++i)
    {
        ctx->outputStatus[i] = InDOStatus[i];
    }

    return 1;
}
IOUI_API int __stdcall GetDeviceDO(uint8 deviceIndex, short* OutDOStatus)
{
    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
        return 0;

    std::lock_guard<std::mutex> lock(ctx->mutex);
    for (int i = 0; i < 255; ++i)
    {
        OutDOStatus[i] = ctx->outputStatus[i];
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceDI(uint8 deviceIndex, BYTE* OutDIStatus)
{
    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
        return 0;

    std::lock_guard<std::mutex> lock(ctx->mutex);
    for (int i = 0; i < 255; ++i)
    {
        OutDIStatus[i] = ctx->inputStatus[i];
    }

    return 1;
}

IOUI_API int __stdcall GetDeviceAD(uint8 deviceIndex, short* OutADStatus)
{
    auto ctx = GetDeviceContext(deviceIndex);
    if (!ctx)
        return 0;

    std::lock_guard<std::mutex> lock(ctx->mutex);
    for (int i = 0; i < 8; ++i)
    {
        OutADStatus[i] = ctx->axisStatus[i];
    }

    return 1;
}

IOUI_API int __stdcall RefreshStreamingData(uint8 deviceIndex, BYTE* Data, unsigned int Size)
{
    return 1;
}
