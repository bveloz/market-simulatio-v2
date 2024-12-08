import json
import matplotlib.pyplot as chart
from matplotlib.ticker import FuncFormatter
import numpy as np
from scipy import stats
import os

# Create the /results folder if it doesn't exist
if not os.path.exists('results'):
    os.makedirs('results')


with open("results/simulation_results.json", "r") as f:
    data = json.load(f)

# Initialize dictionaries to store data
purchase_style_expenses = {}
purchase_style_time_spent = {}
purchase_style_cumulative_time = {}

# Group total expenses, time spent, and cumulative time spent by purchase style
for customer in data["customers"]:
    purchase_style = customer["purchase_style"]
    total_expenses = customer["total_cumulative_expenses"]
    total_time_spent = customer["total_time_spent"]
    
    # Calculate cumulative time spent for this customer
    cumulative_time_spent = [sum(total_time_spent[:i+1]) for i in range(len(total_time_spent))]

    if purchase_style not in purchase_style_expenses:
        purchase_style_expenses[purchase_style] = [0] * len(total_expenses)
        purchase_style_time_spent[purchase_style] = [0] * len(total_time_spent)
        purchase_style_cumulative_time[purchase_style] = [0] * len(cumulative_time_spent)

    # Accumulate values for each day
    for day in range(len(total_expenses)):
        purchase_style_expenses[purchase_style][day] += total_expenses[day]
        purchase_style_time_spent[purchase_style][day] += total_time_spent[day]
        purchase_style_cumulative_time[purchase_style][day] += cumulative_time_spent[day]

# Calculate averages for expenses, time spent, and cumulative time spent
num_customers = len(data["customers"])
for purchase_style in purchase_style_expenses:
    purchase_style_expenses[purchase_style] = [
        expense / num_customers for expense in purchase_style_expenses[purchase_style]
    ]
    purchase_style_time_spent[purchase_style] = [
        time / num_customers for time in purchase_style_time_spent[purchase_style]
    ]
    purchase_style_cumulative_time[purchase_style] = [
        time / num_customers for time in purchase_style_cumulative_time[purchase_style]
    ]

# Define custom labels for the purchase styles
custom_labels = {
    0: "Low Gas",
    1: "Half Tank",
    2: "Cheapest Gas",
}

# Perform t-tests and calculate variance for both expenses and time
purchase_styles = list(purchase_style_expenses.keys())

# Initialize lists to store t-test results and variances for display
t_test_results = []
variance_results = []

for i in range(len(purchase_styles)):
    for j in range(i + 1, len(purchase_styles)):
        style_i = purchase_styles[i]
        style_j = purchase_styles[j]
        
        # Perform t-test for average daily expenses
        t_expenses, p_expenses = stats.ttest_ind(purchase_style_expenses[style_i], purchase_style_expenses[style_j])
        
        # Perform t-test for average time spent
        t_time, p_time = stats.ttest_ind(purchase_style_time_spent[style_i], purchase_style_time_spent[style_j])
        
        # Variance for expenses and time spent
        var_expenses_i = np.var(purchase_style_expenses[style_i])
        var_expenses_j = np.var(purchase_style_expenses[style_j])

        var_time_i = np.var(purchase_style_time_spent[style_i])
        var_time_j = np.var(purchase_style_time_spent[style_j])

        # Append results to the lists
        t_test_results.append((
            custom_labels.get(style_i, f"Style {style_i}"),
            custom_labels.get(style_j, f"Style {style_j}"),
            t_expenses, p_expenses, t_time, p_time
        ))

        variance_results.append((
            custom_labels.get(style_i, f"Style {style_i}"),
            custom_labels.get(style_j, f"Style {style_j}"),
            var_expenses_i, var_expenses_j, var_time_i, var_time_j
        ))

# Plotting the data
chart.figure(figsize=(10, 6))

# Plot average daily expenses
for purchase_style, avg_expenses in purchase_style_expenses.items():
    x = list(range(len(avg_expenses)))
    label = custom_labels.get(purchase_style, f"Style {purchase_style}")
    chart.plot(x, avg_expenses, marker='o', label=label)

# Format y-axis to display in dollars for expenses
def format_dollars(x, _):
    return f'${x:.2f}'

chart.gca().yaxis.set_major_formatter(FuncFormatter(format_dollars))
chart.title("Average Daily Expenses by Purchase Style")
chart.xlabel("Days")
chart.ylabel("Average Daily Expenses (USD)")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig('results/output_expenses.png')

# Plot cumulative time spent
chart.figure(figsize=(10, 6))

for purchase_style, avg_cumulative_time in purchase_style_cumulative_time.items():
    x = list(range(len(avg_cumulative_time)))
    label = custom_labels.get(purchase_style, f"Style {purchase_style}")
    chart.plot(x, avg_cumulative_time, marker='s', linestyle='--', label=label)

chart.title("Cumulative Time Spent Travelling to Purchase Gas")
chart.xlabel("Days")
chart.ylabel("Cumulative Time Spent (Minutes)")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig('results/output_cumulative_time.png')

# Display t-test results in a table format
chart.figure(figsize=(10, 6))
chart.axis('off')

# Create a table for the t-test results
table_data = [["Comparison", "T-stat (Expenses)", "P-value (Expenses)", "T-stat (Time)", "P-value (Time)"]]
for result in t_test_results:
    table_data.append([f"{result[0]} vs {result[1]}", f"{result[2]:.3f}", f"{result[3]:.3f}", f"{result[4]:.3f}", f"{result[5]:.3f}"])

# Plot the table for t-tests
table = chart.table(cellText=table_data, loc='center', cellLoc='center', colWidths=[0.3, 0.2, 0.2, 0.2, 0.2])
table.auto_set_font_size(False)
table.set_fontsize(10)
table.auto_set_column_width([0, 1, 2, 3, 4])

chart.title("T-Test Results: Expenses and Time")
chart.tight_layout()
chart.savefig('results/output_t_test_results.png')

# Display variance results in a table format
chart.figure(figsize=(10, 6))
chart.axis('off')

# Create a table for the variance results
table_data = [["Comparison", "Variance (Expenses)", "Variance (Time)"]]
for result in variance_results:
    table_data.append([f"{result[0]} vs {result[1]}", f"{result[2]:.3f}", f"{result[4]:.3f}"])

# Plot the table for variance results
table = chart.table(cellText=table_data, loc='center', cellLoc='center', colWidths=[0.3, 0.3, 0.3])
table.auto_set_font_size(False)
table.set_fontsize(10)
table.auto_set_column_width([0, 1, 2])

chart.title("Variance Results: Expenses and Time")
chart.tight_layout()
chart.savefig('results/output_variance_results.png')
