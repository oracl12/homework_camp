#include "utils.h"
#include <iostream>
#include <winsock2.h>
#include <cwchar>
#include <vector>

ULARGE_INTEGER *getDriveInfo()
{
    std::vector<std::wstring> driveLetters;
    WCHAR drives[1024];
    DWORD drivesCount = GetLogicalDriveStringsW(sizeof(drives) / sizeof(drives[0]), drives);

    if (drivesCount == 0)
    {
        std::cerr << "Error getting logical drive strings." << std::endl;
        return nullptr;
    }

    driveLetters.reserve(drivesCount);
    for (WCHAR *drive = drives; *drive != L'\0'; drive += wcslen(drive) + 1)
    {
        driveLetters.push_back(std::wstring(drive));
    }

    ULARGE_INTEGER totalSpace = {0};
    ULARGE_INTEGER freeSpace = {0};

    for (const std::wstring &driveLetter : driveLetters)
    {
        if (GetDriveTypeW(driveLetter.c_str()) == DRIVE_FIXED)
        {
            ULARGE_INTEGER total, free, dummy;
            if (GetDiskFreeSpaceExW(driveLetter.c_str(), &dummy, &total, &free))
            {
                totalSpace.QuadPart += total.QuadPart;
                freeSpace.QuadPart += free.QuadPart;
                continue;
            }

            std::cerr << "Error getting disk space information for drive ";
            std::wcerr << driveLetter << std::endl;
        }
    }

    ULARGE_INTEGER *total = new ULARGE_INTEGER[2];
    total[0].QuadPart = totalSpace.QuadPart / (1024 * 1024);
    total[1].QuadPart = freeSpace.QuadPart / (1024 * 1024);
    return total;
}

char *getPcName()
{
    char infoBuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD bufCharCount = MAX_COMPUTERNAME_LENGTH + 1;

    if (GetComputerNameA(infoBuf, &bufCharCount))
    {
        char *result = new char[bufCharCount];
        strcpy(result, infoBuf);
        return result;
    }

    std::cerr << "Error getting computer name" << std::endl;
    return nullptr;
}

char *getUnifiedData()
{
    ULARGE_INTEGER *driveInfo = getDriveInfo();
    char *pcName = getPcName();

    if (pcName == nullptr || driveInfo == nullptr)
    {
        delete[] pcName;
        delete[] driveInfo;
        return nullptr;
    }
    char *result = new char[strlen(pcName) + 90];

    sprintf(result, "PC Name: %s, Free Space: %lld MB, Total Space: %lld MB", pcName, driveInfo[1].QuadPart, driveInfo[0].QuadPart);

    delete[] driveInfo;
    delete[] pcName;
    return result;
}