NightShiftUnlocker
==================
### Introduction
A Lilu plugin that unlocks the Night Shift function for all Macs.  
You don't have to manually modify and codesign the CoreBrightness binary any longer.

### Requirement
NightShiftUnlocker requires macOS Sierra 10.12.4 or later and [Lilu](https://github.com/vit9696/Lilu) 1.1.5 or later.

### Installation
Disable the System Integrity Protection.  
Download the latest version of [Lilu](https://github.com/vit9696/Lilu/releases) and [NightShiftUnlocker](https://github.com/Austere-J/NightShiftUnlocker/releases).  
Install the kexts to /Library/Extensions/ using your favourite kext installer (e.g. [KCPM Utility Pro](https://www.firewolf.science/2016/09/kcpm-utility-pro-v6-brand-new-kexts-ezinstaller-macos-sierra-supported-repairing-permissions-configuring-rootless-and-more/), [Kext Utility](http://cvad-mac.narod.ru/index/0-4), etc.) or command line.
Repair the file permissions and rebuild the kernel cache.  
Reboot.

### Issue
Feel free to report any bugs or ask questions.

### How to compile
All Lilu related headers and library can be found at the latest **DEBUG** version of Lilu.

### Background Info
Apple introduced the Night Shift function as of macOS 10.12.4.  
Night Shift is provided by a private system framework named CoreBrightness.
It uses a global function `CBU_IsNightShiftSupported()` and an array of supported Mac models to check whether the running machine supports Night Shift.  
Instead of modifying the minimum version of supported Mac models, I make this function always return `true`.

### Update Logs
#### 2.1 @ 2017.06.29
- Fix an issue that Night Shift panel may not show up in the System Preference >> Display.

#### 2.0 @ 2017.06.28
- Dynamically generate binary patches. (Thanks to vit9696's new APIs)
- Supports the latest macOS High Sierra.
- Supports Clover's kext injection.
##### **Known issue in this version**
Night Shift panel may not show up in the System Preference >> Display on some machines.
I have already noticed this issue, and I am still working on this.

#### 1.0 @ 2017.05.20
- Initial release.

### License
NightShiftUnlocker is licensed under BSD 3-Clause.

### Credits
- [Apple](https://www.apple.com) for Night Shift
- [vit9696](https://github.com/vit9696) for [Lilu](https://github.com/vit9696/Lilu)

#### NightShiftUnlocker is brought to you by [FireWolf](https://www.firewolf.science). Enjoy : )  

---

### 介绍
苹果在 macOS Sierra 10.12.4 中带来了 Night Shift 功能，但是有机型限制。  
NightShiftUnlocker 是一个为全系列 Mac 动态解锁此功能的插件。
用户安装后无需手动修改二进制里的最低机型以及重新签名等操作，并且也不会受到系统更新导致覆盖原来文件的限制。

### 系统要求
NightShiftUnlocker 需要 macOS 10.12.4 以及以上的系统，并需要 Lilu 1.1.5 版本。

### 使用方法
请先关闭 SIP 系统完整性保护功能  
从 [Lilu](https://github.com/vit9696/Lilu/releases) 和 [NightShiftUnlocker](https://github.com/Austere-J/NightShiftUnlocker/releases) 仓库的发布页面下载最新版本的内核扩展  
用命令行或者驱动安装工具([KCPM Utility Pro](https://www.firewolf.science/2016/09/kcpm-utility-pro-v6-brand-new-kexts-ezinstaller-macos-sierra-supported-repairing-permissions-configuring-rootless-and-more/), [Kext Utility](http://cvad-mac.narod.ru/index/0-4) 等)将两个内核扩展安装到 `/Library/Extensions/`.
修复权限，重建缓存，重新启动机器。

### 问题反馈
请使用 Github Issue 功能来反馈BUG或者提问题。

### 如何编译
NightShiftUnlocker 项目所使用的 Lilu 头文件和 API 均可以在 DEBUG 版本的 Lilu 里找到。

### 背景信息
Night Shift 作为 10.12.4 的新功能由系统私有框架 CoreBrightness.framework 来提供。  
苹果使用一个全局函数 `CBU_IsNightShiftSupported()` 和一个定义好的机型数组来判断当前机型是否支持此功能。  
所以我们可以直接让这个函数永远返回 `true` 来达到让任意 SMBIOS 支持 Night Shift 功能。  
如此用户不需要再手动去修改最低机型的定义以及重新签名二进制了，也不会受到系统升级导致原有二进制文件被覆盖的限制。  

### 更新日志
#### 2.1 @ 2017.06.29
- 修复 Night Shift 面板在系统偏好设置里不显示的问题

#### 2.0 @ 2017.06.28
- 动态生成二进制补丁 (感谢 vit9696 开发的新API)
- 支持最新的 macOS High Sierra
- 支持 Clover 的驱动注入功能
##### **已知问题**
在某些机器上，Night Shift 面板可能不会在『系统偏好设置 >> 显示』里面显示。
我已经注意到这个问题，正在尝试解决中...

#### 1.0 @ 2017.05.20
- 初始版本

### 授权许可
NightShiftUnlocker 使用 BSD 3-Clause 许可

### 鸣谢与其他
NightShiftUnlocker 的实现离不开 [vit9696](https://github.com/vit9696) 的 [Lilu](https://github.com/vit9696/Lilu) 内核扩展

#### NightShiftUnlocker 由 [FireWolf](https://www.firewolf.science) 为您提供，ENJOY : )
