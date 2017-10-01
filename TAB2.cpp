// TAB2.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "TAB2.h"
#include "afxdialogex.h"
#include <iterator>
BEGIN_EASYSIZE_MAP(CTAB2)
	//EASYSIZE(IDC_Scan, ES_KEEPSIZE, ES_BORDER, ES_BORDER, IDC_ModuleList, 0)
	EASYSIZE(IDC_PROCLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	//EASYSIZE(IDC_ProcessList, ES_BORDER, IDC_Refresh, IDC_ModuleList, ES_BORDER, 0)
	//EASYSIZE(IDC_ProcessName, IDC_ProcessList, IDC_Scan, ES_BORDER, IDC_ModuleList, 0)
	//EASYSIZE(IDC_Refresh, ES_KEEPSIZE, IDC_ProcessList, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDD_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	//EASYSIZE(IDD_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
/*=======================================================*/

// CTAB2 對話方塊

IMPLEMENT_DYNAMIC(CTAB2, CDialog)

CTAB2::CTAB2(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB2, pParent)
{
	m_Menu.LoadMenuA(IDR_HookInfoMENU);
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

CTAB2::~CTAB2()
{
}

void CTAB2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCLIST, m_ProcList);
}


BEGIN_MESSAGE_MAP(CTAB2, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_PROCLIST, &CTAB2::OnRclickProclist)
	ON_COMMAND(ID_DETOURHOOK_FINDDETOURADDR, &CTAB2::OnDetourhookFinddetouraddr)
	ON_COMMAND(ID_DETOURHOOK_DUMP, &CTAB2::OnDetourhookDump)
	ON_COMMAND(ID_DETOURHOOK_COPYADDRESS, &CTAB2::OnDetourhookCopyaddress)
	ON_COMMAND(ID_DETOURHOOK_COPYPROC, &CTAB2::OnDetourhookCopyproc)
	ON_COMMAND(ID_DETOURHOOK_SEARCHPROC, &CTAB2::OnDetourhookSearchproc)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROCLIST, &CTAB2::OnCustomdrawProclist)
END_MESSAGE_MAP()


// CTAB2 訊息處理常式


void CTAB2::FillProcList(CString keyword)
{
	m_ProcList.DeleteAllItems();
	bool useKeyword = keyword.GetLength() > 0 ? true : false;
	for (int i = 0; i < m_vecProc.Size(); i++)
	{
		CodeSeg p = m_vecProc.At(i);
		TCHAR tmp[256];
		//wsprintf(tmp, "%s [0x%X]", p.m_Module.m_strName.c_str(), p.m_Module.m_dwAddr);
		if (useKeyword && p.m_Proc.m_strName.find(keyword) == string::npos)
			continue;
		int iDex = m_ProcList.InsertItem(i, p.m_Module.m_strName.c_str());
		m_ProcList.SetItemText(iDex, 1, p.m_Proc.m_strName.c_str());
		wsprintf(tmp, "0x%X", p.m_Proc.m_dwAddr);
		m_ProcList.SetItemText(iDex, 2, tmp);
		//wsprintf(tmp, "0x%X", p.m_Proc.m_dwSize);
		//m_ProcList.SetItemText(iDex, 3, tmp);
	} // while
}

int CTAB2::findProcListIndexBySelected()
{
	POSITION pos = m_ProcList.GetFirstSelectedItemPosition();
	if (pos == NULL) return -1;
	int iSelected = m_ProcList.GetNextSelectedItem(pos);
	if (iSelected >= 0)
	{
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = iSelected;
		lvi.iSubItem = 1;
		lvi.pszText = new TCHAR[256];
		lvi.cchTextMax = 255;
		m_ProcList.GetItem(&lvi);
		string str = lvi.pszText;
		delete[] lvi.pszText;
		int i = 0;
		for (i = 0; i < m_vecProc.Size(); i++)
		{
			if (m_vecProc.At(i).m_Proc.m_strName.compare(str) == 0)
			{
				m_selectedProc = &(m_vecProc.At(i));
				return i;
			} // if
		} // for
	} // if

	return -1;
}

int CTAB2::findProcByName(CString strKeyword)
{
	for (int i = 0; i < m_vecProc.Size(); i++)
	{
		if (m_vecProc.At(i).m_Proc.m_strName.find(strKeyword) != string::npos)
		{
			m_selectedProc = &(m_vecProc.At(i));
			return i;
		} // if
	} // for

	return 0;
}

void CTAB2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		UPDATE_EASYSIZE;
		m_ProcList.SetColumnWidth(0, cx * 0.3);
		m_ProcList.SetColumnWidth(1, cx * 0.4);
		m_ProcList.SetColumnWidth(2, cx * 0.3);
	} // if
}


void CTAB2::OnRclickProclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		

		POINT point = { pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y };
		CMenu* pPopup = m_Menu.GetSubMenu(2);
		ASSERT(pPopup != NULL);
		::ClientToScreen(this->m_hWnd, &point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
		//
		//
	} // if
	else
		m_selectedProc = NULL;
	*pResult = 0;
}


void CTAB2::OnDetourhookFinddetouraddr()
{
	int i = findProcListIndexBySelected();
	if ( i != -1 )
	{
		vector<HookInfo> vecStrResult;
		m_vecProc.At(i).m_Proc.FindDetourAddress(m_vecProc.At(i).m_Module.m_strName, &vecStrResult);
		if (vecStrResult.size() > 0)
		{
			DetourInfo dDlg;
			copy(vecStrResult.begin(), vecStrResult.end(), back_inserter(dDlg.m_vecHookInfo));
			vector<HookInfo>().swap(vecStrResult);
			dDlg.DoModal();
		} // if
	} // if
}


void CTAB2::OnDetourhookDump()
{
	int i = findProcListIndexBySelected();
	if (i != -1)
	{
		vector<HookInfo> vecDump; // 1.detour addr 2.return addr 3.detour length 4.dump bytes 5.dump text
		m_vecProc.At(i).m_Proc.DumpFunction(m_vecProc.At(i).m_Module.m_strName, &vecDump);
		if (vecDump.size() > 0)
		{
			CDumpInfoDlg dDlg;
			copy(vecDump.begin(), vecDump.end(), back_inserter(dDlg.m_vecDumpInfo));
			vector<HookInfo>().swap(vecDump);
			dDlg.DoModal();
		} // if
	} // if
}


void CTAB2::OnDetourhookCopyaddress()
{
	int i = findProcListIndexBySelected();
	if (i != -1)
	{
		TCHAR t[100];
		wsprintf( t, "0x%x",m_vecProc.At(i).m_Proc.m_dwAddr );
		string s = t;
		copyToClipBoard(s);
	} // if
}


void CTAB2::OnDetourhookCopyproc()
{
	int i = findProcListIndexBySelected();
	if (i != -1)
	{
		copyToClipBoard(m_vecProc.At(i).m_Proc.m_strName);
	} // if
}


void CTAB2::OnDetourhookSearchproc()
{
	/*sDlg.Create(IDD_DLGFIND, this);
	sDlg.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	sDlg.m_tab2 = this;*/

	//MessageBox("", "", MB_OK);
	sDlg.DoModal();
	if ((sDlg.m_strKeyword.GetLength() == 0 && sDlg.m_isResume) || sDlg.m_strKeyword.GetLength() > 0)
		FillProcList(sDlg.m_strKeyword);
	
}


BOOL CTAB2::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if (m_hAccelTable) {
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) {
			return(TRUE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CTAB2::OnCustomdrawProclist(NMHDR *pNMHDR, LRESULT *pResult)
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
		// 2591403 13085601
		if ((CDIS_SELECTED == (pNMCD->nmcd.uItemState & CDIS_SELECTED)))
		{
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
			pNMCD->clrText = 13085601;
			pNMCD->clrTextBk = 273033;
		}
		else
		{
			pNMCD->clrText = 13085601;
			pNMCD->clrTextBk = RGB(39, 48, 51);
			pNMCD->nmcd.uItemState = CDIS_DEFAULT;
		}

	}
	break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}
