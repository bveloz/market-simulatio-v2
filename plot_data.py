import json
import matplotlib.pyplot as chart
from matplotlib.ticker import FuncFormatter

# Load the data from the JSON file
with open("simulation_results.json", "r") as f:
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
chart.savefig("output_expenses.png")

# Plot cumulative time spent
chart.figure(figsize=(10, 6))

for purchase_style, avg_cumulative_time in purchase_style_cumulative_time.items():
    x = list(range(len(avg_cumulative_time)))
    label = custom_labels.get(purchase_style, f"Style {purchase_style}")
    chart.plot(x, avg_cumulative_time, marker='s', linestyle='--', label=label)

chart.title("Cumulative Time Spent by Purchase Style")
chart.xlabel("Days")
chart.ylabel("Cumulative Time Spent (Minutes)")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig("output_cumulative_time.png")
