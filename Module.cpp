#include "stdafx.h"
#include <iterator>

Module::Module()
{
}

Module::Module(string n, DWORD a, string p)
{
	m_strName = n;
	m_dwAddr = a;
	m_strPath = p;
}

Module::~Module()
{

}

/*bool Module::GetModuleProcs(CodeSegList* prList)
{
	bool b_NeedRemoveDLL = false;
	HMODULE	hModule = GetModuleHandle(m_strName.c_str());
	if (hModule == NULL)
	{
		b_NeedRemoveDLL = true;
		hModule = LoadLibraryA(m_strName.c_str());
	} // if

	if (hModule == NULL) return false;
	uint8_t* dwModuleBase = (uint8_t*)hModule;
	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)dwModuleBase;
	if (pDosHeader == NULL) return false;
	IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)(dwModuleBase + pDosHeader->e_lfanew);

	if (pNTHeaders == NULL) return false;
	// 加載器將文件裝入內存並將導入表（導出表）登記的動態鏈接庫（一般是DLL格式）文件一併裝入地址空間,                                   //数据的起始RVA
	IMAGE_EXPORT_DIRECTORY* pExpHeader = (IMAGE_EXPORT_DIRECTORY*)(pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + dwModuleBase);
	if (pExpHeader == NULL) return false;

	size_t szModuleSize = pNTHeaders->OptionalHeader.SizeOfImage;
	DWORD* pName = (DWORD*)(pExpHeader->AddressOfNames + dwModuleBase);
	DWORD* pAddress = (DWORD*)(pExpHeader->AddressOfFunctions + dwModuleBase); // 起始function Address
	WORD* pOrdinals = (WORD*)(pExpHeader->AddressOfNameOrdinals + dwModuleBase); // 取得指向輸出函數序號的RVA

	for (int i = 0; i < pExpHeader->NumberOfNames; i++)
	{
		//WORD wAddressIndex = pOrdinals[i]; // 取得函數序號的內容
		const char* pszFunctionName = (const char *)(pName[i] + dwModuleBase); // function name
																			   //MessageBox(NULL, pszFunctionName, "", MB_OK);
		CodeSeg mTmp;
		mTmp.m_Module.m_strName = m_strName;
		mTmp.m_Module.m_dwAddr = (DWORD)dwModuleBase;
		mTmp.m_Proc.m_dwAddr = (DWORD)dwModuleBase + pAddress[pOrdinals[i]];
		mTmp.m_Proc.m_strName = pszFunctionName;
		if ( i < pExpHeader->NumberOfNames - 1)
			mTmp.m_Proc.m_dwSize = (DWORD)dwModuleBase + (pAddress[pOrdinals[i+1]] - pAddress[pOrdinals[i]]);
		else
			mTmp.m_Proc.m_dwSize = (DWORD)dwModuleBase + (pAddress[pOrdinals[i + 1]] - pAddress[pOrdinals[i]]);
		prList->Push_Back(mTmp);

	} // for

	prList->Sort();
	CodeSeg mEnd;
	mEnd.m_Proc.m_dwAddr = (DWORD)(dwModuleBase + szModuleSize);
	CodeSegList tmpList;
	tmpList.Copy(*prList);
	for (int i = 0; i < pExpHeader->NumberOfNames; i++)
	{
		CodeSeg* mCur = &tmpList.At(0);
		CodeSeg* mNext = &mEnd;
		tmpList.Pop_Front();
		if (tmpList.Size() > 0)
			mNext = &tmpList.At(0);
		mCur->m_Proc.m_dwSize = mNext->m_Proc.m_dwAddr - mCur->m_Proc.m_dwAddr;
	}

	if (b_NeedRemoveDLL)
		FreeLibrary(hModule);
	tmpList.Clear();
}*/


// ===========================================================================================
ModuleList::ModuleList()
{
}

ModuleList::~ModuleList()
{
	vector<Module>().swap(m_vecModuleList);
}

vector<Module>::iterator ModuleList::Begin()
{
	return m_vecModuleList.begin();
}

vector<Module>::iterator ModuleList::End()
{
	return m_vecModuleList.end();
}

void ModuleList::Copy(ModuleList src)
{
	copy(src.Begin(), src.End(), back_inserter(m_vecModuleList));
}

void ModuleList::Clear()
{
	vector<Module>().swap(m_vecModuleList);
}

int ModuleList::Size()
{
	return m_vecModuleList.size();
}

void ModuleList::Push_Back(Module m)
{
	m_vecModuleList.push_back(m);
	
}

void ModuleList::Pop_Back()
{
	//m_vecModuleList[m_vecModuleList.size() - 1].Clear();
	m_vecModuleList.pop_back();
}

Module ModuleList::At(int i)
{
	return m_vecModuleList[i];
}

void ModuleList::Pop_Front()
{
	if (m_vecModuleList.size() == 1) {
		m_vecModuleList.pop_back();
		return;
	} // if

	iter_swap(m_vecModuleList.begin(), m_vecModuleList.end() - 1);
	Pop_Back();
	if (m_vecModuleList.size() > 1) {
		SortByIndex(0);
	} // if
}

void ModuleList::Sort()
{
	if (m_vecModuleList.size() <= 1) return;

	for (int i = floor((m_vecModuleList.size() - 2) / 2); i >= 0; i--)
		SortByIndex(i);
	
}

void ModuleList::SortByIndex(int root)
{
	int lchild = root * 2 + 1;
	int rchild = lchild + 1;
	int min = root;
	int size = m_vecModuleList.size();
	int pValue = m_vecModuleList[root].m_dwAddr;
	int lValue = m_vecModuleList[lchild].m_dwAddr;
	int rValue = -1;


	if (rchild < size) rValue = m_vecModuleList[rchild].m_dwAddr;
	if (lchild < m_vecModuleList.size() && (lValue < pValue)) {
		min = lchild;
		if (rchild < size && (rValue < lValue)) min = rchild;
	} // if
	else {
		if (rchild < size && (rValue < pValue)) min = rchild;
	}


	if (min != root) {
		iter_swap(m_vecModuleList.begin() + root, m_vecModuleList.begin() + min);
		if (min * 2 + 1 < size) SortByIndex(min);
	}
}
