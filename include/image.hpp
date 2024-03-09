#pragma once
#include <memory>

using std::unique_ptr;
using std::make_shared;

enum ImageType {
  PNG, JPG, BMP, TGA
};

class Image {
private:
  // the image will be stored in hwc format
  // the channels (from the first to last) is red, green, blue
  // or in hue, saturation, value (if the image is converted to hsv)
  int m_height, m_width, m_channels, m_size;
  unique_ptr<float[]> m_img;

public:
  Image() = default;
  Image(int width, int height, int channels);
  Image(const char* path);
  ~Image();

  ImageType get_file_type(const char* filename);
  void make_image(int width, int height, int channels);
  void load_image(const char* path);
  void write_image(const char* filename);
  float get_pixel(int x, int y, int c);
  void set_pixel(int x, int y, int c, float v);
  float clamp_pix(float v);
  void internal_clamp();
  float max_three(float r, float g, float b);
  float min_three(float r, float g, float b);
  float nn_interpolate(float x, float y, int c);
  float bilinear_interpolate(float x, float y, int c);

  unique_ptr<unsigned char[]> conv_to_stb();
  unique_ptr<Image> copy();
  unique_ptr<Image> zerochannel(int c);
  unique_ptr<Image> grayscale();
  unique_ptr<Image> shift_color(int c, float v);
  unique_ptr<Image> clamp_img();
  unique_ptr<Image> rgb_to_hsv();
  unique_ptr<Image> hsv_to_rgb();
  unique_ptr<Image> scale_image(int c, float v);
  unique_ptr<Image> nn_resize(int w, int h);
  unique_ptr<Image> bilinear_resize(int w, int h);
  unique_ptr<Image> l1_normalize();
  unique_ptr<Image> make_box_filter(int w);
  unique_ptr<Image> convolvte_image(unique_ptr<Image> filter, int preseve);

  // test function
  static void test_set_pixel();
  static void test_copy();
  static void test_grayscale();
  static void test_shift();
  static void test_hsv();
  static void test_scale();
  static void test_nn_resize();
  static void test_bilinear_resize();
};
