#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "Module.h"
#include "Process.h"

// CHookInfoDlg 對話方塊

class CHookInfoDlg : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CHookInfoDlg)

public:
	CHookInfoDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CHookInfoDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGHOOKINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_RightClickMenu;
	HACCEL  m_hAccelTable;
	Process* m_Process;
	CodeSeg* m_SelectedCode;
	CListCtrl m_hookList;
	void FillHookList( Process* pCur );
	afx_msg void OnRclickHooklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHookinforepair();
//	afx_msg void OnMenu1Menu3();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnHookinfoCopy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHookinfoSelectall();
	afx_msg void OnCustomdrawHooklist(NMHDR *pNMHDR, LRESULT *pResult);
};
