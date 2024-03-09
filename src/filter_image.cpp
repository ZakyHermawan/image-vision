#include "image.hpp"

unique_ptr<Image> Image::l1_normalize() {
  auto normalized_img(copy());
  int size = m_height*m_width;
  int pix_sum = 0;
  for(int i=0; i<m_channels; ++i) {
    for(int j=0; j<m_height; ++j) {
      for(int k=0; k<m_width; ++k) {
        pix_sum += get_pixel(k, j, i);
      }
    }
  }
  for(int i=0; i<m_channels; ++i) {
    normalized_img->scale_image(i, 1.0f/static_cast<float>(pix_sum));
  }
  return normalized_img;
}

unique_ptr<Image> Image::make_box_filter(int w) {
  unique_ptr<Image> box_filter(new Image(w, w, 1));
  for(int i=0; i<box_filter->m_channels; ++i) {
    for(int j=0; j<box_filter->m_height; ++j) {
      for(int k=0; k<box_filter->m_width; ++k) {
        box_filter->set_pixel(k, j, i, 1.0f);
      }
    }
  }

  auto normalized_filter = box_filter->l1_normalize();
  return normalized_filter;
}
