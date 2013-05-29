
// EnjoyPlayDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "PlayListDlg.h"
#include "SettingDlg.h"
#include "AboutSoftDlg.h"
#include "afxwin.h"
#include "afxext.h"

#include <mmsystem.h> //导入声音头文件库   
#pragma comment(lib,"winmm.lib")//导入声音的链接库


// CEnjoyPlayDlg 对话框
class CEnjoyPlayDlg : public CDialogEx
{
// 构造
public:
	CEnjoyPlayDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ENJOYPLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CPlayListDlg	m_PlayListDlg;
	CSettingDlg     m_SettingDlg;
	CAboutSoftDlg	m_AboutSoftDlg;

	CString			m_szCurrentSong;	//当前播放的音乐全路径
	CString			m_szSinger;			//歌手
	int				m_nDeviceId;		//播放设备ID
	DWORD			m_nSongLen;			//歌曲长度
	bool			m_isPlaying;		//正在播放
	int				m_nPlayStatus;		//0 playing 1 paused 2 stopped 
	int				m_nPlayMode;		//0 only_one 1 one_loop 2 all_loop 3 all_random
	MCI_PLAY_PARMS  m_PlayParms;		//歌曲播放信息

private:
	CToolTipCtrl     m_Mytip;			//用于添加提示信息
public:
	void			InitTabControl();	//初始化TabControl控件
	void			InitPlayControl();	//初始化播放相关的控件
	void			PlayMusic(CString song); //播放音乐，并返回设备ID
	void			StopMusic();		//停止正在播放的音乐
	void			PauseMusic();		//停止正在播放的音乐
	void			ContinueMusic();	//停止正在播放的音乐

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	CSliderCtrl m_ctrlSlide;		//时间条
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
