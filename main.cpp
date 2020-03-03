#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>
struct Machine
{
  int  buy_cost = 0;
  int  resell_value = 0;
  long daily_product = 0;
};

class Financial_situation
{
private:
  // SEE: member variable should be private and initialized
  long    funds;         // initial funds are always needed
  Machine owned_machine; // Start with no machine by default

public:
  //Optional machine. Default is a "virtual" machine with no production and no value
  Financial_situation(long funds, Machine m = {}) : funds(funds), owned_machine(m){};

  long total() const
  {
    return funds + owned_machine.resell_value;
  }
  long get_funds() const
  {
    return funds;
  }
  Machine get_machine() const
  {
    return owned_machine;
  }
  bool operator>(Financial_situation s) const
  {
    return total() > s.total();
  }
  bool buy_machine(Machine m)
  {
    long affordability = funds + owned_machine.resell_value - m.buy_cost;
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
  long current_income() const
  {
    return owned_machine.daily_product;
  }
  void compute_income(int current_day, int target_day)
  {
    funds += (target_day - current_day) * current_income();
    current_day = target_day;
  }
};

class Pb_instance
{
private:
  //machines_availability stores each machine available on day d, d being a key of the map
  std::multimap<int, Machine> machines_availability;
  //best_situation_each_day stores the best money score atteinable each day and the machine to own
  //at the beginning of the day in order to reach this score, making retreival of optimal strategy trivial
  std::map<int, Financial_situation> best_situation_each_day;
  int                                restruct_duration;
  Financial_situation                initial_situation;
  int                                current_day = 0;

public:
  Pb_instance(int restruct_duration, int initial_funds)
    : restruct_duration(restruct_duration), initial_situation(initial_funds)
  {
    //The day the restructuration ends, also add a default best situation
    //Used algorithm computes the best situation at the beginning of each day
    //We are interested in the situation at the end of day D, which is equivalent to situation
    //at the beginning of day D+1
    best_situation_each_day.insert({restruct_duration + 1, initial_situation});
  }

  void add_machine(Machine m, int day_available)
  {
    machines_availability.insert({day_available, m});
    //Store days on which available money will have to be computed and choices made
    //Fill the best situation so far for each day with a default (as if no machine was ever bought)
    best_situation_each_day.emplace(day_available, initial_situation);
  }

  //Compare each current best situation with future situations if current machine is kept
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

      //Compute income to next decision day
      current_situation.compute_income(current_day, it->first);
      if (current_situation > best_situation_each_day.at(it->first))
      {
        best_situation_each_day.at(it->first) = current_situation;
      }
      current_day = it->first;
      ++it;
    }
  }
  long solve()
  {
    //Iterator to upcoming choice days
    auto it = machines_availability.lower_bound(current_day);
    while (it != machines_availability.end())
    {
      //Compute the future starting from current day
      current_day = it->first;
      //available_choices is the list of available machines this day
      auto available_choices = machines_availability.equal_range(it->first);
      //Browse all available machines (We consider the initial situation to own a machine with 0 cost and 0 production)
      for (auto machine = available_choices.first; machine != available_choices.second;
           ++machine)
      {
        Financial_situation current_situation = best_situation_each_day.at(current_day);

        //Buy all affordable choices and compare outcomes
        //For each decision day, we will proceed to determine the best situation that can be attained
        //If you know you have the most money on a particular day, you do not need to worry about
        //affordability of subsequent machines, as if you can't afford one with the current best
        //funds available, the machine will not be affordable for any strategy
        if (current_situation.buy_machine(machine->second))
        {
          update_best_situations(current_situation);
        }
      }
      ++it;
    }
    return best_situation_each_day.at(restruct_duration + 1).total();
  }
};

std::pair<Machine, int> read_machine(std::istream& is)
{
  int day_available;
  int buy_cost;
  int resell_value;
  int daily_product;

  is >> day_available;
  is >> buy_cost;
  is >> resell_value;
  is >> daily_product;

  return {Machine{buy_cost, resell_value, daily_product}, day_available};
}

std::optional<Pb_instance> read_problem(std::istream& is)
{
  int machines_count;
  int initial_funds;
  int restruct_duration;

  is >> machines_count;
  is >> initial_funds;
  is >> restruct_duration;

  // if this is the "end" of the test cases
  if (machines_count == 0 && initial_funds == 0 && restruct_duration == 0)
  {
    return std::nullopt;
  }

  Pb_instance pb{restruct_duration, initial_funds};

  for (int m_idx = 0; m_idx < machines_count; ++m_idx)
  {
    auto [machine, day_available] = read_machine(is);
    pb.add_machine(machine, day_available);
  }
  return pb;
}

Pb_instance generate_problem()
{
  constexpr int n10p9 = 1'000'000'000;

  int machines_count = 50'000;
  int initial_funds = 20;
  int restruct_duration = n10p9;

  std::random_device              rd;
  std::mt19937                    gen(0);
  std::uniform_int_distribution<> machine_resell_value_dis(1, 50);
  std::uniform_int_distribution<> machine_buy_price_dis(2, 50);
  std::uniform_int_distribution<> machine_availability_dis(1, restruct_duration);
  std::uniform_int_distribution<> machine_product_dis(1, n10p9);

  Pb_instance pb{restruct_duration, initial_funds};

  for (int m_idx = 0; m_idx < machines_count; ++m_idx)
  {

    Machine m{machine_buy_price_dis(gen), machine_resell_value_dis(gen),
              machine_product_dis(gen)};
    int     day_available = machine_availability_dis(gen);
    m.resell_value = std::min(m.resell_value, m.buy_cost - 1);
    pb.add_machine(m, day_available);
  }
  return pb;
}

int main()
{
  //SEE select input file here
  std::string   input_file_name = "input.txt"; // SEE: take that in the param
  std::ifstream input_file(input_file_name);
  std::string   output_file_name = "output.txt";

  std::ofstream output_file(output_file_name, std::ios::binary);
  if (!input_file.is_open())
  {
    std::cout << "failed to open " << input_file_name << '\n';
    return 0;
  }

  // Pb_instance pb = generate_problem();
  // output_file << pb.solve();

  int pb_index = 1;
  // Iterate over problem instances
  while (input_file)
  {
    auto opt_pb = read_problem(input_file);
    if (opt_pb.has_value())
    {
      Pb_instance pb = opt_pb.value();
      long        pb_sol = pb.solve();
      output_file << "Case " + std::to_string(pb_index) + " : " + std::to_string(pb_sol) +
                       '\n';
      ++pb_index;
    }
  }
  return 0;
}