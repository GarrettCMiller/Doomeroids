///////////////////////////////////////////////////////////////////////////////
//		File:						PalDlg.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afxtempl.h>
#include <atlimage.h>

// PalDlg dialog

class PalDlg : public CDialog
{
	DECLARE_DYNAMIC(PalDlg)

	CImage *tileset;

public:
	PalDlg(/*CImage *palette,*/ CWnd* pParent = NULL);   // standard constructor
	virtual ~PalDlg();

	void SetTileSet(CImage *tset)
	{
		tileset = tset;
	}

	void SetParent(CWnd *parent)
	{
		this->m_pParentWnd = parent;
	}

// Dialog Data
	enum { IDD = IDD_PALETTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
