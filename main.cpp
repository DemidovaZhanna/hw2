#include "test.h"
#include "generateNums.h"

int main()
{
    // generateNumbersFile("../test/numbers.txt", 5000);

    TestRunner runner;
    runner.RunTest(testMultiplyInt, "testStandardMultiplication");
    runner.RunTest(testMultiplyLargeIntegers, "testFastMultiplication");
}
