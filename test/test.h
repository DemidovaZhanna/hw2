#ifndef TEST_H
#define TEST_H

#include <sstream>
#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <fstream>

#include "standardMultiplication.h"
#include "fastMultiplication.h"
#include "generateNums.h"

template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint)
{
    if (t != u)
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u << " hint: " << hint;
        throw std::runtime_error(os.str());
    }
}

class TestRunner
{
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name)
    {
        try
        {
            func();
            std::cout << test_name << " OK" << std::endl;
        } catch (std::runtime_error& e)
        {
            ++fail_count;
            std::cout << test_name << " fail: " << e.what() << std::endl;
        }
    }

    ~TestRunner()
    {
        if (fail_count > 0)
        {
            std::cout << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }
private:
    int fail_count = 0;
};

template <typename Func, typename... Args>
auto measureExecutionTime(Func&& func, Args&&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " microseconds\n";

    return result;
}

inline void testMultiplyInt()
{
    std::string num1 = "3141592653589793238462643383279502884197169399375105820974944592";
    std::string num2 = "2718281828459045235360287471352662497757247093699959574966967627";
    std::string result = "8539734222673567065463550869546574495034888535765114961879601127067743044893204848617875072216249073013374895871952806582723184";

    measureExecutionTime(multiplyInt, num1, num2);
    AssertEqual(multiplyInt(num1 , num2), result, "Standard method: Multiplying large positive numbers");
    AssertEqual(multiplyInt(num1 , std::string("-") + std::string(num2)), "-" + result,
                        "Standard method: Multiplying large positive and negative numbers");
    AssertEqual(multiplyInt(std::string("-") + std::string(num1) , std::string("-") + std::string(num2)),
                            result, "Standard method: Multiplying large negative numbers");
    AssertEqual(multiplyInt(num1 , "0"), "0", "Standard method: Multiplying a large positive number and zero");

    std::string num3, num4, result1;
    auto last_pos = std::streampos(0);
    for (int i = 0; i < 2; i++)
    {
        readNumbersFromFile(num3, num4, result1, last_pos);
        measureExecutionTime(multiplyInt, num3, num4);
        AssertEqual(multiplyInt(num3 , num4), result1, "Standard method: Multiplying large positive numbers");
    }
}

inline void testMultiplyLargeIntegers()
{
    std::string num1 = "3141592653589793238462643383279502884197169399375105820974944592";
    std::string num2 = "2718281828459045235360287471352662497757247093699959574966967627";
    std::string result = "8539734222673567065463550869546574495034888535765114961879601127067743044893204848617875072216249073013374895871952806582723184";

    measureExecutionTime(karatsuba, num1, num2, 1000);
    AssertEqual(karatsuba(num1 , num2), result, "Karatsuba algorithm: Multiplying large positive numbers");
    // AssertEqual()

    std::string num3, num4, result1;
    auto last_pos = std::streampos(0);

    for (int i = 0; i < 2; i++)
    {
        readNumbersFromFile(num3, num4, result1, last_pos);
        measureExecutionTime(karatsuba, num3, num4, 1000);
        AssertEqual(karatsuba(num3 , num4), result1, "Karatsuba algorithm: Multiplying large positive numbers");
    }
}


#endif //TEST_H
