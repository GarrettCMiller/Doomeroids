///////////////////////////////////////////////////////////////////////////////
//		File:						MinimapDlg.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
#pragma once


// MinimapDlg dialog

class MinimapDlg : public CDialog
{
	DECLARE_DYNAMIC(MinimapDlg)

public:
	MinimapDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MinimapDlg();

// Dialog Data
	enum { IDD = IDD_MINIMAP };

	void SetParent(CWnd *parent)
	{
		this->m_pParentWnd = parent;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	float	m_fZoom;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnClose();
};
