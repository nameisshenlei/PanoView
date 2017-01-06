#pragma once
#include "afxwin.h"
#include "PanoramaView.h"
#include "stdafx.h"
//#include "AttrDlg.h"

class CStaticRender :
	public CStatic
{
public:
	enum VIEW_TYPE {PICTURE, VIDEO};

	CStaticRender();
	~CStaticRender();
	void setPanoramaView(PanoramaView* view);
	void setViewType(VIEW_TYPE type);
	void setAttrFlag(bool b);
	bool isAttrActive() const;
	bool getCtrlShowFlag() const;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	PanoramaView* view;
	VIEW_TYPE     viewType;
	bool attrFlag;
	bool m_bTracking;
	bool ctrlShowFlag;
	bool isLBtnDown;
	HCURSOR m_hCur;
	HCURSOR m_hCur2;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuAttr();
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

