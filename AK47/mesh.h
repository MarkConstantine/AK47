#pragma once

#define INVALID_MATERIAL 0xFFFFFFFF
using uint = unsigned int;

struct MeshInfo
{
    std::string ModelFile;
    std::string DiffuseTextureFile;
    std::string NormalMapFile;
    std::string SpecularMapFile;
};

class Mesh
{
public:
    Mesh(const MeshInfo& info);
    ~Mesh();
    void Render(GLuint program_id);
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

    struct TextureEntry {
        GLuint TextureUnit;
        std::string TextureSampler;
    };

    enum VBO_LOCATION {
        POSITION_LOCATION,
        TEXCOORD_LOCATION,
        NORMAL_LOCATION,
        TANGENT_LOCATION,
        BITANGENT_LOCATION,
        NUM_LOCATIONS
    };

    enum BUFFER_TYPE {
        INDEX_BUFFER,
        POSITION_VBO,
        TEXCOORD_VBO,
        NORMAL_VBO,
        TANGENT_VBO,
        BITANGENT_VBO,
        NUM_BUFFERS
    };

    enum TEXTURE_TYPE {
        BASE_COLOR,
        NORMAL_MAP,
        SPECULAR_MAP,
        NUM_TEXTURES
    };

    void Clear();
    void LoadTexture(TextureEntry& texture, const std::string& texture_filename, const std::string& sampler_name);
    void LoadModel(const std::string& model_filename);
    void ReserveSpace(const aiScene* pScene);
    void InitAllMeshes(const aiScene* pScene);
    void InitSingleMesh(const aiMesh* pAiMesh, const MeshEntry& meshInfo);
    void PopulateBuffers();

    GLuint m_VAO = 0;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };
    TextureEntry m_Textures[NUM_TEXTURES] = { 0 };
    std::vector<MeshEntry> m_Meshes;
    std::vector<glm::vec3> m_Positions, m_Normals, m_Tangents, m_Bitangents;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<uint> m_Indices;
    Assimp::Importer m_Importer;
};