#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#define MAX_QUANT 1000

BitcoinExchange::BitcoinExchange()
{
    this->data_stored = false;
}

bool BitcoinExchange::isDataStored()
{
    return (this->data_stored);
}

void errExit(const char *msg)
{
    std::cerr << msg << std::endl;
    exit(1);
}

int BitcoinExchange::getDayspassed(std::string& year, std::string& month, std::string& day)
{
    return ((std::stoi(year) - 2009) * 365 + (std::stoi(month) - 1) * 30
        + std::stoi(day));
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
    for (int i = 0 ; i < year.size() ; i++)
    {
        if (!std::isdigit(year[i]))
            return (false);
    }
    if (std::stoi(year) < 2009 || std::stoi(year) > CURRENT_YEAR)
        return (false);
    if (date[4] != '-' || date[7] != '-')
        return (false);
    for (int i = 0; i < month.size(); i++)
    {
        if (!(std::isdigit(month[i])) || !(std::isdigit(day[i])))
            return (false);
    }
    if ((std::stoi(month) > 12) || (std::stoi(month) <= 0)
         || (std::stoi(day) <= 0) || (std::stoi(day) > 31))
         return (false);
    //std::cout << "valid date " << std::endl;
    int days = this->getDayspassed(year, month, day);
    date = std::to_string(days);
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
    if  (std::stod(quant) > MAX_QUANT)
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
    int dayspassed = std::stoi(date);
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
        long double res = std::stold(it->second) * quant;
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
    return (std::pair<int, double>(std::stoi(date), std::stod(quant)));
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
    unsigned int days;
    //day 0 is 2009-01-0
    std::getline(ifs, str);
    int i = 0;
    while(std::getline(ifs, str) && ++i)
    {
        if (this->is_valid_line(str, date, price)) /*date parsed*/
            this->map.insert(std::pair<unsigned int, std::string>(std::stoi(date), price));
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
    // <days, quant>
    std::pair<int , double>a = this->check_input(line);
    if (a.first == -1)
            return ;
    long double result = this->getExchangePrice(a.second, (unsigned int)a.first, this->map);
   // 2011-01-03 => 3 = 0.9
    std::cout  << line.substr(0, 10) <<  " => " << a.second << " = " << result << std::endl;
}

// provide date | quant to be fetched form data.csv
bool is_valid_header(std::ifstream& ifs, const char* header)
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
    if (!is_valid_header(ifs, "date | value"))
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