#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
std::vector<int> split(const std::string& s, char delimiter)
{
  std::vector<int>   tokens;
  std::string        token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(std::stoi(token));
  }
  return tokens;
}
struct Machine
{
  int buy_cost;
  int resell_value;
  int daily_product;
  //An useful function for ordering the machines by availability date
};
struct Pb_instance
{
  //Since c++ 17, inline definition
  inline static std::multimap<int, Machine> machines_availability{};
  inline static std::map<int, int>          best_funds_each_day{};
  inline static int                         restruct_duration;

  //Before c++17, define as follow outside of class in cpp file
  // std::multimap<int, Machine> Pb_instance::machines_availability{};
  static void wipe()
  {

    //Clear static members in case a former problem was solved
    machines_availability.clear();
    best_funds_each_day.clear();
  }

  int     current_funds;
  int     current_day = 0;
  Machine currently_owned_machine = {0, 0, 0};
  int     current_income()

  {
    return currently_owned_machine.daily_product;
  };
  bool operator>=(Pb_instance p2)
  {
    return current_funds >= p2.current_funds;
  }
  //Start with no machine (no income, no resell value)
  //We need a function for passing time whether or not a machine is bought
  bool buy_machine(Machine machine_bought)
  {
    int affordability =
      current_funds + currently_owned_machine.resell_value - machine_bought.buy_cost;
    if (affordability >= 0)
    {
      current_funds = affordability;
      currently_owned_machine = machine_bought;
      return true;
    }
    else
    {
      return false;
    }
  };

  void next_day()
  {
    current_day++;
  };

  void compute_next_funds()
  {
    //Do not forget to add resell value of currently owned machine to final funds, as it can be used
    //to buy another machine
    // current_funds += currently_owned_machine.resell_value;
    //Get an iterator to upcoming choice days
    auto it = best_funds_each_day.lower_bound(current_day);
    //We just bought a machine. Wait until next day for income start
    //We still need to store a value for the day of the sale
    best_funds_each_day[it->first] =
      std::max(best_funds_each_day[it->first],
               currently_owned_machine.resell_value + current_funds);
    ++it;
    while (it != best_funds_each_day.end())
    {
      //Here we want to store the best value attainable at the beginning of the day
      //This is necessary to check affordability of machines each day
      best_funds_each_day[it->first] =
        std::max(best_funds_each_day[it->first],
                 currently_owned_machine.resell_value + current_funds);
      current_funds += current_income();
      current_day = it->first;
      ++it;
    }
  }
  void solve()
  {
    Pb_instance current_best_choice = *this;
    //For each decision day, determine the best situation that can be attained
    //If you know you have the most money on a particular day, you do not need to worry about affordability of subsequent machines, as
    //if you can't afford one with the current best  funds available, the machine will not be affordable for any strategy
    // for (int day_index = 0; day_index < decision_days.size() - 1; ++day_index)

    //Iterator to upcoming choice days
    auto it = machines_availability.lower_bound(current_day);
    while (it != machines_availability.end())
    {
      //Compute the future starting from current day
      current_day = it->first;
      //available_choices is the list of available machines this day
      auto        available_choices = machines_availability.equal_range(it->first);
      Pb_instance current_situation = *this;
      //Browse all available machines (We consider the initial situation to own a machine with 0 cost and 0 production)
      for (auto machine = available_choices.first; machine != available_choices.second;
           ++machine)
      {
        current_situation = *this;
        //Time to simulate affordable choices and compare outcomes
        current_situation.buy_machine(machine->second);
        current_situation.compute_next_funds();
        //If we just "bought" the ending machine (0,0,0), we need not to compute next incomes
        //Instead add current funds to best funds each day

        //FIXME current situation each day should be best starting situation found from last round
      }
      ++it;
    }
    std::cout << "best available funds : " << best_funds_each_day[restruct_duration]
              << "\n";
  }

  std::vector<int> line_info(std::istream& s)
  {
    std::string line;
    std::getline(s, line);
    //remove the nasty \r at the end of line
    line.pop_back();
    return split(line, ' ');
  }

  bool get_problem(std::istream& s)
  {
    current_day = 0;

    std::vector<int> info = line_info(s);
    std::vector<int> end_indicator(3, 0);
    if (info == end_indicator)
    {
      return false;
    }
    int machines_count = info[0];
    //This map stores each machine available on day d, d being a key of the map
    current_funds = 0;
    restruct_duration = info[2];
    Machine m;
    //Insert a machine of cost 0, income 0 and resell value that represent the initial available funds
    m.buy_cost = 0;
    m.resell_value = info[1];
    m.daily_product = 0;
    machines_availability.insert({0, m});

    best_funds_each_day[0] = current_funds;
    best_funds_each_day[info[2]] = 0;
    for (int m_idx = 0; m_idx < machines_count; ++m_idx)
    {
      info = line_info(s);
      m.buy_cost = info[1];
      m.resell_value = info[2];
      m.daily_product = info[3];
      machines_availability.insert({info[0], m});
      //Store days on which available money will habe to be computed and choices made
      best_funds_each_day[info[0]] = 0;
    }
    return true;
  }
};

//Compute_subset uses dynamic programming to determine for each machine whether or not to buy it
//In the end, only the amount of money after the last machine is sold is to be optimized
//Past decision informations are kept

int main()
{
  //SEE input file might be at another location or the file system might be different
  std::string   file_name = "input.txt";
  std::ifstream s(file_name);
  if (!s.is_open())
  {
    std::cout << "failed to open " << file_name << '\n';
  }
  else
  {
    Pb_instance pb;

    while (pb.get_problem(s)) //Iterate over problem instances
    {
      pb.solve();
      Pb_instance::wipe();
    }
    s.close();
    return 0;
  }
}