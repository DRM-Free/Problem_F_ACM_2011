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

  //Before c++17, define as follow outside of class in cpp file
  // std::multimap<int, Machine> Pb_instance::machines_availability{};
  void wipe()
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
  int  restruct_duration;
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

  void compute_next_incomes()
  {
    //Get an iterator to upcoming days
    auto it = machines_availability.upper_bound(current_day);
    while (it != machines_availability.end())
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
    //Compute list of decision days
    std::vector<int> decision_days;
    for (auto&& day : machines_availability)
    {
      decision_days.push_back(day.first);
    }
    Pb_instance current_best_choice = *this;
    //For each decision day, determine the best situation that can be attained
    //If you know you have the most money on a particular day, you do not need to worry about affordability of subsequent machines, as
    //if you can't afford one with the current best  funds available, the machine will not be affordable for any strategy
    for (int day_index = 0; day_index < decision_days.size() - 1; ++day_index)
    {
      //available_choices is the list of available machines this day
      auto available_choices =
        machines_availability.equal_range(decision_days[day_index]);
      //Create a default scenario to which compare first choice (this scenario corresponds to buying no machine)
      current_best_choice.wait_income(decision_days[day_index + 1]);
      //Browse all available machines
      for (auto machine = available_choices.first; machine != available_choices.second;
           ++machine)
      {
        //Time to simulate affordable choices and compare outcomes
        Pb_instance hyphothesis = *this;
        hyphothesis.buy_machine(machine->second);
        //We just bought a machine. Wait until next day for income start
        hyphothesis.next_day();
        hyphothesis.wait_income(decision_days[day_index + 1]);
        current_best_choice =
          current_best_choice >= hyphothesis ? current_best_choice : hyphothesis;
      }
    }
    std::cout << "best available funds : " << current_best_choice.current_funds << "\n";
  }

  std::vector<int> line_info(std::istream& s)
  {
    std::string line;
    std::getline(s, line);
    //remove the nasty \r at the end of line
    line.pop_back();
    return split(line, ' ');
  }

  bool get_problem(Pb_instance& pb, std::istream& s)
  {

    std::vector<int> info = line_info(s);
    std::vector<int> end_indicator(3, 0);
    if (info == end_indicator)
    {
      return false;
    }
    int machines_count = info[0];
    //This map stores each machine available on day d, d being a key of the map
    pb.current_funds = info[1];
    pb.restruct_duration = info[2];
    Machine m;
    for (int m_idx = 0; m_idx < machines_count; ++m_idx)
    {
      info = line_info(s);
      m.buy_cost = info[1];
      m.resell_value = info[2];
      m.daily_product = info[3];
      machines_availability.insert({info[0], m});
    }
    //Insert a -virtual- machine at the end for simulating the sale of lastly owned machine
    m.buy_cost = 0;
    m.daily_product = 0;
    m.resell_value = 0;
    machines_availability.insert({info[2], m});
    return true;
  }

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

      while (get_problem(pb, s)) //Iterate over problem instances
      {
        pb.solve();
        Pb_instance::wipe();
      }

      return 0;
    }
  }