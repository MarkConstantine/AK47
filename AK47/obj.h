#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

void LoadObjFile(
    const std::string& obj_path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals);