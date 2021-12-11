#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <memory>

using std::unique_ptr;
using std::make_shared;

enum ImageType {
  PNG, JPG, BMP, TGA
};

class Image {
private:
  // the image will be stored in hwc format

public:
  int m_height, m_width, m_channels, m_size;
  unique_ptr<float[]> m_img;

  Image() = default;
  Image(int width, int height, int channels);
  Image(const char* path);
  
  ImageType get_file_type(const char* filename);
  void make_image(int width, int height, int channels);
  unique_ptr<unsigned char[]> conv_to_stb();
  void load_image(const char* path);
  void write_image(const char* filename);
  float get_pixel(int x, int y, int c);
  void set_pixel(int x, int y, int c, float v);
  unique_ptr<Image> copy();
  unique_ptr<Image> grayscale();
  ~Image();
};
