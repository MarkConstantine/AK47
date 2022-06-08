#pragma once
#include <vector>

#include <gl/glew.h>
#include <glm/glm.hpp>

#include "drawable.h"

class AK47 : public Drawable
{
public:
	AK47(GLuint program_id);
	~AK47();
	void Draw() override;
private:
	void LoadObj();
private:
	GLuint m_program_id;
	GLuint m_vertex_array_id;
	GLuint m_vertex_buffer;
	GLuint m_uv_buffer;
	GLuint m_normal_buffer;
	GLuint m_index_buffer;
	GLuint m_texture;
	GLuint m_texture_id;
	GLuint m_texture_sampler_id;
	std::vector<unsigned short> m_indices;
	std::vector<glm::vec3> m_indexed_vertices;
	std::vector<glm::vec2> m_indexed_uvs;
	std::vector<glm::vec3> m_indexed_normals;
};