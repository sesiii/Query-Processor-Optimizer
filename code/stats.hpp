#ifndef STATS_H
#define STATS_H

#include <stdlib.h>

typedef struct ColumnStats {
    char *table;
    char *column;
    int distinct_values;
    int min_value;
    int max_value;
    double selectivity;
} ColumnStats;

typedef struct TableStats {
    char *name;
    int row_count;          // Number of tuples (ntups)
    int column_count;       // Number of columns (ncols)
    char **column_names;    // Array of column names
    ColumnStats **columns;  // Array of column statistics
    int size_in_bytes;      // Average row size * row count
} TableStats;

// Initialize statistics from metadata file
void init_stats();

// Free statistics memory
void free_stats();

// Get statistics for a table
TableStats* get_table_stats(const char *table_name);

// Get column statistics
ColumnStats* get_column_stats(const char *table_name, const char *column_name);

// Calculate join selectivity between two columns
double calculate_join_selectivity(const char *table1, const char *column1, 
                                 const char *table2, const char *column2);

// Calculate selectivity for a condition
double calculate_condition_selectivity(const char *table, const char *column, 
                                     const char *op, int value);

// Calculate estimated size after applying a condition
int estimate_result_size(const char *table_name, double selectivity);

#endif