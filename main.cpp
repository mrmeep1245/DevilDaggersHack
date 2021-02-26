#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <bitset>
#include <TlHelp32.h>
#include "utils.h"
using namespace std;

uintptr_t base = 0xFF774D87090;

struct game_addresses
{
	uintptr_t gem_address;
	uintptr_t points_address;
	uintptr_t anti_cheat_address;
};

auto main() -> int
{
	DWORD procId = utils::get_process_by_name(L"dd.exe");
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

	char anti_cheat_opcode_bac = 0x74;
	char anti_cheat_opcode_bypass = 0xEB;
	bool anti_cheat_disabled = false;

	if (!proc)
	{
		cout << "Game isn't running\n";
	}
	else
	{
		game_addresses addresses;
		cout << "Found Process id: " << procId << '\n';
		uintptr_t base_addr = utils::get_module_base_address(procId, L"dd.exe");
		addresses.gem_address = base_addr + 0x221340;
		addresses.points_address = base_addr + 0x2214C0;

		//gems
		DWORD gems_count = 0;
		uintptr_t new_address;
		if (ReadProcessMemory(proc, (void*)addresses.gem_address, &new_address, sizeof(new_address), NULL))
		{
			addresses.gem_address = new_address;
			if (ReadProcessMemory(proc, (void*)addresses.gem_address, &new_address, sizeof(new_address), NULL))
			{
				addresses.gem_address = new_address;
				addresses.gem_address += 0x60;
				if (ReadProcessMemory(proc, (void*)addresses.gem_address, &new_address, sizeof(new_address), NULL))
				{
					addresses.gem_address = new_address;
					addresses.gem_address += 0x354;
					ReadProcessMemory(proc, (void*)addresses.gem_address, &gems_count, sizeof(gems_count), NULL);
				}
			}
		}

		DWORD points_count = 0;
		if (ReadProcessMemory(proc, (void*)addresses.points_address, &new_address, sizeof(new_address), NULL))
		{
			addresses.points_address = new_address;
			if (ReadProcessMemory(proc, (void*)addresses.points_address, &new_address, sizeof(new_address), NULL))
			{
				addresses.points_address = new_address;
				addresses.points_address += 0x360;
				ReadProcessMemory(proc, (void*)addresses.points_address, &points_count, sizeof(points_count), NULL);
			}
		}

		cout << hex << "BASE: " << base_addr << '\n';
		cout << "GEMS (address): " << addresses.gem_address << '\n'
			<< "POINTS (address): " << addresses.points_address << '\n'
			<< "GEMS (count): " << gems_count << '\n'
			<< "POINTS (count): " << points_count << '\n';

		cout << "Press F2 to add gems\n"
			"Press F3 to add points\n"
			"Press F6 to disable cheking for cheats\n";

		while (true)
		{
			if ((GetKeyState(VK_F2) & 0x8000))
			{
				DWORD val = 0xffffff;
				if (WriteProcessMemory(proc, (void*)addresses.gem_address, &val, sizeof(val), NULL))
				{
					cout << "Gems amount set\n";
				}
			}
			if ((GetKeyState(VK_F3) & 0x8000))
			{
				DWORD val = 0xffffff;
				if (WriteProcessMemory(proc, (void*)addresses.points_address, &val, sizeof(val), NULL))
				{
					cout << "Points amount set\n";
				}
			}

			//if ((GetKeyState(VK_F6) & 0x8000))
			//{
			//	if (anti_cheat_disabled)
			//	{
			//		anti_cheat_disabled = false;
			//		if (WriteProcessMemory(proc, (void*)addresses.anti_cheat_address, &anti_cheat_opcode_bac, sizeof(anti_cheat_opcode_bac), NULL))
			//		{
			//			cout << "Anti-cheat enabled\n";
			//		}
			//	}
			//	else
			//	{
			//		anti_cheat_disabled = true;
			//		if (WriteProcessMemory(proc, (void*)addresses.anti_cheat_address, &anti_cheat_opcode_bypass, sizeof(anti_cheat_opcode_bypass), NULL))
			//		{
			//			cout << "Anti-cheat disabled\n";
			//		}
			//	}

			//}
			Sleep(100);
		}
	}

	return 0;
}