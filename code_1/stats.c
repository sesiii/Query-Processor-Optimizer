#include "stats.h"
#include <string.h>
#include <stdio.h>

#define MAX_TABLES 10
#define MAX_COLUMNS_PER_TABLE 10

TableStats *tables[MAX_TABLES];
int table_count = 0;

// Initialize hardcoded statistics for testing
void init_stats() {
    printf("Initializing statistics with hardcoded values\n");
    
    // Employee table statistics
    TableStats *employees = malloc(sizeof(TableStats));
    employees->name = strdup("employees");
    employees->row_count = 10000;
    employees->column_count = 3;
    employees->size_in_bytes = employees->row_count * 30; // Assuming 30 bytes per row
    employees->columns = malloc(employees->column_count * sizeof(ColumnStats*));
    
    employees->columns[0] = malloc(sizeof(ColumnStats));
    employees->columns[0]->table = strdup("employees");
    employees->columns[0]->column = strdup("emp_id");
    employees->columns[0]->distinct_values = 10000; // Primary key
    employees->columns[0]->min_value = 1;
    employees->columns[0]->max_value = 10000;
    employees->columns[0]->selectivity = 0.0001; // 1/distinct_values
    
    employees->columns[1] = malloc(sizeof(ColumnStats));
    employees->columns[1]->table = strdup("employees");
    employees->columns[1]->column = strdup("name");
    employees->columns[1]->distinct_values = 9500; // Some duplicate names
    employees->columns[1]->min_value = 0; // Not applicable for names
    employees->columns[1]->max_value = 0; // Not applicable for names
    employees->columns[1]->selectivity = 0.00011; // Slightly higher than 1/distinct_values
    
    employees->columns[2] = malloc(sizeof(ColumnStats));
    employees->columns[2]->table = strdup("employees");
    employees->columns[2]->column = strdup("dept_id");
    employees->columns[2]->distinct_values = 20; // 20 departments
    employees->columns[2]->min_value = 1;
    employees->columns[2]->max_value = 20;
    employees->columns[2]->selectivity = 0.05; // 1/distinct_values
    
    tables[table_count++] = employees;
    
    // Salaries table statistics
    TableStats *salaries = malloc(sizeof(TableStats));
    salaries->name = strdup("salaries");
    salaries->row_count = 10000;
    salaries->column_count = 3;
    salaries->size_in_bytes = salaries->row_count * 20; // Assuming 20 bytes per row
    salaries->columns = malloc(salaries->column_count * sizeof(ColumnStats*));
    
    salaries->columns[0] = malloc(sizeof(ColumnStats));
    salaries->columns[0]->table = strdup("salaries");
    salaries->columns[0]->column = strdup("emp_id");
    salaries->columns[0]->distinct_values = 10; // Foreign key to employees
    salaries->columns[0]->min_value = 1;
    salaries->columns[0]->max_value = 100;
    salaries->columns[0]->selectivity = 0.1; // 1/distinct_values
    
    salaries->columns[1] = malloc(sizeof(ColumnStats));
    salaries->columns[1]->table = strdup("salaries");
    salaries->columns[1]->column = strdup("salary");
    salaries->columns[1]->distinct_values = 1000; // 1000 unique salary values
    salaries->columns[1]->min_value = 30000;
    salaries->columns[1]->max_value = 150000;
    salaries->columns[1]->selectivity = 0.001; // 1/distinct_values
    
    salaries->columns[2] = malloc(sizeof(ColumnStats));
    salaries->columns[2]->table = strdup("salaries");
    salaries->columns[2]->column = strdup("year");
    salaries->columns[2]->distinct_values = 10; // 10 years of data
    salaries->columns[2]->min_value = 2013;
    salaries->columns[2]->max_value = 2023;
    salaries->columns[2]->selectivity = 0.1; // 1/distinct_values
    
    tables[table_count++] = salaries;
    
    printf("Statistics initialized for %d tables\n", table_count);
}

void free_stats() {
    for (int i = 0; i < table_count; i++) {
        for (int j = 0; j < tables[i]->column_count; j++) {
            free(tables[i]->columns[j]->table);
            free(tables[i]->columns[j]->column);
            free(tables[i]->columns[j]);
        }
        free(tables[i]->columns);
        free(tables[i]->name);
        free(tables[i]);
    }
    table_count = 0;
}

TableStats* get_table_stats(const char *table_name) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i]->name, table_name) == 0) {
            return tables[i];
        }
    }
    printf("Warning: No statistics found for table %s\n", table_name);
    return NULL;
}

ColumnStats* get_column_stats(const char *table_name, const char *column_name) {
    TableStats *table = get_table_stats(table_name);
    if (!table) return NULL;
    
    for (int i = 0; i < table->column_count; i++) {
        if (strcmp(table->columns[i]->column, column_name) == 0) {
            return table->columns[i];
        }
    }
    
    printf("Warning: No statistics found for column %s.%s\n", table_name, column_name);
    return NULL;
}

double calculate_join_selectivity(const char *table1, const char *column1, 
                                 const char *table2, const char *column2) {
    ColumnStats *stats1 = get_column_stats(table1, column1);
    ColumnStats *stats2 = get_column_stats(table2, column2);
    
    if (!stats1 || !stats2) return 1.0; // Can't determine, assume full cartesian product
    
    // For primary key - foreign key joins, selectivity is 1/max(distinct_values)
    return 1.0 / (double)stats1->distinct_values;
}

double calculate_condition_selectivity(const char *table, const char *column, 
                                      const char *op, int value) {
    ColumnStats *stats = get_column_stats(table, column);
    if (!stats) return 1.0;
    
    double range = stats->max_value - stats->min_value;
    if (range <= 0) return 0.5; // Default if range is not applicable
    
    if (strcmp(op, "=") == 0) {
        // Equality has selectivity of 1/distinct_values
        return 1.0 / stats->distinct_values;
    } else if (strcmp(op, "<") == 0 || strcmp(op, "<=") == 0) {
        // Less than - estimate based on range and value
        double fraction = (value - stats->min_value) / range;
        return fraction > 1.0 ? 1.0 : (fraction < 0.0 ? 0.0 : fraction);
    } else if (strcmp(op, ">") == 0 || strcmp(op, ">=") == 0) {
        // Greater than - estimate based on range and value
        double fraction = (stats->max_value - value) / range;
        return fraction > 1.0 ? 1.0 : (fraction < 0.0 ? 0.0 : fraction);
    }
    
    return 0.5; // Default if operator not recognized
}

int estimate_result_size(const char *table_name, double selectivity) {
    TableStats *stats = get_table_stats(table_name);
    if (!stats) return 0;
    
    return (int)(stats->row_count * selectivity);
}