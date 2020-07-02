#ifndef UCT_SUMMARY_CARD_H_
#define UCT_SUMMARY_CARD_H_

#include <stdint.h>
#include <ap_int.h>

// Number of central regions
#define NR_CNTR_REG (252)
// Number of forward regions
#define NR_FWD_REG (216)
// Number of calorimeter regions (central + forward)
#define NR_CALO_REG (NR_CNTR_REG + NR_FWD_REG)

#define PUM_LEVEL_BITSIZE (9)

#define N_CENTRAL_JETS 8
#define N_FWD_JETS 8
#define N_NONISO_EGAMMAS 8
#define N_ISO_EGAMMAS 8
#define N_NONISO_TAUS 8
#define N_ISO_TAUS 8

typedef struct
{
	ap_uint<10> pum_thr;
	ap_uint<10> jet_seed;

	ap_uint<10> egamma_seed;
	ap_ufixed<7, 1, AP_RND, AP_SAT> egamma_IsoFact;

	ap_uint<10> tau_seed;
	ap_ufixed<7, 1, AP_RND, AP_SAT> tau_IsoFact;

} algo_config_t;

typedef struct
{
	ap_uint<12> et;
	ap_uint<8> pho;

} energ_out_t;

typedef struct
{
	ap_uint<11> et;
	ap_uint<1> side;
	ap_uint<7> iphi;
	ap_uint<6> ieta;
} jet_out_t;

typedef struct
{
	ap_uint<9> et;
	ap_uint<1> side;
	ap_uint<7> iphi;
	ap_uint<6> ieta;
} eg_out_t;

typedef struct
{
	ap_uint<9> et;
	ap_uint<1> side;
	ap_uint<7> iphi;
	ap_uint<6> ieta;
} tau_out_t;

typedef struct
{
	energ_out_t et;
	energ_out_t met;
	energ_out_t ht;
	energ_out_t mht;

	jet_out_t jet_central[N_CENTRAL_JETS];
	jet_out_t jet_forward[N_FWD_JETS];

	eg_out_t eg_noniso[N_NONISO_EGAMMAS];
	eg_out_t eg_iso[N_ISO_EGAMMAS];

	tau_out_t tau_noniso[N_NONISO_TAUS];
	tau_out_t tau_iso[N_ISO_TAUS];

} algo_outputs_t;

typedef struct
{
	ap_uint<10> et;
	bool eg_veto;
	bool tau_veto;
	ap_uint<2> rloc_phi;
	ap_uint<2> rloc_eta;
} region_t;

void UCTSummaryCard(region_t centr_region[252],
		region_t fwd_region[18 * 2 * 6],

algo_config_t algo_config,

algo_outputs_t & algo_outputs

);

ap_uint<8> popcount(ap_uint<NR_CALO_REG> bitString);

void egamma(ap_uint<10> egamma_seed, ap_ufixed<7, 1, AP_RND, AP_SAT> egIsoFact,
		region_t regions[NR_CNTR_REG], ap_uint<10> et_3by3[NR_CNTR_REG],
		ap_uint<10> nonIso_egamma_et[NR_CNTR_REG],
		ap_uint<10> Iso_egamma_et[NR_CNTR_REG]);

void tau(ap_uint<10> tau_seed, ap_ufixed<7, 1, AP_RND, AP_SAT> egIsoFact,
		region_t regions[NR_CNTR_REG], ap_uint<10> et_3by3[NR_CNTR_REG],
		ap_uint<10> nonIso_tau_et[NR_CNTR_REG],
		ap_uint<10> Iso_tau_et[NR_CNTR_REG]);

void et_3by3(ap_uint<10> et[NR_CNTR_REG], ap_uint<10> et_3by3[NR_CNTR_REG]);

void jet(ap_uint<10> jet_seed, ap_uint<10> et[NR_CNTR_REG],
		ap_uint<10> et_3by3[NR_CNTR_REG], ap_uint<10> jet_et[NR_CNTR_REG]);

#endif
