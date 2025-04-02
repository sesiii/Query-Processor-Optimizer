
SELECT 
    employee_id,
    first_name,
    last_name,
    department,
    salary
FROM 
    employees
WHERE 
    salary > 50000
ORDER BY 
    salary DESC;