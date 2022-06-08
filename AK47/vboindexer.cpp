#include <map>
#include <vector>
#include <string.h>
#include <glm/glm.hpp>

struct PackedVertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	}
};

bool GetSimilarVertexIndex(
	const PackedVertex& packed,
	const std::map<PackedVertex, unsigned short>& vertex_to_out_index,
	unsigned short& result)
{
	auto it = vertex_to_out_index.find(packed);

	if (it == vertex_to_out_index.end())
	{
		return false;
	}

	result = it->second;
	return true;
}

void IndexVbo(
	const std::vector<glm::vec3>& in_vertices,
	const std::vector<glm::vec2>& in_uvs,
	const std::vector<glm::vec3>& in_normals,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::vector<unsigned short>& out_indices)
{
	std::map<PackedVertex, unsigned short> vertex_to_out_index;

	for (auto i = 0; i < in_vertices.size(); ++i)
	{
		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };

		unsigned short index = 0;
		auto found = GetSimilarVertexIndex(packed, vertex_to_out_index, index);

		if (found)
		{
			out_indices.push_back(index);
			continue;
		}

		// New vertex
		out_vertices.push_back(in_vertices[i]);
		out_uvs.push_back(in_uvs[i]);
		out_normals.push_back(in_normals[i]);
		auto new_index = (unsigned short) out_vertices.size() - 1;
		out_indices.push_back(new_index);
		vertex_to_out_index[packed] = new_index;
	}
}