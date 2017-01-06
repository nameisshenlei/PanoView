
// PanoViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PanoView.h"
#include "PanoViewDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPanoViewDlg �Ի���



CPanoViewDlg::CPanoViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPanoViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPanoViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_RENDER, m_renderArea);
}

BEGIN_MESSAGE_MAP(CPanoViewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CPanoViewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPanoViewDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPanoViewDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPanoViewDlg ��Ϣ�������
BOOL CPanoViewDlg::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;

	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA;

	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 0;
	pixelDesc.cRedShift = 0;
	pixelDesc.cGreenBits = 0;
	pixelDesc.cGreenShift = 0;
	pixelDesc.cBlueBits = 0;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 0;
	pixelDesc.cAccumRedBits = 0;
	pixelDesc.cAccumGreenBits = 0;
	pixelDesc.cAccumBlueBits = 0;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 0;
	pixelDesc.cStencilBits = 1;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;

	pixelFormat = ChoosePixelFormat(hDC, &pixelDesc);
	if (pixelFormat == 0) {
		pixelFormat = 1;
		if (DescribePixelFormat(hDC, pixelFormat,
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
		{
			return FALSE;
		}
	}
	if (SetPixelFormat(hDC, pixelFormat, &pixelDesc) == FALSE)
		return FALSE;

	return TRUE;
}

BOOL CPanoViewDlg::CreateViewGLContext(HDC hDC)
{
	hrenderRC = wglCreateContext(hDC);

	if (hrenderRC == NULL)
		return FALSE;

	if (wglMakeCurrent(hDC, hrenderRC) == FALSE)
		return FALSE;

	return TRUE;
}

void CPanoViewDlg::initPlayer()
{
	CWnd *wnd = GetDlgItem(IDC_PICTURE_RENDER);
	hrenderDC = ::GetDC(wnd->m_hWnd);

	if (SetWindowPixelFormat(hrenderDC) == FALSE)
		return;
	if (CreateViewGLContext(hrenderDC) == FALSE)
		return;

	CRect rect;
	wnd->GetClientRect(&rect);

	panoramaView = new PanoramaView(hrenderDC);
	panoramaView->init(rect.Width(), rect.Height());

	int width = 5376;
	int height = 2688;
	int rgbLenght = width * height * 3;

	rgbData = new unsigned char[rgbLenght];

	FILE* rgbfile = fopen("pano.rgb", "rb+");
	fread(rgbData, 1, rgbLenght, rgbfile);
	fclose(rgbfile);

	SetTimer(1, 40, 0);   //����ˢ�¶�ʱ��

	m_renderArea.setPanoramaView(panoramaView);
}

BOOL CPanoViewDlg::OnInitDialog()
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	initPlayer();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPanoViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPanoViewDlg::OnPaint()
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
HCURSOR CPanoViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);

}


void CPanoViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);

	panoramaView->render(rgbData, 5376, 2688);

	//TRACE("SSSS\n");
}


void CPanoViewDlg::OnBnClickedButton1()
{
	panoramaView->switchPanoMode(PanoramaView::PANO_MODE::PLANTET);
}


void CPanoViewDlg::OnBnClickedButton2()
{
	panoramaView->switchPanoMode(PanoramaView::PANO_MODE::SPHERE);
}


void CPanoViewDlg::OnBnClickedButton3()
{
	panoramaView->switchPanoMode(PanoramaView::PANO_MODE::NORMAL);
}
