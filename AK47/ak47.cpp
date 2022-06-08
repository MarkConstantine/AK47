#include <vector>

#include <gl/glew.h>
#include <glm/glm.hpp>

#include "obj.h"
#include "vboindexer.h"
#include "ak47.h"

AK47::AK47(GLuint program_id)
{
    m_program_id = program_id;
    LoadObj();

    glGenVertexArrays(1, &m_vertex_array_id);
    glBindVertexArray(m_vertex_array_id);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_indexed_vertices.size(), m_indexed_vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_indexed_normals.size(), m_indexed_normals.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_indexed_normals.size(), m_indexed_normals.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_indices.size(), m_indices.data(), GL_DYNAMIC_DRAW);
}

AK47::~AK47()
{
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_uv_buffer);
    glDeleteBuffers(1, &m_normal_buffer);
    glDeleteBuffers(1, &m_index_buffer);
    glDeleteVertexArrays(1, &m_vertex_array_id);
    fprintf(stdout, "Suzanne destructed.\n");
}

void AK47::Draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(m_texture_sampler_id, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void AK47::LoadObj()
{
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;
    LoadObjFile("ak47.obj", vertices, uvs, normals);
    IndexVbo(vertices, uvs, normals, m_indexed_vertices, m_indexed_uvs, m_indexed_normals, m_indices);
}