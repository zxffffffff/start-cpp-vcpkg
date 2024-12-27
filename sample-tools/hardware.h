/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "cpp_version.h"
#include <thread>
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <cassert>

#ifdef _WIN32
#include <Winsock2.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#elif __APPLE__
#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#if (MAC_OS_X_VERSION_MAX_ALLOWED < 120000) // Before macOS 12 Monterey
#define kIOMainPortDefault kIOMasterPortDefault
#endif
#elif __linux__
#include "sys/sysinfo.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#else
#error "Unknown compiler"
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Hardware
{
public:
    /* 获取 CPU 逻辑核心数 */
    static int GetCPUs()
    {
        /* 硬件支持的并发线程数，失败返回0 */
        int n = std::thread::hardware_concurrency();

#ifdef _WIN32
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        // logical cpu
        assert(sysInfo.dwNumberOfProcessors == n);
        return sysInfo.dwNumberOfProcessors;

#elif __APPLE__
        uint64_t logicalcpu = 0;
        size_t logicalcpu_size = sizeof(logicalcpu);
        sysctlbyname("hw.logicalcpu", &logicalcpu, &logicalcpu_size, nullptr, 0);
        // or "hw.physicalcpu"
        assert(logicalcpu == n);
        return logicalcpu;

#elif __linux__
        int nprocs = get_nprocs();
        // or get_nprocs_conf()
        assert(nprocs == n);
        return nprocs;
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
                {
                    // boost::split(result, str, boost::is_any_of("-"));
                    std::stringstream ss(str);
                    std::string item;
                    while (std::getline(ss, item, '-'))
                        result.push_back(item);
                }
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
        io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMainPortDefault, "IOService:/");
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
                if (virtual_mac_prefix.count(mac_prefix))
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
};
