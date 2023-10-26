#include "utility.hpp"

int ft_stoi(const std::string& str)
{
    int num;
    std::istringstream ss(str);

    ss >> num;
    return num;
}