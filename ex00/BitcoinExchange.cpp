#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>

#define MAX_QUANT 1000

int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int num;
    iss >> num;
    return num;
}

long double stringToLongDouble(const std::string& str) {
    std::istringstream iss(str);
    long double value;
    iss >> value;
    return value;
}

std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

BitcoinExchange::BitcoinExchange()
{
    this->data_stored = false;
}

bool BitcoinExchange::isDataStored()
{
    return (this->data_stored);
}

int BitcoinExchange::getDayspassed(std::string& year, std::string& month, std::string& day)
{
    return ((stringToInt(year) - 2009) * 365 + (stringToInt(month) - 1) * 30
        + stringToInt(day));
}

bool    BitcoinExchange::check_and_convert_date(std::string& date)
{
    if (date.size() != 10)
        return (printf("%d", 8), false);
    // 2009-02-12
    std::string year = date.substr(0, 4);
    std::string month = date.substr(5, 2);
    std::string day = date.substr(8, 2);
    std::string dayspassed;
    //std::cout << "month is " << month << std::endl;
    //std::cout << "day is " << day << std::endl;
    for (size_t i = 0 ; i < year.size() ; i++)
    {
        if (!std::isdigit(year[i]))
            return (false);
    }
    if (stringToInt(year) < 2009 || stringToInt(year) > CURRENT_YEAR)
        return (false);
    if (date[4] != '-' || date[7] != '-')
        return (false);
    for (size_t i = 0; i < month.size(); i++)
    {
        if (!(std::isdigit(month[i])) || !(std::isdigit(day[i])))
            return (false);
    }
    if ((stringToInt(month) > 12) || (stringToInt(month) <= 0)
         || (stringToInt(day) <= 0) || (stringToInt(day) > 31))
         return (false);
    //std::cout << "valid date " << std::endl;
    int days = this->getDayspassed(year, month, day);
    date = intToString(days);
    //std::cout << "days passed are " << dayspassed << std::endl;
    return (true);
}

bool BitcoinExchange::isValidUserInput(const std::string& line, std::string& date, std::string& quant)
{
    // 2011-01-03 | 3
    size_t ko_pos = line.find('|', 0);
    if (ko_pos == std::string::npos || line.size() < 14)
    {
        std::cout << "Error: bad input" << " => " << line << std::endl;
        return (false);
    }
    if (line[11] != '|')
        return (false);
    date = line.substr(0, 10);
    quant = line.substr(13, line.size() - 12);
    //std::cout << "quant to parse ist " << quant << std::endl;
    if (!isDouble(quant) || !check_and_convert_date(date))
    {
        std::cout << "Error: bad input" << " => " << line << std::endl;
        return (false);
    }
    if  (stringToLongDouble(quant) > MAX_QUANT)
    {
        std::cout << "Error: too large number" << std::endl;
        return false;
    }
    return (true);
}
/*
int BitcoinExchange::exchangeValue(const std::string& line)
{
    std::string date;
    std::string quant;
    if (!isValidUserInput(line, date, quant))
    {
         std::cout << "invalid date format" << std::endl;
         return -1;
    }
    int dayspassed = stringToInt(date);
    std::cout << "dayspassed " << dayspassed << " Quant : " << quant << std::endl; 
    exit(1);
    return (0);   
}*/


bool    BitcoinExchange::isDouble(const std::string &str) {
    if (str.length() == 0)
        return false;
    size_t i = 0;
    // handle "+. OR -."
    bool isPoint = false;
    while (i < str.length()) {
    if (str[i] == '.' && !isPoint)
      isPoint = true;
    else if (str[i] < '0' || str[i] > '9')
      return (false);
    i++;
  }
    if (str.length() == 1 && isPoint)
        return false;
    return (i == str.length());
}

bool BitcoinExchange::is_valid_price(const std::string& price)
{
    if (this->isDouble(price))
        return true;
    return false;
}

bool BitcoinExchange::is_valid_line(std::string line, std::string &date, std::string& price)
{
    // 2009-02-12,price.
    // is int - is int and len == 2 || is int and len == 4.
    //string substr (size_t pos = 0, size_t len = npos) const;
    size_t ko_pos = line.find(",", 0);
    if (ko_pos == std::string::npos || line.size() <  12)
        return (false);
    date = line.substr(0, 10);
    price = line.substr(11, line.size() - 10);
    //std::cout << "data is " << date << std::endl;
    //std::cout << "price is " << price  << std::endl;
    if (!this->check_and_convert_date(date))
        return (false);
    if (!this->is_valid_price(price))
        return (false);
    //std::cout << "valid price and date" << std::endl;
    return true;
}

/*return quant * map[dayspassed]
* find <= dayspasse 
*/
long double BitcoinExchange::getExchangePrice(double quant, unsigned int dayspassed, std::map<unsigned int, std::string> map)
{
    std::map<unsigned int, std::string>::iterator it;
    while ((it = map.find(dayspassed)) == map.end())
        dayspassed--;
    if (it != map.end())
    {
        long double res = stringToLongDouble(it->second) * quant;
        return (res);
    }
    return (0);
}

std::pair<int ,double> BitcoinExchange::check_input(const std::string& line)
{
    std::string date;
    std::string quant;
    if (!this->isValidUserInput(line, date, quant))
        return (std::pair<int, double>(-1, -1.0));
    return (std::pair<int, double>(stringToInt(date), stringToLongDouble(quant)));
}

void BitcoinExchange::storeData(const char* filepath)
{
    std::ifstream ifs(filepath);
    std::string str;
    std::string date;
    std::string price;
    if (!ifs)
        return ;
    // read line by line :
    // skip first line
    //day 0 is 2009-01-0
    std::getline(ifs, str);
    int i = 0;
    while(std::getline(ifs, str) && ++i)
    {
        if (this->is_valid_line(str, date, price)) /*date parsed*/
            this->map.insert(std::pair<unsigned int, std::string>(stringToInt(date), price));
        else
            std::cout << str << ": is incorrect" << "line : " << i-- << std::endl;
    }
    if (map.size() > 0)
    {
        this->data_stored = true;
        std::cout << "DATA UPDATED SUCCESSFULY ✅ : total lines " << i << std::endl;	
    }
    else
    {
        std::cout << "NO DATA STORED!" << std::endl;
        this->data_stored = false;
    }
}

void BitcoinExchange::getData(const std::string& line)
{
    // check input then returns exchange price;
    if (this->data_stored == false)
    {
       std::cout << "no data stored yet : please provide storeData(filepath) with a valid 'data.csv' file with format 'YYYY-MM-DD,PRICE'"
       << std::endl;
       return ;
    }
    std::pair<int , double>a = this->check_input(line);
    if (a.first == -1)
            return ;
    long double result = this->getExchangePrice(a.second, (unsigned int)a.first, this->map);
   // 2011-01-03 => 3 = 0.9
    std::cout  << line.substr(0, 10) <<  " => " << a.second << " = " << result << std::endl;
}

// provide date | quant to be fetched form data.csv
bool BitcoinExchange::is_valid_header(std::ifstream& ifs, const char* header)
{
    std::string line;
    if (std::getline(ifs, line))
    {
        if (line.compare(header) ==  0)
            return true;
    }
    std::cerr << "invalid input file header :: make sure that input file header is " << header << std::endl;
    return (false);
}