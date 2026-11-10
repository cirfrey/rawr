import rawr.this_target;
#include "rawr/abi/main.pp"

RAWR_ABI_MAIN(){
    if constexpr(
        rawr::this_compiler == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_COMPILER &&
        rawr::this_abi == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_ABI &&
        rawr::this_cxx == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_CXX &&
        rawr::this_bin == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_BIN &&
        rawr::this_platform == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_PLATFORM &&
        rawr::this_arch == RAWR_TESTING_FIXTURE_DETECTION_EXPECTED_ARCH &&
        // So on...
    )
    {
        return 0;
    }
    else
    {
        // TODO: print out the actual and expected OS values for debugging.
        return 1;
    }
}
