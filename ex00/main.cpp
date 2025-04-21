#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>


int main(int ac, char **av)
{
    if (ac != 2)
        return (1);
    std::string date;
    std::string price;
    BitcoinExchange btc;
    btc.storeData("data.csv");
    // readline by line of user input and get data
    std::ifstream ifs;
    ifs.open(av[1]);
    if (!ifs)
    {
        std::cout << "couldnt open " << av[1] << std::endl;
        return (1); 
    }
    if (!btc.is_valid_header(ifs, "date | value"))
        return 1;
    std::string line;
    if (btc.isDataStored())
    {
        while (std::getline(ifs, line))
            btc.getData(line);
    }
    else
    {
        std::cerr << "no data stored" << std::endl;
    }
}