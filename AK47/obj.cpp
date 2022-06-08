#define _CRT_SECURE_NO_WARNINGS
#include "obj.h"

void LoadObjFile(
    const std::string& obj_path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals)
{
    fprintf(stdout, "Loading %s...\n", obj_path.c_str());

    std::vector<unsigned int> vertex_indices, uv_indices, normal_indices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE* file = nullptr; ;
    if (fopen_s(&file, obj_path.c_str(), "r") || file == nullptr)
    {
        fprintf(stderr, "Could not open %s.\n", obj_path.c_str());
        throw false;
    }

    while (true) {
        char line_header[128] = { 0 };

        int first = fscanf(file, "%s", line_header);
        if (first == EOF)
            break;

        // Parse line header
        if (strcmp(line_header, "v") == 0) { // Vertex
            glm::vec3 vertex;
            auto matches = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            if (matches != 3)
            {
                fprintf(stderr, "Failed to read vertex: %s\n", line_header);
                fclose(file);
                throw false;
            }
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(line_header, "vt") == 0) { // Vertex texture
            glm::vec2 uv;
            auto matches = fscanf(file, "%f %f\n", &uv.x, &uv.y);
            if (matches != 2)
            {
                fprintf(stderr, "Failed to read vertex texture: %s\n", line_header);
                fclose(file);
                throw false;
            }
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }
        else if (strcmp(line_header, "vn") == 0) { // Vertex Normal
            glm::vec3 normal;
            auto matches = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            if (matches != 3)
            {
                fprintf(stderr, "Failed to read vertex normal: %s\n", line_header);
                fclose(file);
                throw false;
            }
            temp_normals.push_back(normal);
        }
        else if (strcmp(line_header, "f") == 0) {
            unsigned int v_i[3], uv_i[3], n_i[3];
            auto matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &v_i[0], &uv_i[0], &n_i[0],
                &v_i[1], &uv_i[1], &n_i[1],
                &v_i[2], &uv_i[2], &n_i[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                throw false;
            }
            vertex_indices.push_back(v_i[0]);
            vertex_indices.push_back(v_i[1]);
            vertex_indices.push_back(v_i[2]);
            uv_indices.push_back(uv_i[0]);
            uv_indices.push_back(uv_i[1]);
            uv_indices.push_back(uv_i[2]);
            normal_indices.push_back(n_i[0]);
            normal_indices.push_back(n_i[1]);
            normal_indices.push_back(n_i[2]);
        }
        else {
            // Eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    for (auto i = 0; i < vertex_indices.size(); ++i) {
        // Get the indices of its attributes
        auto vertex_index = vertex_indices[i];
        auto uv_index = uv_indices[i];
        auto normal_index = normal_indices[i];

        // Get the attributes thanks to the index
        auto vertex = temp_vertices[vertex_index - 1];
        auto uv = temp_uvs[uv_index - 1];
        auto normal = temp_normals[normal_index - 1];

        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }
    
    fclose(file);
}