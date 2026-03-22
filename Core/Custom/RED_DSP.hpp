/*
 * RED_DSP.h
 *
 *  Created on: Jan 2, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_RED_DSP_HPP_
#define CUSTOM_RED_DSP_HPP_

#include "arm_math.h" // DSP math functions

namespace DSP {
	void exampleInitFilter(void);
	void exampleFilterSample(float32_t* input, float32_t* output);
}

#endif /* CUSTOM_RED_DSP_HPP_ */
