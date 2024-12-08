#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp" 
#include <random>

using namespace std;
using json = nlohmann::json;

int main () {
    int number_of_items;
    int min_weight;
    int max_weight;
    int value;

    cout << "Enter the number of items: ";
    cin >> number_of_items;

    cout << "Enter the minimum weight: ";
    cin >> min_weight;
    while (min_weight < 0) {
        cout << "Minimum weight cannot be negative. Please enter a non-negative value: ";
        cin >> min_weight;
    }

    cout << "Enter the maximum weight: ";
    cin >> max_weight;
    while (max_weight < min_weight) {
        cout << "Maximum weight cannot be less than minimum weight. Please enter a value greater than or equal to " << min_weight << ": ";
        cin >> max_weight;
    }

    cout << "Starting to generate data for " << number_of_items << " items..." << endl;

    json jsonData;


    jsonData["number_of_items"] = number_of_items;
    jsonData["min_weight"] = min_weight;
    jsonData["max_weight"] = max_weight;
    jsonData["id"] = 0;
    jsonData["items"] = json::array();


    // Generate random weights and values for the items
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> weightDist(min_weight, max_weight);
    uniform_int_distribution<int> valueDist(0, 100);

    for (int i = 0; i < number_of_items; i++) {
        json item;
        item["id"] = i + 1;
        item["weight"] = weightDist(gen);
        item["value"] = valueDist(gen);
        jsonData["items"].push_back(item);
    }

    ofstream outFile("data_knapsack.json");
    if (outFile.is_open()) {
        outFile << jsonData.dump(4); // Pretty-print with an indent of 4 spaces
        outFile.close();
        cout << "Data written to data_knapsack.json" << endl;
    } else {
        cerr << "Unable to open file for writing!" << endl;
    }
}