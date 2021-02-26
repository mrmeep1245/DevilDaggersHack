#pragma once
#include <string>
#include <windows.h>
#include <TlHelp32.h>

namespace utils
{
	std::string xor_string(const std::string& str);
	DWORD get_process_by_name(const wchar_t* procName);
	uintptr_t get_module_base_address(DWORD procId, const wchar_t* modName);
} //namespace utils