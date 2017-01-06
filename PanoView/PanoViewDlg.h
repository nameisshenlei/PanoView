
// PanoViewDlg.h : ͷ�ļ�
//

#pragma once

#include "PanoramaView.h"
#include "StaticRender.h"


// CPanoViewDlg �Ի���
class CPanoViewDlg : public CDialogEx
{
// ����
public:
	CPanoViewDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PANOVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
