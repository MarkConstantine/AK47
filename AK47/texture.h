#pragma once

#include <string>

struct Bmp {
    unsigned char header[54];
    unsigned int data_pos;
    unsigned int width;
    unsigned int height;
    unsigned int image_size;
    unsigned char* data;
};

Bmp LoadBmp(const std::string& bmp_path);
GLuint LoadDds(const std::string& dds_path);