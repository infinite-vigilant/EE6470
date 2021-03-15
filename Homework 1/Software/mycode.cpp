#include <cassert>
#include <cmath>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace std;

const int MASK_N = 2;
const int MASK_X = 5;
const int MASK_Y = 5;
const int WHITE = 255;
const int BLACK = 0;
double PI = 3.141592653589793;

unsigned char *image_s = nullptr; // source image array
unsigned char *image_t = nullptr; // target image array
FILE *fp_s = nullptr;             // source file handler
FILE *fp_t = nullptr;             // target file handler

unsigned int width = 0;               // image width
unsigned int height = 0;              // image height
unsigned int rgb_raw_data_offset = 0; // RGB raw data offset
unsigned char bit_per_pixel = 0;      // bit per pixel
unsigned short byte_per_pixel = 0;    // byte per pixel

// bitmap header
unsigned char header[54] = {
    0x42,          // identity : B
    0x4d,          // identity : M
    0,    0, 0, 0, // file size
    0,    0,       // reserved1
    0,    0,       // reserved2
    54,   0, 0, 0, // RGB data offset
    40,   0, 0, 0, // struct BITMAPINFOHEADER size
    0,    0, 0, 0, // bmp width
    0,    0, 0, 0, // bmp height
    1,    0,       // planes
    24,   0,       // bit per pixel
    0,    0, 0, 0, // compression
    0,    0, 0, 0, // data size
    0,    0, 0, 0, // h resolution
    0,    0, 0, 0, // v resolution
    0,    0, 0, 0, // used colors
    0,    0, 0, 0  // important colors
};

// Function to read bmp file
int read_bmp(const char *fname_s) {
  fp_s = fopen(fname_s, "rb");
  if (fp_s == nullptr) {
    std::cerr << "fopen fp_s error" << std::endl;
    return -1;
  }

  // move offset to 10 to find rgb raw data offset
  fseek(fp_s, 10, SEEK_SET);
  assert(fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

  // move offset to 18 to get width & height;
  fseek(fp_s, 18, SEEK_SET);
  assert(fread(&width, sizeof(unsigned int), 1, fp_s));
  assert(fread(&height, sizeof(unsigned int), 1, fp_s));

  // get bit per pixel
  fseek(fp_s, 28, SEEK_SET);
  assert(fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s));
  byte_per_pixel = bit_per_pixel / 8;

  // move offset to rgb_raw_data_offset to get RGB raw data
  fseek(fp_s, rgb_raw_data_offset, SEEK_SET);

  size_t size = width * height * byte_per_pixel;
  image_s = reinterpret_cast<unsigned char *>(new void *[size]);
  if (image_s == nullptr) {
    std::cerr << "allocate image_s error" << std::endl;
    return -1;
  }

  image_t = reinterpret_cast<unsigned char *>(new void *[size]);
  if (image_t == nullptr) {
    std::cerr << "allocate image_t error" << std::endl;
    return -1;
  }

  assert(fread(image_s, sizeof(unsigned char),
               (size_t)(long)width * height * byte_per_pixel, fp_s));
  fclose(fp_s);

  return 0;
}

//function to write bmp file
int write_bmp(const char *fname_t,unsigned char *image_t) {
  unsigned int file_size = 0; // file size

  fp_t = fopen(fname_t, "wb");
  if (fp_t == nullptr) {
    std::cerr << "fopen fname_t error" << std::endl;
    return -1;
  }

  // file size
  file_size = width * height * byte_per_pixel + rgb_raw_data_offset;
  header[2] = (unsigned char)(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  // width
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  // height
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  // bit per pixel
  header[28] = bit_per_pixel;

  // write header
  fwrite(header, sizeof(unsigned char), rgb_raw_data_offset, fp_t);

  // write image
  fwrite(image_t, sizeof(unsigned char),
         (size_t)(long)width * height * byte_per_pixel, fp_t);

  fclose(fp_t);

  return 0;
}

// Function to create Gaussian filter
unsigned char * GaussianBlur(unsigned char *scl, unsigned char *image_t, int w, int h, int ch, int r)
{
    int radius = static_cast<int>(ceil(r * 2.57));
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w*ch; j += ch)
        {
            glm::vec3 color = glm::vec3(0.0f);
            float allWeights = 0.0f;
            for (int ix = i - radius; ix < i + radius + 1; ix++)
            {
                for (int iy = j - radius*ch; iy < j + (radius + 1)*ch; iy += ch)
                {
                    int dsq = (iy / ch - j / ch)*(iy / ch - j / ch) + (ix - i)*(ix - i);// x^2 + y^2
                    float weight = exp(-dsq / (2 * r * r)) / (PI * 2 * r * r);// gaussian function: 1/(2*pi*sgima^2) * e^(-(x^2+y^2)/(2*sigma^2))

                    int x = glm::min((w - 1) * ch, glm::max(0, iy));
                    int y = glm::min(h - 1, glm::max(0, ix));

                    color.r += scl[y*w*ch + x] * weight;
                    color.g += scl[y*w*ch + x + 1] * weight;
                    color.b += scl[y*w*ch + x + 2] * weight;

                    allWeights += weight;
                }
            }
            image_t[i*w*ch + j] = glm::round(color.r / allWeights);
            image_t[i*w*ch + j + 1] = glm::round(color.g / allWeights);
            image_t[i*w*ch + j + 2] = glm::round(color.b / allWeights);
        }
    }
    return image_t;
}


int main(void) {
    read_bmp("lena.bmp");
    image_t = GaussianBlur(image_s, image_t, 512, 512, 3, 1);
    
    write_bmp("lena_my_code.bmp", image_t);

  return 0;
}
