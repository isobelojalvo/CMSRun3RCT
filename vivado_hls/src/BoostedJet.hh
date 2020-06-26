#ifndef BoostedJet_hh
#define BoostedJet_hh

#include <stdint.h>

const uint16_t NCrates = 3;
const uint16_t NCardsInCrate = 6;
const uint16_t NRegionsInCard = 7;
const uint16_t NEtaInRegion = 4;
const uint16_t NPhiInRegion = 4;
const uint16_t NPhiInCard = NPhiInRegion;
const uint16_t HFEtaOffset = NRegionsInCard * NEtaInRegion + 1;
const uint16_t NHFRegionsInCard = 6;
const uint16_t NHFEtaInRegion = 2;
const uint16_t NSides = 2;
const uint16_t NEta = NRegionsInCard * NEtaInRegion;
const uint16_t NPhi = NCrates * NCardsInCrate * NPhiInRegion;
const uint16_t NHFEta = NHFRegionsInCard * NHFEtaInRegion;
const uint16_t NHFPhi = NCrates * NCardsInCrate * NPhiInRegion;
const uint16_t MaxCrateNumber = NCrates - 1;
const uint16_t MaxCardNumber = NCardsInCrate - 1;
const uint16_t MaxRegionNumber = NRegionsInCard - 1;
const uint16_t MaxEtaInRegion = NEtaInRegion - 1;
const uint16_t MaxPhiInRegion = NPhiInRegion - 1;
const uint16_t MaxCaloEta = 41;
const uint16_t MaxCaloPhi = 72;
const uint16_t CaloHFRegionStart = 7;
const uint16_t CaloVHFRegionStart = 12;
const uint16_t MaxUCTRegionsPhi = MaxCaloPhi / NPhiInRegion;
const uint16_t MaxUCTRegionsEta = NRegionsInCard + NHFRegionsInCard; // Labelled -MaxUCTRegionsEta to +MaxUCTRegionsEta skipping 0

uint16_t getPeakBinOf4(uint16_t et[4], uint16_t etSum);

bool makeRegion(uint16_t towers[NEtaInRegion*NPhiInRegion], uint16_t *peakEta, uint16_t *peakPhi, uint16_t *regionET);

uint16_t getRegionIndex(uint16_t eta, uint16_t phi);

bool getRegionTowers(uint16_t regionTowers[NCrates*NCardsInCrate*NRegionsInCard][NEtaInRegion*NPhiInRegion], uint16_t eta, uint16_t phi, uint16_t *towers[NEtaInRegion*NPhiInRegion]);

bool getRegionNorth(uint16_t centerEta, uint16_t centerPhi, uint16_t *northEta, uint16_t *northPhi);

bool getRegionSouth(uint16_t centerEta, uint16_t centerPhi, uint16_t *southEta, uint16_t *southPhi);

bool getRegionEast(uint16_t centerEta, uint16_t centerPhi, uint16_t *eastEta, uint16_t *eastPhi);

bool getRegionWest(uint16_t centerEta, uint16_t centerPhi, uint16_t *westEta, uint16_t *westPhi);

bool getRegionNE(uint16_t centerEta, uint16_t centerPhi, uint16_t *neEta, uint16_t *nePhi);

bool getRegionNW(uint16_t centerEta, uint16_t centerPhi, uint16_t *nwEta, uint16_t *nwPhi);

bool getRegionSE(uint16_t centerEta, uint16_t centerPhi, uint16_t *seEta, uint16_t *sePhi);

bool getRegionSW(uint16_t centerEta, uint16_t centerPhi, uint16_t *swEta, uint16_t *swPhi);

bool makeBoostedJet(uint16_t centerEta, uint16_t centerPhi, uint16_t regionTowers[NCrates*NCardsInCrate*NRegionsInCard][NEtaInRegion*NPhiInRegion], bitset<12> *etaPattern, bitset<12> *phiPattern, uint16_t *jetET);

#endif
