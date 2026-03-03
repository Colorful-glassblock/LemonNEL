# LemonNEL 🍋
一个简易的开源指令提权并免杀工具，使用 `GPL v3` 开源协议，采用 `C++17` 标准

Made by @Colorful-glassblock and You

---

## 🚀 快速开始

### 1. 克隆仓库
```bash
git clone https://github.com/Colorful-glassblock/LemonNEL.git
```

### 2. 编译程序
使用 **MinGW** 编译两个 `.cpp` 文件：

**编译 buildcmd.cpp：**
```bash
g++ -std=c++17 -o buildcmd.exe buildcmd.cpp -static
```

**编译 test1.cpp：**
```bash
g++ -std=c++17 -o run.exe run.cpp -ladvapi32 -lshell32 -luser32 -lkernel32 -static
```

### 3. 生成指令文件
运行 `buildcmd.exe`，输入你想要提权执行的指令（支持多行），例如：
```batch
mkdir C:\test
del "C:\test" /f /s /q
```

输入完成后：
- **Windows 用户**：按 `Ctrl+Z`，然后按 `Enter`
- **类 Unix 系统用户**：按 `Ctrl+D`，然后按 `Enter`

### 4. 部署执行
将生成的 `out` 文件夹复制到 `run.exe` 的同级目录，然后运行 `run.exe`。

### 5. ALL DONE！🎉

---

## ⚠️ 注意事项
- 请遵守当地法律法规，仅用于授权测试
- 确保编译时使用 `-static` 参数
- 运行 `run.exe` 时需要输入验证码 `y`

---

## 📄 开源协议
本项目基于 **GPL v3** 开源协议发布。详情请参阅 [LICENSE](LICENSE) 文件。

---

*简单 · 高效 · 安全*
