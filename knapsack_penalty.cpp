#include <iostream>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp" 

using namespace std;
using json = nlohmann::json;

json items;

json readJSON () {
    ifstream inFile("data_knapsack.json");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    json jsonData;
    inFile >> jsonData;
    inFile.close();

    return jsonData;

};

vector<vector<int>> generateSubsets(const vector<int>& items) {
    vector<vector<int>> subsets;
    int n = items.size();

    for (int mask = 0; mask < (1 << n); ++mask) {
        vector<int> subset;

        for (int i = 0; i < n; i++) {

            if (mask & (1 << i)) {
                subset.push_back(items[i]);
            }
        }

        subsets.push_back(subset);

    }
    return subsets;
};


void brute_force(const vector<vector<int>>& subsets, const unordered_map<int, int>& item_weight, const unordered_map<int, int>& item_value, int& best_value, vector<int>& best_subset) {

    if (subsets.empty()) {
        cerr << "Error: No items available for brute force!" << endl;
        return;
    }

    for (int i = 0; i < subsets.size(); i++) {
        int sum_weight = 0;
        int sum_value = 0;
        int backpack_capacity = 20;
        int penalty_factor = 5;

        cout << "Brute force on subset " << i + 1 << ": { ";
        
        for (int item : subsets[i]) {
            cout << item << " ";
            sum_weight += item_weight.at(item);
            sum_value += item_value.at(item);
        }
        cout << "}" << endl;
        cout << "Sum_without_penalty: " << sum_value << endl;
        if (sum_weight > backpack_capacity) {
            int penalty = sum_weight - backpack_capacity;
            sum_value -= penalty * penalty_factor; // kara liniowa
            //sum_value -= penalty * (pow(penalty_factor, 2)); //kara kwadratowa
        }
        cout << "Sum_weight: " << sum_weight << endl << "Sum_value: " << sum_value << endl << endl;

        if (sum_value > best_value) {
            best_value = sum_value;
            best_subset = subsets[i]; 
        }
    }
    cout << "Best subset (value = " << best_value << "): { ";
    for (int item : best_subset) {
        cout << item << " ";
    }
    cout << "}" << endl;
};



int main () {
    json jsonData = readJSON();

    int id = jsonData.at("id");
    int min_weight = jsonData.at("min_weight");
    int max_weight = jsonData.at("max_weight");
    json items = jsonData.at("items");

    vector<int> best_subset;
    int best_value = -1;

    unordered_map<int, int> item_weight;

    for (auto& item : items) {
        int item_id = item.at("id");
        int weight = item.at("weight");
        item_weight[item_id] = weight; 
    }

    unordered_map<int, int> item_value;

    for (auto& item : items) {
        int item_id = item.at("id");
        int value = item.at("value");
        item_value[item_id] = value;  
    }

    vector<int> itemIds;
    for (auto& item : items) {
        itemIds.push_back(item.at("id"));
    }

    vector<vector<int>> subsets = generateSubsets(itemIds);

    brute_force(subsets, item_weight, item_value, best_value, best_subset);



};