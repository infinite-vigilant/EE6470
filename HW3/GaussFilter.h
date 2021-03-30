#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "tlm"
#include "tlm_utils/simple_target_socket.h"

#include "filter_def.h"

class GAUSSFilter : public sc_module {
public:
  tlm_utils::simple_target_socket<GAUSSFilter> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_result;

  SC_HAS_PROCESS(GAUSSFilter);
  GAUSSFilter(sc_module_name n);
  ~GAUSSFilter() = default;

private:
  void do_filter();
  int val[MASK_N];

  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
