#ifndef INFO_H
#define INFO_H

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
typedef struct
{
    uint64_t QuadPart;
} ULARGE_INTEGER;
#endif

class SystemInfo
{
public:
    SystemInfo();
    ~SystemInfo();
    std::string getUnifiedData();

private:
    ULARGE_INTEGER *driveInfo = nullptr;
    char *pcName = nullptr;

    ULARGE_INTEGER *getDriveInfo();
    char *getPcName();
};

#endif