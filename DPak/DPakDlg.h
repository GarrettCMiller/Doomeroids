// DPakDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "FileTreeCtrl.h"
#include "afxwin.h"

// CDPakDlg dialog
class CDPakDlg : public CDialog
{
// Construction
public:
	CDPakDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DPAK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddFolder();
	afx_msg void OnBnClickedSave();
	void ProcTree(HTREEITEM& root);
	CTreeFileCtrl m_FileStruct;
	CListBox m_FilesAdded;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedReadpak();
	bool m_bIncludeSubFolders;
	CTreeFileCtrl m_PakStruct;
};
