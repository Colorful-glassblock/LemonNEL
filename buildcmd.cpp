#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

// 计算最大公因数
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// 计算多个数的最大公因数
int calculateGCD(const std::vector<int>& numbers) {
    if (numbers.empty()) return 1;
    
    int result = numbers[0];
    for (size_t i = 1; i < numbers.size(); i++) {
        result = gcd(result, numbers[i]);
    }
    return result;
}

// 确保输出目录存在
bool ensureOutputDirectory(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "创建输出目录失败: " << e.what() << std::endl;
        return false;
    }
}

// 将字符串转换为16进制并直接分块保存（修复版本）
void processAndSaveHex(const std::string& inputText, const std::string& outputDir, int blockCount) {
    if (inputText.empty()) {
        std::cerr << "错误：输入文本为空" << std::endl;
        return;
    }
    
    // 计算16进制总长度
    int hexTotalLength = inputText.length() * 2;
    int blockSize = hexTotalLength / blockCount;
    
    std::cout << "输入文本长度: " << inputText.length() << " 个字符" << std::endl;
    std::cout << "16进制总长度: " << hexTotalLength << " 个字符" << std::endl;
    std::cout << "分块数量: " << blockCount << std::endl;
    std::cout << "每块大小: " << blockSize << " 个字符" << std::endl;
    
    // 创建并打开所有输出文件
    std::vector<std::ofstream> outputFiles;
    std::vector<std::string> filenames;
    
    for (int i = 0; i < blockCount; i++) {
        std::string filename = outputDir + "/" + 
                              std::string(3 - std::to_string(i + 1).length(), '0') + 
                              std::to_string(i + 1);
        filenames.push_back(filename);
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "错误：无法创建文件 " << filename << std::endl;
            return;
        }
        outputFiles.push_back(std::move(file));
    }
    
    // 处理每个字符并写入对应的块
    int currentBlock = 0;
    int currentPosition = 0;
    
    for (size_t i = 0; i < inputText.length(); i++) {
        unsigned char c = static_cast<unsigned char>(inputText[i]);
        
        // 转换为16进制
        std::stringstream hexStream;
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        std::string hexStr = hexStream.str();
        
        // 写入当前块
        if (currentBlock < blockCount) {
            outputFiles[currentBlock] << hexStr;
        }
        
        currentPosition += 2;
        
        // 检查是否需要切换到下一个块
        if (currentPosition >= (currentBlock + 1) * blockSize && currentBlock < blockCount - 1) {
            currentBlock++;
        }
    }
    
    // 显式关闭所有文件
    for (auto& file : outputFiles) {
        if (file.is_open()) {
            file.flush();  // 强制刷新缓冲区
            file.close();  // 显式关闭文件
        }
    }
    
    // 验证文件是否成功写入
    std::cout << "\n文件保存验证:" << std::endl;
    for (int i = 0; i < blockCount; i++) {
        std::ifstream checkFile(filenames[i], std::ios::binary | std::ios::ate);
        if (checkFile.is_open()) {
            std::streamsize size = checkFile.tellg();
            checkFile.close();
            std::cout << "文件 " << filenames[i] << " 大小: " << size << " 字节" << std::endl;
        } else {
            std::cerr << "警告：无法验证文件 " << filenames[i] << std::endl;
        }
    }
}

// 读取输入文本
std::string readInputText() {
    std::string inputText;
    std::string line;
    
    std::cout << "请输入文本（支持多行，Ctrl+D结束输入）:" << std::endl;
    
    // 恢复cin的正常状态
    std::cin.clear();
    std::cin >> std::skipws;
    
    while (std::getline(std::cin, line)) {
        inputText += line + "\n";
    }
    
    // 清除错误状态
    std::cin.clear();
    
    // 移除最后一个多余的换行符（如果有）
    if (!inputText.empty() && inputText.back() == '\n') {
        inputText.pop_back();
    }
    
    return inputText;
}

// 简单测试函数，用于验证
void testWithSampleData() {
    std::cout << "\n=== 测试模式 ===" << std::endl;
    
    // 创建测试数据
    std::string testData = "Hello World! This is a test message for hex conversion.";
    
    // 确保输出目录存在
    ensureOutputDirectory("test_out");
    
    // 计算分块数量
    int hexLength = testData.length() * 2;
    std::vector<int> factors;
    for (int i = 1; i <= hexLength; i++) {
        if (hexLength % i == 0) {
            factors.push_back(i);
        }
    }
    int commonGCD = calculateGCD(factors);
    int blockCount = hexLength / commonGCD;
    
    std::cout << "测试数据: " << testData << std::endl;
    std::cout << "分块数量: " << blockCount << std::endl;
    
    processAndSaveHex(testData, "test_out", blockCount);
}

int main() {
    std::cout << "16进制分块保存程序" << std::endl;
    std::cout << "==================" << std::endl;
    
    // 先运行测试
    testWithSampleData();
    
    std::cout << "\n=== 正式运行 ===" << std::endl;
    
    // 读取用户输入
    std::string inputText = readInputText();
    
    if (inputText.empty()) {
        std::cout << "输入为空，程序结束。" << std::endl;
        return 0;
    }
    
    std::cout << "成功读取输入，长度: " << inputText.length() << " 个字符" << std::endl;
    
    // 计算16进制长度
    int hexLength = inputText.length() * 2;
    std::cout << "预计16进制长度: " << hexLength << " 个字符" << std::endl;
    
    // 计算所有可能因数的最大公因数
    std::vector<int> factors;
    for (int i = 1; i <= hexLength && i <= 1000; i++) {
        if (hexLength % i == 0) {
            factors.push_back(i);
        }
    }
    
    if (factors.empty()) {
        std::cout << "错误：无法计算因数" << std::endl;
        return 1;
    }
    
    int commonGCD = calculateGCD(factors);
    std::cout << "最大公因数: " << commonGCD << std::endl;
    
    // 计算分块数量
    int blockCount = hexLength / commonGCD;
    
    // 限制分块数量在合理范围内
    if (blockCount > 100) {
        std::cout << "分块数量过多 (" << blockCount << ")，调整为10块" << std::endl;
        blockCount = 10;
    } else if (blockCount < 1) {
        blockCount = 1;
    }
    
    std::cout << "最终分块数量: " << blockCount << std::endl;
    
    // 确保输出目录存在
    if (!ensureOutputDirectory("out")) {
        std::cerr << "无法创建输出目录，程序结束。" << std::endl;
        return 1;
    }
    
    // 处理并保存文件
    std::cout << "开始处理并保存文件..." << std::endl;
    processAndSaveHex(inputText, "out", blockCount);
    
    std::cout << "\n处理完成！请检查 out/ 目录下的文件。" << std::endl;
    
    return 0;
}
