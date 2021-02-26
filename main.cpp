#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include "utils.h"

using namespace std;

struct game_addresses
{
	uintptr_t gem_address;
	uintptr_t points_address;	
	uintptr_t kills_address;
	uintptr_t timer_address;
};

auto main() -> int
{
	DWORD procId = utils::get_process_by_name(L"dd.exe");
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

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
		addresses.kills_address = base_addr + 0x226BD0;
		addresses.timer_address = base_addr + 0x226BD0;
		
		uintptr_t new_address;

		addresses.gem_address = utils::read_memory_offsets(proc, addresses.gem_address, 0x0ull, 0x60ull, 0x354ull);
		addresses.points_address = utils::read_memory_offsets(proc, addresses.points_address,0x0ull, 0x360ull);
		addresses.kills_address = utils::read_memory_offsets(proc, addresses.kills_address, 0x340ull);
		addresses.timer_address = utils::read_memory_offsets(proc, addresses.timer_address, 0x32Cull);

		cout << hex << "BASE: " << base_addr << '\n'
					<< "GEMS (address): " << addresses.gem_address << '\n'
					<< "POINTS (address): " << addresses.points_address << '\n'
					<< "TIMER (address): " << addresses.timer_address << '\n'
					<< "KILLS (address): " << addresses.kills_address << '\n'
					<< dec;

		cout << "Press F2 to add gems\n"
				"Press F3 to add points\n"
				"Press F4 to add kills\n"
				"Press F5 to increase timer\n";

		while (true)
		{
			//add gems
			if ((GetKeyState(VK_F2) & 0x8000))
			{
				DWORD val = 0xffffff;
				if (WriteProcessMemory(proc, (void*)addresses.gem_address, &val, sizeof(val), NULL))
				{
					cout << "Gems amount set\n";
				}
			}
			//add points
			if ((GetKeyState(VK_F3) & 0x8000))
			{
				DWORD val = 0xffffff;
				if (WriteProcessMemory(proc, (void*)addresses.points_address, &val, sizeof(val), NULL))
				{
					cout << "Points amount set\n";
				}
			}
			//add kills
			if ((GetKeyState(VK_F4) & 0x8000))
			{
				DWORD kills_value = 0;
				ReadProcessMemory(proc, (void*)addresses.kills_address, &kills_value, sizeof(kills_value), NULL); // reading kill count value
				kills_value += 100; // increasing kill count value
				if (WriteProcessMemory(proc, (void*)addresses.kills_address, &kills_value, sizeof(kills_value), NULL))
				{
					cout << "Kill count increased. New value: " << kills_value << '\n';
				}
			}
			//increase timer
			if ((GetKeyState(VK_F5) & 0x8000))
			{
				float timer_value = 0;
				ReadProcessMemory(proc, (void*)addresses.timer_address, &timer_value, sizeof(timer_value), NULL); // reading timer value
				timer_value += 100.0;
				if (WriteProcessMemory(proc, (void*)addresses.timer_address, &timer_value, sizeof(timer_value), NULL))
				{
					cout << "Timer value increased\n";
				}
			}
			Sleep(100);
		}
	}
	return 0;
}