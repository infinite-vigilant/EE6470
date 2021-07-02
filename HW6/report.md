


# Homework - 6 : gaussian filter in RISC-V

## I. Introduction
In this homework we are told to port the Gaussian blur module to the "basic-acc" platform given by the TAs. We are guided to use the RISCV-VP core models to control hardware modules on the platform. RISC V virtual platform is implemented in SystemC TLM-2.0. It is useful scince it is flexible for both hardware and software simuntaneously. Here we are dealing with a gaussian blur filter which is the result of blurring an image by a gaussian function (named after mathematician and scientist Carl Friedrich Gauss). The gauss function can be formulated as follows : 
>[1 / (2.pi.(sigma)^2)]e^-(x^2 / 2.(sigma)^2)]


It is a widely used effect in graphics software, typically to reduce image noise and reduce detail. The kernel I used is described here: 

```c++
double filter[filterHeight][filterWidth] =
{
  1, 2, 1,
  2, 4, 2,
  1, 2, 1,
};

double factor = 1.0 / 16.0;
double bias = 0.0;
```

## II. Implementation details

We are givena RISC V docker image which contains the source code for a sobel filter. We were asked to use these codes provided by the TAs for a gaussian blur filter. There are two parts in this implementation :

- Use RISCV-VP model with TLM 2.0 interface.
- Write a software application to control the hardware modules through bus.

Now I will show the things which are added in our case of gauss filter. First I will go through the Platform and then the src.

- Platform 
Inside the platform we gave the gaussfilter.cpp file which is the main software link. The code which is added on top of socle filter is mentioned here: 
 > NOTE: This is just some snaps from the code and not the complete code. 

```c++

#include "GaussFilter.h"

class BasicOptions : public Options {
public:
	typedef unsigned int addr_t;
	addr_t gaussFilter_start_addr = 0x75000000;
	addr_t gaussFilter_size = 0x01000000;
	addr_t gaussFilter_end_addr = gaussFilter_start_addr + gaussFilter_size - 1;
	
	}


int sc_main(int argc, char **argv) {
	GaussFilter gauss_filter("gauss_filter");

	// address mapping
	
	bus.ports[14] = new PortMapping(opt.gaussFilter_start_addr, opt.gaussFilter_end_addr);

	// connect TLM sockets

	bus.isocks[14].bind(gauss_filter.tsock);
```
- Src
 The src folder contains the main.cpp file which is essential for hardware communicaton. The sample code is decribed here : 

```c++
// Gauss Filter 
static char* const GAUSSFILTER_START_ADDR = reinterpret_cast<char* const>(0x75000000);
static char* const GAUSSFILTER_READ_ADDR  = reinterpret_cast<char* const>(0x75000004);

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){}

int main() {
  #include "lena_std_short.h" //included here to avoid compiler issue of not initializing global arrays
	unsigned char* source_array= lena_std_short_bmp;
  
  unsigned char  buffer[4] = {0};
  for(int i = 0; i < width; i++){
    for(int j = 0; j < length; j++){
      for(int v = -1; v <= 1; v ++){
        for(int u = -1; u <= 1; u++){
          if((v + i) >= 0  &&  (v + i ) < width && (u + j) >= 0 && (u + j) < length ){
            buffer[0] = *(source_bitmap + bytes_per_pixel * ((j + u) * width + (i + v)) + 2);
            buffer[1] = *(source_bitmap + bytes_per_pixel * ((j + u) * width + (i + v)) + 1);
            buffer[2] = *(source_bitmap + bytes_per_pixel * ((j + u) * width + (i + v)) + 0);
            buffer[3] = 0;
          }else{
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
            buffer[3] = 0;
          }
          write_data_to_ACC(GAUSSFILTER_START_ADDR, buffer, 4);
        }
      }
      read_data_from_ACC(GAUSSFILTER_READ_ADDR, buffer, 4);
}
    }
}

```

## III. Design Model
![o](risc.jpg)



## IV. Experimental results




## V. Discussion and Conclusion
In this homework I learnt the use of stratus high level sysnthesis. We experiment with diffrent kinds of simulations like sim_B, sim_V_BASIC and sim_V_DPA. The TAs sample code has been very helpful in writing the stratus codes. This was the first time we did Hardware level synthesis which was quite helpful.  

