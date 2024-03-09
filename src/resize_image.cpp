#include "image.hpp"
#include <cmath>
#include <algorithm>

float Image::nn_interpolate(float x, float y, int c) {
  return get_pixel(round(x), round(y), c);
}

unique_ptr<Image> Image::nn_resize(int w, int h) {
  unique_ptr<Image> new_img(new Image(w, h, m_channels));
  float row_ratio = static_cast<float>(static_cast<float>(m_height) / static_cast<float>(h));
  float col_ratio = static_cast<float>(static_cast<float>(m_width) / static_cast<float>(w));

  float w_displacement_factor = -0.5 + 0.5*col_ratio;
  float h_displacement_factor = -0.5 + 0.5*row_ratio;

  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<h; ++j) {
      for(int k=0; k<w; ++k) {
        new_img->set_pixel(k, j, i, nn_interpolate((static_cast<float>(k)*col_ratio), (static_cast<float>(j)*row_ratio), i));
      }
    }
  }

  return new_img;
}
#include <iostream>
float Image::bilinear_interpolate(float x, float y, int c) {
  float x_floor = std::floor(static_cast<float>(x));
  float x_ceil = std::ceil(static_cast<float>(x));
  float y_floor = std::floor(static_cast<float>(y));
  float y_ceil = std::ceil(static_cast<float>(y));

  float v1 = get_pixel(x_floor, y_floor, c);
  float v2 = get_pixel(x_ceil, y_floor, c);
  float v3 = get_pixel(x_floor, y_ceil, c);
  float v4 = get_pixel(x_ceil, y_ceil, c);

  float dx1 = x - x_floor;
  float dx2 = x_ceil - x;
  float dy1 = y - y_floor;
  float dy2 = y_ceil - y;

  float q1 = v4 * dx1 + v3 * dx2;
  float q2 = v2 * dx1 + v1 * dx2;

  float q = q2 * dy2 + q1 * dy1;

  return q;
}

unique_ptr<Image> Image::bilinear_resize(int w, int h) {
  unique_ptr<Image> new_img(new Image(w, h, m_channels));
  float row_ratio = static_cast<float>(m_height) / static_cast<float>(h);
  float col_ratio = static_cast<float>(m_width) / static_cast<float>(w);

  float w_displacement_factor = -0.5 + 0.5*col_ratio;
  float h_displacement_factor = -0.5 + 0.5*row_ratio;

  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<h; ++j) {
      for(int k=0; k<w; ++k) {
        new_img->set_pixel(k, j, i, bilinear_interpolate((static_cast<float>(k)*col_ratio + w_displacement_factor), 
          (static_cast<float>(j)*row_ratio + h_displacement_factor),
          i
        ));
      }
    }
  }

  return new_img;
}
