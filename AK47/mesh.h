#pragma once

#define INVALID_MATERIAL 0xFFFFFFFF
using uint = unsigned int;

class Mesh
{
public:
    Mesh(const std::string& model_filename, const std::string& texture_filename);
    ~Mesh();
    void Render();
private:
    struct MeshEntry {
        MeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        uint NumIndices;
        uint BaseVertex;
        uint BaseIndex;
        uint MaterialIndex;
    };

    void Clear();
    void LoadTexture(const std::string& texture_filename);
    void LoadModel(const std::string& model_filename);
    void ReserveSpace(const aiScene* pScene);
    void InitAllMeshes(const aiScene* pScene);
    void InitSingleMesh(const aiMesh* pAiMesh, const MeshEntry& meshInfo);
    void PopulateBuffers();

    enum BUFFER_TYPE {
        INDEX_BUFFER,
        POSITION_VBO,
        TEXCOORD_VBO,
        NORMAL_VBO,
        NUM_BUFFERS
    };

    GLuint m_VAO = 0;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };
    GLuint m_Texture = 0;
    std::vector<MeshEntry> m_Meshes;
    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<uint> m_Indices;
    Assimp::Importer m_Importer;
};