SELECT e.name, s.salary FROM employees e JOIN salaries s ON e.emp_id = s.emp_id WHERE s.salary > 50000;
