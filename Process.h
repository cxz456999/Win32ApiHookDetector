#pragma once
#include "stdafx.h"
using namespace std;

class Process
{
	public:
		BOOL isAllow;
		BOOL is32Bit;
		DWORD m_dwPid;
		HWND m_hWnd;
		string m_strName;
		string m_strPath;
		HANDLE m_hProcess;
		HICON m_hIcon;
		ModuleList m_vecModule;
		CodeSegList m_vecDifferentCode;
		Process();
		Process(DWORD dw, char* str, HANDLE handle);
		~Process();
		bool Terminate();
		void Copy( Process* src );
		void DeleteDifferentCodeList();
		void Clear();
		void GetPath();
		void GetIcon();
		void OpenProcessHandle();
		void CloseProcessHandle();
		bool Repair(const char* str_DLL, const char* str_FuncName, DWORD dw_BeginAddr, int length);
		/*void Free() {
			CloseHandle(m_hProcess);
		}*/
		bool ScanModuleHook(char* strDLL);
		bool GetModuleProcs(char* strDLL, CodeSegList* prList);
		bool GetAllModule();
		void GetPermission();
	private:
		//BOOL CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lParam);
		bool compareFunction(CodeSeg& csdCode, DWORD funcAddr, DWORD nextFuncAddr);
		void GetHwndByPid();
		void runAsDbg();
		HANDLE GetModuleAddr(const char* lps_ModuleName );

};

BOOL Is64BitWindows();
bool EnumAllProcesses(vector<Process>& vec_ProcessList);
