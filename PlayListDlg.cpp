// PlayListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnjoyPlay.h"
#include "PlayListDlg.h"
#include "afxdialogex.h"

#include <vector>

using namespace std;

typedef struct {
	CString name;   //������
	CString singer; //����
	CString path;	//Ŀ¼
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

	
	m_ctrlPlayList.DeleteAllItems();//���

	m_ctrlPlayList.ModifyStyle(0L,LVS_REPORT); 
	//m_ctrlPlayList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EDITLABELS|LVS_EX_ONECLICKACTIVATE|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
	m_ctrlPlayList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_GRIDLINES);

	this->m_ctrlPlayList.InsertColumn(0,_T("����"),LVCFMT_LEFT,80);
	this->m_ctrlPlayList.InsertColumn(1,_T("����"),LVCFMT_LEFT,60);
	this->m_ctrlPlayList.InsertColumn(2,_T("Ŀ¼"),LVCFMT_LEFT,80);

	
	//���ز����б�
	LoadPlayList();

	//this->m_ctrlPlayList.InsertItem(0,_T("123.mp3"));   //ÿһ�еĵ�һ������Ҫ��InsertItem
	//this->m_ctrlPlayList.SetItemText(0,1,_T("JarvisChu"));
	//this->m_ctrlPlayList.SetItemText(0,2,_T("C:\\"));
	//m_ctrlPlayList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	//����buttonͼ��
	m_btnIconAddSong.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_OPEN_GREEN)));//IDI_ICON_ADD_MUSIC
	m_btnIconRemoveSong.SetIcon(LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_ICON_DEL_MUSIC)));

	//��ʼ������
	m_nCurrentItem = 0; //��ǰѡ�е���

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//��Ӹ���
void CPlayListDlg::OnBnClickedBtnAddSong()
{
	//�ļ��򿪶Ի���
	CFileDialog dlg(true,//TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
					nullptr,//ȱʡ����չ�� 
					nullptr,//��ʾ���ļ����༭����ļ��� ������Ϊnullptr
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,//�Ի����� (�ɶ�ѡ)
					(LPCTSTR)_TEXT("Mp3 Files (*.mp3)|*.mp3|Wave Filse (*.wav)|*.wav|All Files (*.*)|*.*||"),//��չ��
					nullptr//������
					);

	//���û�����������ڱ���õ��Ķ���ļ�ȫ·����Ϣ
	const DWORD maxBufSize = 512;
	TCHAR buf[maxBufSize];			//������������ļ�����Ļ�����

	dlg.m_ofn.nMaxFile = maxBufSize;
	dlg.m_ofn.lpstrFile = buf;
	dlg.m_ofn.lpstrFile[0] = NULL;

//  ��CFileDialog�ಢ����OFN_ALLOWMULTISELECT��־ʱ��
//	OPENFILENAME�ṹ���lpstrFile��Ա��һ��ָ���û��������Ļ�������
//	���������ѡ��·�����ļ����б�
//	����б��ÿһ����һ��NULL��������ĩ������NULL������
//	nMaxFile��Աָ���˻������Ĵ�С��
//	�����ѡ����ļ����ļ��������ܳ��ȳ����˴�ֵ����DoModal��������IDCANCEL��
//	����û���ͼ�򿪳�����������С���ļ����Ļ���CommDlgExtendedError()������FNERR_BUFFERTOOSMALL��
//	��ʱ��lpstrFile��ǰ����byte�������ʵ����Ҫ�Ļ��������ֽ����� 

	if(dlg.DoModal()==IDOK)
	{
		POSITION pos = dlg.GetStartPosition(); //�����ʼλ��

		int start  = this->m_ctrlPlayList.GetItemCount();	//�����б������еĸ�����
		//CString str;str.Format(_T("%d"),start);AfxMessageBox(str);

		while (pos != NULL)
		{
			CString fullPath = dlg.GetNextPathName(pos); //��û������е�һ���ļ�  "C:\123.mp3"

			//�ָ���ļ�����·��  
			CString name = fullPath.Mid(fullPath.ReverseFind('\\')+1);  //�ļ���
			CString path = fullPath.Mid(0,fullPath.ReverseFind('\\')); //·��
			
			//���浽�б���
			//this->m_ctrlPlayList.SetItemText(start,0,name);
			this->m_ctrlPlayList.InsertItem(start,name);
			this->m_ctrlPlayList.SetItemText(start,1,_T("δ֪����")); //TODO: ���һ���ܻ�ȡ���ֵ� ����
			this->m_ctrlPlayList.SetItemText(start++,2,path);
		}
    }
	UpdateData(false);
}

//��ȡ��ǰѡ�еĵ�һ�׸���
bool CPlayListDlg::GetCurrentSelectedSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	for(int i=0;i<count;i++)
	{
		int state = m_ctrlPlayList.GetItemState(i,LVIS_SELECTED); //�Ƿ�ѡ��
		if(state !=0)
		{
			CString name,path;
			name = m_ctrlPlayList.GetItemText(i,0);
			singer = m_ctrlPlayList.GetItemText(i,1);
			path = m_ctrlPlayList.GetItemText(i,2);
			song = path+_T("\\")+name;

			m_ctrlPlayList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_nCurrentItem = i; //���µ�ǰ��

			UpdateData(false);
			return true;
		}
	}
	return false;
}

//��ȡ�б�ĵ�һ��
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

		//����һ������Ϊѡ��״̬
		m_ctrlPlayList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_nCurrentItem = 0;

		UpdateData(false);
		return true;
	}
	return false;
}

//��ȡ��һ����Ϣ
bool CPlayListDlg::GetPreviousSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	if(count>=1)
	{
		//���õ�ǰ��û��ѡ��
		m_ctrlPlayList.SetItemState(m_nCurrentItem,0, LVIS_SELECTED|LVIS_FOCUSED);

		if (m_nCurrentItem==0) m_nCurrentItem=count-1;
		else m_nCurrentItem--;

		CString name,path;
		name = m_ctrlPlayList.GetItemText(m_nCurrentItem,0);
		singer = m_ctrlPlayList.GetItemText(m_nCurrentItem,1);
		path = m_ctrlPlayList.GetItemText(m_nCurrentItem,2);
		song = path+_T("\\")+name;

		//����һ������Ϊѡ��״̬
		m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		return true;
	}

	return false;
}

//��ȡ��һ����Ϣ
bool CPlayListDlg::GetNextSong(CString& song,CString& singer)
{
	int count = m_ctrlPlayList.GetItemCount();
	if(count>=1)
	{
		//CString msg;
		//msg.Format(_T("count:%d  current:%d"),count,m_nCurrentItem);AfxMessageBox(msg);

		//���õ�ǰ��û��ѡ��
		m_ctrlPlayList.SetItemState(m_nCurrentItem,0, LVIS_SELECTED|LVIS_FOCUSED);

		m_nCurrentItem++;
		m_nCurrentItem %=count;//��ǰ��

		CString name,path;
		name = m_ctrlPlayList.GetItemText(m_nCurrentItem,0);
		singer = m_ctrlPlayList.GetItemText(m_nCurrentItem,1);
		path = m_ctrlPlayList.GetItemText(m_nCurrentItem,2);
		song = path+_T("\\")+name;

		//����һ������Ϊѡ��״̬
		m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		return true;
	}
	return false;
}


//˫���¼�
void CPlayListDlg::OnNMDblclkPlaylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//��ȡ��ǰѡ�еĸ������ƺ͸���
	CString song,singer;
	GetCurrentSelectedSong(song,singer);

	//SendMessage(WM_PLAY_NEW_SONG);
	HWND hMainWnd = ::GetParent(*GetParent());  //��ȡ���Ŵ��ڵľ���������׵ĸ��ף�
	::SendMessage(hMainWnd,WM_PLAY_NEW_SONG,(WPARAM)(&song),(LPARAM)(&singer));
	*pResult = 0;
}


void CPlayListDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//m_ctrlPlayList.SetItemState(m_nCurrentItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

//���ز����б�û���򴴽�
bool CPlayListDlg::LoadPlayList()
{
	//IsFileExist
	//HANDLE hFile = CreateFile(_T("playlist.ep"),GENERIC_READ|GENERIC_WRITE,NULL,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	//CFile* pFile = new CFile(_T("playlist.ep"),CFile::modeCreate);
	//ar<<
	//file.Close();
	//CArchive ar(pFile,CArchive::load); //�鵵�ļ�

	//CStdioFile  file;
	//file.Open(_T("playlist.ep"),CFile::modeCreate,NULL);

	//file.ReadString(
	//file.Close();
	//

	//CloseHandle(hFile);
	return true;
}

//���沥���б�
bool CPlayListDlg::SavePlayList()
{
	return true;
}