#include <testing/test.h>
#include "common.h"
#include "blitzbuffers.h"

int main() {
    blitzbuffers::ExpandableBufferBuilder builder(1024);
    BlitzbuffersArchiver test(builder, "general");
    return runTest(test);
}