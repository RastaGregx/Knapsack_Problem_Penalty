/**
@file generate.cpp
@version 1.0
@author Grzegorz Rybakowski 160379
 
  * Kompilacja g++:
  * Wariant bez _DEBUG:
  ** g++ -o generate generate.cpp 
  * Wariant z _DEBUG:
  ** g++ -o generate generate.cpp -D_DEBUG

  * Użycie: ./generate <seed>
  * Opis: Generuje plik JSON z losowymi danymi przedmiotów dla problemu plecakowego, jeśli seed nie zostanie podany w terminalu to używa losowego seeda
*/

#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp" 
#include <random>

void randomGenerator(int min_weight, int max_weight, 
                     std::uniform_int_distribution<int>& weightDist, 
                     std::uniform_int_distribution<int>& valueDist) {

    weightDist = std::uniform_int_distribution<int>(min_weight, max_weight);
    valueDist = std::uniform_int_distribution<int>(0, 10);
}

int getValidInput(const std::string& prompt, int minValue) {
    int input;
    std::cout << prompt;
    std::cin >> input;
    while (input < minValue) {
        std::cout << "Input cannot be less than " << minValue << ". Please enter a valid value: ";
        std::cin >> input;
    }
    return input;
}

int main(int argc, char *argv[]) {
    int number_of_items;
    int min_weight;
    int max_weight;
    int seed = 0; 

    number_of_items = getValidInput("Enter the number of items: ", 0);
    min_weight = getValidInput("Enter the minimum weight: ", 0);
    max_weight = getValidInput("Enter the maximum weight: ", min_weight);

    if (argc > 1) {
        try {
            seed = std::stoi(argv[1]); 
            std::cout << std::endl << "Using user-provided seed: " << seed << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid seed value. Using random seed." << std::endl;
            seed = 0; 
        }
    } else {
        std::cout << std::endl <<  "No seed provided. Using random seed." << std::endl;
    }

    std::cout << "Starting to generate data for " << number_of_items << " items..." << std::endl;

    nlohmann::json jsonData;

    std::uniform_int_distribution<int> weightDist;
    std::uniform_int_distribution<int> valueDist;

    randomGenerator(min_weight, max_weight, weightDist, valueDist);

    jsonData["number_of_items"] = number_of_items;
    jsonData["min_weight"] = min_weight;
    jsonData["max_weight"] = max_weight;
    jsonData["id"] = 0;
    jsonData["items"] = nlohmann::json::array();


    std::random_device rd;
    std::mt19937 gen(seed == 0 ? rd() : seed); 

    for (int i = 0; i < number_of_items; i++) {
        nlohmann::json item;
        item["id"] = i + 1;
        item["weight"] = weightDist(gen); 
        item["value"] = valueDist(gen);   
        jsonData["items"].push_back(item);
    }


    std::ofstream outFile("data_knapsack.json");
    if (outFile.is_open()) {
        outFile << jsonData.dump(4); 
        outFile.close();
        std::cout << "Data written to data_knapsack.json" << std::endl;
    } else {
        std::cerr << "Unable to open file for writing!" << std::endl;
    }

    return 0;
}
