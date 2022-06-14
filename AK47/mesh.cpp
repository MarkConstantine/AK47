#include "stdafx.h"
#include "mesh.h"
#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define VECTOR_SIZE(v) (sizeof(v[0]) * v.size())

glm::mat4 Convert(aiMatrix4x4 matrix)
{
    glm::mat4 m;
    m[0][0] = matrix.a1; m[0][1] = matrix.a2; m[0][2] = matrix.a3; m[0][3] = matrix.a4;
    m[1][0] = matrix.b1; m[1][1] = matrix.b2; m[1][2] = matrix.b3; m[1][3] = matrix.b4;
    m[2][0] = matrix.c1; m[2][1] = matrix.c2; m[2][2] = matrix.c3; m[2][3] = matrix.c4;
    m[3][0] = matrix.d1; m[3][1] = matrix.d2; m[3][2] = matrix.d3; m[3][3] = matrix.d4;
    return m;
}

Mesh::Mesh(const std::string& model_filename, const std::string& texture_filename)
{
    Clear();

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    LoadModel(model_filename);
    LoadTexture(texture_filename);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    Clear();
}

void Mesh::Render()
{
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

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

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void Mesh::LoadTexture(const std::string& texture_filename)
{
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    std::string absolute_path = util::GetTextureDirectory().append(texture_filename).string();
    auto data = stbi_load(absolute_path.c_str(), &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        fprintf(stderr, "Failed to load texture: %s", absolute_path.c_str());
        exit(EXIT_FAILURE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Mesh::LoadModel(const std::string& model_filename)
{
    const auto ASSIMP_LOAD_FLAGS = (
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs
        | aiProcess_JoinIdenticalVertices
        );

    std::string absolute_path = util::GetModelDirectory().append(model_filename).string();
    const aiScene* pScene = m_Importer.ReadFile(absolute_path, ASSIMP_LOAD_FLAGS);
    if (pScene == nullptr)
    {
        fprintf(stdout, "Error loading %s: %s\n", absolute_path.c_str(), m_Importer.GetErrorString());
        exit(EXIT_FAILURE);
    }
    
    ReserveSpace(pScene);
    InitAllMeshes(pScene);
    PopulateBuffers();
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
    m_Indices.resize(numIndices);
}

void Mesh::InitAllMeshes(const aiScene* pScene)
{
    for (int i = 0; i < m_Meshes.size(); ++i)
    {
        InitSingleMesh(pScene->mMeshes[i], m_Meshes[i]);
    }
}

void Mesh::InitSingleMesh(const aiMesh* pAiMesh, const MeshEntry& meshInfo)
{
    // Populate the vertex attribute vectors
    assert(pAiMesh->mNumVertices > 0);
    for (uint i = 0; i < pAiMesh->mNumVertices; ++i)
    {
        const uint idx = meshInfo.BaseVertex + i;
        const aiVector3D& pos = pAiMesh->mVertices[i];
        const aiVector3D& normal = pAiMesh->mNormals 
            ? pAiMesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);
        const aiVector3D& texCoord = pAiMesh->HasTextureCoords(0) 
            ? pAiMesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
        
        m_Positions[idx] = glm::vec3(pos.x, pos.y, pos.z);
        m_Normals[idx] = glm::vec3(normal.x, normal.y, normal.z);
        m_TexCoords[idx] = glm::vec2(texCoord.x, texCoord.y);
    }

    // Populate the index buffer
    assert(pAiMesh->mNumFaces > 0);
    for (uint f = 0; f < pAiMesh->mNumFaces; f++)
    {
        const aiFace& face = pAiMesh->mFaces[f];
        assert(face.mNumIndices > 0);
        for (uint i = 0; i < face.mNumIndices; ++i)
        {
            const uint idx = meshInfo.BaseIndex + f * face.mNumIndices + i;
            m_Indices[idx] = face.mIndices[i];
        }
    }
}

void Mesh::PopulateBuffers()
{
    const auto DRAW_TYPE = GL_STATIC_DRAW;
    const auto POSITION_LOCATION = 0;
    const auto TEXCOORD_LOCATION = 1;
    const auto NORMAL_LOCATION = 2;

    glGenBuffers(NUM_BUFFERS, m_Buffers);
    assert(m_Buffers[INDEX_BUFFER] > 0);
    assert(m_Buffers[POSITION_VBO] > 0);
    assert(m_Buffers[TEXCOORD_VBO] > 0);
    assert(m_Buffers[NORMAL_VBO] > 0);

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, VECTOR_SIZE(m_Indices), m_Indices.data(), DRAW_TYPE);
}