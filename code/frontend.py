import streamlit as st
import subprocess
import os

# Create temp directory if it doesn't exist
os.makedirs("temp", exist_ok=True)

st.title("SQL Query Processor")
st.write("Enter your SQL query below and click 'Process Query' to analyze it.")

# Text area for query input
query = st.text_area("Enter SQL Query:", height=150)

# Process button
if st.button("Process Query"):
    if query:
        # Write query to input.txt
        with open("temp/input.txt", "w") as f:
            f.write(query)
        
        # Execute the query processor
        try:
            subprocess.run(["./query_processor"], check=True)
            
            # Read results from output.txt
            try:
                with open("temp/output.txt", "r") as f:
                    result = f.read()
                
                # Display results
                st.subheader("Query Processing Result:")
                st.text(result)
            except FileNotFoundError:
                st.error("Output file not found. The query processor may have failed to generate results.")
        except subprocess.CalledProcessError:
            st.error("Error executing query processor. Please check if the executable exists and has proper permissions.")
    else:
        st.warning("Please enter a query before processing.")

# Add instructions
st.sidebar.header("Instructions")
st.sidebar.write("""
1. Enter your SQL query in the text area
2. Click 'Process Query' to analyze
3. View the results below
""")
