#pragma once
//The program will take as input a second database, storing the different prices/dates
//to evaluate.
#include <map>
#include <string>
#define CURRENT_YEAR 2025
#define CURRENT_MONTH 04
#define CURRENT_DAY 20
class BitcoinExchange{
    private:
        std::map<unsigned int, std::string> map;
        bool data_stored;
        
    public:
        BitcoinExchange();
        void storeData(const char *filepath);
        void getData(const std::string& line);
        void outputData() const;
        std::pair<int , double> check_input(const std::string& line);
        long double getExchangePrice(double quant, unsigned int dayspassed, std::map<unsigned int, std::string> map);
        bool is_valid_line(std::string line, std::string& date, std::string& price);
        bool is_valid_price(const std::string& price);
        bool isDouble(const std::string &str);
        bool isValidUserInput(const std::string& line, std::string& date, std::string& quant);
        bool isvalidquant(const std::string& num);
        bool    check_and_convert_date(std::string& date);
        int getDayspassed(std::string& year, std::string& month, std::string& day);
        bool isDataStored();
        
};
