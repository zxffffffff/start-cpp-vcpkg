/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32
#include <Winsock2.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#elif __APPLE__
#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#elif __linux__
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/utsname.h>
#else
#error "Unknown compiler"
#endif

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#undef ERROR /* 和glog冲突 */
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Platform
{
public:
    /* utf-8 -> 本地编码（Windows:GB2312） */
    static std::string utf8_to_local(const std::string &str)
    {
#ifdef _WIN32
        if (str.empty())
            return str;

        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
        size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);

        while (result.size() && result.back() == '\0')
            result.pop_back();
        return result;
#else
        return str;
#endif
    }

    /* 本地编码（Windows:GB2312）-> utf-8 */
    static std::string local_to_utf8(const std::string &str)
    {
#ifdef _WIN32
        if (str.empty())
            return str;

        int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, L'\0');
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size);
        size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);

        while (result.size() && result.back() == '\0')
            result.pop_back();
        return result;
#else
        return str;
#endif
    }

    /* 使用mac地址作为设备唯一标识 (00-00-00-00-00-00)
     * 1、如果存在多个网卡，会将所有网卡mac地址相加
     * 2、如果存在虚拟网卡，会将其mac地址剔除，除非全部都是虚拟网卡
     */
    static std::string GetMacAddr()
    {
        std::string macAddr;

        const std::set<std::string> virtual_mac_prefix = {
            "00-05-69", "00-0C-29", "00-1C-14", "00-50-56",             /* VMware */
            "00-16-3E",                                                 /* Xen */
            "00-1C-42",                                                 /* Parallels */
            "00-0F-4B", "00-50-48", "00-50-FC",                         /* Virtual Iron */
            "08-00-27", "0A-00-27",                                     /* VirtualBox */
            "00-03-FF", "00-15-5D", "00-1D-D8", "00-1E-67", "00-23-7D", /* Hyper-V */
        };

        auto genMac = [](std::vector<std::string> strList) -> std::string
        {
            std::vector<uint8_t> retMac;
            for (auto &str : strList)
            {
                if (str.empty())
                    continue;
                std::vector<std::string> result;
                boost::split(result, str, boost::is_any_of("-"));
                if (retMac.empty())
                    retMac.resize(result.size(), 0);
                for (int i = 0; i < retMac.size(); ++i)
                    retMac[i] += std::strtol(result[i].c_str(), NULL, 16);
            }
            std::stringstream ss;
            for (int i = 0; i < retMac.size(); ++i)
            {
                if (i > 0)
                    ss << "-";
                ss << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(retMac[i]);
            }
            return ss.str();
        };

#ifdef _WIN32
        DWORD dwSize = 0;
        DWORD dwRetVal = 0;
        if (GetAdaptersInfo(NULL, &dwSize) == ERROR_BUFFER_OVERFLOW)
        {
            PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(dwSize);
            GetAdaptersInfo(pAdapterInfo, &dwSize);
            PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
            std::vector<std::string> macList, virtualList;
            while (pAdapter != NULL)
            {
                std::string macAddress;
                for (UINT i = 0; i < pAdapter->AddressLength; i++)
                {
                    if (i > 0)
                        macAddress += "-";
                    char macByte[3];
                    sprintf_s(macByte, "%02X", pAdapter->Address[i]);
                    macAddress += macByte;
                }

                std::string mac_prefix = macAddress.substr(0, 8);
                if (virtual_mac_prefix.find(mac_prefix) != virtual_mac_prefix.end())
                {
                    virtualList.push_back(macAddress);
                }
                else
                {
                    macList.push_back(macAddress);
                }
                pAdapter = pAdapter->Next;
            }

            if (!macList.empty())
            {
                macAddr = genMac(macList);
            }
            else if (!virtualList.empty())
            {
                macAddr = genMac(virtualList);
            }
            free(pAdapterInfo);
        }

#elif __APPLE__
        char buf[1024];
        io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
        CFStringRef uuidCf = (CFStringRef)IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
        IOObjectRelease(ioRegistryRoot);
        CFStringGetCString(uuidCf, buf, sizeof(buf), kCFStringEncodingMacRoman);
        CFRelease(uuidCf);
        macAddr = buf;

#elif __linux__
        ifconf ifc{0};
        ifreq ifr[100]{0};

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        ifc.ifc_len = 10 * sizeof(ifreq);
        ifc.ifc_ifcu.ifcu_buf = (char *)ifr;
        ioctl(sock, SIOCGIFCONF, (char *)&ifc);
        int if_count = ifc.ifc_len / sizeof(ifreq);

        std::vector<std::string> macList, virtualList;
        for (int i = 0; i < if_count; ++i)
        {
            if (ioctl(sock, SIOCGIFHWADDR, &ifr[i]) == 0)
            {
                unsigned char mac[6];
                memcpy(mac, ifr[i].ifr_ifru.ifru_hwaddr.sa_data, 6);

                std::stringstream ss;
                for (int i = 0; i < 6; ++i)
                {
                    if (i > 0)
                        ss << "-";
                    ss << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(mac[i]);
                }
                std::string macAddress = ss.str();
                std::string mac_prefix = macAddress.substr(0, 8);
                if (virtual_mac_prefix.contains(mac_prefix))
                {
                    virtualList.push_back(macAddress);
                }
                else
                {
                    macList.push_back(macAddress);
                }
            }
            if (!macList.empty())
            {
                macAddr = genMac(macList);
            }
            else if (!virtualList.empty())
            {
                macAddr = genMac(virtualList);
            }
        }
#endif
        return macAddr;
    }

    /* OS类型 */
    static std::string GetOSType()
    {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
        return "macOS";
#elif __linux__
        /* uname sysname 获取发行版名 */
        return "Linux";
#endif
    }

    /* OS版本号 */
    static std::string GetOSVersion()
    {
#ifdef _WIN32
        // KUSER_SHARED_DATA
        BYTE *sharedUserData = (BYTE *)0x7FFE0000;
        ULONG major = *(ULONG *)(sharedUserData + 0x26c);
        ULONG minor = *(ULONG *)(sharedUserData + 0x270);
        ULONG build = *(ULONG *)(sharedUserData + 0x260);
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build);
#elif __APPLE__
        size_t size = 0;
        sysctlbyname("kern.osrelease", nullptr, &size, nullptr, 0);
        char *sysctlResult = new char[size];
        sysctlbyname("kern.osrelease", sysctlResult, &size, nullptr, 0);
        std::string version(sysctlResult);
        delete[] sysctlResult;
        return version;
#elif __linux__
        struct utsname uts;
        uname(&uts);
        // CentOS 7 "3.10.0-1160.102.1.el7.x86_64"
        return uts.release;
#endif
    }

    /* 随机guid (00000000-0000-0000-0000-000000000000) */
    static std::string GenGuid()
    {
        boost::uuids::random_generator gen;
        return boost::uuids::to_string(gen());
    }
};
