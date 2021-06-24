#include <string.h>
#include "stdio.h"

// Sobel Filter ACC
static char* const lstm_START_ADDR = reinterpret_cast<char* const>(0x77000000);
static char* const lstm_START1_ADDR = reinterpret_cast<char* const>(0x77000036);
static char* const lstm_READ_ADDR  = reinterpret_cast<char* const>(0x77000070);
static char* const lstm_READ1_ADDR = reinterpret_cast<char* const>(0x7700000120);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;

unsigned int ReadfromByteArray(unsigned char* array, unsigned int offset) {
	unsigned int output = (array[offset] << 0) | (array[offset + 1] << 8) | (array[offset + 2] << 16) | (array[offset + 3] << 24);
	return output;
}

union word{
    int uint;
    char uc[4];
};

void write_data_to_ACC(char* ADDR, int buffer, int len){
	word data;
	unsigned char buff[4];
	
	data.uint = buffer;
	buff[0] = data.uc[0];
        buff[1] = data.uc[1];
        buff[2] = data.uc[2];
        buff[3] = data.uc[3];
	
	 if(_is_using_dma){  
	    // Using DMA 
	    *DMA_SRC_ADDR = (uint32_t)(buff);
	    *DMA_DST_ADDR = (uint32_t)(ADDR);
	    *DMA_LEN_ADDR = len;
	    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
	  }else{
	    // Directly Send
	    memcpy(ADDR, buff, sizeof(unsigned char)*len);
	  }
	}


void read_data_from_ACC(char* ADDR, int buffer, int len){

	word data;
	unsigned char buff[4];
	
	
	
	 if(_is_using_dma){  
	    // Using DMA 
	    *DMA_SRC_ADDR = (uint32_t)(ADDR);
	    *DMA_DST_ADDR = (uint32_t)(buff);
	    *DMA_LEN_ADDR = len;
	    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
	  }else{
	    // Directly Send
	    memcpy(buff,ADDR, sizeof(unsigned char)*len);
	  }
	  data.uc[0] = buff[0]; 
         data.uc[1] = buff[1];
         data.uc[2] = buff[2];
	 data.uc[3] = buff[3];
	 //printf("%d",buff[0]);
	 buffer = data.uint;
	}

int main() {
  #include "input_real.h" //included here to avoid compiler issue of not initializing global arrays
  #include "input_imag.h"
	

  /*unsigned int input_rgb_raw_data_offset = ReadfromByteArray(source_array, 10);
	unsigned int width = ReadfromByteArray(source_array, 18);
	unsigned int length = ReadfromByteArray(source_array, 22);
	unsigned int bytes_per_pixel = ReadfromByteArray(source_array, 28) / 8;
	unsigned char* source_bitmap = &source_array[input_rgb_raw_data_offset];
  printf("======================================\n");
  printf("\t  Reading from array\n");
  printf("======================================\n");
	printf(" input_rgb_raw_data_offset\t= %d\n", input_rgb_raw_data_offset);
	printf(" width\t\t\t\t= %d\n", width);
	printf(" length\t\t\t\t= %d\n", length);
	printf(" bytes_per_pixel\t\t= %d\n",bytes_per_pixel);
  printf("======================================\n");*/

  int buffer_r= 0; int buffer_i =0; int out_r = 0; int out_i = 0;
  for(int i = 0; i < 2000; i++){
            buffer_r = input_r[i];
            buffer_i = input_i[i];
         
          write_data_to_ACC(lstm_START_ADDR, buffer_r, 4);
          write_data_to_ACC(lstm_START1_ADDR, buffer_i, 4);
        
      
      read_data_from_ACC(lstm_READ_ADDR, buffer_r, 4);
      read_data_from_ACC(lstm_READ1_ADDR, buffer_i, 4);

     printf("[%d] >>> %d , %d \n",i ,buffer_r,buffer_i );
      // if(*(buffer + 0) > 90){
      //   printf("%d,", 255 );
      // }else{
      //   printf("%d,", 0 );        
      // }
    }
  }

