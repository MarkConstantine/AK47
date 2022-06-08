#pragma once
#include <vector>
#include <glm/glm.hpp>

void IndexVbo(
	const std::vector<glm::vec3>& in_vertices,
	const std::vector<glm::vec2>& in_uvs,
	const std::vector<glm::vec3>& in_normals,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::vector<unsigned short>& out_indices);