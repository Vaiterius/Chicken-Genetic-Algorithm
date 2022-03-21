/*
 * Breed a species of chicken to make them long-distance flyers!
 */
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

int NUM_CHICKENS = 10;  // max held per generation
const int GENERATION_LIM = 500;
const int NUM_EGGS = 6;  // per parent pair, assuming all are fertile
const int EGGS_PER_YEAR = 50; // made this one up

// Distance in meters.
const double GOAL_DIST = 1000.0;  // measured for distance, not speed

const double INITIAL_MIN_SP = 4.0;  // speed per second
const double INITIAL_MAX_SP = 8.0;
const double INITIAL_MIN_STA = 35.0;  // stamina
const double INITIAL_MAX_STA = 60.0;
const double INITIAL_MIN_STA_PER_SEC = 7.0;  // subtracted per sec
const double INITIAL_MAX_STA_PER_SEC = 12.0;

const double MUTATE_ODDS = 0.01;  // % chance to mutate
const double MUTATE_MIN = 0.70;  // lower bound scalar
const double MUTATE_MAX = 1.20;  // upper bound scalar

// Seeding for random numbers.
random_device rd;
mt19937 gen(rd());


// Chicken object.
typedef struct
{
    double speed;
    double stamina;
    double stamina_per_s;
    double last_distance;
}
Chicken;


double get_rand_float(double, double);
void print_chickens(vector<Chicken>);
void print_random_chicken(vector<Chicken>);
vector<Chicken> populate(int);
void record_flights(vector<Chicken>&);
double fitness(vector<Chicken>, double);
double get_average_distance(vector<Chicken>);
vector<Chicken> select(vector<Chicken>&, vector<Chicken>&, vector<Chicken>&, int);
void breed(vector<Chicken>&, vector<Chicken>&, vector<Chicken>&, int);
void mutate(vector<Chicken>&, double, double, double);


// Initialize chicken population, then select, breed, and mutate until goal is reached.
int main()
{
    // Ensure chickens are an even number (for perfect pairs).
    if (NUM_CHICKENS % 2 != 0)
        NUM_CHICKENS++;

    int generations = 0;

    // Populate chickens then grade initial values.
    vector<Chicken> chickens = populate(NUM_CHICKENS);
    record_flights(chickens);
    double population_fitness = fitness(chickens, GOAL_DIST);
    double average_distance = get_average_distance(chickens);

    cout << "Initial population fitness = " << population_fitness << endl;
    cout << "Initial average distance traveled = " << average_distance << " m" << endl << endl;

    // Track distance averages per generation.
    vector<double> averages;
    averages.push_back(average_distance);

    streamsize s = cout.precision();
    cout << fixed << setprecision(2);

    print_random_chicken(chickens);  // Typical chicken stats for initial generation.
    cout << endl;

    // The select-breed-mutate process.
    while (generations < GENERATION_LIM && population_fitness < 1.0)
    {
        vector<Chicken> selected_males, selected_females, chicks;
        // Select fittest chickens.
        chickens = select(chickens, selected_males, selected_females, NUM_CHICKENS);
        // Breed them and crossover genes.
        breed(chicks, selected_males, selected_females, NUM_EGGS);
        // Mutate some offspring.
        mutate(chicks, MUTATE_ODDS, MUTATE_MIN, MUTATE_MAX);

        // Add offspring to population and score performances for all.
        chickens.insert(chickens.end(), chicks.begin(), chicks.end());

        record_flights(chickens);
        population_fitness = fitness(chickens, GOAL_DIST);

        average_distance = get_average_distance(chickens);
        averages.push_back(average_distance);

        cout << "Generation #" << generations + 1 << ": The average chicken flew a distance of " << average_distance << " m" << endl;

        generations++;
    }

    cout << endl;
    print_random_chicken(chickens);  // Typical chicken stats for converged generation.

    // Display final statistics.
    cout << setprecision(s);
    cout << endl;
    cout << "Final population fitness = " << population_fitness << endl;
    cout << setprecision(2);
    cout << "Final average distance traveled = " << average_distance << " m" << endl;
    cout << "Number of generations = " << generations << endl;
    cout << "Number of years = " << (float) generations / EGGS_PER_YEAR << endl;

    return 0;
}


// Return random float number from two range values.
double get_rand_float(double min, double max)
{
    uniform_real_distribution<> num(min, max);
    return num(gen);
}


// Return random int number from two range values.
double get_rand_int(int min, int max)
{
    uniform_int_distribution<> num(min, max);
    return num(gen);
}


// Output each chicken's stats into the terminal.
void print_chickens(vector<Chicken> chickens)
{
    for (int i = 0; i < chickens.size(); i++)
    {
        cout << "Chicken #" << i + 1<< " stats:" << endl;
        cout << "speed: " << chickens.at(i).speed << " m/s" << endl;
        cout << "stamina: " << chickens.at(i).stamina << endl;
        cout << "stamina cost per sec: " << -chickens.at(i).stamina_per_s << endl;
        cout << "last recorded distance: " << chickens.at(i).last_distance << " m" << endl;
        cout << endl;
    }
}


// Output a random chicken's stats.
void print_random_chicken(vector<Chicken> chickens)
{
    int rand_chicken_idx = get_rand_int(0, chickens.size() - 1);
    Chicken chicken = chickens.at(rand_chicken_idx);

    cout << "Random chicken:" << endl;
    cout << "speed: " << chicken.speed << " m/s" << endl;
    cout << "stamina: " << chicken.stamina << endl;
    cout << "stamina cost per sec: " << -chicken.stamina_per_s << endl;
    cout << "last recorded distance: " << chicken.last_distance << " m" << endl;
}


// Initial population of chickens with randomized stats.
vector<Chicken> populate(int num_chickens)
{
    vector<Chicken> chickens;
    for (int i = 0; i < num_chickens; i++)
    {
        Chicken chicken;
        chicken.speed = get_rand_float(INITIAL_MIN_SP, INITIAL_MAX_SP);
        chicken.stamina = get_rand_float(INITIAL_MIN_STA, INITIAL_MAX_STA);
        chicken.stamina_per_s = get_rand_float(INITIAL_MIN_STA_PER_SEC, INITIAL_MAX_STA_PER_SEC);
        chicken.last_distance = 0.0;
        chickens.push_back(chicken);
    }

    return chickens;
}


// Get distance each chicken is able to travel and record.
void record_flights(vector<Chicken> &chickens)
{
    for (int i = 0; i < chickens.size(); i++)
    {
        double session_speed = chickens.at(i).speed;
        double session_stamina = chickens.at(i).stamina;
        double session_stamina_per_s = chickens.at(i).stamina_per_s;

        // Make the chicken fly until it gets tired.
        double distance_traveled = (session_stamina / session_stamina_per_s) * session_speed;

        chickens.at(i).last_distance = distance_traveled;
    }
}


// Judge population's ability to fly long-distance relative to the goal.
double fitness(vector<Chicken> chickens, double goal_dist)
{
    double average = get_average_distance(chickens);
    return average / goal_dist;  // Score between 0 and 1, with higher the better.
}


// Compute mean distance traveled from the population.
double get_average_distance(vector<Chicken> chickens)
{
    double total_distance, average;
    for (Chicken chicken: chickens)
    {
        total_distance += chicken.last_distance;
    }
    average = total_distance / chickens.size();

    return average;
}


// Select fittest chickens from population and cull the rest.
vector<Chicken> select(
    vector<Chicken> &population, vector<Chicken> &males, vector<Chicken> &females, int num_to_retain
)
{
    vector<Chicken> new_chickens;

    // Sort chickens by distance traveled using this lambda function.
    sort(population.begin(), population.end(), [](Chicken a, Chicken b) {
        return a.last_distance < b.last_distance;
    });

    // Assumption: upper half are males, lower half are females.
    int to_retain_by_sex = num_to_retain / 2;
    int members_per_sex = population.size() / 2;

    males = {population.begin() + members_per_sex, population.end()};
    females = {population.begin(), population.end() - members_per_sex};

    // Only retain the fittest of the bunch to the max chickens allowed.
    // This means culling the lowest-performing chickens per sex.
    males.erase(males.begin(), males.begin() + (members_per_sex - to_retain_by_sex));
    females.erase(females.begin(), females.begin() + (members_per_sex - to_retain_by_sex));

    new_chickens.insert(new_chickens.end(), males.begin(), males.end());
    new_chickens.insert(new_chickens.end(), females.begin(), females.end());

    return new_chickens;
}


// Each male and female chicken pair will produce chicks for the next generation.
void breed(vector<Chicken> &chicks, vector<Chicken> &males, vector<Chicken> &females, int num_eggs)
{
    // Shuffle the males and females as to have random partners.
    shuffle(males.begin(), males.end(), gen);
    shuffle(females.begin(), females.end(), gen);

    for (int i = 0; i < males.size(); i++)  // Number of males & females should be the same.
    {
        Chicken curr_male = males.at(i);
        Chicken curr_female = females.at(i);
        for (int j = 0; j < num_eggs; j++)  // Chicks born per parent pair.
        {
            Chicken chicken;  // Assumption: female's stats < male's stats to get value in between.
            chicken.speed = get_rand_float(curr_female.speed, curr_male.speed);
            chicken.stamina = get_rand_float(curr_female.stamina, curr_male.stamina);
            chicken.stamina_per_s = get_rand_float(curr_female.stamina_per_s, curr_male.stamina_per_s);
            chicken.last_distance = 0.0;
            chicks.push_back(chicken);
        }
    }
}


// Randomly mutate a chick/some chicks by chance with respect to its stats.
void mutate(vector<Chicken> &chicks, double mutate_odds, double mutate_min, double mutate_max)
{
    for (int i = 0; i < chicks.size(); i++)
    {
        if (MUTATE_ODDS >= get_rand_float(0.0, 1.0))  // % chance to mutate the chick.
        {
            // Apply mutation scalars to a random stat (3 stats so choose one randomly).
            // cout << "MUTATION!: ";
            if (0.333 > get_rand_float(0.0, 1.0))
            {
                // cout << "old speed: " << chicks.at(i).speed;
                chicks.at(i).speed *= get_rand_float(mutate_min, mutate_max);
                // cout << ", new speed: " << chicks.at(i).speed;
            }
            else if (0.666 > get_rand_float(0.0, 1.0))
            {
                // cout << "old stamina: " << chicks.at(i).speed;
                chicks.at(i).stamina *= get_rand_float(mutate_min, mutate_max);
                // cout << ", new stamina: " << chicks.at(i).speed;
            }
            else
            {
                // cout << "old stamina_per_s: " << chicks.at(i).speed;
                chicks.at(i).stamina_per_s *= get_rand_float(mutate_min, mutate_max);
                // cout << ", new stamina_per_s: " << chicks.at(i).speed;
            }
            // cout << endl;
        }
    }
}

