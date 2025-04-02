# import os
# import subprocess
# import time
# from datetime import datetime

# def get_untracked_files():
#     """ Get a list of untracked (new) files """
#     result = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], capture_output=True, text=True)
#     return result.stdout.splitlines()

# def get_modified_files():
#     """ Get a list of modified files """
#     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
#     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# def get_deleted_files():
#     """ Get a list of deleted files """
#     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
#     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# def commit_and_push():
#     modified_files = get_modified_files()
#     untracked_files = get_untracked_files()
#     deleted_files = get_deleted_files()
#     all_files = modified_files + untracked_files + deleted_files

#     if not all_files:
#         print("No new, modified, or deleted files to commit.")
#         return

#     for file in all_files:
#         if file in deleted_files:
#             subprocess.run(["git", "rm", file])  # Remove deleted files
#         else:
#             subprocess.run(["git", "add", file])  # Add new/modified files

#         current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
#         commit_message = f"changes in {os.path.basename(file)} on {current_time}"
#         subprocess.run(["git", "commit", "-m", commit_message])

#         # Push immediately after each commit to increase contribution count
#         subprocess.run(["git", "push", "origin", "main"])  # Change branch if necessary
#         time.sleep(2)  # Small delay to mimic manual commits

#     print("All changes committed and pushed successfully!")

# if __name__ == "__main__":
#     commit_and_push()


# # # # import os
# # # # import subprocess
# # # # import time
# # # # from datetime import datetime

# # # # def get_untracked_files():
# # # #     """ Get a list of untracked (new) files """
# # # #     result = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], capture_output=True, text=True)
# # # #     return result.stdout.splitlines()

# # # # def get_modified_files():
# # # #     """ Get a list of modified files """
# # # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# # # # def get_deleted_files():
# # # #     """ Get a list of deleted files """
# # # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# # # # def commit_and_push():
# # # #     modified_files = get_modified_files()
# # # #     untracked_files = get_untracked_files()
# # # #     deleted_files = get_deleted_files()
# # # #     all_files = modified_files + untracked_files + deleted_files

# # # #     if not all_files:
# # # #         print("No new, modified, or deleted files to commit.")
# # # #         return

# # # #     for file in all_files:
# # # #         if file in deleted_files:
# # # #             subprocess.run(["git", "rm", file])  # Remove deleted files
# # # #         else:
# # # #             subprocess.run(["git", "add", file])  # Add new/modified files

# # # #         current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
# # # #         commit_message = f"changes in {os.path.basename(file)} on {current_time}, #auto-commit"
# # # #         # Commit changes with a message
# # # #         subprocess.run(["git", "commit", "-m", commit_message])

# # # #         # Push immediately after each commit to increase contribution count
# # # #         subprocess.run(["git", "push", "origin", "main"])  # Change branch if necessary
# # # #         time.sleep(2)  # Small delay to mimic manual commits

# # # #     print("All changes committed and pushed successfully!")

# # # # if __name__ == "__main__":
# # # #     commit_and_push()


# # # # import os
# # # # import subprocess
# # # # import time
# # # # from datetime import datetime

# # # # def get_untracked_files():
# # # #     """ Get a list of untracked (new) files """
# # # #     result = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], capture_output=True, text=True)
# # # #     return result.stdout.splitlines()

# # # # def get_modified_files():
# # # #     """ Get a list of modified files """
# # # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# # # # def get_deleted_files():
# # # #     """ Get a list of deleted files """
# # # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# # # # def commit_and_push():
# # # #     modified_files = get_modified_files()
# # # #     untracked_files = get_untracked_files()
# # # #     deleted_files = get_deleted_files()
# # # #     all_files = modified_files + untracked_files + deleted_files

# # # #     if not all_files:
# # # #         print("No new, modified, or deleted files to commit.")
# # # #         return

# # # #     for file in all_files:
# # # #         if file in deleted_files:
# # # #             subprocess.run(["git", "rm", file])  # Remove deleted files
# # # #         else:
# # # #             subprocess.run(["git", "add", file])  # Add new/modified files

# # # #         current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
# # # #         commit_message = f"changes in {os.path.basename(file)} on {current_time}"
# # # #         subprocess.run(["git", "commit", "-m", commit_message])

# # # #         # Push immediately after each commit to increase contribution count
# # # #         subprocess.run(["git", "push", "origin", "main"])  # Change branch if necessary
# # # #         time.sleep(2)  # Small delay to mimic manual commits

# # # #     print("All changes committed and pushed successfully!")

# # # # if __name__ == "__main__":
# # # #     commit_and_push()


# # # import os
# # # import subprocess
# # # import time
# # # import random
# # # from datetime import datetime

# # # def get_untracked_files():
# # #     """ Get a list of untracked (new) files """
# # #     result = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], capture_output=True, text=True)
# # #     return result.stdout.splitlines()

# # # def get_modified_files():
# # #     """ Get a list of modified files """
# # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# # # def get_deleted_files():
# # #     """ Get a list of deleted files """
# # #     result = subprocess.run(["git", "status", "--porcelain"], capture_output=True, text=True)
# # #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# # # def generate_commit_message(filename):
# # #     """ Generate a dynamic and realistic commit message """
# # #     templates = [
# # #         "Updated {file} with new changes",
# # #         "Refactored {file} for better performance",
# # #         "Fixed minor issues in {file}",
# # #         "Added new content to {file}",
# # #         "Cleaned up {file} and optimized code",
# # #         "Improved readability of {file}",
# # #         "Resolved conflicts in {file}",
# # #         "Enhanced {file} with additional functionality",
# # #         "Removed deprecated sections from {file}",
# # #         "General improvements to {file}",
# # #     ]
    
# # #     commit_message = random.choice(templates).format(file=os.path.basename(filename))
# # #     timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
# # #     return f"{commit_message} on {timestamp}"

# # # def commit_and_push():
# # #     modified_files = get_modified_files()
# # #     untracked_files = get_untracked_files()
# # #     deleted_files = get_deleted_files()
# # #     all_files = modified_files + untracked_files + deleted_files

# # #     if not all_files:
# # #         print("No new, modified, or deleted files to commit.")
# # #         return

# # #     for file in all_files:
# # #         if file in deleted_files:
# # #             subprocess.run(["git", "rm", file])  # Remove deleted files
# # #         else:
# # #             subprocess.run(["git", "add", file])  # Add new/modified files

# # #         commit_message = generate_commit_message(file)
# # #         subprocess.run(["git", "commit", "-m", commit_message])

# # #         # Push immediately after each commit
# # #         subprocess.run(["git", "push", "origin", "main"])  # Change branch if necessary
# # #         time.sleep(2)  # Small delay to mimic manual commits

# # #     print("All changes committed and pushed successfully!")

# # # if __name__ == "__main__":
# # #     commit_and_push()


# # import os
# # import subprocess
# # import time
# # from datetime import datetime
# # from together import Together

# # # Hardcoded Together AI API key
# # API_KEY = "b1f8ad7fd5671241f7901997ee157ec8fb7475ba8cbf6a85118a06d1f3de7fd5"
# # client = Together(api_key=API_KEY)

# # # Specify the directory for Git operations (change this as needed)
# # DIRECTORY = "/home/dadi/Desktop/Labs_6th sem/OS_Lab"  # e.g., "/home/user/myproject"

# # def run_git_command(command, cwd=None):
# #     """Run a Git command in the specified directory"""
# #     return subprocess.run(command, capture_output=True, text=True, cwd=cwd or DIRECTORY)

# # def get_untracked_files():
# #     """Get a list of untracked (new) files"""
# #     result = run_git_command(["git", "ls-files", "--others", "--exclude-standard"])
# #     return result.stdout.splitlines()

# # def get_modified_files():
# #     """Get a list of modified files"""
# #     result = run_git_command(["git", "status", "--porcelain"])
# #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# # def get_deleted_files():
# #     """Get a list of deleted files"""
# #     result = run_git_command(["git", "status", "--porcelain"])
# #     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# # def get_file_diff(filename):
# #     """Get the git diff for a modified file"""
# #     result = run_git_command(["git", "diff", "--cached", "--", filename])
# #     if not result.stdout:  # If not staged, get the unstaged diff
# #         result = run_git_command(["git", "diff", "--", filename])
# #     return result.stdout

# # def call_together_api(diff):
# #     """Call Together AI API to generate a commit message based on the diff"""
# #     print(f"Calling Together AI API with diff:\n{diff}")  # Print the diff being sent
    
# #     prompt = (
# #         "You are a senior developer writing a concise, realistic Git commit message. "
# #         "Based on the following git diff, write a detailed and professional commit message "
# #         "describing the changes. Include specifics about what was added, removed, or modified, "
# #         "and why. Keep it under 80 characters if possible:\n\n"
# #         f"```diff\n{diff}\n```"
# #     )

# #     try:
# #         response = client.chat.completions.create(
# #             model="meta-llama/Llama-3.1-70B-Instruct-Turbo",
# #             messages=[{"role": "user", "content": prompt}],
# #             max_tokens=100,
# #             temperature=0.7,
# #             top_p=0.9,
# #         )
# #         api_response = response.choices[0].message.content.strip()
# #         print(f"API response received: {api_response}")  # Print the API's output
# #         return api_response
# #     except Exception as e:
# #         print(f"API call failed: {e}")  # Print any errors
# #         raise  # Re-raise the exception to handle it upstream

# # def generate_fallback_message(filename, is_deleted=False):
# #     """Generate a fallback commit message for new or deleted files"""
# #     file_name = os.path.basename(filename)
# #     timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
# #     if is_deleted:
# #         return f"Removed {file_name} - obsolete as of {timestamp}"
    
# #     file_ext = os.path.splitext(filename)[1].lower()
# #     if file_ext in [".py", ".js"]:
# #         return f"Added {file_name} with initial implementation ({timestamp})"
# #     elif file_ext in [".md", ".txt"]:
# #         return f"Created {file_name} with initial draft ({timestamp})"
# #     else:
# #         return f"Introduced {file_name} to the project ({timestamp})"

# # def generate_commit_message(filename, is_deleted=False):
# #     """Generate a commit message, using LLM for modified files or fallback for others"""
# #     timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
# #     if is_deleted:
# #         return generate_fallback_message(filename, is_deleted=True)
    
# #     if filename in get_modified_files():
# #         diff = get_file_diff(filename)
# #         if diff:
# #             try:
# #                 message = call_together_api(diff)
# #                 return f"{message} ({timestamp})"
# #             except Exception as e:
# #                 print(f"Using fallback due to API failure for {filename}")
# #                 return f"Updated {os.path.basename(filename)} with recent changes ({timestamp})"
    
# #     # Fallback for untracked (new) files
# #     return generate_fallback_message(filename)

# # def commit_and_push():
# #     # Ensure the directory exists and is a Git repo
# #     if not os.path.isdir(DIRECTORY) or not os.path.isdir(os.path.join(DIRECTORY, ".git")):
# #         print(f"Error: {DIRECTORY} is not a valid Git repository.")
# #         return

# #     modified_files = get_modified_files()
# #     untracked_files = get_untracked_files()
# #     deleted_files = get_deleted_files()
# #     all_files = modified_files + untracked_files + deleted_files

# #     if not all_files:
# #         print("No new, modified, or deleted files to commit.")
# #         return

# #     for file in all_files:
# #         if file in deleted_files:
# #             run_git_command(["git", "rm", file])  # Remove deleted files
# #             commit_message = generate_commit_message(file, is_deleted=True)
# #         else:
# #             run_git_command(["git", "add", file])  # Add new/modified files
# #             commit_message = generate_commit_message(file)

# #         print(f"Committing: {commit_message}")  # Print the final commit message
# #         run_git_command(["git", "commit", "-m", commit_message])

# #         # Push immediately after each commit
# #         run_git_command(["git", "push", "origin", "main"])  # Change branch if necessary
# #         time.sleep(2)  # Small delay to mimic manual commits

# #     print("All changes committed and pushed successfully!")

# # if __name__ == "__main__":
# #     commit_and_push()


# import os
# import subprocess
# import time
# from datetime import datetime
# from together import Together

# # Hardcoded Together AI API key
# API_KEY = "b1f8ad7fd5671241f7901997ee157ec8fb7475ba8cbf6a85118a06d1f3de7fd5"
# client = Together(api_key=API_KEY)

# # Set to your current working directory
# DIRECTORY = "/home/dadi/Desktop/Labs_6th sem/OS_Lab"

# def run_git_command(command, cwd=None):
#     """Run a Git command in the specified directory"""
#     return subprocess.run(command, capture_output=True, text=True, cwd=cwd or DIRECTORY)

# def get_untracked_files():
#     """Get a list of untracked (new) files, excluding env/ directory"""
#     result = run_git_command(["git", "ls-files", "--others", "--exclude-standard"])
#     # Filter out files in the env/ directory
#     return [f for f in result.stdout.splitlines() if not f.startswith("env/")]

# def get_modified_files():
#     """Get a list of modified files"""
#     result = run_git_command(["git", "status", "--porcelain"])
#     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" M ")]

# def get_deleted_files():
#     """Get a list of deleted files"""
#     result = run_git_command(["git", "status", "--porcelain"])
#     return [line.split()[-1] for line in result.stdout.splitlines() if line.startswith(" D ")]

# def get_file_diff(filename):
#     """Get the git diff for a modified file"""
#     result = run_git_command(["git", "diff", "--cached", "--", filename])
#     if not result.stdout:  # If not staged, get the unstaged diff
#         result = run_git_command(["git", "diff", "--", filename])
#     return result.stdout

# def call_together_api(diff):
#     """Call Together AI API to generate a commit message based on the diff"""
#     print(f"Calling Together AI API with diff:\n{diff}")
    
#     prompt = (
#         "You are a senior developer writing a concise, realistic Git commit message. "
#         "Based on the following git diff, write a detailed and professional commit message "
#         "describing the changes. Include specifics about what was added, removed, or modified, "
#         "and why. Keep it under 80 characters if possible:\n\n"
#         f"```diff\n{diff}\n```"
#     )

#     try:
#         response = client.chat.completions.create(
#             model="meta-llama/Llama-3.1-70B-Instruct-Turbo",
#             messages=[{"role": "user", "content": prompt}],
#             max_tokens=100,
#             temperature=0.7,
#             top_p=0.9,
#         )
#         api_response = response.choices[0].message.content.strip()
#         print(f"API response received: {api_response}")
#         return api_response
#     except Exception as e:
#         print(f"API call failed: {e}")
#         raise

# def generate_fallback_message(filename, is_deleted=False):
#     """Generate a fallback commit message for new or deleted files"""
#     file_name = os.path.basename(filename)
#     timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
#     if is_deleted:
#         return f"Removed {file_name} - obsolete as of {timestamp}"
    
#     file_ext = os.path.splitext(filename)[1].lower()
#     if file_ext in [".py", ".js"]:
#         return f"Added {file_name} with initial implementation ({timestamp})"
#     elif file_ext in [".md", ".txt"]:
#         return f"Created {file_name} with initial draft ({timestamp})"
#     else:
#         return f"Introduced {file_name} to the project ({timestamp})"

# def generate_commit_message(filename, is_deleted=False):
#     """Generate a commit message, using LLM for modified files or fallback for others"""
#     timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
#     if is_deleted:
#         return generate_fallback_message(filename, is_deleted=True)
    
#     if filename in get_modified_files():
#         diff = get_file_diff(filename)
#         if diff:
#             try:
#                 message = call_together_api(diff)
#                 return f"{message} ({timestamp})"
#             except Exception as e:
#                 print(f"Using fallback due to API failure for {filename}")
#                 return f"Updated {os.path.basename(filename)} with recent changes ({timestamp})"
    
#     # Fallback for untracked (new) files
#     return generate_fallback_message(filename)

# def commit_and_push():
#     # Ensure the directory exists and is a Git repo
#     if not os.path.isdir(DIRECTORY) or not os.path.isdir(os.path.join(DIRECTORY, ".git")):
#         print(f"Error: {DIRECTORY} is not a valid Git repository.")
#         return

#     modified_files = get_modified_files()
#     untracked_files = get_untracked_files()
#     deleted_files = get_deleted_files()
#     all_files = modified_files + untracked_files + deleted_files

#     if not all_files:
#         print("No new, modified, or deleted files to commit.")
#         return

#     for file in all_files:
#         if file in deleted_files:
#             run_git_command(["git", "rm", file])  # Remove deleted files
#             commit_message = generate_commit_message(file, is_deleted=True)
#         else:
#             run_git_command(["git", "add", file])  # Add new/modified files
#             commit_message = generate_commit_message(file)

#         print(f"Committing: {commit_message}")
#         run_git_command(["git", "commit", "-m", commit_message])

#         # Push immediately after each commit
#         run_git_command(["git", "push", "origin", "main"])  # Change branch if necessary
#         time.sleep(2)  # Small delay to mimic manual commits

#     print("All changes committed and pushed successfully!")

# if __name__ == "__main__":
#     commit_and_push()


import os
import subprocess
import time
from datetime import datetime
from together import Together

# Hardcoded Together AI API key
API_KEY = "b1f8ad7fd5671241f7901997ee157ec8fb7475ba8cbf6a85118a06d1f3de7fd5"
client = Together(api_key=API_KEY)

# Set to your current working directory
DIRECTORY = "/home/dadi/Desktop/Query_Processor_Optimizer"

def run_git_command(command, cwd=None):
    """Run a Git command in the specified directory"""
    return subprocess.run(command, capture_output=True, text=True, cwd=cwd or DIRECTORY)

def get_untracked_files():
    """Get a list of untracked (new) files, excluding env/ and a/ directories"""
    result = run_git_command(["git", "ls-files", "--others", "--exclude-standard"])
    files = [f for f in result.stdout.splitlines() if not f.startswith(("env/", "a/"))]
    print(f"Untracked files: {files}")
    return files

def get_modified_files():
    """Get a list of modified files, excluding changes in env/ and a/ directories"""
    result = run_git_command(["git", "status", "--porcelain"])
    files = [line.split()[-1] for line in result.stdout.splitlines() if not line.endswith(("env/", "a/")) and line.startswith(" M ")]
    print(f"Modified files: {files}")
    return files

def get_deleted_files():
    """Get a list of deleted files, excluding deletions in env/ and a/ directories"""
    result = run_git_command(["git", "status", "--porcelain"])
    files = [line.split()[-1] for line in result.stdout.splitlines() if not line.endswith(("env/", "a/")) and line.startswith(" D ")]
    print(f"Deleted files: {files}")
    return files

def get_file_diff(filename):
    """Get the git diff for a modified file"""
    result = run_git_command(["git", "diff", "--cached", "--", filename])
    if not result.stdout:  # If not staged, get the unstaged diff
        result = run_git_command(["git", "diff", "--", filename])
    return result.stdout

def call_together_api(diff):
    """Call Together AI API to generate a commit message based on the diff"""
    print(f"Calling Together AI API with diff:\n{diff}")
    
    prompt = (
        "You are a senior developer writing a concise, realistic Git commit message. "
        "Based on the following git diff, write a detailed and professional commit message "
        "describing the changes. Include specifics about what was added, removed, or modified, "
        "and why. Keep it under 20 characters if possible:\n\n"
        f"```diff\n{diff}\n```"
    )

    try:
        response = client.chat.completions.create(
            model="meta-llama/Llama-3.3-70B-Instruct-Turbo-Free",
            messages=[{"role": "user", "content": prompt}],
            max_tokens=100,
            temperature=0.7,
            top_p=0.9,
        )
        api_response = response.choices[0].message.content.strip()
        print(f"API response received: {api_response}")
        return api_response
    except Exception as e:
        print(f"API call failed: {e}")
        raise

def generate_fallback_message(filename, is_deleted=False):
    """Generate a fallback commit message for new or deleted files"""
    file_name = os.path.basename(filename)
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    if is_deleted:
        return f"Removed {file_name} - obsolete as of {timestamp}"
    
    file_ext = os.path.splitext(filename)[1].lower()
    if file_ext in [".py", ".js"]:
        return f"Added {file_name} with initial implementation ({timestamp})"
    elif file_ext in [".md", ".txt"]:
        return f"Created {file_name} with initial draft ({timestamp})"
    else:
        return f"Introduced {file_name} to the project ({timestamp})"

def generate_commit_message(filename, modified_files, is_deleted=False):
    """Generate a commit message, using LLM for modified files or fallback for others"""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    if is_deleted:
        print(f"Using fallback for deleted file: {filename}")
        return generate_fallback_message(filename, is_deleted=True)
    
    if filename in modified_files:  # Use the original modified_files list
        diff = get_file_diff(filename)
        if diff:
            try:
                message = call_together_api(diff)
                return f"{message} ({timestamp})"
            except Exception as e:
                print(f"Using fallback due to API failure for {filename}")
                return f"Updated {os.path.basename(filename)} with recent changes ({timestamp})"
        else:
            print(f"No diff found for modified file: {filename}, using fallback")
            return f"Updated {os.path.basename(filename)} with minor changes ({timestamp})"
    
    # Fallback for untracked (new) files
    print(f"Using fallback for untracked file: {filename}")
    return generate_fallback_message(filename)

def commit_and_push():
    if not os.path.isdir(DIRECTORY) or not os.path.isdir(os.path.join(DIRECTORY, ".git")):
        print(f"Error: {DIRECTORY} is not a valid Git repository.")
        return

    modified_files = get_modified_files()
    untracked_files = get_untracked_files()
    deleted_files = get_deleted_files()
    all_files = modified_files + untracked_files + deleted_files

    if not all_files:
        print("No new, modified, or deleted files to commit.")
        return

    for file in all_files:
        if file in deleted_files:
            run_git_command(["git", "rm", file])  # Remove deleted files
            commit_message = generate_commit_message(file, modified_files, is_deleted=True)
        else:
            run_git_command(["git", "add", file])  # Add new/modified files
            commit_message = generate_commit_message(file, modified_files)

        print(f"Committing: {commit_message}")
        run_git_command(["git", "commit", "-m", commit_message])

        # Push immediately after each commit
        run_git_command(["git", "push", "origin", "main"])  # Change branch if necessary
        time.sleep(2)  # Small delay to mimic manual commits

    print("All changes committed and pushed successfully!")

if __name__ == "__main__":
    commit_and_push()