# Query Processor and Optimizer

This repository contains the source code and project report for a Query Processor and Optimizer developed for the Database Management Systems course (CS43002) at the Indian Institute of Technology Kharagpur.

## Project Summary

The project implements a query processor in C that parses SQL SELECT queries, builds an Abstract Syntax Tree (AST), and optimizes execution plans. It uses Flex for lexical analysis and Bison for parsing. The optimizer applies **selection pushdown** and **projection pushdown** to minimize computational cost, using table/column statistics for cost-based plan selection.

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
├── images/
│   ├── cost_comparison.png
│   ├── iit_logo.png
│   ├── original_ast.png
│   ├── original_execplan.png
│   ├── projection_pushdown_execplan.png
│   ├── selection_pushdown_exceplan.png
├── src/
│   ├── lexer.l
│   ├── parser.y
│   ├── main.cpp
│   ├── optimizer.cpp
│   ├── optimizer.hpp
│   ├── stats.cpp
│   ├── stats.hpp
│   ├── parser.hpp
├── report.tex
├── query.sql
├── README.md
```

- **images/**: Contains report diagrams and the title page logo (iit_logo.png)
- **src/**: C source files for the query processor
- **report.tex**: LaTeX source for the project report
- **query.sql**: Sample SQL query file
- **README.md**: This file

## Project Visuals

### Original Abstract Syntax Tree
![Original AST](images/original_ast.png)

### Original Execution Plan
![Original Execution Plan](images/original_execplan.png)

### Selection Pushdown Execution Plan
![Selection Pushdown Execution Plan](images/selection_pushdown_exceplan.png)

### Projection Pushdown Execution Plan
![Projection Pushdown Execution Plan](images/projection_pushdown_execplan.png)

### Cost Comparison
![Cost Comparison of Execution Plans](images/cost_comparison.png)

## Setup and Compilation

### Clone the Repository:
```bash
git clone https://github.com/sesiii/query-processor-optimizer.git
cd query-processor-optimizer
```

## Acknowledgments

- Flex & Bison documentation
- References: Database System Concepts (Silberschatz et al.), Database Management Systems (Ramakrishnan & Gehrke), Flex & Bison documentation

Contributions and feedback are welcome!
