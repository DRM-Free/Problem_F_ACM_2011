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

  //Before c++17, define as follow outside of class in cpp file
  // std::multimap<int, Machine> Pb_instance::machines_availability{};

  int     current_funds;
  int     current_day = 0;
  Machine currently_owned_machine = {0, 0, 0};
  int     current_income()

  {
    return currently_owned_machine.daily_product;
  };
  int          restruct_duration;
  virtual bool compare_situations(Pb_instance p1, Pb_instance p2)
  {
    return p1.current_funds >= p2.current_funds;
  }
  //Start with no machine (no income, no resell value)
  //We need a function for passing time whether or not a machine is bought
  void buy_machine(Machine machine_bought)
  {
    current_funds =
      current_funds + currently_owned_machine.resell_value - machine_bought.buy_cost;
    currently_owned_machine = machine_bought;
  };

  void next_day()
  {
    current_day++;
  };

  void wait_income(int target_day)
  {
    int income = current_income() * (target_day - current_day);
    current_funds += income;
    current_day = target_day;
  }
  void solve()
  {
    //Compute list of decision days
    std::vector<int> decision_days;
    for (auto day = Pb_instance::machines_availability.begin();
         day != Pb_instance::machines_availability.end(); ++day)
    {
      decision_days.push_back(day->first);
    }
    //For each decision day, determine the best situation to be in
    for (int day_index = 0; day_index < decision_days.size() - 1; ++day_index)
    {
      //available_choices is the list of available machines this day
      auto available_choices{
        Pb_instance::machines_availability.equal_range(decision_days[day_index])};
      //Create a default scenario to which compare first choice (this scenario corresponds to buying no machine)
      Pb_instance current_best_choice = *this;
      current_best_choice.wait_income(decision_days[day_index + 1]);
      //Browse all available machines
      for (auto machine = available_choices.first; machine != available_choices.second;
           ++machine)
      {
        //Time to simulate affordable choices and compare outcomes
        Pb_instance hyphothesis = *this;
        hyphothesis.buy_machine(machine->second);
        hyphothesis.wait_income(decision_days[day_index + 1]);

        // Pb_instance current_best_choice =
        //   Pb_instance::compare_situations(current_best_choice, ) ?:
      }
    }
    do_the_flop();
  }
  //Compute the smart move to be done (machine to be bought). -1 is buy nothing
  //Compare situations and select which machine is bought, if any)
  void do_the_flop() {}
};

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
    Pb_instance::machines_availability.insert({info[0], m});
  }
  //Insert a -virtual- machine at the end for simulating the sale of lastly owned machine
  m.buy_cost = 0;
  m.daily_product = 0;
  m.resell_value = 0;
  Pb_instance::machines_availability.insert({info[2], m});
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
    }

    return 0;
  }
}