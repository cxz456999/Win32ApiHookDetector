#include "stdafx.h"
using namespace std;
#include <sys/types.h> // uint8_t ...
#include <Psapi.h>
#include <Shellapi.h>
#include <commoncontrols.h>
#include <fstream>
#include <Dbghelp.h>
#include <iterator>
#include "disasm.h"

static ProcList procList;
Process::Process()
{
}

Process::Process( DWORD dw, char* str, HANDLE handle )
{
	m_dwPid = dw;
	m_strName = str;
	m_hProcess = handle;
}

Process::~Process()
{
	m_vecModule.Clear();
	m_vecDifferentCode.Clear();
	OpenProcessHandle();
}

void Process::DeleteDifferentCodeList()
{
	/*for ( int i = m_vecDifferentCode.size()-1; i >=0 ; i-- )
	{
		CodeSeg* cs = &m_vecDifferentCode[i];
		//m_vecDifferentCode.erase(m_vecDifferentCode.begin()+i, m_vecDifferentCode.begin()+(i+1));
		if (cs != NULL) 
		{
			delete cs;
			cs = NULL;
		} // if
	} // for

	for (auto cd : m_vecDifferentCode)
	{
		cd.Clear();
	}

	vector<CodeSeg>().swap(m_vecDifferentCode);*/
}

bool Process::Terminate() {
	if (m_dwPid != 0) {
		OpenProcessHandle();
		TerminateProcess(m_hProcess, 1);
		CloseProcessHandle();
		Clear();
		return true;
	}

	MessageBox( NULL, "Terminate process failed.", "ERROR", MB_OK);
	return false;
}

void Process::Copy(Process* src)
{
	m_dwPid = src->m_dwPid;
	m_hIcon = src->m_hIcon;
	m_hProcess = src->m_hProcess;
	m_hWnd = src->m_hWnd;
	m_strName = src->m_strName;
	m_strPath = src->m_strPath;
	m_vecDifferentCode.Copy(src->m_vecDifferentCode);
	m_vecModule.Copy(src->m_vecModule);
	//copy(src->m_vecDifferentCode.begin(), src->m_vecDifferentCode.End(), back_inserter(m_vecDifferentCode));
	//copy(src->m_vecModule.Begin(), src->m_vecModule.End(), back_inserter(m_vecModule));
}

void Process::Clear()
{
	//DeleteDifferentCodeList();
	//vector<CodeSeg>().swap(m_vecDifferentCode);
	m_vecDifferentCode.Clear();
	m_vecModule.Clear();
	CloseProcessHandle();
}

//typedef DWORD (*MyGetModuleFileNameEX)( HANDLE hProcess, HMODULE hModule,LPSTR lpFilename, DWORD nSize);
//typedef BOOL (*MyEnumProcessModulesEX)( HANDLE hProcess, HMODULE *lphModule,  DWORD cb, LPDWORD lpcbNeeded);
bool Process::GetAllModule()
{
	Clear();
	OpenProcessHandle();
	DWORD dwNeed = 0;
	HMODULE* hAllModule = new HMODULE[1000];
	//MyEnumProcessModulesEX EnumProcessModulesEX = (MyEnumProcessModulesEX)GetProcAddress(GetModuleHandle("Psapi.dll"), "EnumProcessModules");
	int er = EnumProcessModules(m_hProcess, hAllModule, 1000, (PDWORD)&dwNeed);
	int count = dwNeed / sizeof(DWORD);

	for (int i = 0; i < count; i++)
	{
		TCHAR tmp[MAX_PATH];
		//GetModuleFileNameEx(m_hProcess, hAllModule[i], tmp, MAX_PATH); // use  GetModuleFileName 會得到亂碼
		//MyGetModuleFileNameEX GetModuleFileNameEXX;
		//GetModuleFileNameEXX = (MyGetModuleFileNameEX)GetProcAddress(GetModuleHandle("Psapi.dll"), "GetModuleFileNameEx");
		GetModuleFileNameEx(m_hProcess, hAllModule[i], tmp, MAX_PATH);
		if (hAllModule[i] == NULL) continue;

		Module mTmp;
		mTmp.m_strPath = "";
		mTmp.m_strPath += tmp;
		mTmp.m_dwAddr = (DWORD)(hAllModule[i]);
		mTmp.m_strName = mTmp.m_strPath.substr( mTmp.m_strPath.find_last_of("\\")+1);
		
		m_vecModule.Push_Back( mTmp );
	} // for
		
	delete[] hAllModule;
	CloseProcessHandle();
	return true;
}

void Process::OpenProcessHandle() {
	runAsDbg();
	m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, m_dwPid);
	//if (m_hProcess == NULL) MessageBox(NULL, "PRO FAILED", "", MB_OK);
}

void Process::CloseProcessHandle()
{
	CloseHandle(m_hProcess);
	m_hProcess = NULL;
}

bool Process::Repair(const char* str_DLL, const char* str_FuncName, DWORD dw_BeginAddr, int length )
{
	//MessageBox(NULL, str_DLL, str_FuncName, MB_OK);
	
	OpenProcessHandle();
	HANDLE hProcessDLL = GetModuleAddr(str_DLL); // get process module address
	if (hProcessDLL == NULL) { // ensure the program has been load this dll.
		MessageBox(NULL, "Process has been closed", m_strName.c_str(), MB_OK);
		CloseHandle(hProcessDLL);
		CloseProcessHandle();
		return false;
	} // if

	CloseHandle(hProcessDLL);
	HMODULE hDLL = GetModuleHandle(str_DLL);
	bool b_NeedFreeLib = false;
	if (hDLL == NULL) {
		LoadLibraryA(str_DLL);
		b_NeedFreeLib = true;
		hDLL = GetModuleHandle(str_DLL);
		if (hDLL == NULL)
		{
			MessageBox(NULL, str_DLL, "Can't find this module.", MB_OK);
			CloseProcessHandle();
			return false;
		} // if
	} // if
	LPVOID lp_Func = GetProcAddress(GetModuleHandle(str_DLL), str_FuncName);
	if (!lp_Func)
	{
		MessageBox(NULL, str_FuncName, "Can't find this function.", MB_OK);
		CloseProcessHandle();
		return false;
	} // if

	
	BYTE* code = new BYTE[length+1];
	memcpy( code, (LPVOID)(dw_BeginAddr), length );
	DWORD dw_Old, dw_Bkp;
	VirtualProtect((LPVOID)(dw_BeginAddr), length, PAGE_EXECUTE_READWRITE, (PDWORD)&dw_Old);
	BOOL b_result = WriteProcessMemory(m_hProcess, (LPVOID)(dw_BeginAddr), code, length, NULL);
	VirtualProtect((LPVOID)(dw_BeginAddr), length, dw_Old, (PDWORD)&dw_Bkp);
	delete[] code;
	if (!b_result)
	{
		MessageBox(NULL, "repair failed", "ERROR", MB_OK);
		if (b_NeedFreeLib) FreeLibrary(hDLL);
		CloseProcessHandle();
		return false;
	} // if

	if (b_NeedFreeLib) FreeLibrary(hDLL);
	CloseProcessHandle();
	return true;
}

HANDLE Process::GetModuleAddr(const char* lps_ModuleName)
{
	MODULEENTRY32 mod_ModuleEntry = { 0 }; // Describes an entry from a list of the modules belonging to the specified process.
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwPid); // Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes.
	if (!hSnapShot) return NULL;
	mod_ModuleEntry.dwSize = sizeof(mod_ModuleEntry);
	BOOL b_runModule = Module32First( hSnapShot, &mod_ModuleEntry );
	while (b_runModule)
	{
		if (strcmp(mod_ModuleEntry.szModule, lps_ModuleName) == 0) 
		{
			CloseHandle(hSnapShot);
			return mod_ModuleEntry.modBaseAddr;
		} // if

		b_runModule = Module32Next( hSnapShot, &mod_ModuleEntry );
	} // while

	CloseHandle(hSnapShot);
	return NULL;
}

void Process::runAsDbg()
{
	HANDLE hCur = NULL;
	BOOL success = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hCur);
	if (success)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hCur, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hCur);
	}
}

HWND g_hWnd = NULL;
BOOL CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (!hwnd) return TRUE;
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, (PDWORD)&lpdwProcessId);
	

	if (lpdwProcessId == lParam)
	{
		g_hWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

void Process::GetHwndByPid()
{
	g_hWnd = NULL;
	EnumWindows(MyEnumWindowsProc, m_dwPid);
	m_hWnd = g_hWnd;
	
}

void Process::GetPath()
{
	OpenProcessHandle();
	TCHAR path[MAX_PATH];
	if (GetModuleFileNameEx(m_hProcess, NULL, path, MAX_PATH) == 0)
	{
		return;
	} // if

	CloseProcessHandle();
	m_strPath = path;
}

void Process::GetIcon()
{

	GetHwndByPid();
	HICON iconHandle = (HICON)::SendMessage(m_hWnd, WM_GETICON, ICON_SMALL2, 0);
	if (!iconHandle)
		iconHandle = (HICON)::SendMessage(m_hWnd, WM_GETICON, ICON_SMALL, 0);
	if (!iconHandle)
		iconHandle = (HICON)::SendMessage(m_hWnd, WM_GETICON, ICON_BIG, 0);
	

	if (!iconHandle)
	{
		HICON h_Big;
		ExtractIconEx(m_strPath.c_str(), 0, &h_Big, &m_hIcon, 1);
	}
	else m_hIcon = iconHandle;
}

/*
IMAGE_EXPORT_DIRECTORY STRUCT【導出表，共40字節】
{
DWORD特性;未使用，總是定義為0
DWORD TimeDateStamp;文件生成時間
WORD MajorVersion;未使用，總是定義為0
WORD MinorVersion;未使用，總是定義為0
DWORD名稱;模塊的真實名稱
DWORD基地;基數，加上序數就是函數地址數組的索引值
DWORD NumberOfFunctions;導出函數的總數
DWORD NumberOfNames;以名稱方式導出的函數的總數
DWORD AddressOfFunctions;指向輸出函數地址的RVA
DWORD AddressOfNames;指向輸出函數名字的RVA
DWORD AddressOfNameOrdinals;指向輸出函數序號的RVA
}; IMAGE_EXPORT_DIRECTORY ENDS
*/
void bytes2String(byte* arr, int stratIndex, int arrSize, char* strBytes)
{
	for (int i = stratIndex; i < arrSize; i++)
	{
		strBytes[i] = (unsigned char)arr[i];
	} // for

	strBytes[arrSize] = '\0';
} // void

bool Process::compareFunction(CodeSeg& csdSampleCode, DWORD funcAddr, DWORD nextFuncAddr)
{
	SIZE_T lpRead;
	DWORD dwSize = nextFuncAddr - funcAddr;
	if (dwSize <= 0)
	{
		MessageBox(NULL, "length of function is wrong.", "ERROR", MB_OK);
		return true;
	} // if
	else if (m_hProcess == NULL)
	{
		MessageBox(NULL, "Didn't open the process.", "ERROR", MB_OK);
		return true;
	} // else if
	
	byte* btCode = new byte[dwSize];
	ReadProcessMemory(m_hProcess, (LPVOID)(funcAddr), btCode, dwSize, &lpRead);
	uchar* btOriginCode = (uchar*)funcAddr;

	CodeSeg* csdCode = new CodeSeg();
	csdCode->m_iLength = 0;
	csdCode->m_Module.m_dwAddr = csdSampleCode.m_Module.m_dwAddr;
	csdCode->m_Proc.m_dwAddr = csdSampleCode.m_Proc.m_dwAddr;
	csdCode->m_Module.m_strName = csdSampleCode.m_Module.m_strName;
	csdCode->m_Proc.m_strName = csdSampleCode.m_Proc.m_strName;

	int i_recordCounts = 0, i_zeroCount = 0;
	ulong cmdLength;
	t_disasm da_Origin, da_New;

	for (int i = 0; i < dwSize; i += cmdLength)
	{
		cmdLength = Disasm((uchar*)(btOriginCode + i), dwSize - i, (DWORD)(btOriginCode + i), &da_Origin, DA_TEXT, NULL, NULL);
		if (cmdLength == 0  )
		{
			break;
		}
		
		if (da_Origin.errors > 0 || da_Origin.warnings > 0)
		{
			continue;
		}

		
		cmdLength = Disasm((uchar*)(btCode + i), dwSize - i, (DWORD)(btOriginCode + i), &da_New, DA_TEXT, NULL, NULL);

		if (cmdLength == 0)
		{
			break;
		}

		if (da_New.errors > 0 || da_Origin.warnings > 0)
		{
			continue;
		}

		if (strcmp(da_New.result, da_Origin.result) != 0 && btCode[i] == 0xe9 ) // (btCode[i] == 0xe9 && btCode[i] != *(btOriginCode + i)) || (btCode[i] == 0xe9 && *(btOriginCode + i) == 0xe9)
		{
			
			csdCode->m_iLength = cmdLength;
			csdCode->m_dwCodeAddr = (DWORD)(btOriginCode + i);
			csdCode->m_strOriginCode = da_Origin.result;
			csdCode->m_strCode = da_New.result;
			m_vecDifferentCode.Push_Back(*csdCode);
			csdCode = new CodeSeg();
			csdCode->m_Module.m_dwAddr = csdSampleCode.m_Module.m_dwAddr;
			csdCode->m_Proc.m_dwAddr = csdSampleCode.m_Proc.m_dwAddr;
			csdCode->m_Module.m_strName = csdSampleCode.m_Module.m_strName;
			csdCode->m_Proc.m_strName = csdSampleCode.m_Proc.m_strName;
			
		} // if

		if ((*(btOriginCode + i) == 0x0 && i_zeroCount++ >= 10) ||
			 *(btOriginCode + i) == 0xcc) break;
	} // for

	delete csdCode;
	
	return i_recordCounts > 0 ? false : true;
}

FARPROC GetProcedureAddress(HANDLE hModule, char* pszProcName)
{
	IMAGE_DOS_HEADER* pdhDosHeader = (IMAGE_DOS_HEADER*)hModule;
	//Check if valid PE
	if (pdhDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return 0;

	IMAGE_NT_HEADERS* pndNTHeader = (IMAGE_NT_HEADERS*)(pdhDosHeader->e_lfanew + (long)hModule);
	if (pndNTHeader->Signature != IMAGE_NT_SIGNATURE) return 0;

	//Traverse the export table to see if we can find the export
	IMAGE_EXPORT_DIRECTORY* iedExports = (IMAGE_EXPORT_DIRECTORY*)(pndNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (long)hModule);
	long* pNames = (long*)(iedExports->AddressOfNames + (long)hModule);
	short wOrdinalIndex = -1;
	for (int i = 0; i < iedExports->NumberOfFunctions; i++)
	{
		char* pszFunctionName = (char *)(pNames[i] + (long)hModule);

		if (lstrcmpi(pszFunctionName, pszProcName) == 0)
		{
			wOrdinalIndex = i;
			break;
		}
	}

	if (wOrdinalIndex == -1) return 0;

	//wIndex now holds the index of the function name in the names array, which is the index of the ordinal.
	//The ordinal also acts as the index of the address
	short* pOrdinals = (short*)(iedExports->AddressOfNameOrdinals + (long)hModule);
	unsigned long* pAddresses = (unsigned long*)(iedExports->AddressOfFunctions + (long)hModule);

	short wAddressIndex = pOrdinals[wOrdinalIndex];
	return (FARPROC)(pAddresses[wAddressIndex] + (long)hModule);
}

bool RemoveLoadedDll( char* strDLL )
{
	HANDLE hCur = GetCurrentProcess();
	
}

bool Process::GetModuleProcs(char* strDLL, CodeSegList* prList )
{
	bool b_NeedRemoveDLL = false;
	HMODULE	hModule = GetModuleHandle(strDLL);
	if (hModule == NULL)
	{
		b_NeedRemoveDLL = true;
		hModule = LoadLibraryA(strDLL);
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
		mTmp.m_Module.m_strName = strDLL;
		mTmp.m_Module.m_dwAddr = (DWORD)dwModuleBase;
		mTmp.m_Proc.m_dwAddr = (DWORD)dwModuleBase + pAddress[pOrdinals[i]];
		mTmp.m_Proc.m_strName = pszFunctionName;
		prList->Push_Back(mTmp);

	} // for

	prList->Sort();
	CodeSeg mEnd;
	mEnd.m_Proc.m_dwAddr = (DWORD)dwModuleBase + szModuleSize;
	CodeSegList tmpList;
	tmpList.Copy(*prList);
	prList->Clear();
	for (int i = 0; i < pExpHeader->NumberOfNames; i++)
	{
		CodeSeg mCur = tmpList.At(0);
		CodeSeg mNext = mEnd;
		tmpList.Pop_Front();
		if (tmpList.Size() > 0)
			mNext = tmpList.At(0);
		mCur.m_Proc.m_dwSize = mNext.m_Proc.m_dwAddr - mCur.m_Proc.m_dwAddr;
		if ( mCur.m_Proc.m_dwSize > 0 )
			prList->Push_Back(mCur);
	}

	/*TCHAR t[100];
	wsprintf( t, "%x, %x", prList->At(prList->Size()-1).m_Proc.m_dwAddr, szModuleSize);
	MessageBox(NULL, t, "", MB_OK);*/
	if (b_NeedRemoveDLL)
		FreeLibrary(hModule);
	tmpList.Clear();
	if (b_NeedRemoveDLL)
		FreeLibrary(hModule);
}

bool Process::ScanModuleHook(char* strDLL)
{
	//ofstream fout("Different.txt");
	
	// traverse module function
	//if (hModule == NULL) // 非 getAllModule 的方式
	//{
	bool b_NeedRemoveDLL = false;
	HMODULE	hModule = GetModuleHandle(strDLL);
	if (hModule == NULL)
	{
		b_NeedRemoveDLL = true;
		hModule = LoadLibraryA(strDLL);
	} // if

	if (hModule == NULL) return false;
	//} // if PVOID hProcessModuleBaseAddr
	//HMODULE hModule = (HMODULE)GetModuleAddr(strDLL);
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
	/*TCHAR fuck[100];
	wsprintf( fuck, "%X", pExpHeader->AddressOfNames);
	MessageBox(NULL, fuck, "", MB_OK);*/
	DWORD* pAddress = (DWORD*)(pExpHeader->AddressOfFunctions + dwModuleBase); // 起始function Address
	WORD* pOrdinals = (WORD*)(pExpHeader->AddressOfNameOrdinals + dwModuleBase); // 取得指向輸出函數序號的RVA

	for (int i = 0; i < pExpHeader->NumberOfNames; i++)
	{
		//WORD wAddressIndex = pOrdinals[i]; // 取得函數序號的內容

		//DWORD funcAddr = (DWORD)pAddress[wAddressIndex] + (DWORD)dwModuleBase; // function address
		//if (funcAddr <= 0 || funcAddr >= (DWORD)hModule + szModuleSize) continue;
		//if (pName[i] + dwModuleBase >= dwModuleBase + szModuleSize) continue;


		const char* pszFunctionName = (const char *)(pName[i] + dwModuleBase); // function name
																			   //MessageBox(NULL, pszFunctionName, "", MB_OK);
		Proc mTmp;
		mTmp.m_dwAddr = (DWORD)dwModuleBase + pAddress[pOrdinals[i]];
		mTmp.m_strName = pszFunctionName;
		procList.Push_Back(mTmp);

	} // for

	//MessageBox(NULL, "STOP2", "", MB_OK);
	OpenProcessHandle();
	Preparedisasm();
	procList.Sort();
	Proc mEnd;
	mEnd.m_dwAddr = (DWORD)hModule + szModuleSize;
	//fout << mEnd.m_dwAddr << endl;
	while (procList.Size() > 0 )
	{
		Proc mCur = procList.At(0);
		Proc mNext = mEnd;
		procList.Pop_Front();
		if (procList.Size() > 0 )
			mNext = procList.At(0);
		if (mNext.m_dwAddr <= mCur.m_dwAddr) // WTF? two different function has the same address...
		{
			continue;
		} // if

		
		CodeSeg csdCode;
		csdCode.m_iLength = 0;
		csdCode.m_Module.m_strName = strDLL;
		csdCode.m_Module.m_dwAddr = (DWORD)hModule;
		csdCode.m_Proc.m_dwAddr = mCur.m_dwAddr;
		csdCode.m_Proc.m_strName = mCur.m_strName;
		if (!compareFunction(csdCode, mCur.m_dwAddr, mNext.m_dwAddr))
		{

		} // if

	} // while

	if (b_NeedRemoveDLL)
		FreeLibrary(hModule);
	Finishdisasm();
	CloseProcessHandle();
	//fout.close();
	procList.Clear();
	return true;
}

void Process::GetPermission()
{

	OpenProcessHandle();
	DWORD dwNeed = 0;
	HMODULE* hAllModule = new HMODULE[1000];
	//MyEnumProcessModulesEX EnumProcessModulesEX = (MyEnumProcessModulesEX)GetProcAddress(GetModuleHandle("Psapi.dll"), "EnumProcessModules");
	int er = EnumProcessModules(m_hProcess, hAllModule, 1000, (PDWORD)&dwNeed);
	int count = dwNeed / sizeof(DWORD);
	if (count > 0)
		isAllow = true;
	else
		isAllow = false;
	CloseProcessHandle();
}

BOOL Is64BitWindows()
{
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL f64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
	return FALSE; // Win64 does not support Win16
#endif
}

bool EnumAllProcesses(vector<Process>& vec_ProcessList)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 proc32;// Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == NULL)
	{
		MessageBox(NULL, "get processes failed", "ERROR", MB_OK);
		return false;
	} // if

	proc32.dwSize = sizeof(proc32);
	if (!Process32First(hSnapshot, &proc32))
	{
		MessageBox(NULL, "get processes failed", "ERROR", MB_OK);
		return false;
	} // if

	do
	{
		if (strlen(proc32.szExeFile) > 0)
		{
			Process tmp(proc32.th32ProcessID, proc32.szExeFile, NULL);
			tmp.GetPath();
			tmp.GetIcon();
			tmp.OpenProcessHandle();
			SYSTEM_INFO si;
			GetNativeSystemInfo(&si);
			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_IA64)
			{   // 64 位操作系统 
				IsWow64Process(tmp.m_hProcess, &tmp.is32Bit);
			} // if
			else
			{   // 32 位操作系统 
				tmp.is32Bit = true;
			} // else

			tmp.GetPermission();
			tmp.Clear();
			vec_ProcessList.push_back(tmp);
		}
	} while (Process32Next(hSnapshot, &proc32));

	return true;
}

bool isModuleBeenLoaded(string strDLL)
{
	HMODULE hDLL = GetModuleHandle(strDLL.c_str());
	if (hDLL == NULL) {
		LoadLibraryA(strDLL.c_str());
		hDLL = GetModuleHandle(strDLL.c_str());
		if (hDLL == NULL)
		{
			MessageBox(NULL, strDLL.c_str(), "Can't find this module.", MB_OK);
			return true;
		} // if

		return false; // need free
	} // if

	return true;
}