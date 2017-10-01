// HookInfoDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "HookInfoDlg.h"
#include "afxdialogex.h"

BEGIN_EASYSIZE_MAP(CHookInfoDlg)
	EASYSIZE(IDC_HookList, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

void CHookInfoDlg::FillHookList(Process* pCur)
{
	//MessageBox("Fill", "ERROR", MB_OK);
	m_hookList.DeleteAllItems();
	
	for (int i = 0; i < pCur->m_vecDifferentCode.Size(); i++ )
	{
		CodeSeg cCur = pCur->m_vecDifferentCode.At(i);
		if (cCur.m_iLength == -1)
		{
			continue;
		} // if

		TCHAR tmp[256];
		wsprintf(tmp, "[0x%X] %s", cCur.m_Module.m_dwAddr, cCur.m_Module.m_strName.c_str());
		int iDex = m_hookList.InsertItem(i, tmp);
		wsprintf(tmp, "[0x%X] %s", cCur.m_Proc.m_dwAddr, cCur.m_Proc.m_strName.c_str());
		m_hookList.SetItemText(iDex, 1, tmp);
		/*if (cCur.m_iLength < 5) // cCur.m_strOriginCode.size() < 22
		{
			MessageBox( "code length Error", "ERROR", MB_OK );
			m_hookList.DeleteItem(i);
			continue;
		} // if*/

		wsprintf(tmp, "[0x%X]-%d bytes %s", cCur.m_dwCodeAddr, cCur.m_iLength, cCur.m_strOriginCode.c_str());
		m_hookList.SetItemText(iDex, 2, tmp);
		m_hookList.SetItemText(iDex, 3, cCur.m_strCode.c_str());
	} // for

	//m_Process->DeleteDifferentCodeList(); // clear code vector
}

// CHookInfoDlg 對話方塊

IMPLEMENT_DYNAMIC(CHookInfoDlg, CDialog)

CHookInfoDlg::CHookInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLGHOOKINFO, pParent)
{
	m_RightClickMenu.LoadMenu(IDR_HookInfoMENU);
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

CHookInfoDlg::~CHookInfoDlg()
{
}

void CHookInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HookList, m_hookList);
}


BEGIN_MESSAGE_MAP(CHookInfoDlg, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_HookList, &CHookInfoDlg::OnRclickHooklist)
	ON_COMMAND(ID_HookInfoRepair, &CHookInfoDlg::OnHookinforepair)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_HOOKINFO_COPY, &CHookInfoDlg::OnHookinfoCopy)
	ON_COMMAND(ID_HOOKINFO_SELECTALL, &CHookInfoDlg::OnHookinfoSelectall)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_HookList, &CHookInfoDlg::OnCustomdrawHooklist)
END_MESSAGE_MAP()


// CHookInfoDlg 訊息處理常式


void CHookInfoDlg::OnRclickHooklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	//MessageBox("Right Click", "", MB_OK);
	
	if (pNMItemActivate->iItem >= 0 && m_Process != NULL)
	{
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = pNMItemActivate->iItem;
		lvi.iSubItem = 2;
		lvi.pszText = new TCHAR[256];
		lvi.cchTextMax = 255;
		m_hookList.GetItem(&lvi);
		string strTmp = lvi.pszText;
		strTmp = strTmp.substr(strTmp.find("[") + 1, strTmp.find("]") - 1);
		int number = (int)strtol(strTmp.c_str(), NULL, 0); // turn hex string to integer e.g. 0x1 -> 1 
		// (CodeSeg cs : m_Process->m_vecDifferentCode)
		int i = 0;
		for ( i = 0; i < m_Process->m_vecDifferentCode.Size(); i++ )
		{
			if (m_Process->m_vecDifferentCode.At(i).m_dwCodeAddr == number)
			{
				m_SelectedCode = &m_Process->m_vecDifferentCode.At(i);
				//MessageBox(m_SelectedCode->m_strFunction.c_str(), m_SelectedCode->m_strDLL.c_str(), MB_OK);
				break;
			} // if
		} // for

		POINT point = { pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y };
		CMenu* pPopup = m_RightClickMenu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		::ClientToScreen(this->m_hWnd, &point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
		//
		//
	} // if
	else
		m_SelectedCode = NULL;
	*pResult = 0;
}



void CHookInfoDlg::OnHookinforepair()
{
	// TODO: 在此加入您的命令處理常式程式碼
	POSITION pos = m_hookList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int iSelected = m_hookList.GetNextSelectedItem(pos);
	do
	{	
		CString strDll = m_hookList.GetItemText(iSelected, 0);
		CString strProc = m_hookList.GetItemText(iSelected, 1);
		CString strCodeAddr = m_hookList.GetItemText(iSelected, 2);
		CString strLength = strCodeAddr.Mid( strCodeAddr.Find("]-")+2);
		strDll = strDll.Mid(strDll.Find("]") + 1);
		strProc = strProc.Mid(strProc.Find("]") + 1);
		strCodeAddr = strCodeAddr.Mid(strCodeAddr.Find("[") + 1);
		strCodeAddr = strCodeAddr.Left(strCodeAddr.Find("]"));
		strLength = strLength.Left( strLength.Find("bytes") );
		strDll.Remove(' ');
		strProc.Remove(' ');
		strCodeAddr.Remove(' ');
		strLength.Remove(' ');
		DWORD intCodeAddr = (int)strtol(strCodeAddr, NULL, 0);
		DWORD intLength = atoi(strLength);
		CT2A charDll(strDll);
		CT2A charProc(strProc);

		if (!m_Process->Repair(charDll, charProc, intCodeAddr, intLength))
			break;
		m_hookList.DeleteItem(iSelected);
		pos = m_hookList.GetFirstSelectedItemPosition();
		iSelected = m_hookList.GetNextSelectedItem(pos);
	} while (iSelected >= 0);// while
	
}



void CHookInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		UPDATE_EASYSIZE;
		m_hookList.SetColumnWidth(0, cx*0.22);
		m_hookList.SetColumnWidth(1, cx*0.23);
		m_hookList.SetColumnWidth(2, cx*0.28);
		m_hookList.SetColumnWidth(3, cx*0.3);
	}// TODO: 在此加入您的訊息處理常式程式碼
}


void CHookInfoDlg::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CRect fRc;
	GetWindowRect(&fRc);
	ofstream fout("HookInfoDlg.ini");
	fout << "[hookDlg.left]= " << fRc.left << endl;
	fout << "[hookDlg.top]= " << fRc.top << endl;
	fout << "[hookDlg.width]= " << fRc.Width() << endl;
	fout << "[hookDlg.height]= " << fRc.Height() << endl;
	fout.close();
	m_Process->DeleteDifferentCodeList();
	m_Process->Clear();
	delete m_Process;
	CDialog::OnClose();
}


void CHookInfoDlg::OnHookinfoCopy()
{
	POSITION pos = m_hookList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int iSelected = m_hookList.GetNextSelectedItem(pos);
	string copyStr = "";
	do
	{
		CString strDll = m_hookList.GetItemText(iSelected, 0);
		CString strProc = m_hookList.GetItemText(iSelected, 1);
		CString strCode_Origin = m_hookList.GetItemText(iSelected, 2);
		CString strCode_New = m_hookList.GetItemText(iSelected, 3);
		TCHAR t[1000];
		wsprintf( t, "%-50s\t%-50s\t%-50s\t%-50s\r\n", strDll, strProc, strCode_Origin, strCode_New );
		copyStr += t;
		iSelected = m_hookList.GetNextSelectedItem(pos);
	} while ( iSelected >= 0);// while

	copyToClipBoard(copyStr);
}


BOOL CHookInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if (m_hAccelTable) {
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg)) {
			return(TRUE);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CHookInfoDlg::OnHookinfoSelectall()
{
	int iCount = m_hookList.GetItemCount();
	for (int i = 0; i < iCount; i++)
	{
		m_hookList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	} // for
} // 


void CHookInfoDlg::OnCustomdrawHooklist(NMHDR *pNMHDR, LRESULT *pResult)
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
