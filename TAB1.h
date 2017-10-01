#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "Module.h"
#include "Process.h"
#include "Tab2.h"
// CTAB1 對話方塊

class CTAB1 : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CTAB1)

public:
	CTAB1(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CTAB1();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	Process* m_selectedProcess;
	bool m_AddCheckBox;
	CImageList m_imgList;
	CListCtrl m_ProcessList;
	CRect m_rect;
	CMenu m_Menu;
	CTAB2* m_tab2;
	CTabCtrl* m_tabCtl;
	afx_msg void OnCustomdrawProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	void sortProcessList(int method);
	void sortModuleList(int method);
	void searchProcess(char* strKeyword);
	void getAllProcess();
	void getAllModules(Process pCur);
	void scanDetourHook();
	void scanHookInfo();
	afx_msg void OnColumnclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedScan();
	CListCtrl m_ModuleList;
	afx_msg void OnClickProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_ProcessName;
	CButton m_SelectDLL;
	afx_msg void OnBnClickedSelectdll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void ReSize(int nID, int cx, int cy, bool changeSize);

	afx_msg void OnColumnclickModulelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawModulelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcesslistTerminate();
	CEdit m_keywordProcess;
	afx_msg void OnChangeSearchname();
	afx_msg void OnBnClickedSethook();
	CComboBox m_ScanOpts;
	afx_msg void OnSelchangeScanopts();
};
