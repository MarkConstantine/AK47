#include "stdafx.h"
#include "util.h"

#include <windows.h>

namespace util
{
    std::filesystem::path GetBaseDirectory()
    {
        CHAR szFullPath[MAX_PATH] = { 0 };
        
        if (!GetModuleFileNameA(NULL, szFullPath, sizeof(szFullPath)))
        {
            fprintf(stderr, "Failed to get base directory: %X", GetLastError());
            exit(EXIT_FAILURE);
        }

        return std::filesystem::path(szFullPath).parent_path();
    }

    std::filesystem::path GetShaderDirectory()
    {
        return GetBaseDirectory();
    }

    std::filesystem::path GetModelDirectory()
    {
        return GetBaseDirectory();
    }
} // util
