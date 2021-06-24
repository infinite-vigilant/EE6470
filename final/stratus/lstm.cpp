#include "stratus_hls.h"
#include "lstm.h"


lstm::lstm(sc_module_name n):
    sc_module(n)
{
    SC_THREAD(do_filter);
    sensitive << i_clk.pos();
	  dont_initialize();
	  reset_signal_is(i_rst, false);

	  i_1.clk_rst(i_clk, i_rst);
    i_2.clk_rst(i_clk, i_rst);
    o_result1.clk_rst(i_clk, i_rst);
    o_result2.clk_rst(i_clk, i_rst);
    
}

lstm::lstm() {}

double aux[N1];
double ker[N1*4*2];
double bias_lstm[N1*4];
double recur_ker[N1*4*N1];
double main[N1];
int precision = 1e10;
unsigned int base_offset;
int main_bias = -1.193787902593612671e-03*1e+12;
int aux_bias = -8.469416643492877483e-04*1e+12;
float sigmoid(float x){
  float exp_v,s;    
  exp_v = exp((double)-x);
  s= 1/(1+exp_v);
  return s;
}

float tanh_func(float x){
  float s;
  s = tanh(x);
  return s;
}

float hidden_multiply(float h_state,float recur_ker[N1]){
     float sum;
     for (int c = 0; c < N1; c++) {
          sum = sum + h_state*recur_ker[c];
        }
     return sum;
}
float f_state = 0.0; float i_state =0.0; float o_state = 0.0; float g_state =0.0;float h_state =0.0; float       c_state = 0.0;float recur_ker_l[N1] = {0.0}; float output_main = 0.0; float output_aux = 0.0;

void lstm::do_filter(){
    sc_int<32> res;
    sc_dt::sc_uint<24> buffer_r;
    sc_dt::sc_uint<24> buffer_i;
    { wait(CLOCK_PERIOD, SC_NS); }
    
    
    while (true) {
        input1 =0; input2 =0; int j = 0;
        wait(CLOCK_PERIOD, SC_NS);
	
       int first = 0;
       for (unsigned int v = 0; v < hidden_layer; ++v) {
          //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
          wait(CLOCK_PERIOD, SC_NS);
          buffer_r = i_1.read();
          buffer_i = i_2.read() ;
          input1 = buffer_r*precision;
          input2 = buffer_i*precision;
         //input state	
      	 i_state = bias_lstm[v] + ker[v]*input1 + ker[N1+v]*input2;
      	 for(j =0;j<N1;j++){
      		recur_ker_l[j] = recur_ker[first + j];
      		}
      	 i_state = i_state + hidden_multiply(h_state,recur_ker_l);
      	 i_state = sigmoid(i_state);
          
          
        //forget state
      	f_state = bias_lstm[N1+v] + ker[2*N1+v]*input1 + ker[3*N1+v]*input2;
      	first =first+N1;
      	for(j =0;j<N1;j++){
      		recur_ker_l[j] = recur_ker[first+j];
      	}
      	
      	f_state = f_state + hidden_multiply(h_state,recur_ker_l);
      	f_state = sigmoid(f_state);
	
		
      	//output state
      	o_state = bias_lstm[2*N1+v] + ker[4*N1+v]*input1 + ker[5*N1+v]*input2;
      	first = first+N1;
      	for(j =0;j<N1;j++){
      		recur_ker_l[j] = recur_ker[first+j];
     		}
     		o_state = o_state + hidden_multiply(h_state,recur_ker_l);
     		o_state = sigmoid(o_state);
	
      	//cell compute state
      	g_state = bias_lstm[3*N1+v] + ker[6*N1+v]*input1 + ker[7*N1+v]*input2;
      	first = first+N1;
      	for(j =0;j<N1;j++){
      		recur_ker_l[j] = recur_ker[first+j];
      	}
      	g_state = g_state + hidden_multiply(h_state,recur_ker_l);
      	g_state = tanh_func(g_state);
	      first =first+N1; // this is given for the i_state increment after one iteration
	
        //cell state
      	if(v-1<0){
      		c_state = (f_state * 0) + (g_state * i_state);
     		}
     		else{
      		c_state = (f_state * c_state) + (g_state * i_state);
     		}
      	//hidden state
      	h_state = o_state * tanh_func(c_state); 
		    wait(CLOCK_PERIOD, SC_NS);
            
      	output_main = output_main+(h_state*main[v]); 
      	output_aux = output_aux+(h_state*aux[v]); 

        }

        int result1 = (int)((output_main + main_bias)*precision);
        int result2 = (int)((output_aux+ aux_bias)*precision);
    
        o_result1.put(result1);
        o_result2.put(result2);
        wait();

}

}