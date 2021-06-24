#ifndef lstm_H_
#define lstm_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include "aux_ker.h"
#include "lstm_ker.h"
#include "lstm_rec.h"
#include "lstm_bias.h"
#include "main_out.h"
#define N1 100
#define input_size 2000
#define hidden_layer 100
#define num_param 2 
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "filter_def.h"
using namespace sc_dt;
using namespace sc_core;

struct lstm : public sc_module {
  tlm_utils::simple_target_socket<lstm> tsock;
//lstm input_1 and input2
  sc_fifo<int> i_1;//since the total input is flatten so at every even position real
  sc_fifo<int> i_2;//after every real is its corresponding imaginary(odd pos)
//output_1 and output2
  sc_fifo<int> o_result1;
  sc_fifo<int> o_result2;


  SC_HAS_PROCESS(lstm);

  lstm(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &lstm::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~lstm() {
	}
	
double aux[N1];
double ker[N1*4*2];
double bias_lstm[N1*4];
double recur_ker[N1*4*N1];
double main[N1];
double input1,input2;
int precision = 1e10;

/*int k1 =0;
for(int i = 0;i<num_param;i++){
for(int j=0;j<4*hidden_layer;i++){
	ker[i][j] = ker_orig[k1];
	k1++;
	}
}

int k1 =0;
for(int i = 0;i<hidden_layer;i++){
for(int j = 0;j<4;j++){
for(int k=0;k<hidden_layer;k++){
	recur_ker[i][j][k] = recur_ker_orig[k1];
	k1++;
	}
}
}*/

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
float f_state = 0.0; float i_state =0.0; float o_state = 0.0; float g_state =0.0;float h_state =0.0; float c_state = 0.0;float recur_ker_l[N1] = {0.0}; float output_main = 0.0; float output_aux = 0.0;
  void do_filter(){
  sc_int<32> res;
        sc_int<32> in_1;
        sc_int<32> input_2;
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
        input1 =0; input2 =0; int j = 0;
        wait(CLOCK_PERIOD, SC_NS);
	
	int first = 0;
       for (unsigned int v = 0; v < hidden_layer; ++v) {
          //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
          wait(CLOCK_PERIOD, SC_NS);
          input1 = i_1.read();
          input2 = i_2.read() ;
          
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

	
	
	///cell compute state
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

    o_result1.write(result1);
    o_result2.write(result2);

    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;
	sc_int<32> res;
        sc_int<32> in_1;
        sc_int<32> input_2;
    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case lstm_RESULT_ADDR:
            res = o_result1.read();
            case lstm_RESULT1_ADDR:
            res = o_result2.read();
            break;
          default:
            std::cerr << "READ Error! lstm::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
          
         buffer.sint = res;
         data_ptr[0] = buffer.uc[0];
         data_ptr[1] = buffer.uc[1];
         data_ptr[2] = buffer.uc[2];
         data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case lstm_R_ADDR:
            in_1.range(7,0) = data_ptr[0];
            in_1.range(15,8) = data_ptr[1];
            in_1.range(23,16) = data_ptr[2];
            in_1.range(31,23) = data_ptr[3];
            i_1.write(in_1);

           case lstm_R1_ADDR:
            input_2.range(7,0) = data_ptr[0];
            input_2.range(15,8) = data_ptr[1];
            input_2.range(23,16) = data_ptr[2];
            input_2.range(31,23) = data_ptr[3];
            i_2.write(input_2);
            break;
          default:
            std::cerr << "WRITE Error! lstm::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
