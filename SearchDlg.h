#pragma once
#include "resource.h"
#include "TAB2.h"
#include "afxwin.h"
// CSearchDlg 對話方塊

class CSearchDlg : public CDialog
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSearchDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGFIND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ProcName;
	CString m_strKeyword;
	bool m_isResume;
	//afx_msg void OnBnClickedFindNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSearchResume();
};
