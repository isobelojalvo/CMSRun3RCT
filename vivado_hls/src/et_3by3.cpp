#include "ap_int.h"
#include "Stage3Jet.hh"
#include "region_neighbors.h"

uint16_t adder_3by3(uint16_t arr_i[9])
{
#pragma HLS ARRAY_RESHAPE variable=arr_i complete dim=1

	ap_uint<14> tmp = 0;

	adder_tree_label: for (int i = 0; i < 9; i++)
	{
#pragma HLS UNROLL
		tmp += arr_i[i];
	}

	if (tmp > 1023)
	{
		tmp = 1023;
	}

	return tmp;
}

void et_3by3(uint16_t et[NRegion], uint16_t et_3by3[NRegion])
{

#pragma HLS PIPELINE II=6
#pragma HLS ARRAY_RESHAPE variable=et complete dim=1
#pragma HLS ARRAY_RESHAPE variable=et_3by3 complete dim=1

	et_3by3_loop: for (int idx = 0; idx < NRegion; idx++)
	{
#pragma HLS UNROLL

		uint16_t tmp[9] = { 0 };

		tmp[0] = et[rgn_nghbr[idx].nb_C];
		tmp[1] = et[rgn_nghbr[idx].nb_N];
		tmp[2] = et[rgn_nghbr[idx].nb_S];

		if (rgn_nghbr[idx].nb_E != -1)
		{
			tmp[3] = et[rgn_nghbr[idx].nb_E];
			tmp[4] = et[rgn_nghbr[idx].nb_NE];
			tmp[5] = et[rgn_nghbr[idx].nb_SE];
		}

		if (rgn_nghbr[idx].nb_W != -1)
		{
			tmp[6] = et[rgn_nghbr[idx].nb_W];
			tmp[7] = et[rgn_nghbr[idx].nb_NW];
			tmp[8] = et[rgn_nghbr[idx].nb_SW];
		}

		et_3by3[idx] = adder_3by3(tmp);
	}

	return;
}
