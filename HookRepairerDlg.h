
// HookRepairerDlg.h : 標頭檔
//

#pragma once
#include "afxcmn.h"
#include "TAB1.h"
#include "TAB2.h"
// CHookRepairerDlg 對話方塊
class CHookRepairerDlg : public CDialogEx
{
	DECLARE_EASYSIZE
// 建構
public:
	CHookRepairerDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOOKREPAIRER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTAB1 m_tab1;
	CTAB2 m_tab2;
	CTAB3 m_tab3;
	CRect m_rect;
	CMenu m_Menu;
	CTabCtrl m_tabCtl;
	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnUpdateIddHookrepairerDialog(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
