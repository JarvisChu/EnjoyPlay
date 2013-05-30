// PlayListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnjoyPlay.h"
#include "PlayListDlg.h"
#include "afxdialogex.h"

#include "tinyxml.h"

#include <vector>

using namespace std;

typedef struct {
	CString name;   //������
	CString singer; //����
	CString path;	//Ŀ¼
}SONG;


//char* ת���� CString
void ConvertCharToCString(const char* ch,CString& cs)
{
	//���char�ĳ���
	int len = strlen(ch);
	
	//��ÿ��ֽڵĳ���
	int wlen = MultiByteToWideChar (CP_ACP,0,ch,len,NULL,0);

	//�������ֽ�
	wchar_t* buf = new wchar_t[wlen+1];

	//ת��
	MultiByteToWideChar (CP_ACP,0,ch,len,buf,wlen);

	//���'\0'
	buf[len]='\0';

	//ת����CString
	cs.Append(buf);

	//ɾ��������
	delete []buf;
}


//CString ת����char* ,����char�ĳ���
int ConvertCStringToChar(CString& cs,char* ch)
{
	//�����ת����char*�ĳ���
	int len = WideCharToMultiByte(CP_ACP,0,cs,cs.GetLength(),NULL,0,NULL,NULL);

	//����char*�ռ�
	ch = new char[len+1];

	//ת��
	WideCharToMultiByte(CP_ACP,0,cs,cs.GetLength()+1,ch,len+1,NULL,NULL);

	//��β
	ch[len+1]='/0';

	return (len+1);
}



CString UTF8toUnicode(const char* utf8Str,UINT length)
{
    CString unicodeStr;
    unicodeStr=_T("");
 
    if (!utf8Str)
        return unicodeStr;
 
    if (length==0)
        return unicodeStr;
 
    //ת��
    WCHAR chr=0;
    for (UINT i=0;i<length;)
    {
        if ((0x80&utf8Str[i])==0) // ASCII
        {
            chr=utf8Str[i];
            i++;
        }
        else if((0xE0&utf8Str[i])==0xC0) // 110xxxxx 10xxxxxx
        {
            chr =(utf8Str[i+0]&0x3F)<<6;
            chr|=(utf8Str[i+1]&0x3F);
            i+=2;
        }
        else if((0xF0&utf8Str[i])==0xE0) // 1110xxxx 10xxxxxx 10xxxxxx
        {
            chr =(utf8Str[i+0]&0x1F)<<12;
            chr|=(utf8Str[i+1]&0x3F)<<6;
            chr|=(utf8Str[i+2]&0x3F);
            i+=3;
        }
        /*
        else if() // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {}
        else if() // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx
        {}
        else if() // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx 10xxxxxx 
        {}
        */
        else // ����UTF-8�ַ���
        {
            return unicodeStr;
        }
        unicodeStr.AppendChar(chr);
    }
 
    return unicodeStr;
}
 
CString UTF8toUnicode(const char* utf8Str)
{
    UINT theLength=strlen(utf8Str);
    return UTF8toUnicode(utf8Str,theLength);
}

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
	TiXmlDocument* pDoc = new TiXmlDocument;
	
	if(pDoc == nullptr) return false;

	//xml�ļ�Ϊutf-8��ʽ
	pDoc->LoadFile("config.xml",TIXML_ENCODING_UTF8);//);TIXML_ENCODING_UNKNOWN

	TiXmlElement* root = pDoc->FirstChildElement();
	
	TiXmlElement* config = root->FirstChildElement();
	TiXmlElement* autoplay = config->FirstChildElement();

	//�Ƿ��Զ�����
	const char* ifAutoPlay = autoplay->GetText();
	CString szAutoPlay;
	ConvertCharToCString(ifAutoPlay,szAutoPlay);
	//AfxMessageBox(szAutoPlay);

	//��ȡ�����б�����ŵ������б�ؼ���
	TiXmlElement* playlist = config->NextSiblingElement();

	this->m_ctrlPlayList.DeleteAllItems();//��ղ����б�
	
	//ѭ����������
	int i=0;
	TiXmlElement* pElem=NULL;
	for(pElem = playlist->FirstChildElement();pElem;pElem=pElem->NextSiblingElement())
	{
		CString name,path,singer;

		//������
		TiXmlAttribute* pAttr = pElem->FirstAttribute();
		name = UTF8toUnicode(pAttr->Value());
		//const char* pName = pAttr->Value();
		//AfxMessageBox(name);		
		//ConvertCharToCString(pName,name);

		//����
		pAttr = pAttr->Next();
		singer = UTF8toUnicode(pAttr->Value());
		//const char* pSinger = pAttr->Value(); 
		//ConvertCharToCString(pSinger,singer);

		//·��
		pAttr = pAttr->Next();
		path = UTF8toUnicode(pAttr->Value());
		//const char* pPath = pAttr->Value(); 
		//ConvertCharToCString(pPath,path);

		//AfxMessageBox(name+path+singer);

		
		//���뵽�б���
		this->m_ctrlPlayList.InsertItem(i,name);   //ÿһ�еĵ�һ������Ҫ��InsertItem
		this->m_ctrlPlayList.SetItemText(i,1,singer);
		this->m_ctrlPlayList.SetItemText(i++,2,path);	

		//AfxMessageBox(_T("afsdfasd"));
	}

	//this->m_ctrlPlayList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	UpdateData(false);

	return true;
}

//���沥���б�
bool CPlayListDlg::SavePlayList()
{
	return true;
}