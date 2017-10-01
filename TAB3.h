#pragma once
#include "resource.h"

// CTAB3 對話方塊

class CTAB3 : public CDialog
{
	DECLARE_DYNAMIC(CTAB3)

public:
	CTAB3(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CTAB3();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
