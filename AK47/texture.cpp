#include <GL/glew.h>

#include "texture.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

Bmp LoadBmp(const std::string& bmp_path)
{
    Bmp bmp = { 0 };

    FILE* file = nullptr;
    if (fopen_s(&file, bmp_path.c_str(), "rb") || file == nullptr) {
        fprintf(stderr, "Could not open texture %s.\n", bmp_path.c_str());
        throw false;
    }

    if (fread(bmp.header, 1, sizeof(bmp.header), file) != sizeof(bmp.header) || bmp.header[0] != 'B' || bmp.header[1] != 'M') {
        fclose(file);
        fprintf(stderr, "Unexpected BMP header\n");
        throw false;
    }

    bmp.data_pos = *(int*)&(bmp.header[0x0A]);
    bmp.image_size = *(int*)&(bmp.header[0x22]);
    bmp.width = *(int*)&(bmp.header[0x12]);
    bmp.height = *(int*)&(bmp.header[0x16]);

    // Some BMP are misformatter, guess the info.
    if (bmp.image_size == 0)
        bmp.image_size = bmp.width * bmp.height * 3;
    if (bmp.data_pos == 0)
        bmp.data_pos = 54;

    bmp.data = new unsigned char[bmp.image_size];
    fread(bmp.data, 1, bmp.image_size, file);
    fclose(file);

    return bmp;
}

GLuint LoadDds(const std::string& dds_path)
{
    char header[124] = { 0 };

    FILE* file = nullptr;
    if (fopen_s(&file, dds_path.c_str(), "rb") || file == nullptr) {
        fprintf(stderr, "Could not open texture %s.\n", dds_path.c_str());
        throw false;
    }

    char file_code[4] = { 0 };
    fread(file_code, 1, sizeof(file_code), file);
    if (strncmp(file_code, "DDS ", 4)) {
        fclose(file);
        fprintf(stderr, "Unexpected file type: %s.\n", file_code);
        throw false;
    }

    if (!fread(&header, sizeof(header), 1, file) != 0)
    {
        fclose(file);
        fprintf(stderr, "Failed to read DDS header.\n");
        throw false;
    }

    auto height = *(unsigned int*)&(header[8]);
    auto width = *(unsigned int*)&(header[12]);
    auto linear_size = *(unsigned int*)&(header[16]);
    auto mipmap_count = *(unsigned int*)&(header[24]);
    auto four_cc = *(unsigned int*)&(header[80]);

    auto data_size = mipmap_count > 1 ? 2 * linear_size : linear_size;
    auto data = new unsigned char[data_size];

    if (!fread(data, 1, data_size, file))
    {
        fclose(file);
        fprintf(stderr, "Failed to read DDS data.\n");
        throw false;
    }

    fclose(file);

    auto components = (four_cc == FOURCC_DXT1) ? 3 : 4;
    auto format = 0;
    switch (four_cc)
    {
        case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        default:
            fprintf(stderr, "Unrecognized fourcc.\n");
            throw false;
    }

    auto block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto offset = 0;
    for (auto level = 0; level < mipmap_count && (width || height); ++level)
    {
        auto size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, data + offset);

        offset += size;
        width /= 2;
        height /= 2;

        // Deal with textures that are not a power of 2.
        if (width < 1) width = 1;
        if (height < 1) height = 1;
    }

    return texture_id;
}