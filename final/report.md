# Final project : Design and Implementation of LSTM Digital pre-distortion model inference on RISC V
## Student 1 : Shaswat Satapathy (309591029)
## Student 2 : Shivani Singh     (309591030) 


## I. Introduction
- With the ever growing number of devices, the signals are getting more complex with dynamic
behaviour, which causes non-linearity in the Power Amplifier (PA) output.
- Digital Pre-distortion (DPD) is considered an effective technique to reduce these effects and to
make this system more real-time, scholars are now diving into deep learning methodologies like
CNN and LSTM.
- To run these algorithms in devices with less capability of resource storage and computation, is a
major challenge since these models are computational and memory escalated.
- We need a scalable and flexible implementation to meet requirements from IoT to high-end
applications, supporting both inference and on-device learning for edge devices.
- In this work, we propose a step-by-step guide to build LSTM deep learning based DPD model
inference accelerators using RISC V ISA.


## II. Implementation details
### 1. Implementation steps 

In this part we describe the basic structure of a DPD model which will be imlemented in our project. 

|Step - 1 | Step - 2|Step - 3|
|---------------|---------------|----------------|
|![i](dpd1.png)|![o](dpd2.png)|![j](dpd3.png)|

### 2. Software Implementation

To model the power amplifer we use python with tesnsorflow running in backend. We take some real power amplifier data and model the PA accordingly. Some of the snaps of training are mention here: 

```python

from keras.layers import LSTM, Dropout, Activation, TimeDistributed, Bidirectional, concatenate, Dense
from tensorflow.keras.layers import LSTM, Dense, Dropout, Activation, TimeDistributed, Bidirectional, concatenate

# In[] Load data

data = sio.loadmat('./Data/indatapa_1G.mat')
indata_1G = np.reshape(data['indatapa_1G'],(-1,1))[:8000]
data = sio.loadmat('./Data/outdatapa_1G.mat')
outdata_1G = np.reshape(data['outdatapa_1G']/11.5,(-1,1))[:8000]

# In[]  PA model


i_r = Input(batch_shape=(batch_size, timesteps, input_dim), name='main_input')
i_i = Input(batch_shape=(batch_size, timesteps, input_dim), name='aux_input')

x = concatenate([i_r, i_i])

o = LSTM(400, return_sequences=True, stateful=True)(x)

o_r = Dense(input_dim, name='main_output')(o)
o_i = Dense(input_dim, name='aux_output')(o)


# In[] Load Weight
m_r.load_weights('./PA_Modeling/weight/LSTM_inv.h5py')

predict_r, predict_i = m_r.predict({'main_input': train_x_r, 'aux_input': train_x_i})
trainPredict_r = np.reshape(scaler_inr.inverse_transform(predict_r_),(-1,))
trainPredict_i = np.reshape(scaler_ini.inverse_transform(predict_i_),(-1,))

```

### Stratus HLS Implementation 

In this part we implemented the DPD model in Stratus HLS for hardware level sysnthesis using SystemC and TLM. We first design the lstm.cpp module and then communicate it with testbench.cpp file. The lstm module is designed as follows: 

```c++
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
          
          wait(CLOCK_PERIOD, SC_NS);
          buffer_r = i_1.read();
          buffer_i = i_2.read() ;
          input1 = buffer_r*precision;
          input2 = buffer_i*precision;
          
         //input state	
      	 i_state = bias_lstm[v] + ker[v]*input1 + ker[N1+v]*input2;
      	
        //forget state
      	f_state = bias_lstm[N1+v] + ker[2*N1+v]*input1 + ker[3*N1+v]*input2;
      	
      	//output state
      	o_state = bias_lstm[2*N1+v] + ker[4*N1+v]*input1 + ker[5*N1+v]*input2;
        
        //cell compute state
      	g_state = bias_lstm[3*N1+v] + ker[6*N1+v]*input1 + ker[7*N1+v]*input2;
	
        //cell state
      	if(v-1<0){
      		c_state = (f_state * 0) + (g_state * i_state);
     		}
     		else{
      		c_state = (f_state * c_state) + (g_state * i_state);
     		}
        
      	//hidden state
      	h_state = o_state * tanh_func(c_state); 
        }
        int result1 = (int)((output_main + main_bias)*precision);
        int result2 = (int)((output_aux+ aux_bias)*precision);
    
        o_result1.put(result1);
        o_result2.put(result2);
}     
```
### 4. RISC-V implementation 

For hardware/software co-realtion we port the DPD model created in SystemC with a RISC V virtual platform using "basic-acc". The sodtware and hardware code written in the VP communicate through DMA. The sample code is mention here : 

```c++

```

## III. Design Model
![design](hw1.png)



## IV. Experimental results
|original input | blurred output|
|---------------|---------------|
|![i](lena_std_short.bmp)|![o](out.bmp)|

## V. Discussion and Conclusion
In this homework I learnt a lot about the gaussian blur filter. Starting with the c++ code to the SystemC code, TAs sample code has been very helpful. The most basic difference I felt was the sending of 3 results, i,e, result_r, result_g, result__b and not just one (unlike soble) from the gaussfilter.cpp file to the testbench.cpp file for file outpu/image dump.  

