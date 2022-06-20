#pragma once

namespace util
{
    std::filesystem::path GetBaseDirectory();
    std::filesystem::path GetShaderDirectory();
    std::filesystem::path GetModelDirectory();
    std::filesystem::path GetTextureDirectory();
    
    inline glm::vec3 ToVec3(const aiVector3D& vec3)
    {
        return glm::vec3(vec3.x, vec3.y, vec3.z);
    }

    inline glm::vec2 ToVec2(const aiVector3D& vec3)
    {
        return glm::vec2(vec3.x, vec3.y);
    }
} // util
