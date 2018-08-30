#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime>

DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress);
void WriteToMemory(HANDLE hProcHanle);

using namespace std;

string GameName = "AssaultCube";
LPCSTR GameWindow_ptr = "AssaultCube";
string GameStatus;

bool isGameAvail;
bool UpdateOnNextRun;

/*Ammo Varuables*/
bool AmmoStatus;
BYTE AmmoValue[] = { 0xA3, 0x1C, 0x0, 0x0 };
DWORD AmmoBaseAdress = 0x509B74;
DWORD AmmoOffsets[] = {0x384, 0x14, 0x0};

/*Health Varuables*/
bool HealthStatus;
BYTE HealthValue[] = { 0x39, 0x5, 0x0, 0x0 };
DWORD HealthBaseAdress = 0x509B74;
DWORD HealthOffsets[] = { 0xF8 };


int main()
{
	HWND hGameWindow = NULL;
	int timeSinceLastUpdate = clock();
	int GameAvailTMR = clock();
	int onePressTMR = clock();
	DWORD dwProcID = NULL;
	HANDLE hProcHandle = NULL;
	UpdateOnNextRun = true;
	string sAmmoStatus = "OFF";
	string sHealthStatus = "OFF";


	while (!GetAsyncKeyState(VK_INSERT))
	{

		if (clock() - GameAvailTMR > 100)
		{
			GameAvailTMR = clock();
			isGameAvail = false;
			hGameWindow = FindWindowA(NULL, GameWindow_ptr);
			if (hGameWindow != NULL)
			{
				GetWindowThreadProcessId(hGameWindow, &dwProcID);
				if (dwProcID != NULL)
				{
					hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
					if (hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL)
					{
						GameStatus = "Failed to open process for valid handle";
					}
					else
					{
						GameStatus = "AssaulCube ready to hack";
						isGameAvail = true;
					}

				}
				else
				{
					GameStatus = "Failed to get get Process ID";
				}

			}
			else
			{
				GameStatus = "AssaultCube NOT FOUND!";
			}

			if (UpdateOnNextRun || clock() - timeSinceLastUpdate > 5000)
			{
				system("cls");
				cout << "-------------------------------------------------------------------------------" << endl;
				cout << "                    Assault cube memory hacker                      " << endl;
				cout << "-------------------------------------------------------------------------------" << endl << endl;
				cout << "GAME STATUS: " << GameStatus << endl << endl;
				cout << "[F1] Unlimited ammo ->" << sAmmoStatus << " <-" << endl << endl;
				cout << "[F2] Unlimited health ->" << sHealthStatus << " <-" << endl << endl;
				cout << "[INSERT] Exit" << endl;

				UpdateOnNextRun = false;

				timeSinceLastUpdate = clock();

			}

			if (isGameAvail == true)
			{
				WriteToMemory(hProcHandle);

			}
		}

		if (clock() - onePressTMR > 400)
		{
			if (isGameAvail == true)
			{
				if (GetAsyncKeyState(VK_F1))
				{
					onePressTMR = clock();
					AmmoStatus = !AmmoStatus;
					UpdateOnNextRun = true;

					if (AmmoStatus == true) sAmmoStatus = "ON";
					else sAmmoStatus = "OFF";
				}

				if (GetAsyncKeyState(VK_F2))
				{
					onePressTMR = clock();
					HealthStatus = !HealthStatus;
					UpdateOnNextRun = true;

					if (HealthStatus == true) sHealthStatus = "ON";
					else sHealthStatus = "OFF";
				}
			}
		}
	}
	/*Closing oppened stuff*/
	CloseHandle(hProcHandle);
	DestroyWindow(hGameWindow);

	return ERROR_SUCCESS;
}


DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[],DWORD BaseAddress)
{
	DWORD pointer = BaseAddress;
	DWORD pTemp;

	DWORD pointerAddr;
	for (int c = 0; c < PointerLevel; ++c)
	{
		if (c == 0)
		{
			ReadProcessMemory(hProcHandle, (LPCVOID)BaseAddress, &pTemp, sizeof(pTemp), NULL);
		}

		pointerAddr = pTemp + Offsets[c];
		ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, sizeof(pTemp), NULL);
	}
	return pointerAddr;

}


void WriteToMemory(HANDLE hProcHandle)
{
	DWORD AdressToWrite;
	if (AmmoStatus == true)
	{
		AdressToWrite = FindDmaAddy(3, hProcHandle, AmmoOffsets, AmmoBaseAdress);
		WriteProcessMemory(hProcHandle, (LPVOID)AdressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);
	}

	if (HealthStatus == true)
	{
		AdressToWrite = FindDmaAddy(1, hProcHandle, HealthOffsets, HealthBaseAdress);
		WriteProcessMemory(hProcHandle, (LPVOID)AdressToWrite, &HealthValue, sizeof(HealthValue), NULL);
	}
}