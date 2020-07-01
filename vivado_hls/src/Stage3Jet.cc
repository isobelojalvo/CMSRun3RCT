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

bool getRegionNorth(uint16_t rEta, uint16_t rPhi, uint16_t *northEta, uint16_t *northPhi){
#pragma HLS PIPELINE II=3
  *northEta = rEta;
  *northPhi = rPhi + 1;
  if(*northPhi == MaxUCTRegionsPhi) *northPhi = 0;
  else if(*northPhi > MaxUCTRegionsPhi) *northPhi = 0xBEEF;
  return true;
}

bool getRegionSouth(uint16_t rEta, uint16_t rPhi, uint16_t *southEta, uint16_t *southPhi){
#pragma HLS PIPELINE II=3
  *southEta = rEta;
  if(rPhi == 0) *southPhi = MaxUCTRegionsPhi - 1;
  else if(rPhi < MaxUCTRegionsPhi) *southPhi = rPhi - 1;
  else *southPhi = 0xBEEF;
  return true;
}

bool getRegionEast(uint16_t rEta, uint16_t rPhi, uint16_t *eastEta, uint16_t *eastPhi){
#pragma HLS PIPELINE II=3
  *eastEta = rEta + 1;
  if(*eastEta == 0) *eastEta = 1;
  if(*eastEta > MaxUCTRegionsEta) *eastEta = 0;
  *eastPhi = rPhi;
  return true;
}

bool getRegionWest(uint16_t rEta, uint16_t rPhi, uint16_t *westEta, uint16_t *westPhi){
#pragma HLS PIPELINE II=3
  *westEta = rEta -1;
  if(*westEta == 0) *westEta = -1;
  if(*westEta < -MaxUCTRegionsEta) *westEta = 0;
  *westPhi = rPhi;
  return true;
}

bool getRegionNE(uint16_t rEta, uint16_t rPhi, uint16_t *neEta, uint16_t *nePhi){
#pragma HLS PIPELINE II=3
  *neEta = rEta + 1;
  if(*neEta == 0) *neEta = 1;
  if(*neEta > MaxUCTRegionsEta) *neEta = 0;
  *nePhi = rPhi + 1;
  if(*nePhi == MaxUCTRegionsPhi) *nePhi = 0;
  else if(*nePhi > MaxUCTRegionsPhi) *nePhi = 0xBEEF;
  return true;
}

bool getRegionNW(uint16_t rEta, uint16_t rPhi, uint16_t *nwEta, uint16_t *nwPhi){
#pragma HLS PIPELINE II=3
  *nwEta = rEta - 1;
  if(*nwEta == 0) *nwEta = -1;
  if(*nwEta < -MaxUCTRegionsEta) *nwEta = 0;
  *nwPhi = rPhi + 1;
  if(*nwPhi == MaxUCTRegionsPhi) *nwPhi = 0; 
  else if(*nwPhi > MaxUCTRegionsPhi) *nwPhi = 0xBEEF;
  return true; 
}

bool getRegionSE(uint16_t rEta, uint16_t rPhi, uint16_t *seEta, uint16_t *sePhi){
#pragma HLS PIPELINE II=3
  *seEta = rEta + 1;
  if(*seEta == 0) *seEta = 1;
  if(*seEta > MaxUCTRegionsEta) *seEta = 0;
  if(rPhi == 0) *sePhi = MaxUCTRegionsPhi - 1;
  else if(rPhi < MaxUCTRegionsPhi) *sePhi = rPhi - 1;
  else *sePhi = 0xBEEF;
  return true;
}

bool getRegionSW(uint16_t rEta, uint16_t rPhi, uint16_t *swEta, uint16_t *swPhi){
#pragma HLS PIPELINE II=3
  *swEta = rEta - 1;
  if(*swEta == 0) *swEta = -1;
  if(*swEta < -MaxUCTRegionsEta) *swEta = 0;
  if(rPhi == 0) *swPhi = MaxUCTRegionsPhi - 1;
  else if(rPhi < MaxUCTRegionsPhi) *swPhi = rPhi - 1;
  else *swPhi = 0xBEEF;
  return true;
}

bool makeStage3Jets(uint16_t regionTowers[NEta][NPhi][NEtaInRegion][NPhiInRegion], uint16_t jetET[NEta*NPhi], uint16_t jetEta[NEta*NPhi], uint16_t jetPhi[NEta*NPhi]){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=regionTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetET complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetEta complete dim=0
#pragma HLS ARRAY_PARTITION variable=jetPhi complete dim=0

   uint16_t northEta, northPhi, southEta, southPhi, eastEta, eastPhi, westEta, westPhi, neEta, nePhi, nwEta, nwPhi, seEta, sePhi, swEta, swPhi;
   uint16_t cTowers[NEtaInRegion][NPhiInRegion], nTowers[NEtaInRegion][NPhiInRegion], sTowers[NEtaInRegion][NPhiInRegion], eTowers[NEtaInRegion][NPhiInRegion], wTowers[NEtaInRegion][NPhiInRegion], neTowers[NEtaInRegion][NPhiInRegion], nwTowers[NEtaInRegion][NPhiInRegion], seTowers[NEtaInRegion][NPhiInRegion], swTowers[NEtaInRegion][NPhiInRegion];
   uint16_t peakEta, peakPhi, rET, cET, nET, sET, eET, wET, neET, nwET, seET, swET;
#pragma HLS ARRAY_PARTITION variable=cTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=nTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=sTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=eTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=wTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=neTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=nwTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=seTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=swTowers complete dim=0

   for(int tPhi = 0; tPhi < NPhiInRegion; tPhi++) {
#pragma HLS UNROLL
      for(int tEta = 0; tEta < NEtaInRegion; tEta++) {
#pragma HLS UNROLL
         cTowers[tEta][tPhi] = 0;
         nTowers[tEta][tPhi] = 0;
         sTowers[tEta][tPhi] = 0;
         eTowers[tEta][tPhi] = 0;
         wTowers[tEta][tPhi] = 0;
         neTowers[tEta][tPhi] = 0;
         nwTowers[tEta][tPhi] = 0;
         seTowers[tEta][tPhi] = 0;
         swTowers[tEta][tPhi] = 0;
      }
   }

   for(int rPhi = 0; rPhi < NPhi; rPhi++) {
#pragma HLS UNROLL
      for(int rEta = 0; rEta < NEta; rEta++) {
#pragma HLS UNROLL

         getRegionTowers(regionTowers, rEta, rPhi, cTowers, &cET);
         makeRegion(cTowers, &peakEta, &peakPhi, &rET);
         if(cET != rET) fprintf(stderr, "region ET not matching!");

         if(getRegionNorth(rEta, rPhi, &northEta, &northPhi)){
           getRegionTowers(regionTowers, northEta, northPhi, nTowers, &nET);
         }
         else nET = 0;

         if(getRegionSouth(rEta, rPhi, &southEta, &southPhi)){
           getRegionTowers(regionTowers, southEta, southPhi, sTowers, &sET);
         }
         else sET = 0;

         if(getRegionWest(rEta, rPhi, &westEta, &westPhi)){
           getRegionTowers(regionTowers, westEta, westPhi, wTowers, &wET);
         }
         else wET = 0;

         if(getRegionEast(rEta, rPhi, &eastEta, &eastPhi)){
           getRegionTowers(regionTowers, eastEta, eastPhi, eTowers, &eET);
         }
         else eET = 0;

         if(getRegionNE(rEta, rPhi, &neEta, &nePhi)){
           getRegionTowers(regionTowers, neEta, nePhi, neTowers, &neET);
         }
         else neET = 0;

         if(getRegionNW(rEta, rPhi, &nwEta, &nwPhi)){
           getRegionTowers(regionTowers, nwEta, nwPhi, nwTowers, &nwET);
         }
         else nwET  = 0;

         if(getRegionSE(rEta, rPhi, &seEta, &sePhi)){
           getRegionTowers(regionTowers, seEta, sePhi, seTowers, &seET);
         }
         else seET  = 0;

         if(getRegionSW(rEta, rPhi, &swEta, &swPhi)){
           getRegionTowers(regionTowers, swEta, swPhi, swTowers, &swET);
         }
         else swET  = 0;

         uint16_t et3x3 = cET + nET + sET + wET + eET + neET + nwET + seET + swET;
         if(et3x3 > 0x3FF) et3x3 = 0x3FF;

         if(cET >= nET && cET >= nwET && cET >= wET && cET >= swET &&
            cET > sET && cET > seET && cET > eET && cET > neET ){
           jetET[rEta*rPhi] = et3x3;
           jetEta[rEta*rPhi] = peakEta;
           jetPhi[rEta*rPhi] = peakPhi;
         }
         else { jetET[rEta*rPhi] = 0; jetEta[rEta*rPhi] = 0; jetPhi[rEta*rPhi] = 0; }
      }
   }

  return true;
}

bool getJetsInCard(uint16_t towers[NEta*NPhi*NEtaInRegion*NPhiInRegion], uint16_t SortedJet_peakEta[NJetsPerCard], uint16_t SortedJet_peakPhi[NJetsPerCard], uint16_t SortedJet_ET[NJetsPerCard]){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=towers complete dim=0
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
         toSort_peakEta[rEta*rPhi] = 0;
         toSort_peakPhi[rEta*rPhi] = 0;
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
