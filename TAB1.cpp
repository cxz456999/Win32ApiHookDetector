// TAB1.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "afxdialogex.h"

extern vector<Process> vec_ProcessList;

CHookInfoDlg* newDlg;
int int_SortSubItem = 0;

/*   resize */


BEGIN_EASYSIZE_MAP(CTAB1)
	//EASYSIZE(IDC_Scan, ES_KEEPSIZE, ES_BORDER, ES_BORDER, IDC_ModuleList, 0)
	EASYSIZE(IDC_ModuleList, IDC_ProcessList, IDC_ProcessName, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_ProcessList, ES_BORDER, IDC_Refresh, IDC_ModuleList, ES_BORDER, 0)
	EASYSIZE(IDC_ProcessName, IDC_ProcessList, IDC_Scan, ES_BORDER, IDC_ModuleList, 0)
	//EASYSIZE(IDC_Refresh, ES_KEEPSIZE, IDC_ProcessList, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDD_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	//EASYSIZE(IDD_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
/*=======================================================*/





bool iequals(const string& a, const string& b)
{
	unsigned int sz = a.size();
	if (b.size() != sz)
		return false;
	for (unsigned int i = 0; i < sz; ++i)
		if (tolower(a[i]) != tolower(b[i]))
			return false;
	return true;
}

void CTAB1::getAllModules(Process pCur)
{
	m_ModuleList.DeleteAllItems();
	pCur.GetAllModule();
	for (int i = 0; i < pCur.m_vecModule.Size(); i++)
	{
		Module mCur = pCur.m_vecModule.At(i);
		int iDex = m_ModuleList.InsertItem(i, mCur.m_strName.c_str());
		TCHAR tmp[256];
		wsprintf(tmp, "0x%X", mCur.m_dwAddr);
		m_ModuleList.SetItemText(iDex, 1, tmp);
		m_ModuleList.SetItemText(iDex, 2, mCur.m_strPath.c_str());
		if ( m_AddCheckBox && m_SelectDLL.GetCheck() && iequals(mCur.m_strName.substr(mCur.m_strName.size() - 3), "dll"))
			m_ModuleList.SetCheck(i);
	} // for
}

void CTAB1::getAllProcess()
{
	vector<Process>().swap(vec_ProcessList); // clear vector
	EnumAllProcesses(vec_ProcessList);
	m_imgList.Create(16, 16, ILC_COLOR32, vec_ProcessList.size(), 0);
	BOOL is64BitOs = Is64BitWindows();
	int i = 0;
	for (Process p : vec_ProcessList)
	{

		if (p.m_hIcon != NULL) {
			m_imgList.Add(p.m_hIcon);
		}
		else
		{
			m_imgList.Add(AfxGetApp()->LoadIconA(IDI_ICON1));
		}


		TCHAR pid[256];
		wsprintf(pid, "%d", p.m_dwPid);
		int iDex = m_ProcessList.InsertItem(i, pid, i);
		if (!p.isAllow)
			m_ProcessList.SetItemText(iDex, 1, ("[X] " + p.m_strName).c_str());
		else
			m_ProcessList.SetItemText(iDex, 1, p.m_strName.c_str());
		i++;
	} // for


	m_ProcessList.SetImageList(&m_imgList, LVSIL_SMALL);
	sortProcessList(0);// sort by pid
	
}

void CTAB1::searchProcess(char* strKeyword)
{

	/*int iCount = m_ProcessList.GetItemCount();
	for (int i = 0; i < iCount; i++)
	{
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = i;
		lvi.iSubItem = 1;
		lvi.pszText = new TCHAR[256];
		lvi.cchTextMax = 255;
		m_ProcessList.GetItem(&lvi);
		string strName = lvi.pszText;
		delete[] lvi.pszText;
		if (strName.find(strKeyword) == string::npos)
			continue;


	} // for*/
	m_ProcessList.DeleteAllItems();
	m_imgList.DeleteImageList();
	m_imgList.Create(16, 16, ILC_COLOR32, vec_ProcessList.size(), 0);
	BOOL is64BitOs = Is64BitWindows();
	int i = 0;
	for (Process p : vec_ProcessList)
	{

		if (p.m_strName.find(strKeyword) == string::npos)
			continue;

		if (p.m_hIcon != NULL) {
			m_imgList.Add(p.m_hIcon);
		}
		else
		{
			m_imgList.Add(AfxGetApp()->LoadIconA(IDI_ICON1));
		}


		TCHAR pid[256];
		wsprintf(pid, "%d", p.m_dwPid);
		int iDex = m_ProcessList.InsertItem(i, pid, i);
		if (!p.isAllow)
			m_ProcessList.SetItemText(iDex, 1, ("[X] " + p.m_strName).c_str());
		else
			m_ProcessList.SetItemText(iDex, 1, p.m_strName.c_str());
		i++;
	} // for


	m_ProcessList.SetImageList(&m_imgList, LVSIL_SMALL);
	sortProcessList(0);// sort by pid
}

static int CALLBACK MySortProcByInt(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl  *pList = (CListCtrl *)lParamSort;
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString   strItem1 = pListCtrl->GetItemText(lParam1, int_SortSubItem); // 0 為第1個 row -> pid,  1 -> name
	CString   strItem2 = pListCtrl->GetItemText(lParam2, int_SortSubItem);
	int i_Item1 = _ttoi(strItem1);
	int i_Item2 = _ttoi(strItem2);
	return i_Item1 - i_Item2;
}

static int CALLBACK MySortProcByStr(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl  *pList = (CListCtrl *)lParamSort;
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString   strItem1 = pListCtrl->GetItemText(lParam1, int_SortSubItem); // 0 為第1個 row -> pid,  1 -> name
	CString   strItem2 = pListCtrl->GetItemText(lParam2, int_SortSubItem);
	return strcmp(strItem1, strItem2);
}

void CTAB1::sortProcessList( int method )
{
	int i_count = m_ProcessList.GetItemCount();
	for (int i = 0; i < i_count; i++)
	{
		m_ProcessList.SetItemData(i, i);
	}
	
	if (method == 0)
	{
		int_SortSubItem = 0;
		m_ProcessList.SortItems(MySortProcByInt, (DWORD_PTR)&m_ProcessList);
	} // if
	else
	{
		int_SortSubItem = 1;
		m_ProcessList.SortItems(MySortProcByStr, (DWORD_PTR)&m_ProcessList);
	} // else
}


void CTAB1::sortModuleList(int method)
{
	int i_count = m_ModuleList.GetItemCount();
	for (int i = 0; i < i_count; i++)
	{
		m_ModuleList.SetItemData(i, i);
	}

	if (method == 0) // sort by address
	{
		int_SortSubItem = 0;
		m_ModuleList.SortItems(MySortProcByStr, (DWORD_PTR)&m_ModuleList);
	} // if
	else if (method == 1)
	{
		int_SortSubItem = 1;
		m_ModuleList.SortItems(MySortProcByStr, (DWORD_PTR)&m_ModuleList);
	} // else if
	else // sort by module anme and module path
	{
		int_SortSubItem = 2;
		m_ModuleList.SortItems(MySortProcByStr, (DWORD_PTR)&m_ModuleList);
	} // else
}
// CTAB1 對話方塊

IMPLEMENT_DYNAMIC(CTAB1, CDialog)

CTAB1::CTAB1(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB1, pParent)
{
	m_AddCheckBox = true;
	m_selectedProcess = NULL;
	m_Menu.LoadMenuA(IDR_HookInfoMENU);
}

CTAB1::~CTAB1()
{
	delete newDlg;
}

void CTAB1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ProcessList, m_ProcessList);
	DDX_Control(pDX, IDC_ModuleList, m_ModuleList);
	DDX_Control(pDX, IDC_ProcessName, m_ProcessName);
	DDX_Control(pDX, IDC_SelectDLL, m_SelectDLL);
	DDX_Control(pDX, IDC_SEARCHNAME, m_keywordProcess);
	DDX_Control(pDX, IDC_SCANOPTS, m_ScanOpts);
}


BEGIN_MESSAGE_MAP(CTAB1, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ProcessList, &CTAB1::OnCustomdrawProcesslist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ProcessList, &CTAB1::OnColumnclickProcesslist)
	ON_BN_CLICKED(IDC_Refresh, &CTAB1::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_Scan, &CTAB1::OnBnClickedScan)
	ON_NOTIFY(NM_CLICK, IDC_ProcessList, &CTAB1::OnClickProcesslist)
	ON_BN_CLICKED(IDC_SelectDLL, &CTAB1::OnBnClickedSelectdll)
	ON_WM_SIZE()
//	ON_WM_CTLCOLOR()
ON_NOTIFY(LVN_COLUMNCLICK, IDC_ModuleList, &CTAB1::OnColumnclickModulelist)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_ModuleList, &CTAB1::OnCustomdrawModulelist)
ON_NOTIFY(NM_RCLICK, IDC_ProcessList, &CTAB1::OnRclickProcesslist)
ON_COMMAND(ID_PROCESSLIST_TERMINATE, &CTAB1::OnProcesslistTerminate)
ON_EN_CHANGE(IDC_SEARCHNAME, &CTAB1::OnChangeSearchname)
ON_CBN_SELCHANGE(IDC_SCANOPTS, &CTAB1::OnSelchangeScanopts)
END_MESSAGE_MAP()


void CTAB1::OnCustomdrawProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	NMLVCUSTOMDRAW* pNMCD = (NMLVCUSTOMDRAW*)(pNMHDR);
	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
	
		if ((CDIS_SELECTED == (pNMCD->nmcd.uItemState & CDIS_SELECTED)))
		{
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = RGB(255, 255, 255);
			pNMCD->clrTextBk = RGB(100, 50, 50);
		}
		else
		{
			COLORREF crText = RGB(0, 0, 0);
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = pNMCD->nmcd.dwItemSpec;
			lvi.iSubItem = 1;
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			m_ProcessList.GetItem(&lvi);
			string strTmp = lvi.pszText;
			if (strTmp.find("[X]") == 0)
				pNMCD->clrTextBk = RGB(40, 80, 40);
			else 
				pNMCD->clrTextBk = RGB(70, 140, 70); 
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = RGB(255, 255, 255);
			delete[] lvi.pszText;
		}

	}
	break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}


void CTAB1::OnColumnclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	sortProcessList(pNMLV->iSubItem);
	*pResult = 0;
}


void CTAB1::OnBnClickedRefresh()
{
	for (Process p : vec_ProcessList)
		p.Clear();
	vector<Process>().swap(vec_ProcessList); // clear vector*/
	m_ProcessName.SetWindowTextA("");
	m_imgList.DeleteImageList();
	m_ProcessList.DeleteAllItems();
	m_ModuleList.DeleteAllItems();
	getAllProcess();
}

static Process* mT_SelectedProcess;
static CListCtrl* mT_ModuleList;
static CTAB1* mT_This;
DWORD WINAPI StartScan(LPVOID lpParamter)
{
	int iCount = mT_ModuleList->GetItemCount();
	for (int i = 0; i < iCount; i++)
	{
		if (mT_ModuleList->GetCheck(i))
		{
			LVITEM lvi, lvi2;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			mT_ModuleList->GetItem(&lvi);
			CString loadName = "Loading... ";
			loadName += lvi.pszText;
			SetWindowTextA(GetDlgItem(mT_This->m_hWnd, IDC_LOADNAME), loadName);
			mT_SelectedProcess->ScanModuleHook(lvi.pszText);
			delete[] lvi.pszText;
			//MessageBox("End scan", "ERROR", MB_OK);
		}

	} // for

	newDlg->FillHookList(mT_SelectedProcess);
	TCHAR tmp[50];//;
	ifstream fin("HookInfoDlg.ini");
	if (!fin)
		newDlg->ShowWindow(SW_SHOW);
	else
	{
		RECT fRc;
		fin >> tmp >> fRc.left;
		fin >> tmp >> fRc.top;
		fin >> tmp >> fRc.right;
		fin >> tmp >> fRc.bottom;
		fin.close();
		newDlg->SetWindowPos(NULL, fRc.left, fRc.top, fRc.right, fRc.bottom, SWP_SHOWWINDOW);
	} // else

	EnableWindow(GetDlgItem(mT_This->m_hWnd, IDC_Refresh), true);
	EnableWindow(GetDlgItem(mT_This->m_hWnd, IDC_Scan), true); 
	EnableWindow(GetDlgItem(mT_This->m_hWnd, IDC_ProcessList), true);
	EnableWindow(GetDlgItem(mT_This->m_hWnd, IDC_SelectDLL), true);
	EnableWindow(GetDlgItem(mT_This->m_hWnd, IDC_SCANOPTS), true);
	SetWindowTextA(GetDlgItem(mT_This->m_hWnd, IDC_LOADNAME), "");
	return 1;
}

void CTAB1::scanDetourHook()
{
	m_selectedProcess->Clear();
	mT_SelectedProcess = m_selectedProcess;
	mT_ModuleList = &m_ModuleList;
	mT_This = this;
	m_ProcessList.EnableWindow(false);
	GetDlgItem(IDC_Refresh)->EnableWindow(false);
	GetDlgItem(IDC_Scan)->EnableWindow(false);
	m_SelectDLL.EnableWindow(false);
	m_ScanOpts.EnableWindow(false);
	CreateThread(NULL, 0, StartScan, NULL, 0, NULL);
	//delete newDlg;
	newDlg = new CHookInfoDlg();
	//newDlg.DoModal();
	newDlg->Create(IDD_DLGHOOKINFO, this);
	//vec_ProcessList.erase(m_selectedProcess);
	newDlg->m_Process = new Process();
	newDlg->m_Process->Copy(mT_SelectedProcess);
	TCHAR tmp[50];//;
	wsprintf(tmp, "[%d] ", mT_SelectedProcess->m_dwPid);
	string strTmp = "Hook Info - ";
	strTmp += (tmp + mT_SelectedProcess->m_strName);
	newDlg->SetWindowTextA(strTmp.c_str());

	newDlg->m_hookList.InsertColumn(0, "DLL", LVCFMT_LEFT, 200);
	newDlg->m_hookList.InsertColumn(1, "Proc", LVCFMT_LEFT, 230);
	newDlg->m_hookList.InsertColumn(2, "Original bytes", LVCFMT_LEFT, 280);
	newDlg->m_hookList.InsertColumn(3, "New bytes", LVCFMT_LEFT, 230);
	//m_ProcessList.SetTextBkColor((COLORREF)RGB(255.0.0));
	newDlg->m_hookList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
}

void CTAB1::scanHookInfo()
{
	POSITION pos = m_ModuleList.GetFirstSelectedItemPosition();
	if (m_selectedProcess == NULL || pos == NULL ) return;
	int index = m_ModuleList.GetNextSelectedItem(pos);
	if (index >= 0)
	{
		m_tab2->m_vecProc.Clear();
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index;
		lvi.iSubItem = 0; // Module name
		lvi.pszText = new TCHAR[256];
		lvi.cchTextMax = 255;
		m_ModuleList.GetItem(&lvi);
		m_selectedProcess->GetModuleProcs(lvi.pszText, &m_tab2->m_vecProc);
		delete[] lvi.pszText;
		//MessageBox("End scan", "ERROR", MB_OK);
	} // if

	m_tab2->FillProcList("");

	/*if (iCount > 1)
	{
		MessageBox("plz choose just one module", "ERROR", MB_OK);
		return;
	} // if

	for (int i = 0; i < iCount; i++)
	{
		if (m_ModuleList.GetCheck(i))
		{
			LVITEM lvi, lvi2;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0; // Module name
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			m_ModuleList.GetItem(&lvi);
			m_selectedProcess->GetModuleProcs(lvi.pszText, &m_tab2->m_vecProc);
			delete[] lvi.pszText;
			//MessageBox("End scan", "ERROR", MB_OK);
		}

	} // for*/

	
}

void CTAB1::OnBnClickedScan()
{
	if (m_selectedProcess == NULL || m_ModuleList.GetItemCount() <= 0) return;
	//m_selectedProcess->ScanModuleHook("ntdll.dll");
	//return;

	if (m_ScanOpts.GetCurSel() == 0)
	{
		scanDetourHook();
	} // if
	else
	{
		scanHookInfo();
		m_tabCtl->SetCurSel(1);
		m_tab2->ShowWindow(SW_SHOW);
		this->ShowWindow(SW_HIDE);
	}
		
	
}

void CTAB1::OnClickProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION pos =  m_ProcessList.GetFirstSelectedItemPosition();
	if ( pos == NULL ) return;
	int iSelected = m_ProcessList.GetNextSelectedItem(pos);
	m_ProcessList.GetItemText(iSelected, 1);
	CString cstrSelectedPid = m_ProcessList.GetItemText(iSelected, 0);
	int i = 0;
	for (i = 0; i < vec_ProcessList.size(); i++)
	{
		if (vec_ProcessList[i].m_dwPid == _ttoi(cstrSelectedPid)) break;
		//else if (vec_ProcessList[i].is32Bit &&  vec_ProcessList[i].m_dwPid == _ttoi((cstrSelectedPid) == 0) break;
	} // for
	//if (GetProcedureAddress(GetModuleHandle("ntdll.dll"), "DbgUiRemoteBreakin") != 0) MessageBox("zero", "", MB_OK);
	TCHAR t[256];
	wsprintf( t, "[ %d ] %s", vec_ProcessList[i].m_dwPid, vec_ProcessList[i].m_strName.c_str());
	m_ProcessName.SetWindowTextA(t);
	getAllModules(vec_ProcessList[i]);
	m_selectedProcess = &vec_ProcessList[i];
	//vec_ProcessList[i].ScanModuleHook("ntdll.dll");
	*pResult = 0;
}
/*
if ( vec_ProcessList.size() <= 0 ) return;
LPINT pItems = new int[vec_ProcessList.size()];
MessageBox(vec_ProcessList[pItems[0]].m_strName.c_str(), "", MB_OK);
*/

void CTAB1::OnBnClickedSelectdll()
{
	if (m_SelectDLL.GetCheck())
	{
		int iCount = m_ModuleList.GetItemCount();
		for (int i = 0; i < iCount; i++)
		{
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			m_ModuleList.GetItem(&lvi);
			string strTmp = lvi.pszText;

			if (iequals(strTmp.substr(strTmp.size() - 3), "dll"))
				m_ModuleList.SetCheck(i);
			delete[] lvi.pszText;
		} // for
	} // if
	else
	{
		int iCount = m_ModuleList.GetItemCount();
		for (int i = 0; i < iCount; i++)
		{
			m_ModuleList.SetCheck(i, false);
		} // for
	} // else
}

void CTAB1::ReSize(int nID, int cx, int cy, bool changeSize)
{
	CWnd *pWnd;
	pWnd = GetDlgItem(nID);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left = (int)(rect.left*((float)cx / (float)m_rect.Width()));
		rect.right = (int)(rect.right*((float)cx / (float)m_rect.Width()));
		if (changeSize)
		{
			rect.top = (int)(rect.top*((float)cy / (float)m_rect.Height()));
			rect.bottom = (int)(rect.bottom*((float)cy / (float)m_rect.Height()));
		} // if

		pWnd->MoveWindow(rect);
	}
}


void CTAB1::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		UPDATE_EASYSIZE;
		m_ModuleList.SetColumnWidth(0, cx/4*3*0.3);
		m_ModuleList.SetColumnWidth(1, cx/4*3*0.2);
		m_ModuleList.SetColumnWidth(2, cx/4*3*0.8);
	} // if

		//CRect rc, wc;
	//GetDlgItem(IDC_Scan)->GetClientRect(&wc);
	//GetClientRect(&rc);
	//GetDlgItem(IDC_Scan)->SetWindowPos(NULL, rc.right-wc.Width(), rc.top , 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	// TODO: 在此加入您的訊息處理常式程式碼
	/*if (nType != SIZE_MINIMIZED)
	{
		//MessageBox("RESIZE", "", MB_OK);
		ReSize(IDC_Refresh, cx, cy, false);
		ReSize(IDC_SelectDLL, cx, cy, false);
		ReSize(IDC_Scan, cx, cy, false);
		ReSize(IDC_ProcessName, cx, cy, true);
		ReSize(IDC_ModuleList, cx, cy, true);
		ReSize(IDC_ProcessList, cx, cy, true);//
		GetClientRect(&m_rect);
	}*/
}


void CTAB1::OnColumnclickModulelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	sortModuleList(pNMLV->iSubItem);
	*pResult = 0;
}


void CTAB1::OnCustomdrawModulelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	NMLVCUSTOMDRAW* pNMCD = (NMLVCUSTOMDRAW*)(pNMHDR);
	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{

		if (m_AddCheckBox && m_ModuleList.GetCheck(pNMCD->nmcd.dwItemSpec)) // been selected
		{
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = RGB(255, 255, 255);
			pNMCD->clrTextBk = RGB(100, 100, 200);
		} // if
		else if (!m_AddCheckBox && (CDIS_SELECTED == (pNMCD->nmcd.uItemState & CDIS_SELECTED)))
		{
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = RGB(255, 255, 255);
			pNMCD->clrTextBk = RGB(100, 100, 200);
		} // else if
		else
		{
			COLORREF crText = RGB(0, 0, 0);
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = pNMCD->nmcd.dwItemSpec;
			lvi.iSubItem = 0;
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			m_ModuleList.GetItem(&lvi);
			string strTmp = lvi.pszText;
			if (iequals(strTmp.substr(strTmp.size() - 3), "dll"))
				pNMCD->clrTextBk = RGB(140, 70, 140); 
			else
				pNMCD->clrTextBk = RGB(80, 40, 80);
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = RGB(255, 255, 255);
			delete[] lvi.pszText;
		}

	}
	break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}


void CTAB1::OnRclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	if (pNMItemActivate->iItem >= 0)
	{
		POINT point = { pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y };
		CMenu* pPopup = m_Menu.GetSubMenu(1);
		ASSERT(pPopup != NULL);
		::ClientToScreen(this->m_hWnd, &point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
	} // if

	*pResult = 0;
}


void CTAB1::OnProcesslistTerminate()
{
	
	if (m_selectedProcess == NULL) return;
	POSITION pos = m_ProcessList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;

	int iSelected = m_ProcessList.GetNextSelectedItem(pos);
	m_ProcessList.DeleteItem(iSelected);
	int i = 0;
	for (auto p : vec_ProcessList)
	{
		if (p.m_dwPid == m_selectedProcess->m_dwPid)
		{
			p.Terminate();
			vec_ProcessList.erase(vec_ProcessList.begin() + i, vec_ProcessList.begin() + (i + 1));
			break;
		} // if

		i++;
	} // for

	m_ModuleList.DeleteAllItems();
	GetDlgItem(IDC_ProcessName)->SetWindowTextA("");
	m_selectedProcess = NULL;
}


void CTAB1::OnChangeSearchname()
{
	TCHAR strKeyword[100];
	m_keywordProcess.GetWindowTextA(strKeyword, 100);
	searchProcess(strKeyword);
}


void CTAB1::OnBnClickedSethook()
{
	int iCount = m_ModuleList.GetItemCount();
	if (m_selectedProcess == NULL || iCount <= 0) return;
	
	for (int i = 0; i < iCount; i++)
	{
		if (m_ModuleList.GetCheck(i))
		{
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0; // Module name
			lvi.pszText = new TCHAR[256];
			lvi.cchTextMax = 255;
			m_ModuleList.GetItem(&lvi);
			m_selectedProcess->GetModuleProcs(lvi.pszText, &m_tab2->m_vecProc);
			delete[] lvi.pszText;
			//MessageBox("End scan", "ERROR", MB_OK);
		}

	} // for

	m_tab2->FillProcList("");
}


void CTAB1::OnSelchangeScanopts()
{
	//m_ModuleList.SetExtendedStyle(dwExStyle | LVS_EX_CHECKBOXES);
	
	if (m_ScanOpts.GetCurSel() == 0)
	{
		m_ModuleList.SetExtendedStyle(m_ModuleList.GetExtendedStyle() | LVS_EX_CHECKBOXES);
		m_ModuleList.ModifyStyle(LVS_SINGLESEL, false);
		m_AddCheckBox = true;
		if (m_selectedProcess == NULL) return;
		getAllModules(*m_selectedProcess);
	}
	else
	{
		m_ModuleList.SetExtendedStyle(m_ProcessList.GetExtendedStyle());
		m_AddCheckBox = false;
		if (m_selectedProcess == NULL) return;
		getAllModules(*m_selectedProcess);
	}
	
}
