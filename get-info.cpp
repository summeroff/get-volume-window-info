#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>

#include <Windows.h>

HHOOK hhook = 0;

LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	std::cout << "GetMsgProc" << nCode << std::endl;
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}

std::map<std::string, int> WS_map = {
	{"WS_OVERLAPPED      ", 0x00000000L},
	{"WS_POPUP           ", 0x80000000L},
	{"WS_CHILD           ", 0x40000000L},
	{"WS_MINIMIZE        ", 0x20000000L},
	{"WS_VISIBLE         ", 0x10000000L},
	{"WS_DISABLED        ", 0x08000000L},
	{"WS_CLIPSIBLINGS    ", 0x04000000L},
	{"WS_CLIPCHILDREN    ", 0x02000000L},
	{"WS_MAXIMIZE        ", 0x01000000L},
	{"WS_CAPTION         ", 0x00C00000L},
	{"WS_BORDER          ", 0x00800000L},
	{"WS_DLGFRAME        ", 0x00400000L},
	{"WS_VSCROLL         ", 0x00200000L},
	{"WS_HSCROLL         ", 0x00100000L},
	{"WS_SYSMENU         ", 0x00080000L},
	{"WS_THICKFRAME      ", 0x00040000L},
	{"WS_GROUP           ", 0x00020000L},
	{"WS_TABSTOP         ", 0x00010000L},
	{"WS_MINIMIZEBOX     ", 0x00020000L},
	{"WS_MAXIMIZEBOX     ", 0x00010000L},
};


std::map<std::string, int> WS_EX_map = {
  {"WS_EX_DLGMODALFRAME    ", 0x00000001L},
  {"WS_EX_NOPARENTNOTIFY   ", 0x00000004L},
  {"WS_EX_TOPMOST          ", 0x00000008L},
  {"WS_EX_ACCEPTFILES      ", 0x00000010L},
  {"WS_EX_TRANSPARENT      ", 0x00000020L},
  {"WS_EX_MDICHILD         ", 0x00000040L},
  {"WS_EX_TOOLWINDOW       ", 0x00000080L},
  {"WS_EX_WINDOWEDGE       ", 0x00000100L},
  {"WS_EX_CLIENTEDGE       ", 0x00000200L},
  {"WS_EX_CONTEXTHELP      ", 0x00000400L},
  {"WS_EX_RIGHT            ", 0x00001000L},
  {"WS_EX_LEFT             ", 0x00000000L},
  {"WS_EX_RTLREADING       ", 0x00002000L},
  {"WS_EX_LTRREADING       ", 0x00000000L},
  {"WS_EX_LEFTSCROLLBAR    ", 0x00004000L},
  {"WS_EX_RIGHTSCROLLBAR   ", 0x00000000L},
  {"WS_EX_CONTROLPARENT    ", 0x00010000L},
  {"WS_EX_STATICEDGE       ", 0x00020000L},
  {"WS_EX_APPWINDOW        ", 0x00040000L},
  {"WS_EX_LAYERED          ", 0x00080000L},
  {"WS_EX_NOINHERITLAYOUT  ", 0x00100000L}, 
  {"WS_EX_NOREDIRECTIONBITMAP", 0x00200000L},
  {"WS_EX_LAYOUTRTL        ", 0x00400000L}, 
  {"WS_EX_COMPOSITED       ", 0x02000000L},
  {"WS_EX_NOACTIVATE       ", 0x08000000L}
};

void print_to_hex_flags(const std::string & flags_type, DWORD dwStyle, const std::map<std::string, int> & flags_map)
{
	DWORD flag = 1;
	while (dwStyle)
	{
		if (dwStyle & flag)
		{
			std::string flag_name = "";

			auto p = std::find_if(std::begin(flags_map), std::end(flags_map), [&flag](auto const & n)->bool { return n.second == flag; });
			if (p != flags_map.end())
			{
				flag_name = p->first;
			}

			std::cout << "0x"<< std::hex << std::setfill('0') << std::setw(8) << flag << " - "<< flag_name << std::endl;
		}

		dwStyle &= ~flag;
		flag = flag << 1;
	}
}

void print_window_info(HWND window_hwnd)
{
	WINDOWINFO wi;
	GetWindowInfo(window_hwnd, &wi);
	
	std::cout << std::dec << "window pos [" << wi.rcWindow.left << ","<< wi.rcWindow.top << "], size ["<< wi.rcWindow.right - wi.rcWindow.left <<"," << wi.rcWindow.bottom - wi.rcWindow.top << "]" << std::endl;

	std::cout << "window info dwExStyle " << std::hex << (DWORD)wi.dwExStyle << std::endl;
	print_to_hex_flags("dwStyle ", wi.dwStyle, WS_map);

	std::cout << "window info dwStyle " << std::hex << (DWORD)wi.dwStyle << std::endl;
	print_to_hex_flags("dwExStyle ", wi.dwExStyle, WS_EX_map);
}

int main(int argc, char* argv[])
{
	HWND first_window = 0;
	HWND volume_window = 0;
	
	while (first_window = FindWindowExA(0, first_window, "NativeHWNDHost", ""))
	{
		std::cout << std::endl << "first found " << (DWORD)first_window << std::endl;
		print_window_info(first_window);

		if (volume_window = FindWindowExA(first_window, 0, "DirectUIHWND", ""))
		{
			std::cout << std::endl << "volume found " << (DWORD)volume_window << std::endl;
			print_window_info(volume_window);

			DWORD processID = 0;
			DWORD actualProcId = GetWindowThreadProcessId(volume_window, &processID);

			if (actualProcId) {
				std::cout << "Process ID is: " << processID << std::endl;
			}
			else {
				std::cout << "Unable to find the process id !" << std::endl;
			}

			hhook = SetWindowsHookEx(WH_GETMESSAGE, &GetMsgProc, (HINSTANCE)NULL, processID );
		}
	}
	
	Sleep(3000);

	if (hhook)
	{
		UnhookWindowsHookEx(hhook);
	}
	return 0;
}