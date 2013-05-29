
// EnjoyPlayDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EnjoyPlay.h"
#include "EnjoyPlayDlg.h"
#include "afxdialogex.h"

//#include <mmsystem.h> //��������ͷ�ļ���   
//#pragma comment(lib,"winmm.lib")//�������������ӿ�

 
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


// CEnjoyPlayDlg �Ի���


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
	ON_MESSAGE(WM_PLAY_NEW_SONG, OnPlayNewSong)   //��������Ϣ
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_ICON_CONTROL, &CEnjoyPlayDlg::OnBnClickedBtnIconControl)
	ON_BN_CLICKED(IDC_BTN_ICON_STOP, &CEnjoyPlayDlg::OnBnClickedBtnIconStop)
	ON_BN_CLICKED(IDC_BTN_PRE, &CEnjoyPlayDlg::OnBnClickedBtnPre)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CEnjoyPlayDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_ICON_MODE, &CEnjoyPlayDlg::OnBnClickedBtnIconMode)
END_MESSAGE_MAP()

//��ʼ��TabControl�ؼ�
void CEnjoyPlayDlg::InitTabControl()
{
	m_TabCtrl.InsertItem(0,_T("�����б�"));
	m_TabCtrl.InsertItem(1,_T("����"));
	m_TabCtrl.InsertItem(2,_T("�������"));

	//���Tabҳ
	m_PlayListDlg.Create(IDD_PLAYLIST,GetDlgItem(IDC_TAB_CONTROL));
	m_SettingDlg.Create(IDD_SETTING,GetDlgItem(IDC_TAB_CONTROL));
	m_AboutSoftDlg.Create(IDD_ABOUT,GetDlgItem(IDC_TAB_CONTROL));

	////Tab control �Ŀͻ�����С
	CRect rc;
	m_TabCtrl.GetClientRect(&rc);

	//�����ӶԻ����ڸ������е�λ��
	rc.top += 30;
	rc.bottom -= 8;
	rc.left += 8;
	rc.right -= 8;

	//�����ӶԻ���ߴ粢�ƶ���ָ��λ��
	m_PlayListDlg.MoveWindow(&rc);
	m_SettingDlg.MoveWindow(&rc);
	m_AboutSoftDlg.MoveWindow(&rc);

	//�ֱ��������غ���ʾ
	m_PlayListDlg.ShowWindow(true);
	m_SettingDlg.ShowWindow(false);
	m_AboutSoftDlg.ShowWindow(false);

	//����Ĭ�ϵ�ѡ�
	m_TabCtrl.SetCurSel(0);
}

//��ʼ��PlayControl�ؼ�
void CEnjoyPlayDlg::InitPlayControl()
{
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
	m_btnIconStop.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_STOP)));
	m_btnIconPre.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PREVIOUS)));
	m_btnIconNext.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_NEXT)));
	m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ALL)));
}

//�����֣������� ,���� DeviceId
void CEnjoyPlayDlg::PlayMusic(CString song)
{
	//���ô��ļ��Ĳ���
	MCI_OPEN_PARMS op;	//����
	op.dwCallback=NULL; 
	op.lpstrAlias=NULL; 
	op.lpstrDeviceType=_T("MPEGAudio");		//�豸���ͣ�������ļ������������� 
	op.lpstrElementName=song;//_T("C:/123.mp3");	//�ļ�·�� 
	op.wDeviceID=NULL;						//���豸�ɹ��Ժ󱣴�����豸�ű��� 


	//����������豸���ɹ�����0�����򷵻ش���ţ������������������MCI_OPEN_ELEMENT  
	UINT rs=mciSendCommand(NULL,				//DeviceID, ��MCI_OPEN ������û��ʹ��
						   MCI_OPEN,			//Command,���͵����� MCI_ CLOSE��MCI_ INFO��MCI_ SAVE��MCI_ RESUME�ȵ�
						   MCI_OPEN_ELEMENT,	//Command��flag���
						   (DWORD)&op			//�򿪲���
						  );

	mciSendCommand(NULL,
		MCI_SET,			//MCI_SET�������������豸����Ϣ�������豸��ʶ��������� 
		MCI_SET_DOOR_OPEN,	//��ý����档MCI_SET_AUDIO_ALL(��������)��MCI_SET_AUDIO_LEFT������������MCI_SET_TIME_FORMAT�ȵ�
		NULL
		);
	
	//��ȡ��Ƶ�ļ�����
	MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback=NULL;//(DWORD)GetSafeHwnd();
	mcistatusparms.dwItem=MCI_STATUS_LENGTH;
	mcistatusparms.dwReturn=0;
	mciSendCommand(op.wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mcistatusparms);

	this->m_nSongLen = mcistatusparms.dwReturn; //��Ƶ����

	//���ò�����
	m_ctrlSlide.SetRange(0,(int)m_nSongLen/1000-1,true);
	//m_ctrlSlide.SetTicFreq(100);
	m_ctrlSlide.SetPos(0);

	SetTimer(1,1000,NULL); //���ö�ʱ��

	//�豸�򿪳ɹ��Ͳ����ļ� 
	if(rs==0)        
	{ 
		//���ò��Ų���
		//MCI_PLAY_PARMS pp; 
		//m_PlayParms = pp;
		m_PlayParms.dwCallback=NULL; 
		m_PlayParms.dwFrom=0;      //��ʼ���ŵ�λ�� 
		mciSendCommand(op.wDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);
		//�����ļ������������������ΪMCI_WAIT����򴰿ڻᱻ������Ϊ�˱����������������ΪMCI_NOTIFY 
	}

	//�����ļ����ȵȴ��������ַ�����MCI_WAITЧ��һ����cdlen�������Կ��Ʋ���ʱ��
	//Sleep(cdlen);

	//���ò��Ŵ��ڵĲ���
	m_szCurrentSong = song;
	m_nDeviceId = op.wDeviceID;
	//m_isPlaying = true;			 
	m_nPlayStatus = PLAYING;//���ڲ���

	CString name = m_szCurrentSong.Mid(m_szCurrentSong.ReverseFind('\\')+1);  //�ļ���

	int m = (m_nSongLen/1000)/60;
	int s = (m_nSongLen/1000)%60;

	CString szLen;szLen.Format(_T("%02d:%02d"),m,s);
	SetDlgItemText(IDC_STATIC_NAME,name);
	SetDlgItemText(IDC_STATIC_LEN,szLen);
	SetDlgItemText(IDC_STATIC_SINGER,m_szSinger);

	//���ÿؼ�
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
}


//��ͣ����
void CEnjoyPlayDlg::PauseMusic()
{
	//MCI_PLAY_PARMS PlayParms;
	mciSendCommand (this->m_nDeviceId, MCI_PAUSE, 0,(DWORD)(LPVOID) &m_PlayParms);
	m_nPlayStatus = PAUSED;

	//���ÿؼ�
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
	
	//������ʱ��
	KillTimer(1);
}

//��������
void CEnjoyPlayDlg::ContinueMusic()
{
	mciSendCommand(m_nDeviceId,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);
	m_nPlayStatus = PLAYING;

	//���ÿؼ�
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PAUSE)));
		
	//������ʱ��
	SetTimer(1,1000,NULL); //���ö�ʱ��
}

//ֹͣ����
void CEnjoyPlayDlg::StopMusic()
{
	mciSendCommand (this->m_nDeviceId, MCI_STOP, NULL, NULL);
	mciSendCommand (this->m_nDeviceId, MCI_CLOSE, NULL, NULL);
	m_nPlayStatus = STOPPED;

	//������ʱ��
	KillTimer(1);
	//������ؿؼ�
	this->m_ctrlSlide.SetPos(0);
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,_T("00:00"));	
	m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
}

//-------------------------------------------------------------------------
//                          CEnjoyPlayDlg ��Ϣ�������
//-------------------------------------------------------------------------

BOOL CEnjoyPlayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//��ʼ��TabControl�ؼ�
	InitTabControl();

	//��ʼ�����ſؼ�
	InitPlayControl();

	//��ʼ�� ��ʾ�ؼ�
	EnableToolTips(TRUE);  
	m_Mytip.Create(this); 

	m_Mytip.AddTool(GetDlgItem(IDC_STATIC_SONG),L"����"); //
	m_Mytip.AddTool( GetDlgItem(IDC_STATIC_MUSICIAN), _T("����") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_STATIC_CLOCK), _T("��������") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_PRE), _T("��һ��") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_NEXT), _T("��һ��") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_CONTROL), _T("���ſ���") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_STOP), _T("ֹͣ����") ); //
	m_Mytip.AddTool( GetDlgItem(IDC_BTN_ICON_MODE), _T("����ģʽ") ); //

	m_Mytip.SetDelayTime(200); //�����ӳ�
	m_Mytip.SetTipTextColor( RGB(0,125,0) ); //������ʾ�ı�����ɫ
	m_Mytip.SetTipBkColor( RGB(255,255,255)); //������ʾ��ı�����ɫ
	m_Mytip.Activate(TRUE); //�����Ƿ�������ʾ

	//��ʼ������
	m_szCurrentSong = _T("");
	m_szSinger = _T("δ֪����");
	m_nPlayStatus = STOPPED;
	m_nPlayMode = ALL_LOOP; //

	if(m_PlayListDlg.GetFirstSong(m_szCurrentSong,m_szSinger))
	{
		PlayMusic(m_szCurrentSong);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}



//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CEnjoyPlayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//Tab Control ��ǩҳ�仯 ��Ϣ
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


//���Ž�����ֵ�仯
void CEnjoyPlayDlg::OnNMReleasedcaptureSliderTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	//����
	*pResult = 0;
}


//�������б���˫�����������¼�
LRESULT CEnjoyPlayDlg::OnPlayNewSong(WPARAM wParam, LPARAM lParam)
{
	//��ȡ��ǰ�ĸ����� �͸���

	this->m_szCurrentSong =*((CString*)wParam);
	this->m_szSinger =*((CString*)lParam);
	StopMusic();
	PlayMusic(m_szCurrentSong);
	return 0;
}


//��ʱ�����������ý������Զ���ǰ���͵�ǰ���ŵ�ʱ��
void CEnjoyPlayDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int pos = m_ctrlSlide.GetPos();
	pos++;
	m_ctrlSlide.SetPos(pos);

	//����ʱ��

	int m = pos/60;
	int s = pos%60;

	CString szLen;
	szLen.Format(_T("%02d:%02d"),m,s);
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,szLen);


	//�жϸ����Ƿ����
	if(pos==m_nSongLen/1000)
	{
		//KillTimer(1); //������ʱ��

	    //AfxMessageBox(L"����");	
		m_nPlayStatus = STOPPED;

		//�жϵ�ǰ�Ĳ���ģʽ
		switch(m_nPlayMode)
		{
		case ONLY_ONE: //����
			//����������ͼ���ʲô������
			//AfxMessageBox(_T("Only one"));
			StopMusic();
			m_btnIconPlayOrPause.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_PLAY)));
			break;
		case ONE_LOOP: //����ѭ��
			StopMusic();
			PlayMusic(m_szCurrentSong);
			break;
		case ALL_LOOP: //ѭ������
			if(m_PlayListDlg.GetNextSong(m_szCurrentSong,m_szSinger))
			{
				StopMusic();
				PlayMusic(m_szCurrentSong);
			}
			break;
		case ALL_RANDOM://�������
			{
				bool rel = false;
				int randomCount = 0;
				randomCount=rand()%10+1; //����һ�������
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


//���Ž������Ļ�����Ϣ
void CEnjoyPlayDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//�����ǰû�в��ŵĸ���
	if(m_szCurrentSong == _T(""))
	{
		this->m_ctrlSlide.SetPos(0);
		return ;
	}

	//��ȡ��ǰֵ
	int pos = m_ctrlSlide.GetPos();

	//����ʱ��
	int m = pos/60;
	int s = pos%60;
	CString szLen;	szLen.Format(_T("%02d:%02d"),m,s);	
	SetDlgItemText(IDC_STATIC_CURRENT_TIME,szLen);

	//���Ʋ���
	//m_PlayParms.dwFrom = pos*1000;
	//mciSendCommand(m_nDeviceId,MCI_PLAY,MCI_NOTIFY,(DWORD)&m_PlayParms);

	//��ת��Ŀ��ʱ�䣬ʱ�䵥λΪ����
	MCI_SEEK_PARMS SeekParms;
	SeekParms.dwTo = pos * 1000;
	mciSendCommand (m_nDeviceId, MCI_SEEK, MCI_TO| MCI_WAIT,(DWORD)(LPVOID)&SeekParms);
	
	ContinueMusic(); //��������

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


//���Ż�����ͣ
void CEnjoyPlayDlg::OnBnClickedBtnIconControl()
{
	if(m_nPlayStatus == PLAYING )  //������ڲ����������ͣ
	{
		//AfxMessageBox(_T("is Playing"));
		PauseMusic();		
	}
	else if(m_nPlayStatus == PAUSED)			//����ͣ�򲥷�
	{
		//AfxMessageBox(_T("is Paused"));
		ContinueMusic(); //��������
	}
	else if(m_nPlayStatus == STOPPED)			//��ֹͣ�����²���
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

//ֹͣ����
void CEnjoyPlayDlg::OnBnClickedBtnIconStop()
{
	// TODO: Add your control notification handler code here
	StopMusic();
}


//��һ��
void CEnjoyPlayDlg::OnBnClickedBtnPre()
{
	if(this->m_PlayListDlg.GetPreviousSong(m_szCurrentSong,m_szSinger))
	{
		//ֹͣ�Ѿ��ڲ��ŵ�
		StopMusic();

		//�������ڵ�
		PlayMusic(m_szCurrentSong);
	}

}

//��һ��
void CEnjoyPlayDlg::OnBnClickedBtnNext()
{
	if(this->m_PlayListDlg.GetNextSong(m_szCurrentSong,m_szSinger))
	{
		//ֹͣ�Ѿ��ڲ��ŵ�
		StopMusic();

		//�������ڵ�
		PlayMusic(m_szCurrentSong);
	}
}


//���ò���ģʽ
void CEnjoyPlayDlg::OnBnClickedBtnIconMode()
{
	//�жϵ�ǰ�Ĳ���ģʽ��Ȼ�����ó���һ��
	switch(m_nPlayMode)
	{
	case ONLY_ONE: //����
		m_nPlayMode = ONE_LOOP;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ONE)));
		break;
	case ONE_LOOP: //����ѭ��
		m_nPlayMode = ALL_LOOP;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_LOOP_ALL)));
		break;
	case ALL_LOOP: //ѭ������
		m_nPlayMode = ALL_RANDOM;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_RANDOM)));
		break;
	case ALL_RANDOM://�������
		m_nPlayMode = ONLY_ONE;
		m_btnIconMode.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_ONLY_ONE)));
		break;
	default:
		break;
	}
}


//������ʾ��Ϣ
BOOL CEnjoyPlayDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	 if(pMsg->message==WM_MOUSEMOVE /*&& pMsg->hwnd == GetDlgItem(IDC_BUTTON1)->GetSafeHwnd())*/)
		 m_Mytip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}



