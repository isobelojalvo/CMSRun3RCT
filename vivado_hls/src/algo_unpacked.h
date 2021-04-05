#ifndef ALGO_UNPACKED_H
#define ALGO_UNPACKED_H

#include <ap_int.h>

#define N_CH_IN 36
#define N_CH_OUT 48

typedef struct
{
        ap_uint<10> et;
        ap_uint<9> idx;
        ap_uint<2> rloc_phi;
        ap_uint<2> rloc_eta;
} t_so;

void algo_unpacked(ap_uint<112> link_in[N_CH_IN], ap_uint<192> link_out[N_CH_OUT]);

#endif
