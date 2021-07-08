#ifndef TestEnc_H_
#define TestEnc_H_

#include "../Parameter.h"
#include "../PrintUtils.h"
#include "../../HEAAN/src/HEAAN.h"
#include "../enc/EncAlgorithm.h"
#include "../MaskingGenerator.h"
#include <math.h>

class TestEnc {
public:
    static void approxSqrt(Parameter param, long iter);

    static void minMax(Parameter param, long iter);

    static void compAndSwap(Parameter param, long iter);
};

#endif // !TestEnc_H_