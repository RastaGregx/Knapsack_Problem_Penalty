#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp" 

using namespace std;
using json = nlohmann::json;


//Brute force solution

int main () {

    ifstream inFile("data_knapsack.json");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    json jsonData;
    inFile >> jsonData;
    inFile.close();

    int id = jsonData.at("id");
    //int capacity = jsonData.at("capacity");
    int min_weight = jsonData.at("min_weight");
    int max_weight = jsonData.at("max_weight");
    auto items = jsonData.at("items");
    int number_of_items = items.size();

    cout << "Number of items: " << number_of_items << "\n";
    //cout << "Backpack capacity: " << capacity << "\n";
    cout << "Min Weight: " << min_weight << "\n";
    cout << "Max Weight: " << max_weight << "\n";
    //cout << "ID: " << id << "\n";

    for (int i = 0; i < number_of_items; i++) {
        cout << "\nItem id: " << items[i].at("id") << "\n";
        cout << "Item value: " << items[i].at("value") << "\n";
        cout << "Item weight: " << items[i].at("weight") << "\n";

    }

}