#include "image.hpp"
#include <memory>

using std::unique_ptr;
using std::make_unique;

void Image::test_set_pixel() {
  auto img = make_unique<Image>("tests/dog.jpg");

  for(int i=0; i<img->m_height; ++i) {
    for(int j=0; j<img->m_width; ++j) {
      img->set_pixel(j, i, 0, 0);
    }
  }
  img->write_image("dogs_nored.jpg");
}

void Image::test_copy() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto copied_img = img->copy();
  copied_img->write_image("copied_image.jpg");
}

void Image::test_grayscale() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto gray = img->grayscale();
  gray->write_image("greyscaled_image.jpg");
}

void Image::test_shift() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto shift1 = img->shift_color(0, .4);
  auto shift2 = shift1->shift_color(1, .4);
  auto shift3 = shift2->shift_color(2, .4);
  auto clamped = shift3->clamp_img();
  clamped->write_image("shifted.jpg");
}

void Image::test_hsv() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto to_hsv = img->rgb_to_hsv();
  auto shifted = to_hsv->shift_color(1, .2);
  auto clamped = shifted->clamp_img();
  auto to_rgb = clamped->hsv_to_rgb();
  to_rgb->write_image("dog_saturated.jpg");
}

void Image::test_scale() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto to_hsv = img->rgb_to_hsv();
  auto scaled = to_hsv->scale_image(1, 2);
  auto clamped = scaled->clamp_img();
  auto to_rgb = clamped->hsv_to_rgb();
  to_rgb->write_image("dog_scale_saturated.jpg");
}

void Image::test_nn_resize() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto resized = img->nn_resize(img->m_width*4, img->m_height*4);
  resized->write_image("resized.jpg");
}
