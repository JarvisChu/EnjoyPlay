#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CPlayListDlg dialog

class CPlayListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayListDlg)

public:
	CPlayListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayListDlg();

private:
	int	m_nCurrentItem; //��ǰ��

public:
	bool GetCurrentSelectedSong(CString& song,CString& singer);
	bool GetFirstSong(CString& song,CString& singer);
	bool GetPreviousSong(CString& song,CString& singer); //��һ��
	bool GetNextSong(CString& song,CString& singer); //��һ��
	bool LoadPlayList();	//���ز����б�
	bool SavePlayList();	//���沥���б�

// Dialog Data
	enum { IDD = IDD_PLAYLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlPlayList;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkPlaylist(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btnIconRemoveSong;
	afx_msg void OnBnClickedBtnAddSong();
	CButton m_btnIconAddSong;
	afx_msg void OnPaint();
};
