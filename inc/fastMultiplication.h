#ifndef FASTMULTIPLICATION_H
#define FASTMULTIPLICATION_H

#include <string>
#include <future>
#include <thread>
#include <mutex>

inline std::mutex mtx;

// Karatsuba algorithm
inline std::string add(const std::string& num1, const std::string& num2)
{
    std::string result;
    int carry = 0;
    size_t maxLength = std::max(num1.size(), num2.size());
    result.reserve(maxLength + 1);

    for (size_t i = 0; i < maxLength || carry; ++i)
    {
        int sum = carry;
        if (i < num1.size()) sum += num1[num1.size() - 1 - i] - '0';
        if (i < num2.size()) sum += num2[num2.size() - 1 - i] - '0';
        carry = sum / 10;
        result.push_back(sum % 10 + '0');
    }

    std::reverse(result.begin(), result.end());
    return result;
}

inline std::string subtract(const std::string& num1, const std::string& num2)
{
    std::string result;
    int carry = 0;
    result.reserve(num1.size());

    for (size_t i = 0; i < num1.size(); ++i)
    {
        int diff = num1[num1.size() - 1 - i] - '0' - carry;
        if (i < num2.size())
            diff -= num2[num2.size() - 1 - i] - '0';

        carry = diff < 0;
        if (carry)
            diff += 10;

        result.push_back(diff + '0');
    }

    while (result.size() > 1 && result.back() == '0') result.pop_back();
    std::reverse(result.begin(), result.end());
    return result;
}

inline std::string multiplySingleDigit(const std::string& num, char digit)
{
    if (digit == '0') return "0";
    if (digit == '1') return num;

    int carry = 0;
    std::string result;
    int n = digit - '0';
    result.reserve(num.size() + 1);

    for (int i = num.size() - 1; i >= 0; --i)
    {
        int product = (num[i] - '0') * n + carry;
        carry = product / 10;
        result.push_back(product % 10 + '0');
    }

    if (carry) result.push_back(carry + '0');
    std::reverse(result.begin(), result.end());
    return result;
}

inline std::string shiftLeft(const std::string& num, int zeros)
{
    if (num == "0") return "0";
    std::string result = num;
    result.append(zeros, '0');
    return result;
}

inline std::string simpleMultiply(const std::string& num1, const std::string& num2)
{
    int len1 = num1.size();
    int len2 = num2.size();
    std::string result(len1 + len2, '0');

    for (int i = len1 - 1; i >= 0; --i)
    {
        int carry = 0;
        for (int j = len2 - 1; j >= 0; --j)
        {
            int sum = (num1[i] - '0') * (num2[j] - '0') + (result[i + j + 1] - '0') + carry;
            carry = sum / 10;
            result[i + j + 1] = sum % 10 + '0';
        }
        result[i] += carry;
    }

    size_t startpos = result.find_first_not_of('0');
    return startpos == std::string::npos ? "0" : result.substr(startpos);
}

inline void addToBuffer(const std::string& num1, const std::string& num2, std::string& result)
{
    result.clear();
    int carry = 0;
    size_t maxLength = std::max(num1.size(), num2.size());
    result.reserve(maxLength + 1);

    for (size_t i = 0; i < maxLength || carry; ++i)
    {
        int sum = carry;
        if (i < num1.size()) sum += num1[num1.size() - 1 - i] - '0';
        if (i < num2.size()) sum += num2[num2.size() - 1 - i] - '0';
        carry = sum / 10;
        result.push_back(sum % 10 + '0');
    }

    std::reverse(result.begin(), result.end());
}

inline std::string karatsuba(const std::string& num1, const std::string& num2, int threshold = 500)
{
    std::string z0, z1, z2, middle, tempResult;
    size_t len1 = num1.size();
    size_t len2 = num2.size();

    if (len1 <= 4 || len2 <= 4)
        return simpleMultiply(num1, num2);

    size_t half = std::min(len1, len2) / 2;
    std::string high1 = num1.substr(0, len1 - half);
    std::string low1 = num1.substr(len1 - half);
    std::string high2 = num2.substr(0, len2 - half);
    std::string low2 = num2.substr(len2 - half);

    if (len1 > threshold && len2 > threshold)
    {
        auto futureZ0 = std::async(std::launch::async, karatsuba, low1, low2, threshold);
        auto futureZ2 = std::async(std::launch::async, karatsuba, high1, high2, threshold);

        addToBuffer(high1, low1, tempResult);
        addToBuffer(high2, low2, middle);
        z1 = karatsuba(tempResult, middle, threshold);

        z0 = futureZ0.get();
        z2 = futureZ2.get();
    }
    else
    {
        addToBuffer(high1, low1, tempResult);
        addToBuffer(high2, low2, middle);

        z0 = karatsuba(low1, low2, threshold);
        z1 = karatsuba(tempResult, middle, threshold);
        z2 = karatsuba(high1, high2, threshold);
    }

    addToBuffer(z2, z0, tempResult);
    middle = subtract(z1, tempResult);
    return add(add(shiftLeft(z2, 2 * half), shiftLeft(middle, half)), z0);
}

inline void karatsubaTask(const std::string& low1, const std::string& low2,
                   const std::string& high1, const std::string& high2,
                   std::string& z0, std::string& z1, std::string& z2)
{
    std::string tempResult, middle;
    z0 = karatsuba(low1, low2);
    z2 = karatsuba(high1, high2);
    {
        std::lock_guard<std::mutex> lock(mtx);
        addToBuffer(high1, low1, tempResult);
        addToBuffer(high2, low2, middle);
        z1 = karatsuba(tempResult, middle);
    }
}

inline std::string karatsubaParallel(const std::string& num1, const std::string& num2, int threshold = 1000)
{
    std::string z0, z1, z2, tempResult, middle;
    size_t len1 = num1.size();
    size_t len2 = num2.size();

    if (len1 <= 4 || len2 <= 4)
        return simpleMultiply(num1, num2);

    size_t half = std::min(len1, len2) / 2;
    std::string high1 = num1.substr(0, len1 - half);
    std::string low1 = num1.substr(len1 - half);
    std::string high2 = num2.substr(0, len2 - half);
    std::string low2 = num2.substr(len2 - half);

    std::thread t1(karatsubaTask, low1, low2, high1, high2, std::ref(z0), std::ref(z1), std::ref(z2));
    t1.join();

    addToBuffer(z2, z0, tempResult);
    middle = subtract(z1, tempResult);
    return add(add(shiftLeft(z2, 2 * half), shiftLeft(middle, half)), z0);
}

#endif //FASTMULTIPLICATION_H
