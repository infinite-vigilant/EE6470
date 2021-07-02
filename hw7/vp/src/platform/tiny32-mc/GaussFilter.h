#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct Filter : public sc_module {
tlm_utils::simple_target_socket<Filter> tsock;

sc_fifo<unsigned char> i_r;
sc_fifo<unsigned char> i_g;
sc_fifo<unsigned char> i_b;
sc_fifo<unsigned char> o_r;
sc_fifo<unsigned char> o_g;
sc_fifo<unsigned char> o_b;

SC_HAS_PROCESS(Filter);

Filter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
    {
        tsock.register_b_transport(this, &Filter::blocking_transport);
        SC_THREAD(do_filter);
    }

    ~Filter() {
    }

    unsigned int base_offset;

    void do_filter(){

        wait(CLOCK_PERIOD, SC_NS);

        while (true) {
            int r_val = 0;
            int g_val = 0;
            int b_val = 0;
            for (unsigned int v = 0; v < MASK_Y; ++v) {
                for (unsigned int u = 0; u < MASK_X; ++u) {
                    unsigned char r = i_r.read();
                    unsigned char g = i_g.read();
                    unsigned char b = i_b.read();
                    wait(CLOCK_PERIOD, SC_NS);
                    r_val += r * mask[u][v];
                    g_val += g * mask[u][v];
                    b_val += b * mask[u][v];
                    wait(CLOCK_PERIOD, SC_NS);
                }
            }
            o_r.write(r_val / 16);
            o_g.write(g_val / 16);
            o_b.write(b_val / 16);
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

        word buffer;

        switch (cmd) {
        case tlm::TLM_READ_COMMAND:
            // cout << "READ" << endl;
            switch (addr) {
            case FILTER_RESULT_ADDR:
                buffer.uc[0] = o_r.read();
                buffer.uc[1] = o_g.read();
                buffer.uc[2] = o_b.read();
                buffer.uc[3] = 0;
                break;
            default:
                std::cerr << "READ Error! Filter::blocking_transport: address 0x"
                        << std::setfill('0') << std::setw(8) << std::hex << addr
                        << std::dec << " is not valid" << std::endl;
            }
            data_ptr[0] = buffer.uc[0];
            data_ptr[1] = buffer.uc[1];
            data_ptr[2] = buffer.uc[2];
            data_ptr[3] = buffer.uc[3];
            break;
        case tlm::TLM_WRITE_COMMAND:
            // cout << "WRITE" << endl;
            switch (addr) {
            case FILTER_R_ADDR:
                i_r.write(data_ptr[0]);
                i_g.write(data_ptr[1]);
                i_b.write(data_ptr[2]);
                break;
            default:
                std::cerr << "WRITE Error! Filter::blocking_transport: address 0x"
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