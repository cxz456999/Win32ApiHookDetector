
// stdafx.cpp : 僅包含標準 Include 檔的原始程式檔
// HookRepairer.pch 會成為先行編譯標頭檔
// stdafx.obj 會包含先行編譯類型資訊

#include "stdafx.h"

BOOL MessageBoxHex(HWND hw, int cc, int tt)
{
	TCHAR t[100];
	wsprintf(t, "%x, %d", cc, tt);
	return MessageBox(hw, t, "", MB_OK);
}

void copyToClipBoard(string str)
{
	OpenClipboard(NULL);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, str.size()+1);
	if (!hg) CloseClipboard();
	memcpy(GlobalLock(hg), str.c_str(), str.size());
	//strcpy((char*)GlobalLock(hg), str.c_str());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}
