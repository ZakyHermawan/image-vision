#include "image.hpp"
#include <fstream>
#include <exception>
#include <memory>
#include <algorithm>

using std::unique_ptr;
using std::make_unique;
using std::runtime_error;

int main() {
  /*
  unique_ptr<Image> img = make_unique<Image>("tests/lenna.png");
  img->write_image("hasil.png");
  unique_ptr<Image>tmp = img->grayscale();
  tmp->write_image("gray.png");
  unique_ptr<Image> tes = make_unique<Image>("tests/example.jpg");
  tes->write_image("yow.jpg");
  unique_ptr<Image> grayscaled = tes->grayscale();
  grayscaled->write_image("graaaay.jpg");
  unique_ptr<Image> kopi(grayscaled->copy());
  kopi->write_image("kopi.png");
  unique_ptr<Image> colorbar(new Image("tests/colorbar.png"));
  auto gray_colorbar(colorbar->grayscale());
  gray_colorbar->write_image("gray_colorbar.png");
  unique_ptr<Image> orig = make_unique<Image>("tests/dog.jpg");
  unique_ptr<Image> nored = orig->zerochannel(0);
  nored->write_image("nored.jpg");
  unique_ptr<Image> shift = orig->shift_color((float)0.4);
  shift->write_image("Shifted.jpg");
  unique_ptr<Image> clamp(shift->clamp_img());
  clamp->write_image("clamp.jpg");
  shift->internal_clamp();
  shift->write_image("Internal clamping.jpg");
  */

  unique_ptr<Image> rgb = make_unique<Image>("tests/dog.jpg");
  unique_ptr<Image> to_hsv = rgb->rgb_to_hsv();
  unique_ptr<Image> to_rgb = to_hsv->hsv_to_rgb();
  to_rgb->write_image("hsv_rgb_conversion.jpg");

  return 0;
}

/*
* will call make_image to create empty image
*/
Image::Image(int width, int height, int channels) {
  make_image(width, height, channels);
}

/*
* will call load_image to load an image and fill m_img
*/
Image::Image(const char* path) {
  std::ifstream infile(path);
  if (!infile.good()) {
    throw runtime_error("File did not exist");
  }
  load_image(path);
}

Image::~Image() {

}

/*
* return the type of an image (ImageType enum) based on it file extension
*/
ImageType Image::get_file_type(const char* filename) {
  const char* ext = strrchr(filename, '.');
  if (ext != nullptr) {
    if (strcmp(ext, ".png") == 0) {
      return PNG;
    }
    else if (strcmp(ext, ".jpg") == 0) {
      return JPG;
    }
    else if (strcmp(ext, ".bmp") == 0) {
      return BMP;
    }
    else if (strcmp(ext, ".tga") == 0) {
      return TGA;
    }


  }
  throw runtime_error("extension not supported");

}


/*
* create Empty Image
*/
void Image::make_image(int width, int height, int channels) {
  m_width = width;
  m_height = height;
  m_channels = channels;
  m_size = m_width * m_height * m_channels;
  m_img = make_unique<float[]>(m_size);
}

/*
* load from path
* and convert it to width, height, channel linear format
* and normalize it (the value between 0 and 1)
*/
void Image::load_image(const char* path) {
  unsigned char* tmp = stbi_load(path, &m_width, &m_height, &m_channels, 0);
  m_size = m_width * m_height * m_channels;
  m_img = make_unique<float[]>(m_size);

  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        int dst_index = k + j * m_width + i * m_width * m_height;
        int src_index = i + m_channels * k + m_channels * m_width * j;
        m_img[dst_index] = (float)tmp[src_index] / 255;
      }
    }
  }
  stbi_image_free(tmp);
}

/*
* write image based on their extension
* make sure that filename on correct path
* you can use relative or absolute path in filename
*/
void Image::write_image(const char* filename) {
  ImageType type = get_file_type(filename);
  unique_ptr<unsigned char[]> conv = conv_to_stb();
  switch (type) {
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

/*
* x, y, and c (channel) is spatial (image) coordinate
* get the normalized pixel value
*/
float Image::get_pixel(int x, int y, int c) {
  if (x < 0 || y < 0) {
    throw runtime_error("Image index cannot be negative");
  }
  if (y < 0) {
    throw runtime_error("Channel cannot be negative");
  }
  return m_img[x + y * m_width + c * m_width * m_height];
}

/*
* x, y, and c (channel) is spatial (image) coordinate
* wet the normalized pixel value at specific pixel and channel of an image
*/
void Image::set_pixel(int x, int y, int c, float v) {
  if (x < 0 || y < 0) {
    throw runtime_error("Image index cannot be negative");
  }
  if (y < 0) {
    throw runtime_error("Channel cannot be negative");
  }
  m_img[x + y * m_width + c * m_width * m_height] = v;
}

/*
* return v that is not below 0 or more than v
*/
float Image::clamp_pix(float v) {
  if (v > 0.) {
    if (v > 1.) {
      return (float)1.0;
    }
    else {
      return v;
    }
  }
  else {
    return (float)0.0;
  }
}

/*
* make sure every pixel in image is clamped
*/
void Image::internal_clamp() {
  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        float pixval = clamp_pix(get_pixel(k, j, i));
        set_pixel(k, j, i, pixval);
      }
    }
  }
}

/*
* return max value of three parameter
*/
float Image::max_three(float a, float b, float c) {
  return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

/*
* return max value of three parameter
*/
float Image::min_three(float a, float b, float c) {
  return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

/*
* convert image from width, height, channel linear format to stb format
* will be used if we want to interact with stb library (such as for writing image)
*/
unique_ptr<unsigned char[]> Image::conv_to_stb() {
  unique_ptr<unsigned char[]> tmp = make_unique<unsigned char[]>(m_size);
  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        size_t src_idx = k + j * m_width + i * m_width * m_height;
        size_t dst_idx = i + k * m_channels + j * m_channels * m_width;
        tmp[dst_idx] = (unsigned char)roundf(m_img[src_idx] * 255);
      }
    }
  }
  return tmp;
}

/*
* return a new copied image
*/
unique_ptr<Image> Image::copy() {
  unique_ptr<Image> new_img(new Image(m_width, m_height, m_channels));
  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        new_img->set_pixel(k, j, i, get_pixel(k, j, i));
      }
    }
  }
  return new_img;
}

/*
* wake set 0 to all value in a channel
*/
unique_ptr<Image> Image::zerochannel(int c) {
  unique_ptr<Image> tmp(copy());
  for (int i = 0; i < m_height; ++i) {
    for (int j = 0; j < m_width; ++j) {
      tmp->set_pixel(j, i, c, 0);
    }
  }
  return tmp;
}

/*
* return an unique_ptr that point to a new greyscaled image
*/
unique_ptr<Image> Image::grayscale() {
  if (m_channels != 3) {
    throw runtime_error("You must have three channels");
  }

  unique_ptr<Image> gray(new Image(m_width, m_height, 1));

  float wred = 0.299f;
  float wgreen = 0.587f;
  float wblue = 0.114f;

  int ctr = 0;
  for (int i = 0; i < m_height; ++i) {
    for (int j = 0; j < m_width; ++j) {
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


/*
* return an unique_ptr that point to a new shifted color image
*/
unique_ptr<Image> Image::shift_color(float v) {
  unique_ptr<Image> tmp(new Image(m_width, m_height, m_channels));
  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        tmp->set_pixel(k, j, i, get_pixel(k, j, i) + v);
      }
    }
  }
  return tmp;
}

/*
* make sure every pixel in image is clamped
*/
unique_ptr<Image> Image::clamp_img() {
  unique_ptr<Image> tmp(new Image(m_width, m_height, m_channels));
  for (int i = 0; i < m_channels; ++i) {
    for (int j = 0; j < m_height; ++j) {
      for (int k = 0; k < m_width; ++k) {
        float pixval = clamp_pix(get_pixel(k, j, i));
        tmp->set_pixel(k, j, i, pixval);
      }
    }
  }
  return tmp;
}

/*
* return a new image in hsv color spaces
*/
unique_ptr<Image> Image::rgb_to_hsv() {
  unique_ptr<Image> tmp(new Image(m_width, m_height, m_channels));
  for (int i = 0; i < m_height; ++i) {
    for (int j = 0; j < m_width; ++j) {
      float red = get_pixel(j, i, 0);
      float green = get_pixel(j, i, 1);
      float blue = get_pixel(j, i, 2);
      float value = max_three(red, green, blue);
      float minval = min_three(red, green, blue); // to compute saturation
      float c = value - minval;

      float saturation = (c == 0) ? 0 : ((value == 0) ? 0 : c / value);
      float hue_prime = (c == 0) ? 0 : static_cast<float>((value == red) ? (green - blue) / c :
        ((value == green) ? ((blue - red) / c) + 2.0 : ((red - green) / c) + 4.0));

      float hue = static_cast<float>((hue_prime < 0) ? (hue_prime / 6.0) + 1 : hue_prime / 6.0);
      tmp->set_pixel(j, i, 0, hue);
      tmp->set_pixel(j, i, 1, saturation);
      tmp->set_pixel(j, i, 2, value);
    }
  }
  return tmp;
}

/*
* return a new image in rgb color spaces
*/
unique_ptr<Image> Image::hsv_to_rgb() {
  unique_ptr<Image> tmp(new Image(m_width, m_height, m_channels));
  for (int i = 0; i < m_height; ++i) {
    for (int j = 0; j < m_width; ++j) {
      float hue = get_pixel(j, i, 0);
      float saturation = get_pixel(j, i, 1);
      float value = get_pixel(j, i, 2);
      float max = value;
      float c = saturation * value;
      float min = max - c;

      float hue_prime = hue * 6.0f;
      float x = static_cast<float>(1 - fabs(fmod(hue_prime, 2) - 1)); // Calulating hue values in range [0-1)

      // In this part of HSV hexagon, R component will have the maximum value, B will have the minimum, and G will have value somehwere between R and B
      float r, g, b;
      if ((hue_prime >= 0.) && (hue_prime < 1.))
      {
        r = max;
        b = min;
        g = (c * x) + b;
      }
      else if ((hue_prime >= 1.) && (hue_prime < 2.)) // In this part of HSV hexagon, G component will have the maximum value, B will have the minimum, and R will have value somehwere between G and B
      {
        g = max;
        b = min;
        r = (c * x) + b;
      }
      else if ((hue_prime >= 2.) && (hue_prime < 3.)) // Same logic exteded
      {
        g = max;
        r = min;
        b = (c * x) + r;
      }
      else if ((hue_prime >= 3.) && (hue_prime < 4.))
      {
        b = max;
        r = min;
        g = (c * x) + r;
      }
      else if ((hue_prime >= 4.) && (hue_prime < 5.))
      {
        b = max;
        g = min;
        r = (c * x) + g;
      }
      else if ((hue_prime >= 5.) && (hue_prime < 6.))
      {
        r = max;
        g = min;
        b = (c * x) + g;
      }
      else {
        r = 0;
        g = 0;
        b = 0;
      }
      tmp->set_pixel(j, i, 0, r);
      tmp->set_pixel(j, i, 1, g);
      tmp->set_pixel(j, i, 2, b);
    }
  }
  return tmp;
}