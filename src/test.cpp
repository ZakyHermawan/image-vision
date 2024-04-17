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
  auto small_img = make_unique<Image>("tests/dogsmall.jpg");
  auto resized = small_img->nn_resize(small_img->m_width*4, small_img->m_height*4);
  resized->write_image("nn_resize_from_dogsmall.jpg");

  auto img = make_unique<Image>("tests/dog.jpg");
  resized = img->nn_resize(img->m_width*4, img->m_height*4);
  resized->write_image("nn_resize_from_dog.jpg");

}

void Image::test_bilinear_resize() {
  auto small_img = make_unique<Image>("tests/dogsmall.jpg");
  auto resized = small_img->bilinear_resize(small_img->m_width*4, small_img->m_height*4);
  resized->write_image("bilinear_resize_from_dogsmall.jpg");

  auto img = make_unique<Image>("tests/dog.jpg");
  resized = img->bilinear_resize(img->m_width*4, img->m_height*4);
  resized->write_image("bilinear_resize_from_dog.jpg");
}

void Image::test_convolve() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto blur = img->convolve_image(img->make_box_filter(7), 1);
  blur->write_image("blur.jpg");
}

void Image::test_filters() {
  auto img = make_unique<Image>("tests/dog.jpg");
  auto ihpf = img->convolve_image(img->make_high_pass_filter(), 1);
  auto isharpen = img->convolve_image(img->make_sharpen_filter(), 1);
  auto iemboss = img->convolve_image(img->make_emboss_filter(), 1);
  ihpf->write_image("hpf.jpg");
  isharpen->write_image("isharpen.jpg");
  iemboss->write_image("iemboss.jpg");
}
