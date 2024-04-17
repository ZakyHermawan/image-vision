#include "image.hpp"
#include <iostream>
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

  auto normalized_filter = box_filter->normalize_channel(0);
  return normalized_filter;
}

unique_ptr<Image> Image::normalize_channel(int c) {
  unique_ptr<Image> normalized_img(new Image(m_width, m_height, m_channels));

  float normalization_factor = (float)m_height*m_width;
  for(int i=0; i<m_height; ++i) {
    for(int j=0; j<m_width; ++j) {
      normalized_img->set_pixel(j, i, c, get_pixel(j, i, c)/normalization_factor);
    }
  }
  return normalized_img;
}

unique_ptr<Image> Image::convolve_image(unique_ptr<Image> filter, int preserve) {
	int filter_x;
	int correl_x;
	int center_x;

	int filter_y;
	int correl_y;
	int center_y;

	int target_channels;

	float filter_coefficient;
	float correlated_pixel;
	float source_pixel;
  if(filter->m_channels != 1) {
    if(filter->m_channels != m_channels) {
      return nullptr;
    }
  }

	target_channels = (preserve) ? m_channels : 1;
  unique_ptr<Image> target_img(new Image(m_width, m_height, target_channels));
  std::cout << m_width << " " << m_height << std::endl;
  std::cout << target_img->m_width << " " << target_img->m_height << std::endl;
  for(int i=0; i<target_img->m_channels; ++i) {
    for(int j=0; j<target_img->m_height; ++j) {
      for(int k=0; k<target_img->m_width; ++k) {
        target_img->set_pixel(k, j, i, 0);
      }
    }
  }

	center_x 	= filter->m_width/2;
	center_y 	= filter->m_height/2;
	for (int source_y = 0; source_y < m_height; source_y++) {
		for (int source_x = 0; source_x < m_width; source_x++) {
			if (!preserve){
        correlated_pixel = 0;
      }
			for(int channel=0; channel<m_channels; channel++) {
				if (preserve) {
          correlated_pixel = 0;
        }
				for (filter_y = 0; filter_y < filter->m_height; filter_y++) {	
					for (filter_x = 0; filter_x < filter->m_width; filter_x++) {
						correl_x = source_x + filter_x - center_x;
						correl_y = source_y + filter_y - center_y;

						if (filter->m_channels == 1) {
              filter_coefficient = filter->get_pixel(filter_x, filter_y, 0);
            }
						else {
              filter_coefficient = filter->get_pixel(filter_x, filter_y, channel);
            }
						source_pixel = get_pixel(correl_x, correl_y, channel);
						correlated_pixel += filter_coefficient*source_pixel;
					}
				}
				if (preserve) {
          target_img->set_pixel(source_x, source_y, channel, correlated_pixel);
        }
				else {
          target_img->set_pixel(source_x, source_y, 0, correlated_pixel);
        }
			}
		}
	}

	return target_img;
}