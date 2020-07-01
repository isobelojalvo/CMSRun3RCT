#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

//#include "algo_unpacked.h"   // This is where you should have had hls_algo - if not find the header file and fix this - please do not copy this file as that defines the interface
#include "algo_unpacked_new.h"
#include "Stage3Jet.hh"

const uint16_t NTowersPerLink = 11; // Bits 16-31, 32-47, ..., 176-191, keeping range(15, 0) unused
const uint16_t MaxTowers = N_CH_IN * NTowersPerLink;

//#define ALGO_PASSTHROUGH


/*
 * algo_unpacked interface exposes fully unpacked input and output link data.
 * This version assumes use of 10G 8b10b links, and thus providing
 * 192  bits per BX (arranged as an arrray of 3x 64 bits)
 *
 * !!! N.B. Do NOT use the first byte (i.e. link_in/out[x].range(7,0) as this
 * portion is reserved for input/output link alignment markers.
 *
 * The remaining 184 bits (link_in/out[x].range(191,8)) are available for
 * algorithm use.
 *
 * !!! N.B. 2: make sure to assign every bit of link_out[] data. First byte should be assigned zero.
 */

void algo_unpacked_new(ap_uint<192> link_in[N_CH_IN], ap_uint<192> link_out[N_CH_OUT])
{
   // !!! Retain these 4 #pragma directives below in your algo_unpacked implementation !!!

#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0
#pragma HLS PIPELINE II=3
#pragma HLS INTERFACE ap_ctrl_hs port=return

ap_uint<192> tmp_link_out[N_CH_OUT];
#pragma HLS ARRAY_PARTITION variable=tmp_link_out    complete dim=0
for (int idx = 0; idx < N_CH_OUT; idx++){
#pragma HLS UNROLL
   tmp_link_out[idx]         = 0;
}

// null algo specific pragma: avoid fully combinatorial algo by specifying min latency
// otherwise algorithm clock input (ap_clk) gets optimized away
#pragma HLS latency min=3

#ifndef ALGO_PASSTHROUGH


static bool first = false; //true to print 
// Pick the input from link_in
uint16_t towers[NEta*NPhi*NEtaInRegion*NPhiInRegion];
#pragma HLS ARRAY_PARTITION variable=towers complete dim=1
towerLoop: for(int towerID = 0; towerID < NEta * NPhi * NEtaInRegion * NPhiInRegion; towerID++) {
#pragma HLS UNROLL
		if(towerID > MaxTowers) {
		   fprintf(stderr, "Too many towers - aborting");
		   exit(1);
		}
		int link_idx = towerID / NTowersPerLink;
		int bitLo = ((towerID - link_idx * NTowersPerLink) % NTowersPerLink + 1) * 16;
		int bitHi = bitLo + 15;
		towers[towerID] = link_in[link_idx].range(bitHi, bitLo);
		if(first && towers[towerID] > 0) printf("towers[%d] = link_in[%d].range(%d, %d) = %d;\n", towerID, link_idx, bitHi, bitLo, towers[towerID]);
	     }

 uint16_t sortedJet_peakEta[12];
 uint16_t sortedJet_peakPhi[12];
 uint16_t sortedJet_ET[12];  // Output 0-2,3-5,6-8,9-11 in four different links - ignore remaining
 
 #pragma HLS ARRAY_PARTITION variable=sortedJet_peakEta complete dim=0
 #pragma HLS ARRAY_PARTITION variable=sortedJet_peakPhi complete dim=0
 #pragma HLS ARRAY_PARTITION variable=sortedJet_ET complete dim=0

 for(int ijet=0; ijet<12; ijet++){
#pragma HLS UNROLL
    sortedJet_peakEta[ijet]=0;
    sortedJet_peakPhi[ijet]=0;
    sortedJet_ET[ijet]=0;
 }

 bool success = getJetsInCard(towers, 
       sortedJet_peakEta, 
       sortedJet_peakPhi, 
       sortedJet_ET);
 
 //----
 int olink;
 for(int item=0; item < 12; item++) {
 #pragma HLS UNROLL
    olink = item / 3;
    int word = item % 3;
    int bLo1 = word * 32 + 32;
    int bHi1 = bLo1 + 2;
    for(int o=olink; o < N_CH_OUT; o+=4) {
       tmp_link_out[o].range(bHi1,bLo1) = ap_uint<3>(sortedJet_peakEta[item]);
       //tmp_link_out[o].range(bHi1,bLo1) = 0;
    }
    if(first) printf("tmp_link_out[%d].range(%d, %d) = ap_uint<3>(sortedJet_peakEta[%d]) = %d;\n", olink, bHi1, bLo1, item, sortedJet_peakEta[item]);
    int bLo2 = bHi1 + 1;
    int bHi2 = bLo2 + 2;
    for(int o=olink; o < N_CH_OUT; o+=4) {
       tmp_link_out[o].range(bHi2,bLo2) = ap_uint<3>(sortedJet_peakPhi[item]);
       //tmp_link_out[o].range(bHi2,bLo2) = 0;
    }
    if(first) printf("tmp_link_out[%d].range(%d, %d) = ap_uint<3>(sortedJet_peakPhi[%d]) = %d;\n", olink, bHi2, bLo2, item, sortedJet_peakPhi[item]);
    int bLo3 = bHi2 + 1;
    int bHi3 = bLo3 + 15;
    for(int o=olink; o < N_CH_OUT; o+=4) {
       tmp_link_out[o].range(bHi3,bLo3) = ap_uint<16>(sortedJet_ET[item]);
       //tmp_link_out[o].range(bHi3,bLo3) = 0;
    }
    if(first) printf("tmp_link_out[%d].range(%d, %d) = ap_uint<16>(sortedJet_ET[%d]) = %d;\n", olink, bHi3, bLo3, item, sortedJet_ET[item]);
 }

for(int i = 0; i < N_CH_OUT; i++){
   link_out[i] = tmp_link_out[i];
}

//std::cout<<"----------------------------------------------------------------------"<<std::endl;
if(first) first = false;
#else
idxLoop: for (int idx = 0; idx < N_CH_OUT; idx++) {
	    link_out[idx] = link_in[idx];
	 }
#endif

}
