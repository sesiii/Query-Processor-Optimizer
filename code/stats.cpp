#include "stats.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_TABLES 10
#define MAX_COLUMNS_PER_TABLE 10

TableStats *tables[MAX_TABLES];
int table_count = 0;

// Helper function to create a new column stat
static ColumnStats* create_column_stat(const char *table, const char *column, 
                                     int distinct, int min, int max, double sel) {
    ColumnStats *stat = (ColumnStats *)malloc(sizeof(ColumnStats));
    stat->table = strdup(table);
    stat->column = strdup(column);
    stat->distinct_values = distinct;
    stat->min_value = min;
    stat->max_value = max;
    stat->selectivity = sel;
    return stat;
}

// Initialize statistics from metadata.txt or defaults
void init_stats() {
    printf("Initializing statistics...\n");

    // Default table configurations
    struct {
        char *name;
        int row_count;
        int column_count;
        int bytes_per_row;
        char *column_names[4];
        struct {
            char *column;
            int distinct;
            int min;
            int max;
            double sel;
        } columns[4];
    } default_tables[] = {
        {"employees", 10000, 4, 40,
         {"emp_id", "name", "dept_id", "salary"},
         {{"emp_id", 10000, 1, 10000, 0.0001},
          {"name", 9500, 0, 0, 0.00011},
          {"dept_id", 20, 1, 20, 0.05},
          {"salary", 1000, 30000, 150000, 0.001}}},
        {"salaries", 10000, 4, 25,
         {"emp_id", "salary", "year", "bonus"},
         {{"emp_id", 10000, 1, 10000, 0.0001},
          {"salary", 1000, 30000, 150000, 0.001},
          {"year", 10, 2013, 2023, 0.1},
          {"bonus", 500, 0, 50000, 0.002}}},
        {"departments", 20, 3, 50,
         {"dept_id", "dept_name", "location"},
         {{"dept_id", 20, 1, 20, 0.05},
          {"dept_name", 20, 0, 0, 0.05},
          {"location", 10, 0, 0, 0.1}}},
        {"projects", 5000, 4, 60,
         {"project_id", "project_name", "dept_id", "budget"},
         {{"project_id", 5000, 1, 5000, 0.0002},
          {"project_name", 2500, 0, 0, 0.0004},
          {"dept_id", 20, 1, 20, 0.05},
          {"budget", 1000, 10000, 1000000, 0.001}}}
    };
    int default_table_count = 4;
    
    // Initialize tables
    for (int i = 0; i < default_table_count; i++) {
        TableStats *table = (TableStats *)malloc(sizeof(TableStats));
        table->name = strdup(default_tables[i].name);
        table->row_count = default_tables[i].row_count;
        table->column_count = default_tables[i].column_count;
        table->size_in_bytes = table->row_count * default_tables[i].bytes_per_row;
        table->column_names = (char **)malloc(table->column_count * sizeof(char *));
        table->columns = (ColumnStats**)malloc(table->column_count * sizeof(ColumnStats *));

        for (int j = 0; j < table->column_count; j++) {
            table->column_names[j] = strdup(default_tables[i].column_names[j]);
            table->columns[j] = create_column_stat(
                table->name,
                default_tables[i].columns[j].column,
                default_tables[i].columns[j].distinct,
                default_tables[i].columns[j].min,
                default_tables[i].columns[j].max,
                default_tables[i].columns[j].sel
            );
        }
        tables[table_count++] = table;
    }

    printf("Statistics initialized for %d tables\n", table_count);
}

void free_stats() {
    for (int i = 0; i < table_count; i++) {
        for (int j = 0; j < tables[i]->column_count; j++) {
            free(tables[i]->columns[j]->table);
            free(tables[i]->columns[j]->column);
            free(tables[i]->columns[j]);
            free(tables[i]->column_names[j]);
        }
        free(tables[i]->columns);
        free(tables[i]->column_names);
        free(tables[i]->name);
        free(tables[i]);
    }
    table_count = 0;
}

TableStats *get_table_stats(const char *table_name) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i]->name, table_name) == 0) {
            return tables[i];
        }
    }
    // printf("No statistics found for table %s\n", table_name);
    return NULL;
}

ColumnStats *get_column_stats(const char *table_name, const char *column_name) {
    TableStats *table = get_table_stats(table_name);
    if (!table) return NULL;

    for (int i = 0; i < table->column_count; i++) {
        if (strcmp(table->columns[i]->column, column_name) == 0) {
            return table->columns[i];
        }
    }
    // printf(" No statistics found for column %s.%s\n", table_name, column_name);
    return NULL;
}

double calculate_join_selectivity(const char *table1, const char *column1, 
                                const char *table2, const char *column2) {
    ColumnStats *stats1 = get_column_stats(table1, column1);
    ColumnStats *stats2 = get_column_stats(table2, column2);

    if (!stats1 || !stats2) {
        TableStats *t1 = get_table_stats(table1);
        TableStats *t2 = get_table_stats(table2);
        double default_sel = 1.0 / fmax(t1 ? t1->row_count : 1000, t2 ? t2->row_count : 1000);
        // printf("No stats for %s.%s or %s.%s, using default selectivity: %f\n",
            //   table1, column1, table2, column2, default_sel);
        return default_sel;
    }

    return 1.0 / fmax(stats1->distinct_values, stats2->distinct_values);
}

double calculate_condition_selectivity(const char *table, const char *column,
                                     const char *op, int value) {
    ColumnStats *stats = get_column_stats(table, column);
    if (!stats) return 1.0;

    if (stats->min_value == 0 && stats->max_value == 0) {
        return 1.0 / stats->distinct_values;
    }

    double range = stats->max_value - stats->min_value;
    if (range <= 0) return 0.5;

    if (strcmp(op, "=") == 0) {
        return 1.0 / stats->distinct_values;
    }
    else if (strcmp(op, "<") == 0 || strcmp(op, "<=") == 0) {
        double fraction = (value - stats->min_value) / range;
        return fraction > 1.0 ? 1.0 : (fraction < 0.0 ? 0.0 : fraction);
    }
    else if (strcmp(op, ">") == 0 || strcmp(op, ">=") == 0) {
        double fraction = (stats->max_value - value) / range;
        return fraction > 1.0 ? 1.0 : (fraction < 0.0 ? 0.0 : fraction);
    }

    return 0.5;
}

int estimate_result_size(const char *table_name, double selectivity) {
    TableStats *stats = get_table_stats(table_name);
    if (!stats) return 0;
    return (int)(stats->row_count * selectivity);
}