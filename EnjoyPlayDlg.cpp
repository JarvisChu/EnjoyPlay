
// EnjoyPlayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EnjoyPlay.h"
#include "EnjoyPlayDlg.h"
#include "afxdialogex.h"

//#include <mmsystem.h> //导入声音头文件库   
//#pragma comment(lib,"winmm.lib")//导入声音的链接库

 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PLAYING 0
#define PAUSED	1
#define STOPPED 2

//playing mode
#define ONLY_ONE 0
#define ONE_LOOP 1
#define ALL_LOOP 2
#define ALL_RANDOM 3


// CEnjoyPlayDlg 对话框


CEnjoyPlayDlg::CEnjoyPlayDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEnjoyPlayDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEnjoyPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabCtrl);
	DDX_Control(pDX, IDC_SLIDER_TIME, m_ctrlSlide);
	DDX_Control(pDX, IDC_BTN_ICON_CONTROL, m_btnIconPlayOrPause);
	DDX_Control(pDX, IDC_BTN_ICON_STOP, m_btnIconStop);
	DDX_Control(pDX, IDC_BTN_PRE, m_btnIconPre);
	DDX_Control(pDX, IDC_BTN_NEXT, m_btnIconNext);
	DDX_Control(pDX, IDC_BTN_ICON_MODE, m_btnIconMode);
}

BEGIN_MESSAGE_MAP(CEnjoyPlayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, &CEnjoyPlayDlg::OnTcnSelchangeTabControl)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TIME, &CEnjoyPlayDlg::OnNMCustomdrawSliderTime)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_TIME, &CEnjoyPlayDlg::OnNMReleasedcaptureSliderTime)
	ON_MESSAGE(WM_PLAY_NEW_SONG, OnPlayNewSong)   //处理新消息
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_ICON_CONTROL, &CEnjoyPlayDlg::OnBnClickedBtnIconControl)
	ON_BN_CLICKED(IDC_BTN_ICON_STOP, &CEnjoyPlayDlg::OnBnClickedBtnIconStop)
	ON_BN_CLICKED(IDC_BTN_PRE, &CEnjoyPlayDlg::OnBnClickedBtnPre)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CEnjoyPlayDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_ICON_MODE, &CEnjoyPlayDlg::OnBnClickedBtnIconMode)
END_MESSAGE_MAP()

//初始化TabControl控件
void CEnjoyPlayDlg::InitTabControl()
{
	m_TabCtrl.InsertItem(0,_T("播放列表"));
	m_TabCtrl.InsertItem(1,_T("设置"));
	m_TabCtrl.InsertItem(2,_T("关于软件"));

	//添加Tab页
	m_PlayListDlg.Create(IDD_PLAYLIST,GetDlgItem(IDC_TAB_CONTROL));
	m_SettingDlg.Create(IDD_SETTING,GetDlgItem(IDC_TAB_CONTROL));
	m_AboutSoftDlg.Create(IDD_ABOUT,GetDlgItem(IDC_TAB_CONTROL));

	////Tab control 的客户区大小
	CRect rc;
	m_TabCtrl.GetClientRect(&rc);

	//调整子对话框在父窗口中的位置
	rc.top += 30;
	rc.bottom -= 8;
	rc.left += 8;
	rc.right -= 8;

	//设置子对话框尺寸并移动到指定位置
	m_PlayListDlg.MoveWindow(&rc);
	m_SettingDlg.MoveWindow(&rc);
	m_AboutSoftDlg.MoveWindow(&rc);

	//分别设置隐藏和显示
	m_PlayListDlg.ShowWindow(true);
	m_SettingDlg.ShowWindow(false);
	m_AboutSoftDlg.ShowWindow(false);

	//设置默认的选项卡
	m_TabCtrl.SetCurSel(0);
}

//初始化PlayControl控件
void CEnjoyPlayDlg::InitPlayControl()
{
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
	m_btnIconStop.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_STOP)));
	m_btnIconPre.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PREVIOUS)));
	m_btnIconNext.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_NEXT)));
	m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ALL)));
}

//打开音乐，并播放 ,返回 DeviceId
void CEnjoyPlayDlg::PlayMusic(CString song)
{
	//配置打开文件的参数
	MCI_OPEN_PARMS op;	//参数
	op.dwCallback=NULL; 
	op.lpstrAlias=NULL; 
	op.lpstrDeviceType=_T("MPEGAudio");		//设备类型，大多数文件可以这样设置 
	op.lpstrElementName=song;//_T("C:/123.mp3");	//文件路径 
	op.wDeviceID=NULL;						//打开设备成功以后保存这个设备号备用 


	//发送命令打开设备，成功返回0，否则返回错误号，第三个参数这里必须MCI_OPEN_ELEMENT  
	UINT rs=mciSendCommand(NULL,				//DeviceID, 在MCI_OPEN 命令中没有使用
						   MCI_OPEN,			//Command,发送的命令 MCI_ CLOSE、MCI_ INFO、MCI_ SAVE、MCI_ RESUME等等
						   MCI_OPEN_ELEMENT,	//Command的flag标记
						   (DWORD)&op			//打开参数
						  );

	mciSendCommand(NULL,
		MCI_SET,			//MCI_SET命令用来设置设备的信息。播放设备会识别这个命令 
		MCI_SET_DOOR_OPEN,	//打开媒体封面。MCI_SET_AUDIO_ALL(所有声道)、MCI_SET_AUDIO_LEFT（左声道）、MCI_SET_TIME_FORMAT等等
		NULL
		);
	
	//获取音频文件长度
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback=NULL;//(DWORD)GetSafeHwnd();
	mcistatusparms.dwItem=MCI_STATUS_LENGTH;
	mcistatusparms.dwReturn=0;
	mciSendCommand(op.wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mcistatusparms);

	this->m_nSongLen = mcistatusparms.dwReturn; //音频长度

	//设置播放条
	m_ctrlSlide.SetRange(0,(int)m_nSongLen/1000-1,true);
	//m_ctrlSlide.SetTicFreq(100);
	m_ctrlSlide.SetPos(0);

	SetTimer(1,1000,NULL); //设置定时器

	//设备打开成功就播放文件 
	if(rs==0)        
	{ 
		//重置播放参数
		//MCI_PLAY_PARMS pp; 
		//m_PlayParms = pp;
		m_PlayParms.dwCallback=NULL; 
		m_PlayParms.dwFrom=0;      //开始播放的位置 
		mciSendCommand(op.wDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);
		//播放文件，如果第三个参数设为MCI_WAIT则程序窗口会被阻塞，为了避免这种情况可以设为MCI_NOTIFY 
	}

	//根据文件长度等待，让音乐放完与MCI_WAIT效果一样，cdlen参数可以控制播放时间
	//Sleep(cdlen);

	//设置播放窗口的参数
	m_szCurrentSong = song;
	m_nDeviceId = op.wDeviceID;
	//m_isPlaying = true;			 
	m_nPlayStatus = PLAYING;//正在播放

	CString name = m_szCurrentSong.Mid(m_szCurrentSong.ReverseFind('\\')+1);  //文件名

	int m = (m_nSongLen/1000)/60;
	int s = (m_nSongLen/1000)%60;

	CString szLen;szLen.Format(_T("%02d:%02d"),m,s);
	SetDlgItemText(IDC_STATIC_NAME,name);
	SetDlgItemText(IDC_STATIC_LEN,szLen);
	SetDlgItemText(IDC_STATIC_SINGER,m_szSinger);

	//设置控件
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
}


//暂停音乐
void CEnjoyPlayDlg::PauseMusic()
{
	//MCI_PLAY_PARMS PlayParms;
	mciSendCommand (this->m_nDeviceId, MCI_PAUSE, 0,(DWORD)(LPVOID) &m_PlayParms);
	m_nPlayStatus = PAUSED;

	//设置控件
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
	
	//结束计时器
	KillTimer(1);
}

//继续音乐
void CEnjoyPlayDlg::ContinueMusic()
{
	mciSendCommand(m_nDeviceId,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);
	m_nPlayStatus = PLAYING;

	//设置控件
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
		
	//结束计时器
	SetTimer(1,1000,NULL); //设置定时器
}

//停止音乐
void CEnjoyPlayDlg::StopMusic()
{
	mciSendCommand (this->m_nDeviceId, MCI_STOP, NULL, NULL);
	mciSendCommand (this->m_nDeviceId, MCI_CLOSE, NULL, NULL);
	m_nPlayStatus = STOPPED;

	//结束计时器
	KillTimer(1);
	//设置相关控件
	this->m_ctrlSlide.SetPos(0);
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,_T("00:00"));	
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
}

//-------------------------------------------------------------------------
//                          CEnjoyPlayDlg 消息处理程序
//-------------------------------------------------------------------------

BOOL CEnjoyPlayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始化TabControl控件
	InitTabControl();

	//初始化播放控件
	InitPlayControl();

	//初始化 提示控件
	EnableToolTips(TRUE);  
	m_Mytip.Create(this); 

	m_Mytip.AddTool(GetDlgItem(IDC_STATIC_SONG),L"歌曲"); //
	m_Mytip.AddTool( GetDlgItem(IDC_STATIC_MUSICIAN), _T("歌手") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_STATIC_CLOCK), _T("歌曲长度") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_PRE), _T("上一首") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_NEXT), _T("下一首") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_CONTROL), _T("播放控制") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_STOP), _T("停止播放") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_MODE), _T("播放模式") ); //

	m_Mytip.SetDelayTime(200); //设置延迟
	m_Mytip.SetTipTextColor( RGB(0,125,0) ); //设置提示文本的颜色
	m_Mytip.SetTipBkColor( RGB(255,255,255)); //设置提示框的背景颜色
	m_Mytip.Activate(TRUE); //设置是否启用提示

	//初始化变量
	m_szCurrentSong = _T("");
	m_szSinger = _T("未知歌手");
	m_nPlayStatus = STOPPED;
	m_nPlayMode = ALL_LOOP; //

	if(m_PlayListDlg.GetFirstSong(m_szCurrentSong,m_szSinger))
	{
		PlayMusic(m_szCurrentSong);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEnjoyPlayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CEnjoyPlayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}



//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CEnjoyPlayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//Tab Control 标签页变化 消息
void CEnjoyPlayDlg::OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	int CurSel = m_TabCtrl.GetCurSel();

    switch(CurSel)
    {
    case 0:
		this->m_PlayListDlg.ShowWindow(true);
		this->m_SettingDlg.ShowWindow(false);
		this->m_AboutSoftDlg.ShowWindow(false);
        break;
    case 1:
        this->m_PlayListDlg.ShowWindow(false);
        this->m_SettingDlg.ShowWindow(true);
		this->m_AboutSoftDlg.ShowWindow(false);
        break;
	case 2:
        this->m_PlayListDlg.ShowWindow(false);
        this->m_SettingDlg.ShowWindow(false);
		this->m_AboutSoftDlg.ShowWindow(true);
        break;
    }
	*pResult = 0;
}


void CEnjoyPlayDlg::OnNMCustomdrawSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


//播放进度条值变化
void CEnjoyPlayDlg::OnNMReleasedcaptureSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	//设置
	*pResult = 0;
}


//处理播放列表中双击歌曲名的事件
LRESULT CEnjoyPlayDlg::OnPlayNewSong(WPARAM wParam, LPARAM lParam)
{
	//获取当前的歌曲名 和歌手

	this->m_szCurrentSong =*((CString*)wParam);
	this->m_szSinger =*((CString*)lParam);
	StopMusic();
	PlayMusic(m_szCurrentSong);
	return 0;
}


//定时器，用于设置进度条自动向前，和当前播放的时间
void CEnjoyPlayDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int pos = m_ctrlSlide.GetPos();
	pos++;
	m_ctrlSlide.SetPos(pos);

	//设置时间

	int m = pos/60;
	int s = pos%60;

	CString szLen;
	szLen.Format(_T("%02d:%02d"),m,s);
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,szLen);


	//判断歌曲是否结束
	if(pos==m_nSongLen/1000)
	{
		//KillTimer(1); //结束计时器

	    //AfxMessageBox(L"结束");	
		m_nPlayStatus = STOPPED;

		//判断当前的播放模式
		switch(m_nPlayMode)
		{
		case ONLY_ONE: //单曲
			//结束，设置图标后，什么都不做
			//AfxMessageBox(_T("Only one"));
			StopMusic();
			m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
			break;
		case ONE_LOOP: //单曲循环
			StopMusic();
			PlayMusic(m_szCurrentSong);
			break;
		case ALL_LOOP: //循环播放
			if(m_PlayListDlg.GetNextSong(m_szCurrentSong,m_szSinger))
			{
				StopMusic();
				PlayMusic(m_szCurrentSong);
			}
			break;
		case ALL_RANDOM://随机播放
			{
				bool rel = false;
				int randomCount = 0;
				randomCount=rand()%10+1; //产生一个随机数
				for(int i=0;i<randomCount;i++)
				{
					rel = m_PlayListDlg.GetNextSong(m_szCurrentSong,m_szSinger);
				}

				if(rel)
				{
					StopMusic();
					PlayMusic(m_szCurrentSong);
				}
			}
			break;
		default:
			break;
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


//播放进度条的滑动消息
void CEnjoyPlayDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//如果当前没有播放的歌曲
	if(m_szCurrentSong == _T(""))
	{
		this->m_ctrlSlide.SetPos(0);
		return ;
	}

	//获取当前值
	int pos = m_ctrlSlide.GetPos();

	//设置时间
	int m = pos/60;
	int s = pos%60;
	CString szLen;	szLen.Format(_T("%02d:%02d"),m,s);	
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,szLen);

	//控制播放
	//m_PlayParms.dwFrom = pos*1000;
	//mciSendCommand(m_nDeviceId,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);

	//跳转的目标时间，时间单位为毫秒
	MCI_SEEK_PARMS SeekParms;
	SeekParms.dwTo = pos * 1000;
	mciSendCommand (m_nDeviceId, MCI_SEEK, MCI_TO| MCI_WAIT,(DWORD)(LPVOID)&SeekParms);
	
	ContinueMusic(); //继续播放

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


//播放或者暂停
void CEnjoyPlayDlg::OnBnClickedBtnIconControl()
{
	if(m_nPlayStatus == PLAYING )  //如果正在播放则可以暂停
	{
		//AfxMessageBox(_T("is Playing"));
		PauseMusic();		
	}
	else if(m_nPlayStatus == PAUSED)			//已暂停则播放
	{
		//AfxMessageBox(_T("is Paused"));
		ContinueMusic(); //继续播放
	}
	else if(m_nPlayStatus == STOPPED)			//已停止则重新播放
	{
		if(m_szCurrentSong==_T(""))
		{
			if(this->m_PlayListDlg.GetFirstSong(m_szCurrentSong,m_szSinger))
			{
				PlayMusic(m_szCurrentSong);
			}			
		}
		else
		{
			PlayMusic(m_szCurrentSong);
		}	
	}
	else{}
}

//停止播放
void CEnjoyPlayDlg::OnBnClickedBtnIconStop()
{
	// TODO: Add your control notification handler code here
	StopMusic();
}


//上一曲
void CEnjoyPlayDlg::OnBnClickedBtnPre()
{
	if(this->m_PlayListDlg.GetPreviousSong(m_szCurrentSong,m_szSinger))
	{
		//停止已经在播放的
		StopMusic();

		//播放现在的
		PlayMusic(m_szCurrentSong);
	}

}

//下一曲
void CEnjoyPlayDlg::OnBnClickedBtnNext()
{
	if(this->m_PlayListDlg.GetNextSong(m_szCurrentSong,m_szSinger))
	{
		//停止已经在播放的
		StopMusic();

		//播放现在的
		PlayMusic(m_szCurrentSong);
	}
}


//设置播放模式
void CEnjoyPlayDlg::OnBnClickedBtnIconMode()
{
	//判断当前的播放模式，然后设置成下一种
	switch(m_nPlayMode)
	{
	case ONLY_ONE: //单曲
		m_nPlayMode = ONE_LOOP;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ONE)));
		break;
	case ONE_LOOP: //单曲循环
		m_nPlayMode = ALL_LOOP;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ALL)));
		break;
	case ALL_LOOP: //循环播放
		m_nPlayMode = ALL_RANDOM;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_RANDOM)));
		break;
	case ALL_RANDOM://随机播放
		m_nPlayMode = ONLY_ONE;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_ONLY_ONE)));
		break;
	default:
		break;
	}
}


//用于提示信息
BOOL CEnjoyPlayDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	 if(pMsg->message==WM_MOUSEMOVE /*&& pMsg->hwnd == GetDlgItem(IDC_BUTTON1)->GetSafeHwnd())*/)
		 m_Mytip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}



