#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "SearchDlg.h"
// CTAB2 對話方塊

class CTAB2 : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CTAB2)

public:
	CTAB2(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CTAB2();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ProcList;
	CodeSegList m_vecProc;
	CodeSeg* m_selectedProc;
	CMenu m_Menu;
	CSearchDlg sDlg;
	HACCEL  m_hAccelTable;
	void FillProcList(CString keyword);
	int findProcListIndexBySelected();
	int findProcByName(CString strKeyword);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickProclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDetourhookFinddetouraddr();
	afx_msg void OnDetourhookDump();
	afx_msg void OnDetourhookCopyaddress();
	afx_msg void OnDetourhookCopyproc();
	afx_msg void OnDetourhookSearchproc();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCustomdrawProclist(NMHDR *pNMHDR, LRESULT *pResult);
};
