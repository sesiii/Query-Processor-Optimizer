// #include "data_handler.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

bool load_table_data(const std::string &table_name, TableData &table) {
    std::string filename = table_name + ".txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Failed to open %s\n", filename.c_str());
        return false;
    }

    table.name = table_name;
    std::string line;

    // Read number of columns
    std::getline(file, line);
    table.num_columns = std::stoi(line);

    // Read number of rows
    std::getline(file, line);
    table.num_rows = std::stoi(line);

    // Read column names
    std::getline(file, line);
    std::stringstream ss(line);
    std::string col_name;
    while (std::getline(ss, col_name, ',')) {
        while (!col_name.empty() && std::isspace(col_name[0])) col_name.erase(0, 1);
        while (!col_name.empty() && std::isspace(col_name.back())) col_name.pop_back();
        table.column_names.push_back(col_name);
    }

    // Read tuples
    while (std::getline(file, line)) {
        std::vector<std::string> tuple;
        std::stringstream tuple_ss(line);
        std::string value;
        while (std::getline(tuple_ss, value, ',')) {
            while (!value.empty() && std::isspace(value[0])) value.erase(0, 1);
            while (!value.empty() && std::isspace(value.back())) value.pop_back();
            tuple.push_back(value);
        }
        if (!tuple.empty() && tuple.size() == (size_t)table.num_columns) {
            table.tuples.push_back(tuple);
        }
    }

    table.num_rows = table.tuples.size(); // Update based on actual data
    file.close();
    return true;
}