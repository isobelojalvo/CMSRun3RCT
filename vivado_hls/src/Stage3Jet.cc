#include <stdlib.h>
#include <stdio.h>

#include "Stage3Jet.hh"

#include <iostream>
using namespace std;

uint16_t getPeakBinOf4(uint16_t et[4], uint16_t etSum) {
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=et complete dim=0
  uint16_t iEtSum =
    (et[0] >> 1)                +  // 0.5xet[0]
    (et[1] >> 1) + et[1]        +  // 1.5xet[1]
    (et[2] >> 1) + (et[2] << 1) +  // 2.5xet[2]
    (et[3] << 2) - (et[3] >> 1) ;  // 3.5xet[3]
  uint16_t iAve = 0xBEEF;
  if(     iEtSum <= etSum) iAve = 0;
  else if(iEtSum <= (etSum << 1)) iAve = 1;
  else if(iEtSum <= (etSum + (etSum << 1))) iAve = 2;
  else iAve = 3;
  return iAve;
}

bool makeRegion(uint16_t towers[NEtaInRegion][NPhiInRegion], uint16_t *peakEta, uint16_t *peakPhi, uint16_t *regionET){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=towers complete dim=0
  
  uint16_t phiStripSum[NPhiInRegion];
#pragma HLS ARRAY_PARTITION variable=phiStripSum complete dim=0
  for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
    phiStripSum[phi] = 0;
    for(int eta = 0; eta < NEtaInRegion; eta++){
#pragma HLS UNROLL
      phiStripSum[phi] += towers[eta][phi];
    }
  }
  
  uint16_t etaStripSum[NEtaInRegion];
#pragma HLS ARRAY_PARTITION variable=etaStripSum complete dim=0
  for(int eta = 0; eta < NEtaInRegion; eta++){
#pragma HLS UNROLL
    etaStripSum[eta] = 0;
    for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
      etaStripSum[eta] += towers[eta][phi];
    }
  }
  
  *regionET = 0;
  for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
    *regionET += phiStripSum[phi];
  }
  
  *peakEta = getPeakBinOf4(etaStripSum, *regionET);
  *peakPhi = getPeakBinOf4(phiStripSum, *regionET); 
  
  return true;
}

bool getRegionTowers(uint16_t regionTowers[NEta][NPhi][NEtaInRegion][NPhiInRegion], uint16_t rEta, uint16_t rPhi, uint16_t towers[NEtaInRegion][NPhiInRegion], uint16_t *regionET){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=regionTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=towers complete dim=0

  *regionET = 0;
  for(int tPhi = 0; tPhi < NPhiInRegion; tPhi++) {
#pragma HLS UNROLL
    for(int tEta = 0; tEta < NEtaInRegion; tEta++){
#pragma HLS UNROLL
      towers[tEta][tPhi] = regionTowers[rEta][rPhi][tEta][tPhi];  
      *regionET += regionTowers[rEta][rPhi][tEta][tPhi];
    }
  }
  return true;
}

bool makeStage3Jets(uint16_t regionTowers[NEta][NPhi][NEtaInRegion][NPhiInRegion], uint16_t jetET[NEta*NPhi], uint16_t jetEta[NEta*NPhi], uint16_t jetPhi[NEta*NPhi]){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=regionTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetET complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetEta complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetPhi complete dim=0

   uint16_t cET;
   uint16_t cTowers[NEtaInRegion][NPhiInRegion];
#pragma HLS ARRAY_PARTITION variable=cTowers complete dim=0
   uint16_t regionET[NEta*NPhi];
#pragma HLS ARRAY_PARTITION variable=regionET complete dim=0
   uint16_t ET3by3[NEta*NPhi];
#pragma HLS ARRAY_PARTITION variable=ET3by3 complete dim=0
   for(int rPhi = 0; rPhi < NPhi; rPhi++) {
#pragma HLS UNROLL
      for(int rEta = 0; rEta < NEta; rEta++) {
#pragma HLS UNROLL
         jetEta[rEta*NEta+rPhi] = 0;
         jetPhi[rEta*NEta+rPhi] = 0;
         regionET[rEta*NEta+rPhi] = 0;
         ET3by3[rEta*NEta+rPhi] = 0;
      }
   }

   for(int rPhi = 0; rPhi < NPhi; rPhi++) {
#pragma HLS UNROLL
      for(int rEta = 0; rEta < NEta; rEta++) {
#pragma HLS UNROLL
         getRegionTowers(regionTowers, rEta, rPhi, cTowers, &cET);
         makeRegion(cTowers, &jetEta[rEta*NEta+rPhi], &jetPhi[rEta*NEta+rPhi], &regionET[rEta*NEta+rPhi]);
         if(cET != regionET[rEta*NEta+rPhi]) fprintf(stderr, "region ET not matching!");
      }
   }
   et_3by3(regionET, ET3by3);
   jet(10, regionET, ET3by3, jetET);

  return true;
}

bool getJetsInCard(uint16_t towers[NEta*NPhi*NEtaInRegion*NPhiInRegion], uint16_t SortedJet_peakEta[NJetsPerCard], uint16_t SortedJet_peakPhi[NJetsPerCard], uint16_t SortedJet_ET[NJetsPerCard]){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=SortedJet_peakEta complete dim=0
#pragma HLS ARRAY_PARTITION variable=SortedJet_peakPhi complete dim=0
#pragma HLS ARRAY_PARTITION variable=SortedJet_ET complete dim=0

   uint16_t towersET[NEta][NPhi][4][4]; // How many 4x4 regions in one card? NJetsPerCard = ?
#pragma HLS ARRAY_PARTITION variable=towersET complete dim=0
   uint16_t toSort_peakEta[NEta*NPhi];
   uint16_t toSort_peakPhi[NEta*NPhi];
   uint16_t toSort_ET[NEta*NPhi];
#pragma HLS ARRAY_PARTITION variable=toSort_peakEta complete dim=0
#pragma HLS ARRAY_PARTITION variable=toSort_peakPhi complete dim=0
#pragma HLS ARRAY_PARTITION variable=toSort_ET complete dim=0

   for(int rPhi = 0; rPhi < NPhi; rPhi++) {
#pragma HLS UNROLL
      for(int rEta = 0; rEta < NEta; rEta++) {
#pragma HLS UNROLL
         toSort_peakEta[rEta*rPhi] = rEta;
         toSort_peakPhi[rEta*rPhi] = rPhi;
         toSort_ET[rEta*rPhi] = 0;
      }
   }

   // this for-loop covers all 4x4 regions in one RCT card
   // How many will fit one card? NRegion  = ? NEta = ? NPhi = ?

   uint16_t region = 0;
   for(int rPhi = 0; rPhi < NPhi; rPhi++) {
#pragma HLS UNROLL
      for(int rEta = 0; rEta < NEta; rEta++) {
#pragma HLS UNROLL
         for(int tPhi = 0; tPhi < NPhiInRegion; tPhi++) {
#pragma HLS UNROLL
            for(int tEta = 0; tEta < NEtaInRegion; tEta++) {
#pragma HLS UNROLL
               int towerID = (rEta*4+tEta)*16 + (rPhi*4+tPhi);
               if(towerID > 16*16) {
                  fprintf(stderr, "towerID too large");
                  exit(1);
               }
               towersET[rEta][rPhi][tEta][tPhi] = towers[towerID];
            }
         }
      }
   }

   makeStage3Jets(towersET, toSort_ET, toSort_peakEta, toSort_peakPhi);

   for(int kk = 0; kk < 4; kk++){ // NJetsPerCard = ? 
#pragma HLS UNROLL 
      SortedJet_peakEta[kk] = toSort_peakEta[kk];
      SortedJet_peakPhi[kk] = toSort_peakPhi[kk];
      SortedJet_ET[kk] = toSort_ET[kk];
   }

   return true;
}
