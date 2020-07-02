#ifndef Stage3Jet_hh
#define Stage3Jet_hh

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
const uint16_t NEta = NRegionsInCard * NEtaInRegion; // 42
const uint16_t NPhi = NCrates * NCardsInCrate * NPhiInRegion; // 72
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

const uint16_t NJetsPerCard = 12; // FIXME
const uint16_t NRegion = NEta*NPhi; // FIXME

uint16_t getPeakBinOf4(uint16_t et[4], uint16_t etSum);

bool makeRegion(uint16_t towers[NEtaInRegion][NPhiInRegion], uint16_t *peakEta, uint16_t *peakPhi, uint16_t *regionET);

bool getRegionTowers(uint16_t regionTowers[NEta][NPhi][NEtaInRegion][NPhiInRegion], uint16_t rEta, uint16_t rPhi, uint16_t *towers[NEtaInRegion][NPhiInRegion], uint16_t *regionET);

bool makeStage3Jets(uint16_t regionTowers[NEta][NPhi][NEtaInRegion][NPhiInRegion], uint16_t jetET[NEta*NPhi]);

void et_3by3(uint16_t et[NEta*NPhi], uint16_t et_3by3[NEta*NPhi]);

void jet(uint16_t jet_seed, uint16_t et[NEta*NPhi], uint16_t et_3by3[NEta*NPhi], uint16_t jet_et[NEta*NPhi]);

bool getJetsInCard(uint16_t towers[NEta*NPhi*NEtaInRegion*NPhiInRegion], uint16_t SortedJet_peakEta[NJetsPerCard], uint16_t SortedJet_peakPhi[NJetsPerCard], uint16_t SortedJet_ET[NJetsPerCard]);

#endif
