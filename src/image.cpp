#include "image.hpp"
#include <fstream>
#include <exception>
#include <memory>
#include <algorithm>

using std::unique_ptr;
using std::make_unique;
using std::runtime_error;

int main() {
  unique_ptr<Image> img = make_unique<Image>("../tests/lenna.png");
  img->write_image("hasil.png");
  unique_ptr<Image>tmp = img->grayscale();
  tmp->write_image("gray.png");
  unique_ptr<Image> tes = make_unique<Image>("../tests/example.jpg");
  tes->write_image("yow.jpg");
  unique_ptr<Image> grayscaled = tes->grayscale();
  grayscaled->write_image("graaaay.jpg");
  unique_ptr<Image> kopi(grayscaled->copy());
  kopi->write_image("kopi.png");
  unique_ptr<Image> colorbar(new Image("../tests/colorbar.png"));
  auto gray_colorbar(colorbar->grayscale());
  gray_colorbar->write_image("gray_colorbar.png");

  return 0;
}

Image::Image(int width, int height, int channels) {
  make_image(width, height, channels);
}

Image::Image(const char* path) {
  std::ifstream infile(path);
  if(!infile.good()) {
    throw runtime_error("File did not exist");
  }
  load_image(path);
}

ImageType Image::get_file_type(const char* filename) {
  const char* ext = strrchr(filename, '.');
  if(ext != nullptr) {
    if(strcmp(ext, ".png") == 0) {
      return PNG;
    }
    else if(strcmp(ext, ".jpg") == 0) {
      return JPG;
    }
    else if(strcmp(ext, ".bmp") == 0) {
      return BMP;
    }
    else if(strcmp(ext, ".tga") == 0) {
      return TGA;
    }
    
    
  }
  throw runtime_error("extension not supported");

}

Image::~Image() {

}


unique_ptr<unsigned char[]> Image::conv_to_stb() {
  unique_ptr<unsigned char[]> tmp = make_unique<unsigned char[]>(m_size);
  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<m_height; ++j) {
      for(int k=0; k<m_width; ++k) {
        size_t src_idx = k + j * m_width + i * m_width * m_height;
        size_t dst_idx = i + k * m_channels + j * m_channels * m_width;
        tmp[dst_idx] = (unsigned char)roundf(m_img[src_idx] * 255);
      }
    }
  }
  return tmp;
}

void Image::write_image(const char* filename) {
  ImageType type = get_file_type(filename);
  unique_ptr<unsigned char[]> conv = conv_to_stb();
  switch(type) {
    case PNG:
      stbi_write_png(filename, m_width, m_height, m_channels, conv.get(), m_width * m_channels);
      break;
    case BMP:
      stbi_write_bmp(filename, m_width, m_height, m_channels, conv.get());
      break;
    case JPG:
      stbi_write_jpg(filename, m_width, m_height, m_channels, conv.get(), 100);
      break;
    case TGA:
      stbi_write_tga(filename, m_width, m_height, m_channels, conv.get());
  }
}

void Image::make_image(int width, int height, int channels) {
  m_width = width;
  m_height = height;
  m_channels = channels;
  m_size = m_width * m_height * m_channels;
  m_img = make_unique<float[]>(m_size);
}

void Image::load_image(const char* path) {
  unsigned char* tmp = stbi_load(path, &m_width, &m_height, &m_channels, 0);
  m_size = m_width * m_height * m_channels;
  m_img = make_unique<float[]>(m_size);

  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<m_height; ++j) {
      for(int k=0; k<m_width; ++k) {
        int dst_index = k + j * m_width + i * m_width * m_height;
        int src_index = i + m_channels * k + m_channels * m_width * j;
        m_img[dst_index] = (float)tmp[src_index]/255;
      }
    }
  }
  stbi_image_free(tmp);
}


float Image::get_pixel(int x, int y, int c) {
  if(x < 0 || y < 0) {
    throw runtime_error("Image index cannot be negative");
  }
  if(y < 0) {
    throw runtime_error("Channel cannot be negative");
  }
  return m_img[x + y * m_width + c * m_width * m_height];
}


void Image::set_pixel(int x, int y, int c, float v) {
  if(x < 0 || y < 0) {
    throw runtime_error("Image index cannot be negative");
  }
  if(y < 0) {
    throw runtime_error("Channel cannot be negative");
  }
  m_img[x + y * m_width + c * m_width * m_height] = v;
}


unique_ptr<Image> Image::copy() {
  unique_ptr<Image> new_img(new Image(m_width, m_height, m_channels));
  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<m_height; ++j) {
      for(int k=0; k<m_width; ++k) {
        new_img->set_pixel(k, j, i, get_pixel(k, j, i));
      }
    }
  }
  return new_img;
}

unique_ptr<Image> Image::grayscale() {
  if(this->m_channels != 3) {
    throw runtime_error("You must have three channels");
  }

  unique_ptr<Image> gray(new Image(m_width, m_height, 1));

  float wred = 0.299;
  float wgreen = 0.587;
  float wblue = 0.114;

  int ctr = 0;
  for(int i=0; i<m_height; ++i) {
    for(int j=0; j<m_width; ++j) {
      float red, green, blue, greyval;
      red = get_pixel(j, i, 0);
      green = get_pixel(j, i, 1);
      blue = get_pixel(j, i, 2);
      greyval = red * wred + green * wgreen + blue * wblue;
      gray->set_pixel(j, i, 0, greyval);
    }
  }
  return gray;
}

