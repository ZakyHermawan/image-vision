#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

class Image {
private:
  // the image will be stored in hwc format
  int m_height, m_width, m_channels;
  float *m_img;
public:
  Image() = default;
  Image(const char* path);

  void make_image(int width, int height, int channels);
  void load_image(const char* path);
  void write_image(const char* imgname); 
  float get_pixel(int x, int y, int c);
  void set_pixel(int x, int y, int c, float v);
  ~Image();
};
