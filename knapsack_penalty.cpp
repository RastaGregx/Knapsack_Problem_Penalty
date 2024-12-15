/**
@file knapsack_penalty.cpp
@version 1.0
@author Grzegorz Rybakowski 160379
 
  * Kompilacja g++:
  * Wariant bez _DEBUG:
  ** g++ -o knapsack_penalty knapsack_penalty.cpp 
  * Wariant z _DEBUG:
  ** g++ -o knapsack_penalty knapsack_penalty.cpp -D_DEBUG

  * Użycie: ./knapsack_penalty
  * Opis: Stosuje algorytm wybrany z terminala do znalezienia rozwiązania problemu plecakowego na podstawie danych z data_knapsack.json
*/

#include <iostream>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp" 
nlohmann::json items;

nlohmann::json readJSON () {
    std::ifstream inFile("data_knapsack.json");
    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open file!" << std::endl;
        return 1;
    }

    nlohmann::json jsonData;
    inFile >> jsonData;
    inFile.close();

    return jsonData;

};

void processItems(const nlohmann::json& items, 
                  std::unordered_map<int, int>& item_weight,
                  std::unordered_map<int, int>& item_value, 
                  std::vector<int>& itemIds) {

    for (const auto& item : items) {
        int item_id = item.at("id");
        item_weight[item_id] = item.at("weight");
        item_value[item_id] = item.at("value");
        itemIds.push_back(item_id);
    }
}

int dynamic_knapsack(const std::unordered_map<int, int> &item_weight,
                     const std::unordered_map<int, int> &item_value,
                     const std::vector<int> &itemIds,
                     int capacity,
                     int number_of_items,
                     std::vector<int>& selected_items,
                     int penalty_factor) {
    int max_value = 1;
    int overflow = capacity*1.5; 
    // Create a DP table initialized with 0
    std::vector<std::vector<int>> dynamic_table(number_of_items + 1, std::vector<int>(overflow + 1, 0));

    for (int i = 1; i <= number_of_items; ++i) { //every item
        int id = itemIds[i - 1];

        for (int current_weight = 1; current_weight <= overflow; ++current_weight) { //every subset

           if (item_weight.at(id) <= current_weight) { 
                int without_item = dynamic_table[i - 1][current_weight];
                int with_item = dynamic_table[i - 1][current_weight - item_weight.at(id)] + item_value.at(id);
                dynamic_table[i][current_weight] = std::max(without_item, with_item);
            } else {
                dynamic_table[i][current_weight] = dynamic_table[i - 1][current_weight];
            }
        }
    }

// std::cout << "Dynamic Programming Table (Item, Capacity, Value): \n";
//     std::cout << "Capacity ";
//     for (int j = 0; j <= overflow; ++j) {
//         std::cout << j << "\t";
//     }
//     std::cout << "\n";

//     for (int i = 0; i <= number_of_items; ++i) {
//         std::cout << "Item " << i << "\t";
//         for (int j = 0; j <= overflow; ++j) {
//             std::cout << dynamic_table[i][j] << "\t";
//         }
//         std::cout << "\n";
//     }

 //   std::cout << dynamic_table[10][capacity] << "\t";


    // std::cout << "Capacity ";
    // for (int j = number_of_items; j <= overflow; ++j) {
    //     std::cout << j << "\t";
    // }
    // std::cout << "\n";


        // std::cout << "Item 10" << "\t";
        for (int j = capacity; j <= overflow; ++j) {
            if (j > capacity) {
                dynamic_table[number_of_items][j] -= (j - capacity)*penalty_factor;
            }
            // std::cout << dynamic_table[number_of_items][j] << "\t";
            if (dynamic_table[number_of_items][j] > dynamic_table[number_of_items][max_value]) {
                max_value = j;
            }
        }
    //     std::cout << "\n";


    // std::cout << max_value << std::endl;


    int weight_left = max_value;
    for (int i = number_of_items; i > 0 && weight_left > 0; --i) {
        if (dynamic_table[i][weight_left] != dynamic_table[i - 1][weight_left]) { 
            // Item i is included
            int id = itemIds[i - 1];
            selected_items.push_back(id);
            weight_left -= item_weight.at(id); // reduce the remaining weight
        }
    }

    // Output the selected items
    // std::cout << "Selected items: ";
    // for (int id : selected_items) {
    //     std::cout << id << " ";
    // }
    // std::cout << "\n";


    return dynamic_table[number_of_items][max_value];
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


    int max_value = dynamic_knapsack(item_weight, item_value, itemIds, capacity, number_of_items, selected_items,penalty_factor);

    // Output the result
    std::cout << "Maximum value that can be carried: " << max_value << std::endl;
    std::cout << "Selected items: ";
    for (int item : selected_items) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    return 0;
}