
// IOUIDebugDlg.h : ͷ�ļ�
//

#pragma once
#include "IODeviceController.h"

namespace dh = IOToolkit;

// CIOUIDebugDlg �Ի���
class CIOUIDebugDlg : public CDialogEx
{
// ����
public:
	CIOUIDebugDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IOUIDEBUG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:

    void OnSampleAxis(float InValue);
    void OnKeyDown(const dh::FKey InKey);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};
