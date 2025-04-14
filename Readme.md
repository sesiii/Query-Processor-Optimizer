# Query Processor and Optimizer

This repository contains the source code and project report for a Query Processor and Optimizer developed for the Database Management Systems course (CS43002) at the Indian Institute of Technology Kharagpur.

## Project Summary

The project implements a query processor in C++ that parses SQL SELECT queries, builds an Abstract Syntax Tree (AST), and optimizes execution plans. It uses Flex for lexical analysis and Bison for parsing. The optimizer applies **selection pushdown** and **projection pushdown** to minimize computational cost, using table/column statistics for cost-based plan selection.

Supported SQL constructs include:
- `SELECT`
- `FROM`
- `WHERE`
- `JOIN`
- Aggregates (`COUNT`, `MAX`, `MIN`, `AVG`)

The report analyzes a sample query:

```sql
SELECT employees.name, departments.dept_name 
FROM employees 
JOIN departments 
ON employees.dept_id = departments.dept_id 
WHERE departments.dept_name = 'Engineering';
```

It compares execution plans (original, selection pushdown, projection pushdown) with cost estimates, demonstrating optimization effectiveness.

## Team Members

- Dadi Sasank Kumar (22CS10020)
- Jeevan Varma (22CS10038)
- Gurram Dhanunjay (22CS10029)
- Venkata Yaswanth (22CS30031)
- Nerella Trilochan (22CS10048)

## Repository Structure

```
├
├── code/
|        |── images/
│        ├── cost_comparison.png
│        ├── original_ast.png
│        ├── original_execplan.png
│        ├── projection_pushdown_execplan.png
│        ├── selection_pushdown_exceplan.png
│   ├── lexer.l
│   ├── parser.y
│   ├── main.cpp
│   ├── optimizer.cpp
│   ├── optimizer.hpp
│   ├── stats.cpp
│   ├── stats.hpp
│   ├── parser.hpp
├── report.tex
|── Query-Processor-Optimizer_Report.pdf
|── Proposal.pdf
├── query.sql
├── README.md
```
## Project Visuals

![image](https://github.com/user-attachments/assets/0d9a556b-1df1-4d3c-bf83-0e3231ca9314)


## Setup and Compilation

### Clone the Repository:
```bash
git clone https://github.com/sesiii/Query_Processor_Optimizer
cd Query_Processor_Optimizer
make
```

## Acknowledgments

- Flex & Bison documentation
- References: Database System Concepts (Silberschatz et al.), Database Management Systems (Ramakrishnan & Gehrke), Flex & Bison documentation

Contributions and feedback are welcome!
