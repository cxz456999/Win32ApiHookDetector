
// HookRepairerDlg.cpp : 實作檔

#include "stdafx.h"
#include "HookRepairer.h"
#include "HookRepairerDlg.h"
#include "afxdialogex.h"
#include "resource.h"
vector<Process> vec_ProcessList;

/*   resize */


BEGIN_EASYSIZE_MAP(CHookRepairerDlg)
	EASYSIZE(IDC_TAB, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	//EASYSIZE(IDD_TAB1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
/*=======================================================*/
// CHookRepairerDlg 對話方塊



CHookRepairerDlg::CHookRepairerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HOOKREPAIRER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHookRepairerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabCtl);
}

BEGIN_MESSAGE_MAP(CHookRepairerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CHookRepairerDlg::OnSelchangeTab)
ON_BN_CLICKED(IDCANCEL, &CHookRepairerDlg::OnBnClickedCancel)
ON_BN_CLICKED(IDOK, &CHookRepairerDlg::OnBnClickedOk)
ON_UPDATE_COMMAND_UI(IDD_HOOKREPAIRER_DIALOG, &CHookRepairerDlg::OnUpdateIddHookrepairerDialog)
ON_WM_SIZE()
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CHookRepairerDlg 訊息處理常式

BOOL CHookRepairerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		/*BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}*/
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	//

	// TODO: 在此加入額外的初始設定

	m_tab1.Create(IDD_TAB1, &m_tabCtl);
	TCITEM item1;
	item1.mask = TCIF_TEXT | TCIF_PARAM;
	item1.lParam = (LPARAM)& m_tab1;
	item1.pszText = "Process/Module";
	m_tabCtl.InsertItem(0, &item1);
	CRect rc;
	m_tabCtl.GetItemRect(0, &rc);
	//m_tab1.SetWindowPos(NULL, rc.left, rc.bottom + 1, rc.left + rc.Width(), rc.bottom + 1 + rc.Height(), SWP_NOZORDER);
	m_tab1.SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_tab1.ShowWindow(SW_SHOW);

	m_tab1.m_ProcessList.InsertColumn(0, "PID", LVCFMT_LEFT, 60);
	m_tab1.m_ProcessList.InsertColumn(1, "NAME", LVCFMT_LEFT, 200);
	//m_ProcessList.SetTextBkColor((COLORREF)RGB(255.0.0));
	m_tab1.m_ProcessList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
	m_tab1.getAllProcess();

	m_tab1.m_ModuleList.InsertColumn(0, "Module", LVCFMT_CENTER, 150);
	m_tab1.m_ModuleList.InsertColumn(1, "Start Address", LVCFMT_LEFT, 80);
	m_tab1.m_ModuleList.InsertColumn(2, "Module Path", LVCFMT_LEFT, 1000);
	m_tab1.m_ModuleList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
	DWORD dwExStyle = m_tab1.m_ModuleList.GetExtendedStyle();
	m_tab1.m_ModuleList.SetExtendedStyle(dwExStyle | LVS_EX_CHECKBOXES );
	m_tab1.m_SelectDLL.SetCheck(true);
	m_tab1.m_ScanOpts.SetCurSel(0);
	m_tab1.m_tabCtl = &m_tabCtl;
	/*LVITEM item2;
	::ZeroMemory(&item2, sizeof(item2));
	item2.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;
	item2.pszText = "Tetst";
	item2.iImage = 1;
	item2.state = LVIS_SELECTED | LVIS_FOCUSED;
	m_tab1.m_ProcessList.InsertItem(&item2);*/
	m_tab2.Create(IDD_TAB2, &m_tabCtl);
	TCITEM item2;
	item2.mask = TCIF_TEXT | TCIF_PARAM;
	item2.lParam = (LPARAM)& m_tab2;
	item2.pszText = "Hook Info";
	m_tabCtl.InsertItem(1, &item2);

	m_tab2.m_ProcList.InsertColumn(0, "Module", LVCFMT_LEFT, 100);
	m_tab2.m_ProcList.InsertColumn(1, "Proc", LVCFMT_LEFT, 250);
	m_tab2.m_ProcList.InsertColumn(2, "Proc Address", LVCFMT_LEFT, 200);
	//m_tab2.m_ProcList.InsertColumn(3, "Proc Size", LVCFMT_LEFT, 200);
	m_tab2.m_ProcList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);
	m_tab1.m_tab2 = &m_tab2;

	m_tab3.Create(IDD_TAB3, &m_tabCtl);
	TCITEM item3;
	item3.mask = TCIF_TEXT | TCIF_PARAM;
	item3.lParam = (LPARAM)& m_tab3;
	item3.pszText = "About";
	m_tabCtl.InsertItem(2, &item3);

	INIT_EASYSIZE;
	ifstream fin("HookDetourMainDlg.ini");
	if ( !fin )
		ShowWindow(SW_NORMAL);
	else
	{
		RECT fRc;
		char tmp[100];
		fin >> tmp >> fRc.left;
		fin >> tmp >> fRc.top;
		fin >> tmp >> fRc.right;
		fin >> tmp >> fRc.bottom;
		fin.close();
		this->SetWindowPos(NULL, fRc.left, fRc.top, fRc.right, fRc.bottom, SWP_SHOWWINDOW);
	} // else

	m_Menu.LoadMenuA(IDR_HookInfoMENU);
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CHookRepairerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialogEx::OnSysCommand(nID, lParam);
	
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CHookRepairerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CHookRepairerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHookRepairerDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSel = m_tabCtl.GetCurSel();
	
	CRect rc;
	switch (iSel) {
	case 0:
		//m_tabCtl.GetItemRect(0, &rc);
		//m_tab1.SetWindowPos(NULL, rc.left, rc.bottom + 1, rc.left + rc.Width(), rc.bottom + 1 + rc.Height(),  SWP_NOZORDER);
		//m_tab1.SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		m_tab1.ShowWindow(SW_SHOW);
		m_tab2.ShowWindow(SW_HIDE);
		m_tab3.ShowWindow(SW_HIDE);
		break;
	case 1:
		//m_tabCtl.GetItemRect(0, &rc);
		//m_tab2.SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		m_tab2.ShowWindow(SW_SHOW);
		m_tab1.ShowWindow(SW_HIDE);
		m_tab3.ShowWindow(SW_HIDE);
		break;
	case 2:
		//m_tabCtl->GetItemRect(0, &rc);
		m_tab3.ShowWindow(SW_SHOW);
		m_tab1.ShowWindow(SW_HIDE);
		m_tab2.ShowWindow(SW_HIDE);
		break;
	} // switch
	*pResult = 0;
}


void CHookRepairerDlg::OnBnClickedCancel()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CDialogEx::OnCancel();
}


void CHookRepairerDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	for (Process p : vec_ProcessList)
		p.Clear();
	vector<Process>().swap(vec_ProcessList); // clear vector
	CDialogEx::OnOK();
}


void CHookRepairerDlg::OnBnClickedButton1()
{
	MessageBox("Close", "", MB_OK);
	// TODO: 在此加入控制項告知處理常式程式碼
	
	
}


void CHookRepairerDlg::OnUpdateIddHookrepairerDialog(CCmdUI *pCmdUI)
{
	// TODO: 在此加入您的命令更新 UI 處理常式程式碼

}

void CHookRepairerDlg::OnSize(UINT nType, int cx, int cy)
{
	
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: 在此加入您的訊息處理常式程式碼
	if (nType != SIZE_MINIMIZED)
	{
		UPDATE_EASYSIZE;
		/*MessageBox("RESIZE", "", MB_OK);*/
		CRect rc, wc;
		m_tabCtl.GetItemRect(0, &rc);
		GetClientRect(&wc);
		m_tab1.SetWindowPos(NULL, rc.left, rc.bottom + 1, rc.left + wc.Width(), rc.bottom + 1 + wc.Height()-60, SWP_NOZORDER);
		m_tab2.SetWindowPos(NULL, rc.left, rc.bottom + 1, rc.left + wc.Width(), rc.bottom + 1 + wc.Height() - 60, SWP_NOZORDER);
		m_tab3.SetWindowPos(NULL, rc.left, rc.bottom + 1, rc.left + wc.Width(), rc.bottom + 1 + wc.Height() - 60, SWP_NOZORDER);
		//m_tab1.m_ModuleList.InsertColumn(2, "Module Path", LVCFMT_LEFT, wc.Width());
		//m_tab1.m_ModuleList.SetColumnWidth( 2, wc.Width()/2);
		/*ReSize(IDOK, cx, cy, false);
		ReSize(IDC_TAB, cx, cy, true);//MessageBox("RESIZE", "", MB_OK);
		ReSize(IDD_TAB1, cx, cy, true);

		GetClientRect(&m_rect);*/
	}
}

void CHookRepairerDlg::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//MessageBox("Close", "", MB_OK);
	CRect fRc;
	GetWindowRect(&fRc);
	m_tab2.m_vecProc.Clear();
	ofstream fout("HookDetourMainDlg.ini");
	fout << "[mainDlg.left]= " << fRc.left << endl;
	fout << "[mainDlg.top]= " << fRc.top << endl;
	fout << "[mainDlg.width]= " << fRc.Width() << endl;
	fout << "[mainDlg.height]= " << fRc.Height() << endl;
	fout.close();
	for (Process p : vec_ProcessList)
		p.Clear();
	vector<Process>().swap(vec_ProcessList); // clear vector
	CDialogEx::OnClose();
}