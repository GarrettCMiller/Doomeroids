///////////////////////////////////////////////////////////////////////////////
//		File:						MapProps.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:			Implementation of 
///////////////////////////////////////////////////////////////////////////////
#pragma once


// MapProps dialog

class MapProps : public CDialog
{
	DECLARE_DYNAMIC(MapProps)

public:
	MapProps(CWnd* pParent = NULL);   // standard constructor
	virtual ~MapProps();

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// width of map (in tiles)
	UINT map_width;
	// height of map (in tiles)
	UINT map_height;
	afx_msg void OnBnClickedOk();
	String MapName;
	String MapDesc;
};
