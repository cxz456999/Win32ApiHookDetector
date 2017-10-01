#pragma once
#include "stdafx.h"
using namespace std;
class Module
{
public:
	Module();
	Module( string n, DWORD a, string p);
	~Module();
	string m_strName;
	DWORD m_dwAddr;
	string m_strPath;
	//bool GetModuleProcs(CodeSegList* prList);
private:
};

class ModuleList
{
public:
	ModuleList();
	~ModuleList();
	vector<Module>::iterator Begin();
	vector<Module>::iterator End();
	void Copy(ModuleList src);
	void Clear();
	int Size();
	void Push_Back(Module m);
	void Pop_Back();
	void Sort();
	void Pop_Front();
	Module At(int i);
private:
	void SortByIndex( int root );
	vector<Module> m_vecModuleList;
};
