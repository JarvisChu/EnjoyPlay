// PlayListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnjoyPlay.h"
#include "PlayListDlg.h"
#include "afxdialogex.h"

#include <vector>

using namespace std;

typedef struct {
	CString name;   //歌曲名
	CString singer; //歌手
	CString path;	//目录
}SONG;

// CPlayListDlg dialog

IMPLEMENT_DYNAMIC(CPlayListDlg, CDialogEx)

CPlayListDlg::CPlayListDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlayListDlg::IDD, pParent)
{
	this->m_pParentWnd = pParent;
}

CPlayListDlg::~CPlayListDlg()
{
}

void CPlayListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYLIST, m_ctrlPlayList);
	DDX_Control(pDX, IDC_BTN_DELETE_SONG, m_btnIconRemoveSong);	
	DDX_Control(pDX, IDI_BTN_ADD_SONG, m_btnIconAddSong);
}


BEGIN_MESSAGE_MAP(CPlayListDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST, &CPlayListDlg::OnNMDblclkPlaylist)
	ON_BN_CLICKED(IDI_BTN_ADD_SONG, &CPlayListDlg::OnBnClickedBtnAddSong)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPlayListDlg message handlers


BOOL CPlayListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	m_ctrlPlayList.DeleteAllItems();//清空

	m_ctrlPlayList.ModifyStyle(0L,LVS_REPORT); 
	//m_ctrlPlayList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EDITLABELS|LVS_EX_ONECLICKACTIVATE|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
	m_ctrlPlayList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_GRIDLINES);

	this->m_ctrlPlayList.InsertColumn(0,_T("歌曲"),LVCFMT_LEFT,80);
	this->m_ctrlPlayList.InsertColumn(1,_T("歌手"),LVCFMT_LEFT,60);
	this->m_ctrlPlayList.InsertColumn(2,_T("目录"),LVCFMT_LEFT,80);

	
	//加载播放列表
	LoadPlayList();

	//this->m_ctrlPlayList.InsertItem(0,_T("123.mp3"));   //每一列的第一个数据要用InsertItem
	//this->m_ctrlPlayList.SetItemText(0,1,_T("JarvisChu"));
	//this->m_ctrlPlayList.SetItemText(0,2,_T("C:\\"));
	//m_ctrlPlayList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	//设置button图标
	m_btnIconAddSong.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_OPEN_GREEN)));//IDI_ICON_ADD_MUSIC
	m_btnIconRemoveSong.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_DEL_MUSIC)));

	//初始化变量
	m_nCurrentItem = 0; //当前选中的行

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//添加歌曲
void CPlayListDlg::OnBnClickedBtnAddSong()
{
	//文件打开对话框
	CFileDialog dlg(true,//TRUE为OPEN对话框，FALSE为SAVE AS对话框
					nullptr,//缺省的扩展名 
					nullptr,//显示在文件名编辑框的文件名 可设置为nullptr
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,//对话框风格 (可多选)
					(LPCTSTR)_TEXT("Mp3 Files (*.mp3)|*.mp3|Wave Filse (*.wav)|*.wav|All Files (*.*)|*.*||"),//扩展名
					nullptr//父窗口
					);

	//设置缓冲参数，用于保存得到的多个文件全路径信息
	const DWORD maxBufSize = 512;
	TCHAR buf[maxBufSize];			//用来存放所有文件结果的缓冲区

	dlg.m_ofn.nMaxFile = maxBufSize;
	dlg.m_ofn.lpstrFile = buf;
	dlg.m_ofn.lpstrFile[0] = NULL;

//  用CFileDialog类并设置OFN_ALLOWMULTISELECT标志时，
//	OPENFILENAME结构体的lpstrFile成员是一个指向用户申请分配的缓冲区，
//	里面接受所选的路径和文件名列表，
//	这个列表的每一项由一个NULL隔开，最末以两个NULL结束。
//	nMaxFile成员指明了缓冲区的大小，
//	如果所选择的文件名的加起来的总长度超过了此值，则DoModal函数返回IDCANCEL，
//	如果用户试图打开超过缓冲区大小的文件集的话，CommDlgExtendedError()将返回FNERR_BUFFERTOOSMALL，
//	这时候，lpstrFile的前两个byte将会包含实际需要的缓冲区的字节数。 

	if(dlg.DoModal()==IDOK)
	{
		POSITION pos = dlg.GetStartPosition(); //获得起始位置

		int start  = this->m_ctrlPlayList.GetItemCount();	//播放列表中现有的歌曲数
		//CString str;str.Format(_T("%d"),start);AfxMessageBox(str);

		while (pos != NULL)
		{
			CString fullPath = dlg.GetNextPathName(pos); //获得缓冲区中的一个文件  "C:\123.mp3"

			//分割出文件名和路径  
			CString name = fullPath.Mid(fullPath.ReverseFind('\\')+1);  //文件名
			CString path = fullPath.Mid(0,fullPath.ReverseFind('\\')); //路径
			
			//保存到列表中
			//this->m_ctrlPlayList.SetItemText(start,0,name);
			this->m_ctrlPlayList.InsertItem(start,name);
			this->m_ctrlPlayList.SetItemText(start,1,_T("未知歌手")); //TODO: 添加一个能获取歌手的 函数
			this->m_ctrlPlayList.SetItemText(start++,2,path);
		}
    }
	UpdateData(false);
}

//获取当前选中的第一首歌曲
bool CPlayListDlg::GetCurrentSelectedSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	for(int i=0;i<count;i++)
	{
		int state = m_ctrlPlayList.GetItemState(i,LVIS_SELECTED); //是否被选中
		if(state !=0)
		{
			CString name,path;
			name = m_ctrlPlayList.GetItemText(i,0);
			singer = m_ctrlPlayList.GetItemText(i,1);
			path = m_ctrlPlayList.GetItemText(i,2);
			song = path+_T("\\")+name;

			m_ctrlPlayList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_nCurrentItem = i; //更新当前行

			UpdateData(false);
			return true;
		}
	}
	return false;
}

//获取列表的第一首
bool CPlayListDlg::GetFirstSong(CString& song,CString& singer)
{
	UpdateData(true);
	int count = m_ctrlPlayList.GetItemCount();
	if(count>=1)
	{
		CString name,path;
		name = m_ctrlPlayList.GetItemText(0,0);
		singer = m_ctrlPlayList.GetItemText(0,1);
		path = m_ctrlPlayList.GetItemText(0,2);
		song = path+_T("\\")+name;

		//将第一首设置为选中状态
		m_ctrlPlayList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_nCurrentItem = 0;

		UpdateData(false);
		return true;
	}
	return false;
}

//获取上一首信息
bool CPlayListDlg::GetPreviousSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	if(count>=1)
	{
		//设置当前行没有选中
		m_ctrlPlayList.SetItemState(m_nCurrentItem,0, LVIS_SELECTED|LVIS_FOCUSED);

		if (m_nCurrentItem==0) m_nCurrentItem=count-1;
		else m_nCurrentItem--;

		CString name,path;
		name = m_ctrlPlayList.GetItemText(m_nCurrentItem,0);
		singer = m_ctrlPlayList.GetItemText(m_nCurrentItem,1);
		path = m_ctrlPlayList.GetItemText(m_nCurrentItem,2);
		song = path+_T("\\")+name;

		//将第一首设置为选中状态
		m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		return true;
	}

	return false;
}

//获取下一首信息
bool CPlayListDlg::GetNextSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	if(count>=1)
	{
		//CString msg;
		//msg.Format(_T("count:%d  current:%d"),count,m_nCurrentItem);AfxMessageBox(msg);

		//设置当前行没有选中
		m_ctrlPlayList.SetItemState(m_nCurrentItem,0, LVIS_SELECTED|LVIS_FOCUSED);

		m_nCurrentItem++;
		m_nCurrentItem %=count;//当前行

		CString name,path;
		name = m_ctrlPlayList.GetItemText(m_nCurrentItem,0);
		singer = m_ctrlPlayList.GetItemText(m_nCurrentItem,1);
		path = m_ctrlPlayList.GetItemText(m_nCurrentItem,2);
		song = path+_T("\\")+name;

		//将第一首设置为选中状态
		m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		return true;
	}
	return false;
}


//双击事件
void CPlayListDlg::OnNMDblclkPlaylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//获取当前选中的歌曲名称和歌手
	CString song,singer;
	GetCurrentSelectedSong(song,singer);

	//SendMessage(WM_PLAY_NEW_SONG);
	HWND hMainWnd = ::GetParent(*GetParent());  //获取播放窗口的句柄（即父亲的父亲）
	::SendMessage(hMainWnd,WM_PLAY_NEW_SONG,(WPARAM)(&song),(LPARAM)(&singer));
	*pResult = 0;
}


void CPlayListDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

//加载播放列表，没有则创建
bool CPlayListDlg::LoadPlayList()
{
	//IsFileExist
	//HANDLE hFile = CreateFile(_T("playlist.ep"),GENERIC_READ|GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	//CFile* pFile = new CFile(_T("playlist.ep"),CFile::modeCreate);
	//ar<<
	//file.Close();
	//CArchive ar(pFile,CArchive::load); //归档文件

	//CStdioFile  file;
	//file.Open(_T("playlist.ep"),CFile::modeCreate,NULL);

	//file.ReadString(
	//file.Close();
	//

	//CloseHandle(hFile);
	return true;
}

//保存播放列表
bool CPlayListDlg::SavePlayList()
{
	return true;
}