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

// 密码验证
bool verifyPassword() {
    const std::string correctPassword = "114514";
    std::string inputPassword;
    
    std::getline(std::cin, inputPassword);
    
    return inputPassword == correctPassword;
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

// 使用RtlAdjustPrivilege提权
bool enableAllPrivilegesNt() {
    // 定义函数指针类型
    typedef UINT (WINAPI* typeRtlAdjustPrivilege)(ULONG, BOOL, BOOL, PINT);
    
    // 加载ntdll.dll
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        return false;
    }
    
    // 获取RtlAdjustPrivilege函数地址
    typeRtlAdjustPrivilege RtlAdjustPrivilege = 
        (typeRtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
    
    if (!RtlAdjustPrivilege) {
        return false;
    }
    
    // 特权常量定义
    const ULONG privileges[] = {
        2,   // SE_CREATE_TOKEN_PRIVILEGE
        3,   // SE_ASSIGNPRIMARYTOKEN_PRIVILEGE  
        4,   // SE_LOCK_MEMORY_PRIVILEGE
        5,   // SE_INCREASE_QUOTA_PRIVILEGE
        6,   // SE_MACHINE_ACCOUNT_PRIVILEGE
        7,   // SE_TCB_PRIVILEGE
        8,   // SE_SECURITY_PRIVILEGE
        9,   // SE_TAKE_OWNERSHIP_PRIVILEGE
        10,  // SE_LOAD_DRIVER_PRIVILEGE
        11,  // SE_SYSTEM_PROFILE_PRIVILEGE
        12,  // SE_SYSTEMTIME_PRIVILEGE
        13,  // SE_PROF_SINGLE_PROCESS_PRIVILEGE
        14,  // SE_INC_BASE_PRIORITY_PRIVILEGE
        15,  // SE_CREATE_PAGEFILE_PRIVILEGE
        16,  // SE_CREATE_PERMANENT_PRIVILEGE
        17,  // SE_BACKUP_PRIVILEGE
        18,  // SE_RESTORE_PRIVILEGE
        19,  // SE_SHUTDOWN_PRIVILEGE
        20,  // SE_DEBUG_PRIVILEGE
        21,  // SE_AUDIT_PRIVILEGE
        22,  // SE_SYSTEM_ENVIRONMENT_PRIVILEGE
        23,  // SE_CHANGE_NOTIFY_PRIVILEGE
        24,  // SE_REMOTE_SHUTDOWN_PRIVILEGE
        25,  // SE_UNDOCK_PRIVILEGE
        26,  // SE_SYNC_AGENT_PRIVILEGE
        27,  // SE_ENABLE_DELEGATION_PRIVILEGE
        28,  // SE_MANAGE_VOLUME_PRIVILEGE
        29,  // SE_IMPERSONATE_PRIVILEGE
        30,  // SE_CREATE_GLOBAL_PRIVILEGE
        31,  // SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE
        32,  // SE_RELABEL_PRIVILEGE
        33,  // SE_INC_WORKING_SET_PRIVILEGE
        34,  // SE_TIME_ZONE_PRIVILEGE
        35   // SE_CREATE_SYMBOLIC_LINK_PRIVILEGE
    };
    
    INT previousValue;
    bool allSuccess = true;
    
    // 启用所有特权
    for (ULONG privilege : privileges) {
        UINT result = RtlAdjustPrivilege(privilege, TRUE, FALSE, &previousValue);
        if (result != 0) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

// 创建系统权限进程
bool createSystemProcess(const std::string& command) {
    if (command.empty()) return true;

    // 首先使用RtlAdjustPrivilege提权
    enableAllPrivilegesNt();

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

    // 尝试以高权限创建进程
    BOOL success = CreateProcessA(
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

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }

    return false;
}

// 使用ShellExecute以最高权限执行
bool executeWithMaxPrivilege(const std::string& command) {
    if (command.empty()) return true;

    std::string fullCommand = "cmd.exe /c \"" + command + "\"";

    SHELLEXECUTEINFOA sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFOA);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpFile = "cmd.exe";
    sei.lpParameters = fullCommand.c_str();
    sei.nShow = SW_SHOW;
    sei.lpVerb = "runas";  // 请求管理员权限

    if (ShellExecuteExA(&sei)) {
        if (sei.hProcess) {
            CloseHandle(sei.hProcess);
        }
        return true;
    }

    return false;
}

// 为单条命令创建高权限进程
void createPrivilegedProcessForCommand(const std::string& command) {
    // 方法1: 使用RtlAdjustPrivilege提权后创建进程
    if (createSystem
