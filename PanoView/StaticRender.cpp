#include "stdafx.h"
#include "StaticRender.h"
#include "resource.h"

CStaticRender::CStaticRender()
{
	view = NULL;
	viewType = PICTURE;
	attrFlag = false;
	m_bTracking = false;
	ctrlShowFlag = false;
	isLBtnDown = false;
	m_hCur = (HCURSOR)::LoadImage(NULL, L"ico\\20160728021617666_easyicon_net_32.ico", IMAGE_ICON, 22, 22, LR_LOADFROMFILE);
	m_hCur2 = (HCURSOR)::LoadImage(NULL, L"ico\\20160728021017697_easyicon_net_32.ico", IMAGE_ICON, 22, 22, LR_LOADFROMFILE);
}

CStaticRender::~CStaticRender()
{
}

void CStaticRender::setPanoramaView(PanoramaView* v)
{
	view = v;
}

void CStaticRender::setViewType(VIEW_TYPE type)
{
	viewType = type;
}

BEGIN_MESSAGE_MAP(CStaticRender, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	//ON_COMMAND(ID_MENU_ATTR, &CStaticRender::OnMenuAttr)
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CStaticRender::OnLButtonDown(UINT nFlags, CPoint point)
{
	isLBtnDown = true;
	::SetCursor(m_hCur2);
	
	if (view != NULL)
		view->mouseDownHandler(point.x, point.y);

	CStatic::OnLButtonDown(nFlags, point);
}

void CStaticRender::OnLButtonUp(UINT nFlags, CPoint point)
{
	//TRACE("OnLButtonUp .\n");
	isLBtnDown = false;
	::SetCursor(m_hCur);

	SetFocus();
	
	if (view != NULL)
		view->mouseUpHandler(point.x, point.y);

	CStatic::OnLButtonUp(nFlags, point);
}

void CStaticRender::OnMouseMove(UINT nFlags, CPoint point)
{
	int h = GetSystemMetrics(SM_CYSCREEN);

	SetFocus();

	if (point.y > h - 48)
		ctrlShowFlag = true;
	else
		ctrlShowFlag = false;
	
	if (!m_bTracking)    {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_HOVER;   //要触发的消息        
		tme.hwndTrack = this->m_hWnd;
		tme.dwHoverTime = 10;                 // 若不设此参数，则无法触发mouseHover       
		if (::_TrackMouseEvent(&tme))         //MOUSELEAVE|MOUSEHOVER消息由此函数触发        
		{
			m_bTracking = true;
		}
	}
	
	if (view != NULL)
		view->mouseMoveHandler(point.x, point.y);

	CStatic::OnMouseMove(nFlags, point);
}

void CStaticRender::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (view != NULL)
	    view->changeSize(cx, cy);
}

BOOL CStaticRender::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect;
	GetClientRect(rect);

	ScreenToClient(&pt);

	if (pt.x > rect.left && pt.x < rect.right && pt.y > rect.top && pt.y < rect.bottom) {
		if (zDelta < 0) {
			view->zoomInStep();
		}
		else {
			view->zoomOutStep();
		}
	}

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

void CStaticRender::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;       
	CMenu *pSubMenu;

	/*menu.LoadMenu(IDR_MENU_VIEW);
	pSubMenu = menu.GetSubMenu(0);

	if (viewType == PICTURE)
	    pSubMenu->ModifyMenuW(0, MF_BYPOSITION, ID_MENU_ATTR, _T("照片属性"));
	else if (viewType == VIDEO)
		pSubMenu->ModifyMenuW(0, MF_BYPOSITION, ID_MENU_ATTR, _T("视频属性"));

	// 将坐标值由客户坐标转换为屏幕坐标   
	ClientToScreen(&point);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);*/

	CStatic::OnRButtonDown(nFlags, point);
}

void CStaticRender::setAttrFlag(bool b)
{
	attrFlag = b;
}

bool CStaticRender::isAttrActive() const
{
	return attrFlag;
}

void CStaticRender::OnMenuAttr()
{
	attrFlag = true;
}


void CStaticRender::OnMouseLeave()
{
	m_bTracking = false;
	if (view != NULL)
		view->mouseUpHandler(0, 0);

	CStatic::OnMouseLeave();
}

bool CStaticRender::getCtrlShowFlag() const
{
	return ctrlShowFlag;
}


BOOL CStaticRender::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{	
	
	/*if (isLBtnDown == true) {
		::SetCursor(m_hCur2);
		return TRUE;
	}
	else {
		::SetCursor(m_hCur);
		return TRUE;
	}*/

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}
