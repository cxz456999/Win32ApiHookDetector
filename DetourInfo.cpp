// DetourInfo.cpp : 實作檔
//

#include "stdafx.h"
#include "HookRepairer.h"
#include "DetourInfo.h"
#include "afxdialogex.h"


// DetourInfo 對話方塊

IMPLEMENT_DYNAMIC(DetourInfo, CDialog)

DetourInfo::DetourInfo(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLGDETOURINFO, pParent)
{
}


DetourInfo::~DetourInfo()
{
}

void DetourInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DETOUR_ADDR, m_Address);
	DDX_Control(pDX, IDC_DETOUR_RETADDR, m_RetAddress);
	DDX_Control(pDX, IDC_DETOUR_LENGTH, m_DetourLength);
	DDX_Control(pDX, IDC_DETOUR_BYTE, m_Bytes);
	DDX_Control(pDX, IDC_DETOUR_CODE, m_Code);
	DDX_Control(pDX, IDC_DETOUR_PROCNAME, m_ProcName);
	DDX_Control(pDX, IDC_DETOUR_INDEX, m_cIndex);
}


BEGIN_MESSAGE_MAP(DetourInfo, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DETOUR_NEXT, &DetourInfo::OnBnClickedDetourNext)
	ON_BN_CLICKED(IDC_DETOUR_BACK, &DetourInfo::OnBnClickedDetourBack)
END_MESSAGE_MAP()


// DetourInfo 訊息處理常式


BOOL DetourInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	m_Index = 0;
	CString title;
	title.Format("Index: %d / %d", m_Index+1, m_vecHookInfo.size());
	m_cIndex.SetWindowTextA(title);
	m_ProcName.SetWindowTextA(m_vecHookInfo[0].m_strProcName);
	m_Address.SetWindowTextA(m_vecHookInfo[0].m_strAddr);
	m_RetAddress.SetWindowTextA(m_vecHookInfo[0].m_strRetAddr);
	m_DetourLength.SetWindowTextA(m_vecHookInfo[0].m_strDLength);
	m_Bytes.SetWindowTextA(m_vecHookInfo[0].m_strBytes);
	m_Code.SetWindowTextA(m_vecHookInfo[0].m_strCode);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void DetourInfo::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	vector<HookInfo>().swap(m_vecHookInfo);
	CDialog::OnClose();
}


void DetourInfo::OnBnClickedDetourNext()
{
	
	if (m_Index == m_vecHookInfo.size() - 1)
		return;
	m_Index++;
	CString title;
	title.Format("Index: %d / %d", m_Index+1, m_vecHookInfo.size());
	m_cIndex.SetWindowTextA(title);
	m_ProcName.SetWindowTextA(m_vecHookInfo[m_Index].m_strProcName);
	m_Address.SetWindowTextA(m_vecHookInfo[m_Index].m_strAddr);
	m_RetAddress.SetWindowTextA(m_vecHookInfo[m_Index].m_strRetAddr);
	m_DetourLength.SetWindowTextA(m_vecHookInfo[m_Index].m_strDLength);
	m_Bytes.SetWindowTextA(m_vecHookInfo[m_Index].m_strBytes);
	m_Code.SetWindowTextA(m_vecHookInfo[m_Index].m_strCode);
}


void DetourInfo::OnBnClickedDetourBack()
{
	
	if (m_Index == 0)
		return;
	m_Index--;
	CString title;
	title.Format("Index: %d / %d", m_Index + 1, m_vecHookInfo.size());
	m_cIndex.SetWindowTextA(title);
	m_ProcName.SetWindowTextA(m_vecHookInfo[m_Index].m_strProcName);
	m_Address.SetWindowTextA(m_vecHookInfo[m_Index].m_strAddr);
	m_RetAddress.SetWindowTextA(m_vecHookInfo[m_Index].m_strRetAddr);
	m_DetourLength.SetWindowTextA(m_vecHookInfo[m_Index].m_strDLength);
	m_Bytes.SetWindowTextA(m_vecHookInfo[m_Index].m_strBytes);
	m_Code.SetWindowTextA(m_vecHookInfo[m_Index].m_strCode);
}
