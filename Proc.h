#pragma once
#include "stdafx.h"
using namespace std;

//typedef vector<CString> VecCString;
struct HookInfo
{
	CString m_strProcName;
	CString m_strAddr;
	CString m_strRetAddr;
	CString m_strDLength;
	CString m_strBytes;
	CString m_strCode;
};

class Proc
{
public:
	string m_strName;
	DWORD m_dwAddr;
	DWORD m_dwSize;
	byte* m_btCode;
	Proc();
	~Proc();
	void Clear();
	bool FindDetourAddress(string strDLL, vector<HookInfo>* vecStrResult);
	bool DumpFunction(string strDLL, vector<HookInfo>* vecDump);
	//bool FindDetourAddress(HANDLE hProcess); // find out the address that we can use it to do detour hook
private:

};

class ProcList
{
public:
	ProcList();
	~ProcList();
	vector<Proc>::iterator Begin();
	vector<Proc>::iterator End();
	void Copy(ProcList src);
	void Clear();
	int Size();
	void Push_Back(Proc c);
	void Pop_Back();
	void Sort();
	void Pop_Front();
	Proc At(int i);
private:
	void SortByIndex(int root);
	vector<Proc> m_vecProcList;
};
