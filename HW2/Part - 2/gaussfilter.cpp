#include <cmath>
#include "gaussfilter.h"

gaussfilter::gaussfilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

const int mask[MASK_X][MASK_Y] = {{1,2,1},{2,4,2},{1,2,1}};

void gaussfilter::do_filter() {
  { wait(); }
  while (true) {
      

    w = i_width.read();
    h = i_height.read();

    int r[3][w];
    int g[3][w];
    int b[3][w];
    int cnt;
    for(y = 0 ; y < h ; y++) {    
        if(y < h - 1){
            for(cnt = 0 ;c < w ; c++) { //reading
                r[2][cnt] = i_r.read();
                g[2][cnt] = i_g.read();
                b[2][cnt] = i_b.read();
            }
        }
        
        for(x = 0 ; x < w; x++){
            R = G = B = 0;
            for (i = -1 ; i < filterHeight - 1 ; i++) {
                for (j = -1 ; j < filterWidth - 1 ; j++) {
                    if(0<=y+i && y+i<height && 0<=x+j && x+j<width){
                        R += r[i+1][x+j+1] * filter[i+1][j+1];
                        G += g[i+1][x+j+1] * filter[i+1][j+1];
                        B += b[i+1][x+j+1] * filter[i+1][j+1];
                    }
                }
            }
            result_r.write(R);
            result_g.write(G);
            result_b.write(B);
            
        }
        //shifting
        for(i = 0 ;i < w ; i++) { 
            r[0][i] = r[1][i];
            g[0][i] = g[1][i];
            b[0][i] = b[1][i];
            r[1][i] = r[2][i];
            g[1][i] = g[2][i];
            b[1][i] = b[2][i];
            r[2][i] = g[2][i] = b[2][i] = 0;
        }
        
    }
}


