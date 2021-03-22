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
      

    //glm::vec3 color = glm::vec3(0.0f);
    double allWeights = 0.0;
      double r=0.0;
      double g = 0.0;
      double b = 0.0;
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
          

          double R = i_r.read();
          double G = i_g.read();
          double B = i_b.read();
          
          wait();
          
          r += R * mask[u][v];
          g += G * mask[u][v];
          b += B * mask[u][v];
          wait();
          
          allWeights += mask[u][v];
          
          wait();
      }
    }
    
    
    double result = allWeights;
    double result_r = r/result;
    o_result_r.write(result_r);
    double result_g =  g/result;
    o_result_g.write(result_g);
    double result_b = b/result;
    o_result_b.write(result_b);
}
  }


