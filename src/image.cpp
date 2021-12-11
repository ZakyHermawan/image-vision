#include "image.hpp"
#include <fstream>
#include <exception>

Image::Image(const char* path) {
  std::ifstream infile(path);
  if(!infile.good()) {
    throw std::runtime_error("File did not exist");
  }
  load_image(path);
}

Image::~Image() {
  stbi_image_free(m_img);
}

void Image::write_image(const char* imgname) {
  unsigned char* img = (unsigned char*)calloc(m_width * m_height * m_channels, sizeof(unsigned char));
  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<m_height; ++j) {
      for(int k=0; k<m_width; ++k) {
        int idx = k + m_width * j + m_width * m_height * i;
        img[idx] = m_img[idx]*255;
      }
    }
  }
  stbi_write_png(imgname, m_width, m_height, m_channels, img, m_width * m_channels);
}

void Image::make_image(int width, int height, int channels) {
  m_img = (float*) calloc(width * height * channels, sizeof(float));
  m_width = width;
  m_height = height;
  m_channels = channels;
}

void Image::load_image(const char* path) {
  // m_img = stbi_load(path, &m_width, &m_height, &m_channels, 0);
  unsigned char* orig = stbi_load(path, &m_width, &m_height, &m_channels, 0); 
 
  make_image(m_width, m_height, m_channels);
  
  // convert to double precision pixel value
  for(int k=0; k<m_channels; ++k) {
    for(int j=0; j<m_height; ++j) {
      for(int i=0; i<m_width; ++i) {
        int idx = i + m_width * j + m_width * m_height * k;
        m_img[idx] = (float) orig[idx]/255; 
      }
    }
  }
}

float Image::get_pixel(int x, int y, int c) {
  if(x < 0 || y < 0) {
    throw std::runtime_error("Image index cannot be negative");
  }
  if(y < 0) {
    throw std::runtime_error("Channel cannot be negative");
  }
  return m_img[y + x * m_width + c * m_width * m_height];
}

void Image::set_pixel(int x, int y, int c, float v) {
  if(x < 0 || y < 0) {
    throw std::runtime_error("Image index cannot be negative");
  }
  if(y < 0) {
    throw std::runtime_error("Channel cannot be negative");
  }
  m_img[y + x * m_width + c * m_width * m_height] = v;
}

int main() {
  Image img("../tests/lenna.png");
  img.write_image("lenna.png");
  return 0;
}
