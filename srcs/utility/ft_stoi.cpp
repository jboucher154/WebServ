#include "utility.hpp"

int ft_stoi(const std::string& str)
{
    int num = 0;
    std::istringstream ss(str);

    ss >> num;
    return num;
}