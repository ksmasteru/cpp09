#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define CURRENT_YEAR 2025
#define CURRENT_MONTH 04
#define CURRENT_DAY 20
#define MAX_QUANT 1000

#include <map>

void errExit(const char *msg)
{
    std::cerr << msg << std::endl;
    exit(1);
}

int getDayspassed(std::string& year, std::string& month, std::string& day)
{
    return ((std::stoi(year) - 2009) * 365 + (std::stoi(month) - 1) * 30
        + std::stoi(day));
}

bool    check_and_convert_date(std::string& date)
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
            errExit("incorrect date format");
    }
    if (std::stoi(year) < 2009 || std::stoi(year) > CURRENT_YEAR)
        errExit("incorrect date year");
    if (date[4] != '-' || date[7] != '-')
        errExit("incorrect date format");
    for (int i = 0; i < month.size(); i++)
    {
        if (!(std::isdigit(month[i])) || !(std::isdigit(day[i])))
            return ("incorrect date format");
    }
    if ((std::stoi(month) > 12) || (std::stoi(month) <= 0)
         || (std::stoi(day) <= 0) || (std::stoi(day) > 31))
         errExit("incorrect year");
    //std::cout << "valid date " << std::endl;
    int days = getDayspassed(year, month, day);
    date = std::to_string(days);
    //std::cout << "days passed are " << dayspassed << std::endl;
    return (true);
}

bool isvalidquant(const std::string& num)
{
    if (num.size() == 0)
        return (printf("invalid quantity\n"), false);
    size_t i = 0;
    for (i = 0; i < num.size(); i++)
    {
        if (num[i] < '0' || num[i] > '9')
            return (false);
    }
    if ((std::stoi(num) >= 0) && (std::stoi(num) <= MAX_QUANT))
        return (true);
    return (printf("out of range quantity \n"),false);
}

bool isValidUserInput(const std::string& line, std::string& date, std::string& quant)
{
    // 2011-01-03 | 3
    size_t ko_pos = line.find('|', 0);
    if (ko_pos == std::string::npos || line.size() < 14)
         return (("invalid line %s\n", line), false);
    if (line[11] != '|')
        return (("invalid line %s missing '|' \n", line), false);
    date = line.substr(0, 10);
    quant = line.substr(13, line.size() - 12);
    std::cout << "quant to parse ist " << quant << std::endl;
    if (!check_and_convert_date(date))/*makes date represent days passed as a string*/
        return (printf("84"), false);
    if (!isvalidquant(quant))
        return (printf("86"), false);
    return (true);
}

int exchangeValue(const std::string& line)
{
    // check if valid same way
    std::string date;
    std::string quant; //  should be parsed as price 0---1000 max;
    if (!isValidUserInput(line, date, quant)) /*converts date to days*/
    {
         std::cout << "invalid date format" << std::endl;
         return -1;
    }
    int dayspassed = std::stoi(date);
    std::cout << "dayspassed " << dayspassed << " Quant : " << quant << std::endl; 
    exit(1);
    return (0);   
}




bool isDouble(const std::string &str) {
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

bool is_valid_price(const std::string& price)
{
    if (isDouble(price))
        return true;
    std::cout << "invalid price" << std::endl;
    return false;
}

bool is_valid_line(std::string line, std::string &date, std::string& price)
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
    if (!check_and_convert_date(date))
        return (false);
    if (!is_valid_price(price))
        return (false);
    //std::cout << "valid price and date" << std::endl;
    return true;
}

/*return quant * map[dayspassed]
* find <= dayspasse 
*/
double getExchangePrice(unsigned int quant, unsigned int dayspassed, std::map<unsigned int, std::string> map)
{
    std::map<unsigned int, std::string>::iterator it;
    while ((it = map.find(dayspassed)) == map.end())
        dayspassed--;
    
    if (it != map.end())
    {
        std::cout << "chosen price is " << std::stod(it->second) << std::endl;
        return (std::stod(it->second) * quant);
    }
    return (0);
}

std::pair<int , int> check_input(const std::string& line)
{
    std::string date;
    std::string quant;
    if (!isValidUserInput(line, date, quant))
        return (std::pair<int, int>(-1, -1));
    return (std::pair<int, int>(std::stoi(date), std::stoi(quant)));
}

void storeData(const char* filepath, const std::string& line)
{
    std::ifstream ifs(filepath);
    std::string str;
    std::string date;
    std::string price;
    std::map <unsigned int, std::string> map;
    if (!ifs)
        return ;
    // read line by line :
    // skip first line
    unsigned int days;
    //day 0 is 2009-01-0 
    std::getline(ifs, str);
    while(std::getline(ifs, str))
    {
        if (is_valid_line(str, date, price)) /*date parsed*/
            map.insert(std::pair<unsigned int, std::string>(std::stoi(date), price));
        else
            std::cout << str << ": is incorrect" << std::endl;
    }
    std::pair<int, int> a = check_input(line);
    if (a.first == -1)
        return ;
    double  result = getExchangePrice((unsigned int)a.second, (unsigned int)a.first, map);
    std::cout << "price is " << result << std::endl;
}

// provide date | quant to be fetched form data.csv
int main(int ac, char **av)
{
    if (ac != 2)
        return (1);
    std::string date;
    std::string price;
    storeData("data.csv", av[1]);
    try {
        
    }
}