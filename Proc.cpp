#include "stdafx.h"
#include <iterator>
#include "disasm.h"
Proc::Proc()
{
	m_dwSize = m_dwAddr = 0;
	m_btCode = NULL;
}

Proc::~Proc()
{

}

void Proc::Clear()
{
	if (m_btCode != NULL)
	{
		delete[] m_btCode;
		m_btCode = NULL;
	} // if
	
}

bool Proc::FindDetourAddress(string strDLL, vector<HookInfo>* vecStrResult)
{
	HMODULE hDLL = GetModuleHandle(strDLL.c_str());
	bool b_NeedFreeLib = false;
	if (hDLL == NULL) {
		LoadLibraryA(strDLL.c_str());
		b_NeedFreeLib = true;
		hDLL = GetModuleHandle(strDLL.c_str());
		if (hDLL == NULL)
		{
			MessageBox(NULL, strDLL.c_str(), "Can't find this module.", MB_OK);
			return false;
		} // if
	} // if

	Preparedisasm();
	SIZE_T lpRead;
	uchar* btOriginCode = (uchar*)m_dwAddr;
	int i_recordCounts = 0, i_zeroCount = 0;
	ulong cmdLength;
	t_disasm da_Origin;
	t_config config;
	config.lowercase = true;
	//config.hiliteoperands = true;
	CString oldCode, oldBytes;
	int oldLength = 0;
	for (int i = 0; i < m_dwSize; i += cmdLength)
	{
		cmdLength = Disasm((uchar*)(btOriginCode + i), m_dwSize - i, (DWORD)(btOriginCode + i), &da_Origin, DA_TEXT | DA_DUMP, &config, NULL);
		if (cmdLength == 0)
		{
			MessageBox(NULL, "Error - Nothing found.", "SORRY", MB_OK);
			break;
		} // if

		/*if (da_Origin.errors > 0 || da_Origin.warnings > 0)
		{
			continue;
		} // if*/
		
		if (cmdLength == 5 || cmdLength == 6)
		{
			i_recordCounts++;
			HookInfo hi;
			hi.m_strProcName = m_strName.c_str();
			hi.m_strAddr.Format( "0x%x", (DWORD)(btOriginCode + i));
			hi.m_strRetAddr.Format( "0x%x", (DWORD)(btOriginCode + i) + cmdLength);
			hi.m_strDLength.Format( "%d", cmdLength);
			hi.m_strBytes = da_Origin.dump;
			hi.m_strCode = da_Origin.result;
			vecStrResult->push_back(hi);
			//MessageBoxHex(NULL, (DWORD)(btOriginCode + i), cmdLength);
			//break;
		} // if
		else if (oldLength + cmdLength == 5 || oldLength + cmdLength == 6) // sutibale detour address
		{ // 7740A40d kernal32
			i_recordCounts++;
			HookInfo hi;
			hi.m_strProcName = m_strName.c_str();
			hi.m_strAddr.Format("0x%x", (DWORD)(btOriginCode + i));
			hi.m_strRetAddr.Format("0x%x", (DWORD)(btOriginCode + i) + cmdLength);
			hi.m_strDLength.Format("%d", cmdLength + oldLength);
			hi.m_strBytes = (oldBytes + "\r\n");
			hi.m_strBytes  += da_Origin.dump;
			hi.m_strCode = (oldCode + "\r\n");
			hi.m_strCode += da_Origin.result;
			vecStrResult->push_back(hi);
			//MessageBoxHex(NULL, (DWORD)(btOriginCode + i - oldLength), oldLength + cmdLength);
			//break;
		} // else if 
			
		if ((*(btOriginCode + i) == 0x0 && i_zeroCount++ >= 10) ||
			 *(btOriginCode + i) == 0xcc) break; // *(btOriginCode + i) == 0xc2 || *(btOriginCode + i) == 0xc3 ||
		oldLength = cmdLength;
		oldCode = da_Origin.result;
		oldBytes = da_Origin.dump;

	} // for

	Finishdisasm();
	if(b_NeedFreeLib) FreeLibrary(hDLL);
	if (i_recordCounts == 0) MessageBox(NULL, "Nothing found.", "SORRY", MB_OK);
	return true;
}
bool Proc::DumpFunction(string strDLL, vector<HookInfo>* vecDump)
{
	HMODULE hDLL = GetModuleHandle(strDLL.c_str());
	bool b_NeedFreeLib = false;
	if (hDLL == NULL) {
		LoadLibraryA(strDLL.c_str());
		b_NeedFreeLib = true;
		hDLL = GetModuleHandle(strDLL.c_str());
		if (hDLL == NULL)
		{
			MessageBox(NULL, strDLL.c_str(), "Can't find this module.", MB_OK);
			return false;
		} // if
	} // if

	Preparedisasm();
	SIZE_T lpRead;
	uchar* btOriginCode = (uchar*)m_dwAddr;
	//MessageBoxHex(NULL, m_dwAddr, 0);
	int i_zeroCount = 0;
	ulong cmdLength;
	t_disasm da_Origin;
	t_config config;
	config.lowercase = true;
	for (int i = 0; i < m_dwSize; i += cmdLength)
	{
		cmdLength = Disasm((uchar*)(btOriginCode + i), m_dwSize - i, (DWORD)(btOriginCode + i), &da_Origin, DA_TEXT | DA_DUMP, &config, NULL);
		if (cmdLength == 0)
		{
			MessageBox(NULL, "Dump Error.", "ERROR", MB_OK);
			break;
		} // if

		/*if (da_Origin.errors > 0 || da_Origin.warnings > 0)
		{
			continue;
		} // if*/

		HookInfo hi;
		hi.m_strProcName = m_strName.c_str();
		hi.m_strAddr.Format("0x%x", (DWORD)(btOriginCode + i));
		hi.m_strDLength.Format("%d", cmdLength);
		hi.m_strBytes.Format( "%-32s", da_Origin.dump);
		hi.m_strCode.Format( "%-50s", da_Origin.result);
		vecDump->push_back(hi);

		if ((*(btOriginCode + i) == 0x0 && i_zeroCount++ >= 10) ||
			 *(btOriginCode + i) == 0xcc) break;
	} // for

	Finishdisasm();
	if (b_NeedFreeLib) FreeLibrary(hDLL);
	return true;
}
// ==================================================================
ProcList::ProcList()
{
}

ProcList::~ProcList()
{
	vector<Proc>().swap(m_vecProcList);
}

vector<Proc>::iterator ProcList::Begin()
{
	return m_vecProcList.begin();
}

vector<Proc>::iterator ProcList::End()
{
	return m_vecProcList.end();

}

void ProcList::Copy(ProcList src)
{
	copy(src.Begin(), src.End(), back_inserter(m_vecProcList));
}
void ProcList::Clear()
{
	for (auto pc : m_vecProcList)
		pc.Clear();
	vector<Proc>().swap(m_vecProcList);
}

int ProcList::Size()
{
	return m_vecProcList.size();
}

void ProcList::Push_Back(Proc m)
{
	m_vecProcList.push_back(m);

}

void ProcList::Pop_Back()
{
	m_vecProcList[m_vecProcList.size() - 1].Clear();
	m_vecProcList.pop_back();
}

Proc ProcList::At(int i)
{
	return m_vecProcList[i];
}

void ProcList::Pop_Front()
{
	if (m_vecProcList.size() == 1) {
		m_vecProcList.pop_back();
		return;
	} // if

	iter_swap(m_vecProcList.begin(), m_vecProcList.end() - 1);
	Pop_Back();
	if (m_vecProcList.size() > 1) {
		SortByIndex(0);
	} // if
}

void ProcList::Sort()
{
	if (m_vecProcList.size() <= 1) return;

	for (int i = floor((m_vecProcList.size() - 2) / 2); i >= 0; i--)
		SortByIndex(i);

}

void ProcList::SortByIndex(int root)
{
	int lchild = root * 2 + 1;
	int rchild = lchild + 1;
	int min = root;
	int size = m_vecProcList.size();
	int pValue = m_vecProcList[root].m_dwAddr;
	int lValue = m_vecProcList[lchild].m_dwAddr;
	int rValue = -1;


	if (rchild < size) rValue = m_vecProcList[rchild].m_dwAddr;
	if (lchild < m_vecProcList.size() && (lValue < pValue)) {
		min = lchild;
		if (rchild < size && (rValue < lValue)) min = rchild;
	} // if
	else {
		if (rchild < size && (rValue < pValue)) min = rchild;
	}


	if (min != root) {
		iter_swap(m_vecProcList.begin() + root, m_vecProcList.begin() + min);
		if (min * 2 + 1 < size) SortByIndex(min);
	}
}
