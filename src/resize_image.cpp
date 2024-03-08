#include "image.hpp"
#include <cmath>
#include <stdio.h>

float Image::nn_interpolate(float x, float y, int c) {
  return get_pixel(round(x), round(y), c);
}

unique_ptr<Image> Image::nn_resize(int w, int h) {
  unique_ptr<Image> new_img(new Image(w, h, m_channels));
  float row_ratio = static_cast<float> (m_height) / static_cast<float>(h);
  float col_ratio = static_cast<float>(m_width) / static_cast<float>(w);

  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<h; ++j) {
      for(int k=0; k<w; ++k) {
        new_img->set_pixel(k, j, i, nn_interpolate((static_cast<float>(k)*col_ratio), (static_cast<float>(j)*row_ratio), i));
      }
    }
  }

  return new_img;
}
