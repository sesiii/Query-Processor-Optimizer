SELECT employees.name, salaries.salary FROM employees JOIN salaries ON employees.emp_id = salaries.emp_id WHERE salaries.salary > 50000;
