#include <cmath>

#include "gaussfilter.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

gaussfilter::gaussfilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}



void gaussfilter::do_filter() {
  { wait(); }
  while (true) {
      glm::vec3 color = glm::vec3(0.0f);
      
      wait();
    }
    glm::vec3 color = glm::vec3(0.0f);
    float allWeights = 0.0;
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
          
          float W = i_w.read();
          double R = i_r.read();
          double G = i_g.read();
          double B = i_b.read();
          
          wait();
          
          color.r += R * W;
          color.g += G * W;
          color.b += B * W;
          wait();
          
          allWeights += W;
          
          wait();
      }
    }
    int result = (int)(allWeights);
    o_result.write(result);
  }


