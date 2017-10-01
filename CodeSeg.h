#pragma once
#include "stdafx.h"
using namespace std;
class CodeSeg
{
public:
	Module m_Module;
	Proc m_Proc;
	DWORD m_dwCodeAddr;
	int m_iLength;
	string m_strCode;
	string m_strOriginCode;
	CodeSeg();
	~CodeSeg();
	void Clear();
private:
};

class CodeSegList
{
public:
	CodeSegList();
	~CodeSegList();
	void Clear();
	int Size();
	void Push_Back(CodeSeg cs);
	void Pop_Back();
	void Sort();
	void Pop_Front();
	void Copy(CodeSegList src);
	vector<CodeSeg>::iterator Begin();
	vector<CodeSeg>::iterator End();
	CodeSeg At(int i);
private:
	void SortByIndex(int root);
	vector<CodeSeg> m_vecCodeSegList;
	
};