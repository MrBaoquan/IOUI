
// IOUIDebugDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CIOUIDebugDlg �Ի���



CIOUIDebugDlg::CIOUIDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IOUIDEBUG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIOUIDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CIOUIDebugDlg::OnKeyDown(const dh::FKey InKey)
{
    OutputDebugStringA(InKey.GetName());
    OutputDebugStringA("\n");
	if (InKey.GetName() == std::string("A")) {
		dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO("OAxis_00", 1);
	}
	else {
		dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").SetDO("OAxis_00", 0);
	}
	
}

BEGIN_MESSAGE_MAP(CIOUIDebugDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CIOUIDebugDlg ��Ϣ�������

BOOL CIOUIDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	dh::IODeviceController::Instance().Load();
    //dh::IOSettings::Instance().SetIOConfigPath("./Config/IODevice.xml");
    dh::IODeviceController::Instance().GetIODevice("ExternalDev_0").
        BindAction("KeyDown", dh::IE_Pressed, this, &CIOUIDebugDlg::OnKeyDown);

    struct __declspec(uuid("B372C9F6-1959-4650-960D-73F20CD479BB")) Interface{};
    auto uid = __uuidof(Interface);

    SetTimer(0, 0.02, nullptr);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIOUIDebugDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CIOUIDebugDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIOUIDebugDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    dh::IODeviceController::Instance().Update();
    CDialogEx::OnTimer(nIDEvent);
}
