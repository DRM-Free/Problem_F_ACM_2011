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
struct Financial_situation
{
  int     funds;
  Machine owned_machine = {0, 0, 0};
  int     total()
  {
    return funds + owned_machine.resell_value;
  }
  bool operator>(Financial_situation s)
  {
    return total() > s.total();
  }
  bool buy_machine(Machine m)
  {
    int affordability = funds + owned_machine.resell_value - m.buy_cost;
    if (affordability >= 0)
    {
      funds = affordability;
      owned_machine = m;
      return true;
    }
    else
    {
      return false;
    }
  }
  int current_income()
  {
    return owned_machine.daily_product;
  }
  void compute_income(int current_day, int target_day)
  {
    funds += (target_day - current_day) * current_income();
    current_day = target_day;
  }
};
struct Pb_instance
{
  //Since c++ 17, inline definition
  std::multimap<int, Machine> machines_availability{};
  //This stores the best money score atteinable each day and the machine to own at the beginning of the day
  //in order to reach this score
  std::map<int, Financial_situation> best_situation_each_day{};
  int                                restruct_duration;
  Financial_situation                initial_situation;

  //Before c++17, define as follow outside of class in cpp file
  // std::multimap<int, Machine> Pb_instance::machines_availability{};
  void wipe()
  {

    //Clear static members in case a former problem was solved
    machines_availability.clear();
    best_situation_each_day.clear();
  }

  int current_day = 0;
  //Compare each current best situation with future situation if current machine is kept
  void update_best_situations(Financial_situation current_situation)
  {
    //SEE possible optimization : as the considered machine is only profitable after a known
    //number of days, update of the best results can be started on this day
    //Iterator to upcoming choice days
    auto it = best_situation_each_day.lower_bound(current_day);
    //We just bought a machine. Income starts with a one day delay
    ++current_day;
    ++it;

    while (it != best_situation_each_day.end())
    {
      //The day the machine is bought, no income is generated, so the machine will not improve over the current best
      //Here we want to store the best value attainable at the beginning of the day
      //This is necessary to check affordability of machines each day
      //Compute income to next decision day
      current_situation.compute_income(current_day, it->first);
      if (current_situation > best_situation_each_day[it->first])
      {
        best_situation_each_day[it->first] = current_situation;
      }
      current_day = it->first;
      ++it;
    }
  }
  //For each decision day, determine the best situation that can be attained
  //If you know you have the most money on a particular day, you do not need to worry about
  //affordability of subsequent machines, as if you can't afford one with the current best
  //funds available, the machine will not be affordable for any strategy
  void solve()
  {
    //Iterator to upcoming choice days
    auto it = machines_availability.lower_bound(current_day);
    while (it != machines_availability.end())
    {
      //Compute the future starting from current day
      current_day = it->first;
      //available_choices is the list of available machines this day
      auto available_choices = machines_availability.equal_range(it->first);
      Financial_situation current_situation;
      //Browse all available machines (We consider the initial situation to own a machine with 0 cost and 0 production)
      for (auto machine = available_choices.first; machine != available_choices.second;
           ++machine)
      {
        current_situation.owned_machine =
          best_situation_each_day[current_day].owned_machine;
        current_situation.funds = best_situation_each_day[current_day].funds;

        //Time to simulate affordable choices and compare outcomes
        if (current_situation.buy_machine(machine->second))
        {
          update_best_situations(current_situation);
        }
      }
      ++it;
    }
    std::cout << "best available funds : "
              << best_situation_each_day[restruct_duration + 1].total() << "\n";
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
    initial_situation.funds = info[1];
    restruct_duration = info[2];
    Machine m;
    //Start with a virtual machine with no value and no production
    m.buy_cost = 0;
    m.daily_product = 0;
    m.resell_value = 0;
    initial_situation.owned_machine = m;
    for (int m_idx = 0; m_idx < machines_count; ++m_idx)
    {
      info = line_info(s);
      m.buy_cost = info[1];
      m.resell_value = info[2];
      m.daily_product = info[3];
      machines_availability.insert({info[0], m});
      //Store days on which available money will have to be computed and choices made
      //Fill the best situation so far for each day with a default (as if no machine was ever bought)
      best_situation_each_day[info[0]] = initial_situation;
    }
    //When the restructuration ends, also add a default best situation
    //We are interested in the situation at the end of day D, which is equivalent to situation
    //at the beginning of day D+1
    best_situation_each_day[restruct_duration + 1] = initial_situation;
    return true;
  }
};

int main()
{
  //SEE select input file here
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
      pb.wipe();
    }
    s.close();
    return 0;
  }
}