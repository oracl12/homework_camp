#include <iostream>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/statvfs.h>
#include <unistd.h>
#include <limits.h>
#endif


#ifdef __linux__
typedef struct {
    uint64_t QuadPart;
} ULARGE_INTEGER;
#endif

ULARGE_INTEGER* getDriveInfo() {
    std::vector<std::string> drivePaths;
    ULARGE_INTEGER totalSpace = {0};
    ULARGE_INTEGER freeSpace = {0};

#ifdef _WIN32
    WCHAR drives[1024];
    DWORD drivesCount = GetLogicalDriveStringsW(sizeof(drives) / sizeof(drives[0]), drives);

    if (drivesCount == 0) {
        std::cerr << "Error getting logical drive strings." << std::endl;
        return nullptr;
    }

    for (WCHAR* drive = drives; *drive != L'\0'; drive += wcslen(drive) + 1) {
        std::wstring drivePath(drive);
        drivePaths.push_back(std::string(drivePath.begin(), drivePath.end()));
    }

    for (const std::string& drivePath : drivePaths) {
        ULARGE_INTEGER total, free, dummy;
        if (GetDiskFreeSpaceExA(drivePath.c_str(), &dummy, &total, &free)) {
            totalSpace.QuadPart += total.QuadPart;
            freeSpace.QuadPart += free.QuadPart;
        } else {
            std::cerr << "Error getting disk space information for drive " << drivePath << std::endl;
        }
    }
#else
    struct statvfs buf;
    if (statvfs("/", &buf) == 0) {
        totalSpace.QuadPart = (buf.f_frsize * buf.f_blocks);
        freeSpace.QuadPart = (buf.f_frsize * buf.f_bfree);
    } else {
        std::cerr << "Error getting disk space information." << std::endl;
        return nullptr;
    }
#endif

    ULARGE_INTEGER* total = new ULARGE_INTEGER[2];
    total[0].QuadPart = totalSpace.QuadPart / (1024 * 1024);
    total[1].QuadPart = freeSpace.QuadPart / (1024 * 1024);
    return total;
}

char* getPcName() {
#ifdef _WIN32
    char infoBuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD bufCharCount = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameA(infoBuf, &bufCharCount)) {
        return strdup(infoBuf);
    } else {
        std::cerr << "Error getting computer name" << std::endl;
        return nullptr;
    }
#else
    char infoBuf[HOST_NAME_MAX + 1];

    if (gethostname(infoBuf, sizeof(infoBuf)) == 0) {
        return strdup(infoBuf);
    } else {
        std::cerr << "Error getting computer name" << std::endl;
        return nullptr;
    }
#endif
}

char* getUnifiedData() {
    ULARGE_INTEGER* driveInfo = getDriveInfo();
    char* pcName = getPcName();

    if (pcName == nullptr || driveInfo == nullptr) {
        delete[] pcName;
        delete[] driveInfo;
        return nullptr;
    }

    char* result = new char[strlen(pcName) + 100];

    sprintf(result, "PC Name: %s, Free Space: %ld MB, Total Space: %ld MB", pcName, driveInfo[1].QuadPart, driveInfo[0].QuadPart);

    delete[] driveInfo;
    delete[] pcName;
    return result;
}