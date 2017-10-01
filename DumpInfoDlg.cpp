// DumpInfoDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "DumpInfoDlg.h"
#include "afxdialogex.h"

BEGIN_EASYSIZE_MAP(CDumpInfoDlg)
	EASYSIZE(IDC_TXTDUMP, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
// CDumpInfoDlg 對話方塊

IMPLEMENT_DYNAMIC(CDumpInfoDlg, CDialog)

CDumpInfoDlg::CDumpInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLGDUMPINFO, pParent)
{

}

CDumpInfoDlg::~CDumpInfoDlg()
{
}

void CDumpInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXTDUMP, m_txtDump);
}


BEGIN_MESSAGE_MAP(CDumpInfoDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDumpInfoDlg 訊息處理常式


void CDumpInfoDlg::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	vector<HookInfo>().swap(m_vecDumpInfo);
	CDialog::OnClose();
}


BOOL CDumpInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString result = (m_vecDumpInfo[0].m_strProcName + "\r\n\r\n");
	CString tab = "\t", csAddr, csBytes, csCode;
	csAddr.Format( "%-10s", "Address:" );
	csBytes.Format("%-32s", "Bytes:");
	csCode.Format("%-40s", "Code:");
	result += ("Size:" + tab + csAddr + tab + csBytes + tab + csCode + "\r\n");
	for (auto dp : m_vecDumpInfo)
	{
		result += (dp.m_strDLength + tab + dp.m_strAddr + tab+ dp.m_strBytes + tab + dp.m_strCode + "\r\n");
	} // for

	m_txtDump.SetWindowTextA(result);
	m_txtDump.ShowScrollBar(SB_VERT, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CDumpInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		UPDATE_EASYSIZE;
	} // if
	// TODO: 在此加入您的訊息處理常式程式碼
}
