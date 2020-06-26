#include <stdlib.h>
#include <stdio.h>

#include "BoostedJet.hh"

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
  uint16_t iAve = 0xDEADBEEF;
  if(     iEtSum <= etSum) iAve = 0;
  else if(iEtSum <= (etSum << 1)) iAve = 1;
  else if(iEtSum <= (etSum + (etSum << 1))) iAve = 2;
  else iAve = 3;
  return iAve;
}

bool makeRegion(uint16_t towers[NEtaInRegion*NPhiInRegion], uint16_t *peakEta, uint16_t *peakPhi, uint16_t *regionET){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=towers complete dim=0
  
  uint16_t phiStripSum[NPhiInRegion];
#pragma HLS ARRAY_PARTITION variable=phiStripSum complete dim=0
  for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
    phiStripSum[phi] = 0;
    for(int eta = 0; eta < NEtaInRegion; eta++){
#pragma HLS UNROLL
      phiStripSum[phi] += towers[eta*NEtaInRegion+phi];
    }
  }
  
  uint16_t etaStripSum[NEtaInRegion];
#pragma HLS ARRAY_PARTITION variable=etaStripSum complete dim=0
  for(int eta = 0; eta < NEtaInRegion; eta++){
#pragma HLS UNROLL
    etaStripSum[eta] = 0;
    for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
      etaStripSum[eta] += towers[eta+NPhiInRegion*phi];
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

uint16_t getRegionIndex(uint16_t eta, uint16_t phi){
//FIXME
}

bool getRegionTowers(uint16_t regionTowers[NCrates*NCardsInCrate*NRegionsInCard][NEtaInRegion*NPhiInRegion], uint16_t eta, uint16_t phi, uint16_t *towers[NEtaInRegion*NPhiInRegion]){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=regionTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=towers complete dim=0

  uint16_t regionIndex = getRegionIndex(eta, phi); 
  for(int phi = 0; phi < NPhiInRegion; phi++) {
#pragma HLS UNROLL
    for(int eta = 0; eta < NEtaInRegion; eta++){
#pragma HLS UNROLL
      *towers[eta*NEtaInRegion+phi] = regionTowers[regionIndex][eta*NEtaInRegion+phi];  
    }
  }
  return true;
}

bool getRegionNorth(uint16_t centerEta, uint16_t centerPhi, uint16_t *northEta, uint16_t *northPhi){
#pragma HLS PIPELINE II=3
  *northEta = centerEta;
  *northPhi = centerPhi + 1;
  if(*northPhi == MaxUCTRegionsPhi) *northPhi = 0;
  else if(*northPhi > MaxUCTRegionsPhi) *northPhi = 0xDEADBEEF;
  return true;
}

bool getRegionSouth(uint16_t centerEta, uint16_t centerPhi, uint16_t *southEta, uint16_t *southPhi){
#pragma HLS PIPELINE II=3
  *southEta = centerEta;
  if(centerPhi == 0) *southPhi = MaxUCTRegionsPhi - 1;
  else if(centerPhi < MaxUCTRegionsPhi) *southPhi = centerPhi - 1;
  else *southPhi = 0xDEADBEEF;
  return true;
}

bool getRegionEast(uint16_t centerEta, uint16_t centerPhi, uint16_t *eastEta, uint16_t *eastPhi){
#pragma HLS PIPELINE II=3
  *eastEta = centerEta + 1;
  if(*eastEta == 0) *eastEta = 1;
  if(*eastEta > MaxUCTRegionsEta) *eastEta = 0;
  *eastPhi = centerPhi;
  return true;
}

bool getRegionWest(uint16_t centerEta, uint16_t centerPhi, uint16_t *westEta, uint16_t *westPhi){
#pragma HLS PIPELINE II=3
  *westEta = centerEta -1;
  if(*westEta == 0) *westEta = -1;
  if(*westEta < -MaxUCTRegionsEta) *westEta = 0;
  *westPhi = centerPhi;
  return true;
}

bool getRegionNE(uint16_t centerEta, uint16_t centerPhi, uint16_t *neEta, uint16_t *nePhi){
#pragma HLS PIPELINE II=3
  *neEta = centerEta + 1;
  if(*neEta == 0) *neEta = 1;
  if(*neEta > MaxUCTRegionsEta) *neEta = 0;
  *nePhi = centerPhi + 1;
  if(*nePhi == MaxUCTRegionsPhi) *nePhi = 0;
  else if(*nePhi > MaxUCTRegionsPhi) *nePhi = 0xDEADBEEF;
  return true;
}

bool getRegionNW(uint16_t centerEta, uint16_t centerPhi, uint16_t *nwEta, uint16_t *nwPhi){
#pragma HLS PIPELINE II=3
  *nwEta = centerEta - 1;
  if(*nwEta == 0) *nwEta = -1;
  if(*nwEta < -MaxUCTRegionsEta) *nwEta = 0;
  *nwPhi = centerPhi + 1;
  if(*nwPhi == MaxUCTRegionsPhi) *nwPhi = 0; 
  else if(*nwPhi > MaxUCTRegionsPhi) *nwPhi = 0xDEADBEEF;
  return true; 
}

bool getRegionSE(uint16_t centerEta, uint16_t centerPhi, uint16_t *seEta, uint16_t *sePhi){
#pragma HLS PIPELINE II=3
  *seEta = centerEta + 1;
  if(*seEta == 0) *seEta = 1;
  if(*seEta > MaxUCTRegionsEta) *seEta = 0;
  if(centerPhi == 0) *sePhi = MaxUCTRegionsPhi - 1;
  else if(centerPhi < MaxUCTRegionsPhi) *sePhi = centerPhi - 1;
  else *sePhi = 0xDEADBEEF;
  return true;
}

bool getRegionSW(uint16_t centerEta, uint16_t centerPhi, uint16_t *swEta, uint16_t *swPhi){
#pragma HLS PIPELINE II=3
  *swEta = centerEta - 1;
  if(*swEta == 0) *swEta = -1;
  if(*swEta < -MaxUCTRegionsEta) *swEta = 0;
  if(centerPhi == 0) *swPhi = MaxUCTRegionsPhi - 1;
  else if(centerPhi < MaxUCTRegionsPhi) *swPhi = centerPhi - 1;
  else *swPhi = 0xDEADBEEF;
  return true;
}

bool makeBoostedJet(uint16_t centerEta, uint16_t centerPhi, uint16_t regionTowers[NCrates*NCardsInCrate*NRegionsInCard][NEtaInRegion*NPhiInRegion], bitset<12> *etaPattern, bitset<12> *phiPattern, uint16_t *jetET){
#pragma HLS PIPELINE II=3
#pragma HLS ARRAY_PARTITION variable=regionTowers complete dim=0
  bitset<12> ePattern = 0, pPattern = 0;
  uint16_t northEta, northPhi, southEta, southPhi, eastEta, eastPhi, westEta, westPhi, neEta, nePhi, nwEta, nwPhi, seEta, sePhi, swEta, swPhi, jetET;
  uint16_t cTowers[16], nTowers[16], sTowers[16], eTowers[16], wTowers[16], neTowers[16], nwTowers[16], seTowers[16], swTowers[16]; 
#pragma HLS ARRAY_PARTITION variable=cTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=nTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=sTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=eTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=wTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=neTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=nwTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=seTowers complete dim=0
#pragma HLS ARRAY_PARTITION variable=swTowers complete dim=0
  uint16_t boostedJetTowers[144];
#pragma HLS ARRAY_PARTITION variable=boostedJetTowers complete dim=0
  for(size_t iPhi = 0; iPhi < 12; iPhi++){
#pragma HLS UNROLL
    for(size_t iEta = 0; iEta < 12; iEta++) {
#pragma HLS UNROLL
      boostedJetTowers[iEta*12+iPhi] = 0;
    }
  }

  getRegionTowers(regionTowers[NRegion][16], eta, phi, &cTowers);
  uint16_t etaOffset = 4;
  uint16_t phiOffset = 4;
  for(size_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
    for(size_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
        if(centerEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = cTowers[iEta*4+iPhi];
        else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = cTowers[iEta*4+iPhi];
      }
    }
  }

  if(getRegionNorth(centerEta, centerPhi, &northEta, &northPhi)){
    getRegionTowers(regionTowers[NRegion][16], northEta, northPhi, &nTowers){
      int etaOffset = 4;
      int phiOffset = 8;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(northEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = nTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = nTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionSouth(centerEta, centerPhi, &southEta, &southPhi)){
    getRegionTowers(regionTowers[NRegion][16], southEta, southPhi, &sTowers){
      int etaOffset = 4;
      int phiOffset = 0;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(southEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = sTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = sTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionWest(centerEta, centerPhi, &westEta, &westPhi)){
    getRegionTowers(regionTowers[NRegion][16], westEta, westPhi, &wTowers){
      int etaOffset = 0;
      int phiOffset = 4;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(westEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = wTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = wTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionEast(centerEta, centerPhi, &eastEta, &eastPhi)){
    getRegionTowers(regionTowers[NRegion][16], eastEta, eastPhi, &eTowers){
      int etaOffset = 8;
      int phiOffset = 4;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(eastEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = eTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = eTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionNE(centerEta, centerPhi, &neEta, &nePhi)){
    getRegionTowers(regionTowers[NRegion][16], neEta, nePhi, &neTowers){
      int etaOffset = 8;
      int phiOffset = 8;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(neEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = neTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = neTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionNW(centerEta, centerPhi, &nwEta, &nwPhi)){
    getRegionTowers(regionTowers[NRegion][16], nwEta, nwPhi, &nwTowers){
      int etaOffset = 0;
      int phiOffset = 8;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(nwEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = nwTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = nwTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionSE(centerEta, centerPhi, &seEta, &sePhi)){
    getRegionTowers(regionTowers[NRegion][16], seEta, sePhi, &seTowers){
      int etaOffset = 8;
      int phiOffset = 0;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(seEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = seTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = seTowers[iEta*4+iPhi];
        }
      }
    }
  }

  if(getRegionSW(centerEta, centerPhi, &swEta, &swPhi)){
    getRegionTowers(regionTowers[NRegion][16], swEta, swPhi, &swTowers){
      int etaOffset = 0;
      int phiOffset = 0;
      for(uint32_t iPhi = 0; iPhi < 4; iPhi++){
#pragma HLS UNROLL
        for(uint32_t iEta = 0; iEta < 4; iEta++) {
#pragma HLS UNROLL
          if(swEta < 0) boostedJetTowers[(etaOffset+3-iEta)*12+(phiOffset+iPhi)] = swTowers[iEta*4+iPhi];
          else boostedJetTowers[(etaOffset+iEta)*12+(phiOffset+iPhi)] = swTowers[iEta*4+iPhi];
        }
      }
    }
  }

  *jetET = 0;
  for(uint32_t iPhi = 0; iPhi < 12; iPhi++){
#pragma HLS UNROLL
    for(uint32_t iEta = 0; iEta < 12; iEta++){
#pragma HLS UNROLL
      *jetET += boostedJetTowers[iEta*12+iPhi];
      if(boostedJetTowers[iEta*12+iPhi] > activityLevel) {
        activeTower[iEta][iPhi] = true;
      }
      else activeTower[iEta][iPhi] = false;
    }
  }
  for(uint32_t iEta = 0; iEta < 12; iEta++){
#pragma HLS UNROLL
    bool activeStrip = false;
    for(uint32_t iPhi = 0; iPhi < 12; iPhi++){
#pragma HLS UNROLL
      if(activeTower[iEta][iPhi]) activeStrip = true;
    }
    if(activeStrip) ePattern |= (0x1 << iEta);
  }
  *etaPattern = ePattern;
  for(uint32_t iPhi = 0; iPhi < 12; iPhi++){
#pragma HLS UNROLL
    bool activeStrip = false;
    for(uint32_t iEta = 0; iEta < 12; iEta++){
#pragma HLS UNROLL
      if(activeTower[iEta][iPhi]) activeStrip = true;
    }
    if(activeStrip) pPattern |= (0x1 << iPhi);
  }
  *phiPattern = pPattern;
}
