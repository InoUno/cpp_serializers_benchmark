#include <testing/test.h>
#include "common.h"
#include "blitzbuffers.h"

int main() {
    blitzbuffers::FixedSizeBufferBuilder builder(pow(2, 14));
    BlitzbuffersArchiver test(builder, "fixed");
    return runTest(test);
}