
// EnjoyPlayDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "PlayListDlg.h"
#include "SettingDlg.h"
#include "AboutSoftDlg.h"
#include "afxwin.h"
#include "afxext.h"

#include <mmsystem.h> //��������ͷ�ļ���   
#pragma comment(lib,"winmm.lib")//�������������ӿ�


// CEnjoyPlayDlg �Ի���
class CEnjoyPlayDlg : public CDialogEx
{
// ����
public:
	CEnjoyPlayDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ENJOYPLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CPlayListDlg	m_PlayListDlg;
	CSettingDlg     m_SettingDlg;
	CAboutSoftDlg	m_AboutSoftDlg;

	CString			m_szCurrentSong;	//��ǰ���ŵ�����ȫ·��
	CString			m_szSinger;			//����
	int				m_nDeviceId;		//�����豸ID
	DWORD			m_nSongLen;			//��������
	bool			m_isPlaying;		//���ڲ���
	int				m_nPlayStatus;		//0 playing 1 paused 2 stopped 
	int				m_nPlayMode;		//0 only_one 1 one_loop 2 all_loop 3 all_random
	MCI_PLAY_PARMS  m_PlayParms;		//����������Ϣ

private:
	CToolTipCtrl     m_Mytip;			//���������ʾ��Ϣ
public:
	void			InitTabControl();	//��ʼ��TabControl�ؼ�
	void			InitPlayControl();	//��ʼ��������صĿؼ�
	void			PlayMusic(CString song); //�������֣��������豸ID
	void			StopMusic();		//ֹͣ���ڲ��ŵ�����
	void			PauseMusic();		//ֹͣ���ڲ��ŵ�����
	void			ContinueMusic();	//ֹͣ���ڲ��ŵ�����

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabCtrl;
	afx_msg void OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnPlayNewSong(WPARAM wParam, LPARAM lParam); //WM_PLAY_NEW_SONG
	CSliderCtrl m_ctrlSlide;		//ʱ����
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CButton m_btnIconPlayOrPause;
	afx_msg void OnBnClickedBtnIconControl();
	CButton m_btnIconStop;
	afx_msg void OnBnClickedBtnIconStop();
	CButton m_btnIconPre;
	CButton m_btnIconNext;
	afx_msg void OnBnClickedBtnPre();
	afx_msg void OnBnClickedBtnNext();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_btnIconMode;
	afx_msg void OnBnClickedBtnIconMode();
};
