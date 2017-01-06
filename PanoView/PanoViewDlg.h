
// PanoViewDlg.h : 头文件
//

#pragma once

#include "PanoramaView.h"
#include "StaticRender.h"


// CPanoViewDlg 对话框
class CPanoViewDlg : public CDialogEx
{
// 构造
public:
	CPanoViewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PANOVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	HDC    hrenderDC;
	int    pixelFormat;
	HGLRC  hrenderRC;
	PanoramaView*   panoramaView;
	unsigned char * rgbData = NULL;

	void initPlayer();
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStaticRender m_renderArea;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
