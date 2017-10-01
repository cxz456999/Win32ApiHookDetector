// SearchDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "SearchDlg.h"
#include "afxdialogex.h"

// CSearchDlg 對話方塊

IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLGFIND, pParent)
{
	m_isResume = false;
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_SEARCH, m_ProcName);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	//ON_BN_CLICKED(IDC_FIND_NEXT, &CSearchDlg::OnBnClickedFindNext)
	ON_BN_CLICKED(IDOK, &CSearchDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEARCH_RESUME, &CSearchDlg::OnBnClickedSearchResume)
END_MESSAGE_MAP()


// CSearchDlg 訊息處理常式

/*void CSearchDlg::OnBnClickedFindNext()
{
//	m_tab2->findProcByName();
}*/


void CSearchDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_isResume = false;
	m_ProcName.GetWindowTextA(m_strKeyword);
	CDialog::OnOK();
}

void CSearchDlg::OnBnClickedSearchResume()
{
	m_strKeyword = "";
	m_isResume = true;
	CDialog::OnOK();
}
