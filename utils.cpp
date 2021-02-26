#include "utils.h"

namespace utils
{
	std::string xor_string(const std::string& str)
	{
		char vals[] = { '\r', '\n', 3, 5, 6, 7, 8 };
		int val_i = 0;
		auto inc_i = [&vals](int& i) -> void
		{
			++i;
			if (i == sizeof(vals))
			{
				i = 0;
			}
		};
		std::string res;
		for (const auto& i : str)
		{
			res += i ^ vals[val_i];
			inc_i(val_i);
		}
		return res;
	}

	DWORD get_process_by_name(const wchar_t* procName)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);		
		if (hSnapshot)
		{
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hSnapshot, &pe32))
			{
				do
				{
					if (!_wcsicmp(procName, pe32.szExeFile))
					{
						CloseHandle(hSnapshot);
						return pe32.th32ProcessID;
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
		}
		return NULL;
	}


	uintptr_t get_module_base_address(DWORD procId, const wchar_t* modName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);
			if (Module32First(hSnap, &modEntry))
			{
				do
				{
					if (!_wcsicmp(modEntry.szModule, modName))
					{
						CloseHandle(hSnap);
						return (uintptr_t)modEntry.modBaseAddr;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}
		return NULL;
	}
}