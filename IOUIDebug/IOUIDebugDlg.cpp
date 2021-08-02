
// IOUIDebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IOUIDebug.h"
#include "IOUIDebugDlg.h"
#include "afxdialogex.h"
#include <string>
#include "IODeviceController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"IODevice.lib")

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIOUIDebugDlg 对话框



CIOUIDebugDlg::CIOUIDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IOUIDEBUG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIOUIDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CIOUIDebugDlg::OnSampleAxis(float InValue)
{
	/*OutputDebugStringA(std::to_string(InValue).data());
	OutputDebugStringA("\r\n");*/
}

void CIOUIDebugDlg::OnKeyDown(const dh::FKey InKey)
{
	OutputDebugStringA(InKey.GetName());
	OutputDebugStringA("\n");
	if (InKey.GetName() == std::string("A")) {
		dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO("DoAngle", 4000);
	}
	else if (InKey.GetName() == std::string("B")) {
		dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO(dh::FKey("OAxis_66"), 1);
	}
	else if (InKey.GetName() == std::string("C")) {
		dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO(dh::FKey("OAxis_66"), 0);
		//dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO(dh::FKey("OAxis_03"), 0);
	}
	
}

BEGIN_MESSAGE_MAP(CIOUIDebugDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CIOUIDebugDlg 消息处理程序

bool bExit = false;
BOOL CIOUIDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	dh::IODeviceController::Instance().Load();

    //dh::IOSettings::Instance().SetIOConfigPath("./Config/IODevice.xml");
    dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").
        BindAction("KeyDown", dh::IE_Pressed, this, &CIOUIDebugDlg::OnKeyDown);

	dh::IODeviceController::Instance().GetIODevice("ExternalDev_0")
		.BindAxis("MoveLR", this, &CIOUIDebugDlg::OnSampleAxis);

	/*dh::IODeviceController::Instance().GetIODevice("ExternalDev_0")
		.BindAction("KeyDown", dh::IE_Pressed, this, &CIOUIDebugDlg::OnKeyDown);*/

    //struct __declspec(uuid("B372C9F6-1959-4650-960D-73F20CD479BB")) Interface{};
    //auto uid = __uuidof(Interface);
	
    SetTimer(0, 0.02, nullptr);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIOUIDebugDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIOUIDebugDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIOUIDebugDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIOUIDebugDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    dh::IODeviceController::Instance().Update();
	
    CDialogEx::OnTimer(nIDEvent);
}


void CIOUIDebugDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}
