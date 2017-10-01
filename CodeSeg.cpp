#include "stdafx.h"
#include <iterator>
CodeSeg::CodeSeg()
{

}

CodeSeg::~CodeSeg()
{

}

void CodeSeg::Clear()
{
	m_Proc.Clear();

}

// ================

CodeSegList::CodeSegList()
{
}

CodeSegList::~CodeSegList()
{
	vector<CodeSeg>().swap(m_vecCodeSegList);
}

vector<CodeSeg>::iterator CodeSegList::Begin()
{
	return m_vecCodeSegList.begin();
}

vector<CodeSeg>::iterator CodeSegList::End()
{
	return m_vecCodeSegList.end();
}

void CodeSegList::Copy(CodeSegList src)
{
	copy(src.Begin(), src.End(), back_inserter(m_vecCodeSegList));
}

void CodeSegList::Clear()
{
	for (auto cd : m_vecCodeSegList)
		cd.Clear();
	vector<CodeSeg>().swap(m_vecCodeSegList);
}

int CodeSegList::Size()
{
	return m_vecCodeSegList.size();
}

void CodeSegList::Push_Back(CodeSeg m)
{
	m_vecCodeSegList.push_back(m);

}

void CodeSegList::Pop_Back()
{
	m_vecCodeSegList[m_vecCodeSegList.size() - 1].Clear();
	m_vecCodeSegList.pop_back();
}

CodeSeg CodeSegList::At(int i)
{
	return m_vecCodeSegList[i];
}

void CodeSegList::Pop_Front()
{
	if (m_vecCodeSegList.size() == 1) {
		m_vecCodeSegList.pop_back();
		return;
	} // if

	iter_swap(m_vecCodeSegList.begin(), m_vecCodeSegList.end() - 1);
	m_vecCodeSegList[m_vecCodeSegList.size()-1].Clear();
	Pop_Back();
	if (m_vecCodeSegList.size() > 1) {
		SortByIndex(0);
	} // if
}

void CodeSegList::Sort()
{
	if (m_vecCodeSegList.size() <= 1) return;

	for (int i = floor((m_vecCodeSegList.size() - 2) / 2); i >= 0; i--)
		SortByIndex(i);

}

void CodeSegList::SortByIndex(int root)
{
	int lchild = root * 2 + 1;
	int rchild = lchild + 1;
	int min = root;
	int size = m_vecCodeSegList.size();
	int pValue = m_vecCodeSegList[root].m_Proc.m_dwAddr;
	int lValue = m_vecCodeSegList[lchild].m_Proc.m_dwAddr;
	int rValue = -1;


	if (rchild < size) rValue = m_vecCodeSegList[rchild].m_Proc.m_dwAddr;
	if (lchild < size && (lValue < pValue)) {
		min = lchild;
		if (rchild < size && (rValue < lValue)) min = rchild;
	} // if
	else {
		if (rchild < size && (rValue < pValue)) min = rchild;
	}


	if (min != root) {
		iter_swap(m_vecCodeSegList.begin() + root, m_vecCodeSegList.begin() + min);
		if (min * 2 + 1 < size) SortByIndex(min);
	}
}

/*
void CodeSegList::SortByIndex(int root)
{
int lchild = root * 2 + 1;
int rchild = lchild + 1;
int min = root;
int size = m_vecCodeSegList.size();
int pValue = m_vecCodeSegList[root].m_Module.m_dwAddr;
int lValue = m_vecCodeSegList[lchild].m_Module.m_dwAddr;
int rValue = -1;


if (rchild < size) rValue = m_vecCodeSegList[rchild].m_Module.m_dwAddr;
if (lchild < m_vecCodeSegList.size() && (lValue <= pValue)) // ml <= mp
{
if (lValue < pValue) // compare module address ml < mp
{
min = lchild;
if (rchild < size && (rValue <= lValue)) // mr <= ml
{
if (rValue < pValue) // mr < ml
min = rchild;
else // if module address are same  mr == ml
{
lValue = m_vecCodeSegList[lchild].m_Proc.m_dwAddr;
rValue = m_vecCodeSegList[rchild].m_Proc.m_dwAddr;
if (rValue < pValue) // compare proc address pr < pl
min = rchild;
} // else
} // if
} // if
else // if module address are same ml == mp
{
pValue = m_vecCodeSegList[root].m_Proc.m_dwAddr;
lValue = m_vecCodeSegList[lchild].m_Proc.m_dwAddr;
if (lValue < pValue) // compare proc address pl < pp
{
min = lchild;
lValue = m_vecCodeSegList[lchild].m_Module.m_dwAddr; // resume module address
if (rchild < size && (rValue <= lValue)) // compare module address mr <= ml
{
if (rValue < lValue) // mr < ml
min = rchild;
else // if module address are same mr == ml
{
lValue = m_vecCodeSegList[lchild].m_Proc.m_dwAddr;
rValue = m_vecCodeSegList[rchild].m_Proc.m_dwAddr;
if (rValue < lValue) // compare proc address pr < pl
min = rchild;
} // else
} // if
} // if

}
} // if
else // ml > mp
{
if (rchild < size && (rValue <= pValue)) // mr <= mp
{
if (rValue < pValue) // mr < mp
min = rchild;
else // if module address are same mr == mp
{
pValue = m_vecCodeSegList[root].m_Proc.m_dwAddr;
rValue = m_vecCodeSegList[rchild].m_Proc.m_dwAddr;
if (rValue < pValue) // compare proc address pr < pp
min = rchild;
} // else
} // if
}


if (min != root) {
iter_swap(m_vecCodeSegList.begin() + root, m_vecCodeSegList.begin() + min);
if (min * 2 + 1 < size) SortByIndex(min);
}
}

*/