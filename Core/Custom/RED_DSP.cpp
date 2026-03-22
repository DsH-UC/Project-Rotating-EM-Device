/*
 * RED_DSP.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#include "RED_DSP.hpp"

#define NUM_STAGES 1
#define CHUNK_SIZE 4
#define SAMPLE_BUFFER_SIZE 1

namespace RED_DSP {

	// Example IIR filter
	static float32_t iirCoefficients[5*NUM_STAGES] = {
			0.03f, 0.1f, 0.08f, 0.8f, 0.3f
	};

	static float32_t iirStateBuffer[CHUNK_SIZE * NUM_STAGES];

	arm_biquad_casd_df1_inst_f32 filter = nullptr;

	void exampleInitFilter(void) {
		arm_biquad_cascade_df1_init_f32(&filter, NUM_STAGES, iirCoefficients, iirStateBuffer);
	}
	void exampleFilterSample(float32_t *input, float32_t* output){
		arm_biquad_cascade_df1_f32(&filter, input, output, SAMPLE_BUFFER_SIZE);
	}
}



