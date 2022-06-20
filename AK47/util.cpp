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

    std::filesystem::path GetShaderFilePath(const std::string& filename)
    {
        return GetBaseDirectory().append(filename);
    }

    std::filesystem::path GetModelFilePath(const std::string& filename)
    {
        return GetBaseDirectory().append(filename);
    }

    std::filesystem::path GetTextureFilePath(const std::string& filename)
    {
        return GetBaseDirectory().append(filename);
    }
} // util
