#ifndef MULTIPLICATION_H
#define MULTIPLICATION_H

#include <string>
#include <vector>
#include <numeric>
#include <ranges>

// Standard method
inline std::string multiplyInt(const std::string& num1, const std::string& num2)
{
    if (num1 == "0" || num2 == "0") return "0";

    bool isNegative = (num1.front() == '-') ^ (num2.front() == '-');

    std::string_view num1_view = num1;
    std::string_view num2_view = num2;

    if (num1_view.front() == '-') num1_view.remove_prefix(1);
    if (num2_view.front() == '-') num2_view.remove_prefix(1);

    const size_t n = num1_view.size(),
                 m = num2_view.size();
    std::vector result(n + m, 0);

    for (int i = 0; i < n; ++i)
    {
        int digit1 = num1_view[n - 1 - i] - '0';
        for (int j = 0; j < m; ++j)
        {
            int digit2 = num2_view[m - 1 - j] - '0';
            result[i + j] += digit1 * digit2;
            result[i + j + 1] += result[i + j] / 10;
            result[i + j] %= 10;
        }
    }

    auto first_non_zero = std::ranges::find_if(result | std::views::reverse, [](int digit) { return digit != 0; });

    std::string product = std::accumulate(first_non_zero, result.rend(), std::string{},
                                [](const std::string& acc, int digit) { return acc + static_cast<char>(digit + '0'); });

    if (isNegative && !product.empty() && product != "0")
        product.insert(product.begin(), '-');

    return product.empty() ? "0" : product;
}

#endif //MULTIPLICATION_H
