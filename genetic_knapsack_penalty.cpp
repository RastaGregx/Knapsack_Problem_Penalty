/**
@file genetic_knapsack_penalty.cpp
@version 1.0
@author Grzegorz Rybakowski 160379
 
  * Kompilacja g++:
  * Wariant bez _DEBUG:
  ** g++ -o genetic_knapsack_penalty genetic_knapsack_penalty.cpp
  * Wariant z _DEBUG:
  ** g++ -o genetic_knapsack_penalty genetic_knapsack_penalty.cpp -D_DEBUG

  * Użycie: ./genetic_knapsack_penalty
  * Opis: Stosuje algorytm wybrany z terminala do znalezienia rozwiązania problemu plecakowego na podstawie danych z data_knapsack.json
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <ctime>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <climits>

// Function to read JSON data from a file
nlohmann::json readJSON(const std::string& filename = "data_knapsack.json") {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open JSON file");
    }
    nlohmann::json jsonData;
    file >> jsonData;
    return jsonData;
}

// Function to process JSON items into data structures
void processItems(const nlohmann::json& items,
                  std::unordered_map<int, int>& item_weight,
                  std::unordered_map<int, int>& item_value,
                  std::vector<int>& itemIds) {
    for (const auto& item : items) {
        int id = item.at("id");
        int weight = item.at("weight");
        int value = item.at("value");
        
        item_weight[id] = weight;
        item_value[id] = value;
        itemIds.push_back(id);
    }
}

// Structure to represent an individual in the population
struct Individual {
    std::vector<int> chromosome; // Binary representation of item inclusion
    int fitness;                // Fitness value
};

// Function to calculate the fitness of an individual
int calculate_fitness(const Individual& individual, 
                      const std::unordered_map<int, int>& item_weight,
                      const std::unordered_map<int, int>& item_value,
                      const std::vector<int>& itemIds,
                      int capacity, int penalty_factor) {
    int total_weight = 0;
    int total_value = 0;

    for (size_t i = 0; i < individual.chromosome.size(); ++i) {
        if (individual.chromosome[i] == 1) {
            int id = itemIds[i];
            total_weight += item_weight.at(id);
            total_value += item_value.at(id);
        }
    }

    // Apply penalty if weight exceeds capacity
    if (total_weight > capacity) {
        total_value -= (total_weight - capacity) * penalty_factor;
    }

    return total_value;
}

// Generate a random individual
Individual generate_individual(int number_of_items) {
    Individual individual;
    individual.chromosome.resize(number_of_items);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < number_of_items; ++i) {
        individual.chromosome[i] = dis(gen);
    }

    individual.fitness = 0;
    return individual;
}

// Perform crossover between two parents to create offspring
Individual crossover(const Individual& parent1, const Individual& parent2) {
    Individual offspring;
    size_t size = parent1.chromosome.size();
    offspring.chromosome.resize(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, size - 1);

    int crossover_point = dis(gen);
    for (size_t i = 0; i < size; ++i) {
        if (i < crossover_point) {
            offspring.chromosome[i] = parent1.chromosome[i];
        } else {
            offspring.chromosome[i] = parent2.chromosome[i];
        }
    }

    offspring.fitness = 0;
    return offspring;
}

// Perform mutation on an individual
void mutate(Individual& individual, double mutation_rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < individual.chromosome.size(); ++i) {
        if (dis(gen) < mutation_rate) {
            individual.chromosome[i] = 1 - individual.chromosome[i];
        }
    }
}

// Select an individual from the population using tournament selection
Individual tournament_selection(const std::vector<Individual>& population, int tournament_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, population.size() - 1);

    Individual best;
    best.fitness = INT_MIN;

    for (int i = 0; i < tournament_size; ++i) {
        const Individual& candidate = population[dis(gen)];
        if (candidate.fitness > best.fitness) {
            best = candidate;
        }
    }

    return best;
}

// Genetic algorithm implementation
int genetic_knapsack(const std::unordered_map<int, int>& item_weight,
                     const std::unordered_map<int, int>& item_value,
                     const std::vector<int>& itemIds,
                     int capacity,
                     int number_of_items,
                     std::vector<int>& selected_items,
                     int penalty_factor,
                     int population_size = 1000,
                     int generations = 500,
                     double mutation_rate = 0.001,
                     int tournament_size = 5) {
    std::ofstream log_file("genetic_algorithm_log.txt");
    if (!log_file) {
        std::cerr << "Error: Unable to open log file." << std::endl;
        return -1;
    }

    // Initialize population
    std::vector<Individual> population;
    for (int i = 0; i < population_size; ++i) {
        Individual individual = generate_individual(number_of_items);
        individual.fitness = calculate_fitness(individual, item_weight, item_value, itemIds, capacity, penalty_factor);
        population.push_back(individual);
    }

    log_file << "Initial Population:\n";
    for (const auto& ind : population) {
        for (int gene : ind.chromosome) {
            log_file << gene;
        }
        log_file << " Fitness: " << ind.fitness << "\n";
    }

    // Evolution process
    for (int gen = 0; gen < generations; ++gen) {
        std::vector<Individual> new_population;

        for (int i = 0; i < population_size; ++i) {
            // Select parents
            Individual parent1 = tournament_selection(population, tournament_size);
            Individual parent2 = tournament_selection(population, tournament_size);

            // Perform crossover
            Individual offspring = crossover(parent1, parent2);

            // Perform mutation
            mutate(offspring, mutation_rate);

            // Calculate fitness of offspring
            offspring.fitness = calculate_fitness(offspring, item_weight, item_value, itemIds, capacity, penalty_factor);

            new_population.push_back(offspring);
        }

        // Replace old population with new population
        population = new_population;

        log_file << "Generation " << gen + 1 << ":\n";
        for (const auto& ind : population) {
            for (int gene : ind.chromosome) {
                log_file << gene;
            }
            log_file << " Fitness: " << ind.fitness << "\n";
        }
    }

    // Find the best individual in the population
    Individual best;
    best.fitness = INT_MIN;
    for (const Individual& individual : population) {
        if (individual.fitness > best.fitness) {
            best = individual;
        }
    }

    // Extract the selected items from the best individual
    for (size_t i = 0; i < best.chromosome.size(); ++i) {
        if (best.chromosome[i] == 1) {
            selected_items.push_back(itemIds[i]);
        }
    }

    log_file << "Best Individual:\n";
    for (int gene : best.chromosome) {
        log_file << gene;
    }
    log_file << " Fitness: " << best.fitness << "\n";

    log_file.close();

    return best.fitness;
}

int main(int argc, char *argv[]) {
    nlohmann::json jsonData = readJSON();

    int id = jsonData.at("id");
    int min_weight = jsonData.at("min_weight");
    int max_weight = jsonData.at("max_weight");
    int number_of_items = jsonData.at("number_of_items");
    nlohmann::json items = jsonData.at("items");

    // Prepare data structures
    std::unordered_map<int, int> item_weight;
    std::unordered_map<int, int> item_value;
    std::vector<int> itemIds;
    int penalty_factor = 1;
    int capacity = 10;

    if (argc > 1) {
        try {
            capacity = std::stoi(argv[1]); 
            std::cout << std::endl << "Using user-provided capacity: " << capacity << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid capacity value. Using default capacity." << std::endl;
            capacity = 10; 
        }
    } else {
        std::cout << std::endl <<  "No capacity provided. Using default capacity." << std::endl;
    }

    processItems(items, item_weight, item_value, itemIds);

    std::vector<int> selected_items;

    int max_value = genetic_knapsack(item_weight, item_value, itemIds, capacity, number_of_items, selected_items, penalty_factor);

    // Output the result
    std::cout << "Maximum value that can be carried: " << max_value << std::endl;
    std::cout << "Selected items: ";
    for (int item : selected_items) {
        std::cout << item << " ";
    }
    std::cout << std::endl;

    return 0;
}
