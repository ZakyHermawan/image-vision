#include "image.hpp"

class Image {
private:
  int m_width, m_height, m_channels;
  unsigned char *m_img;
public:
  Image() = delete;
  Image(const char* path)
    : m_img(stbi_load(path, &m_width, &m_height, &m_channels, 0)) {}
  /*
  {
    unsigned char* img = stbi_load(path, &m_width, &m_height, &m_channels);
  }
  */
  void write_image(const char* imgname) {
    stbi_write_png(imgname, m_width, m_height, m_channels, m_img, m_width * m_channels);
  }
  ~Image() {
    printf("free img\n");
    stbi_image_free(m_img);
  }
};

int main() {
  Image img("../tests/lenna.png");
  img.write_image("lenna.png");
  /*
  unsigned char *img = stbi_load("../tests/lenna.png", &width, &height, &channels, 0);
  if(img == NULL) {
    fprintf(stderr, "Error in loading the image\n");
    exit(1);
  }
  printf("Loaded image with width: %d height: %d number of channels %d\n", width, height, channels);
  stbi_write_png("lenna.png", width, height, channels, img, width*channels);
  
  stbi_image_free(img);
  */

}
