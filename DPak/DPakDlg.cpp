// DPakDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DPak.h"
#include "DPakDlg.h"

#include "DPakFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDPakFile pak;

// CDPakDlg dialog

CString BasePath = "C:\\Documents and Settings\\Michael McCord\\My Documents\\Visual Studio Projects\\Doomeroids Pack\\Doomeroids";

CDPakDlg::CDPakDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDPakDlg::IDD, pParent)
	, m_bIncludeSubFolders(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDPakDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_STRUCTURE, m_FileStruct);
	DDX_Control(pDX, IDC_LIST, m_FilesAdded);
	DDX_Control(pDX, IDC_PAK_FILE_STRUCTURE, m_PakStruct);
}

BEGIN_MESSAGE_MAP(CDPakDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADD_FOLDER, OnBnClickedAddFolder)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_READPAK, OnBnClickedReadpak)
END_MESSAGE_MAP()


// CDPakDlg message handlers

BOOL CDPakDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	String strBasePath = BasePath.GetBuffer();

	m_FileStruct.SetRootFolder(BasePath);
	pak.SetBasePath(strBasePath);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDPakDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDPakDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDPakDlg::OnBnClickedAddFolder()
{
	HTREEITEM node = m_FileStruct.GetRootItem();	//m_FileStruct.GetSelectedItem();

	ProcTree(node);

	CString filecount = "Added ";
	filecount.AppendFormat("%d", m_FilesAdded.GetCount());
	filecount += " files!";

	m_FilesAdded.AddString(filecount);
}

void CDPakDlg::ProcTree(HTREEITEM& root)
{
	static HTREEITEM parent = NULL;

	while (root)
	{
		//If I'm checked or my parent item is checked, expand me
		if (/*((m_FileStruct.IsFolder(root) && m_bIncludeSubFolders) || m_FileStruct.IsFile(root))
			&&*/ (m_FileStruct.GetChecked(root) || (parent && m_FileStruct.GetChecked(parent))))
		{
			m_FileStruct.Expand(root, TVE_EXPAND);
			m_FileStruct.SetChecked(root, TRUE);
		}
		
		if (m_FileStruct.IsFile(root) && 
			((parent && m_FileStruct.GetChecked(parent)) || m_FileStruct.GetChecked(root)))
		{
			CString path = m_FileStruct.ItemToPath(root);
			CString text = "Added " + path + "...";

			m_FilesAdded.AddString(text);

			String strPath = path.GetBuffer();
			pak.AddFile(strPath);

			//CString newpath		= (((const char *) path) + strspn(path, BasePath));
			//CString filename	= (strrchr(newpath, '\\') + 1);
			//m_PakStruct.InsertFileItem(filename, "", TVI_LAST);
		}
		else
		{
			HTREEITEM hChild = m_FileStruct.GetChildItem(root);
			
			HTREEITEM temp = parent;
			parent = root;
			ProcTree(hChild);
			parent = temp;
		}

		root = m_FileStruct.GetNextSiblingItem(root);
	}
}

void CDPakDlg::OnBnClickedSave()
{
	pak.WritePakFile();

	MessageBox("Pak File Written Successfully!", "Success");
}

void CDPakDlg::OnBnClickedButton1()
{
	pak.ClearAllFiles();
	m_FilesAdded.ResetContent();
}

void CDPakDlg::OnBnClickedReadpak()
{
	pak.ReadPakFile();

	char buffer[1024] = {'\0'};

	DPakFile dpf;
	
	pak.GetFileByName(dpf, "Script3.scr");

	ASSERT(dpf.IsValid());

	strncpy(buffer, (const char *) dpf.FileData, min(1024, dpf.FileSize));

	MessageBox(buffer);
}
