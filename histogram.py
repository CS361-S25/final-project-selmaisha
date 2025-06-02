# import pandas as pd
# import matplotlib.pyplot as plt

# # Load your CSV file
# df = pd.read_csv('worlddata.csv')

# # List of logic tasks (excluding 'update', 'org', and 'dead')
# logic_tasks = ['not', 'nand', 'and', 'orn', 'or', 'andn', 'nor', 'xor', 'equ']

# # Sum the values for each task across all updates
# task_totals = df[logic_tasks].sum()

# # Plot the histogram
# plt.figure(figsize=(10, 6))
# plt.bar(task_totals.index, task_totals.values)
# plt.title('Total Organisms Solving Each Logic Task')
# plt.xlabel('Logic Task')
# plt.ylabel('Total Count Across All Updates')
# plt.grid(axis='y', linestyle='--', alpha=0.7)
# plt.tight_layout()
# plt.show()
import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv('num100000pop10rew10size80age1000.csv')

# Define logic task columns
logic_tasks = ['not', 'nand', 'and', 'orn', 'or', 'andn', 'nor', 'xor', 'equ']

# Count how many tasks were solved (non-zero) at each update
task_diversity_per_update = (df[logic_tasks] > 0).sum(axis=1)

# Sum across all updates
task_totals = df[logic_tasks].sum()

# Plot as a bar chart
plt.figure(figsize=(10, 6))
task_totals.plot(kind='bar', color='skyblue', edgecolor='black')
plt.title('Total Organisms That Solved Each Task Over Time')
plt.xlabel('Logic Task')
plt.ylabel('Total Count (All Updates Combined)')
plt.xticks(rotation=45)
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.show()
