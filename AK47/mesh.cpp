#include "stdafx.h"
#include "shader.h"
#include "mesh.h"
#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define VECTOR_SIZE(v) (sizeof(v[0]) * v.size())

Mesh::Mesh(const MeshLoadInfo& load_info)
{
    Clear();

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    
    LoadModel(load_info.ModelFile);
    LoadTexture(m_Textures[BASE_COLOR], load_info.DiffuseTextureFile, "diffuseMap");
    LoadTexture(m_Textures[NORMAL_MAP], load_info.NormalMapFile, "normalMap");
    LoadTexture(m_Textures[SPECULAR_MAP], load_info.SpecularMapFile, "specularMap");

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    Clear();
}

void Mesh::Render(ShaderProgram& shader_program)
{
    glBindVertexArray(m_VAO);

    for (int i = 0; i < NUM_TEXTURES; ++i)
    {
        auto& texture = m_Textures[i];
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture.TextureUnit);
        shader_program.SetUniform(texture.TextureSampler, i);
    }

    for (int i = 0; i < m_Meshes.size(); ++i)
    {
        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            m_Meshes[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(uint) * m_Meshes[i].BaseIndex),
            m_Meshes[i].BaseVertex
        );
    }

    glBindVertexArray(0);
}

void Mesh::Clear()
{
    if (m_Buffers[0] != 0)
    {
        glDeleteBuffers(NUM_BUFFERS, m_Buffers);
    }

    for (int i = 0; i < NUM_TEXTURES; ++i)
    {
        glDeleteTextures(1, &m_Textures[i].TextureUnit);
    }

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void Mesh::LoadTexture(TextureEntry& texture_entry, const std::string& texture_filename, const std::string& sampler_name)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    
    int width, height, numChannels;
    std::string absolute_path = util::GetTextureFilePath(texture_filename).string();
    auto data = stbi_load(absolute_path.c_str(), &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        fprintf(stderr, "Failed to load texture: %s", absolute_path.c_str());
        exit(EXIT_FAILURE);
    }

    GLenum format = GL_RGB;
    switch (numChannels)
    {
        case 1: format = GL_RED;    break;
        case 2: format = GL_RG;     break;
        case 3: format = GL_RGB;    break;
        case 4: format = GL_RGBA;   break;
    }

    glGenTextures(1, &texture_entry.TextureUnit);
    glBindTexture(GL_TEXTURE_2D, texture_entry.TextureUnit);
    
    texture_entry.TextureSampler = sampler_name;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    auto stop_time = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time);
    fprintf(stdout, "Loaded textured %s with %d channel(s) in %lld ms.\n", absolute_path.c_str(), numChannels, load_time.count());
}

void Mesh::LoadModel(const std::string& model_filename)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    
    const auto ASSIMP_LOAD_FLAGS = (
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs
        | aiProcess_JoinIdenticalVertices
        | aiProcess_CalcTangentSpace
    );

    std::string absolute_path = util::GetModelFilePath(model_filename).string();
    const aiScene* pScene = m_Importer.ReadFile(absolute_path, ASSIMP_LOAD_FLAGS);
    if (pScene == nullptr)
    {
        fprintf(stdout, "Error loading %s: %s\n", absolute_path.c_str(), m_Importer.GetErrorString());
        exit(EXIT_FAILURE);
    }

    ReserveSpace(pScene);
    InitAllMeshes(pScene);
    PopulateBuffers();

    auto stop_time = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time);
    fprintf(stdout, "Loaded model %s in %lld ms.\n", absolute_path.c_str(), load_time.count());
}

void Mesh::ReserveSpace(const aiScene* pScene)
{
    m_Meshes.resize(pScene->mNumMeshes);

    uint numVertices = 0, numIndices = 0;

    for (int i = 0; i < m_Meshes.size(); ++i)
    {
        const aiMesh* pMesh = pScene->mMeshes[i];
        m_Meshes[i].MaterialIndex = pMesh->mMaterialIndex;
        m_Meshes[i].NumIndices = pMesh->mNumFaces * pMesh->mFaces[0].mNumIndices;
        m_Meshes[i].BaseVertex = numVertices;
        m_Meshes[i].BaseIndex = numIndices;

        numVertices += pMesh->mNumVertices;
        numIndices += m_Meshes[i].NumIndices;
    }

    m_Positions.resize(numVertices);
    m_Normals.resize(numVertices);
    m_TexCoords.resize(numVertices);
    m_Tangents.resize(numVertices);
    m_Bitangents.resize(numVertices);
    m_Indices.resize(numIndices);
}

void Mesh::InitAllMeshes(const aiScene* pScene)
{
    for (int i = 0; i < m_Meshes.size(); ++i)
    {
        InitSingleMesh(pScene->mMeshes[i], m_Meshes[i]);
    }
}

void Mesh::InitSingleMesh(const aiMesh* pAiMesh, const MeshEntry& mesh_entry)
{
    // Populate the vertex attribute vectors
    assert(pAiMesh->mNumVertices > 0);
    for (uint i = 0; i < pAiMesh->mNumVertices; ++i)
    {
        const uint idx = mesh_entry.BaseVertex + i;
        m_Positions[idx] = util::ToVec3(pAiMesh->mVertices[i]);
        m_Normals[idx] = util::ToVec3(pAiMesh->mNormals[i]);
        m_Tangents[idx] = util::ToVec3(pAiMesh->mTangents[i]);
        m_Bitangents[idx] = util::ToVec3(pAiMesh->mBitangents[i]);
        m_TexCoords[idx] = util::ToVec2(pAiMesh->HasTextureCoords(0)
            ? pAiMesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f));
    }

    // Populate the index buffer
    assert(pAiMesh->mNumFaces > 0);
    for (uint f = 0; f < pAiMesh->mNumFaces; f++)
    {
        const aiFace& face = pAiMesh->mFaces[f];
        assert(face.mNumIndices > 0);
        for (uint i = 0; i < face.mNumIndices; ++i)
        {
            const uint idx = mesh_entry.BaseIndex + f * face.mNumIndices + i;
            m_Indices[idx] = face.mIndices[i];
        }
    }
}

void Mesh::PopulateBuffers()
{
    const auto DRAW_TYPE = GL_DYNAMIC_DRAW;

    glGenBuffers(NUM_BUFFERS, m_Buffers);
    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        assert(m_Buffers[i] > 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION_VBO]);
    glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE(m_Positions), m_Positions.data(), DRAW_TYPE);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VBO]);
    glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE(m_TexCoords), m_TexCoords.data(), DRAW_TYPE);
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VBO]);
    glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE(m_Normals), m_Normals.data(), DRAW_TYPE);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VBO]);
    glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE(m_Tangents), m_Tangents.data(), DRAW_TYPE);
    glEnableVertexAttribArray(TANGENT_LOCATION);
    glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BITANGENT_VBO]);
    glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE(m_Bitangents), m_Bitangents.data(), DRAW_TYPE);
    glEnableVertexAttribArray(BITANGENT_LOCATION);
    glVertexAttribPointer(BITANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, VECTOR_SIZE(m_Indices), m_Indices.data(), DRAW_TYPE);
}