#pragma once

using uint = unsigned int;

struct MeshLoadInfo
{
    std::string ModelFile;
    std::string DiffuseTextureFile;
    std::string NormalMapFile;
    std::string SpecularMapFile;
};

class Mesh
{
public:
    Mesh(const MeshLoadInfo& load_info);
    ~Mesh();
    void Render(ShaderProgram& shader_program);
private:
    struct MeshEntry {
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
    void LoadTexture(TextureEntry& texture_entry, const std::string& texture_filename, const std::string& sampler_name);
    void LoadModel(const std::string& model_filename);
    void ReserveSpace(const aiScene* pScene);
    void InitAllMeshes(const aiScene* pScene);
    void InitSingleMesh(const aiMesh* pAiMesh, const MeshEntry& mesh_entry);
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