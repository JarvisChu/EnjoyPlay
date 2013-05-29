#pragma once


// CAboutSoftDlg dialog

class CAboutSoftDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutSoftDlg)

public:
	CAboutSoftDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutSoftDlg();

// Dialog Data
	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
