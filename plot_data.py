import json
import matplotlib.pyplot as chart
from matplotlib.ticker import FuncFormatter

# Load the data from the JSON file
with open("simulation_results.json", "r") as f:
    data = json.load(f)

# Initialize a dictionary to store the total expenses grouped by purchase style
purchase_style_expenses = {}
purchase_style_time_spent = {}

# Group total expenses by purchase style and accumulate the values
for customer in data["customers"]:
    purchase_style = customer["purchase_style"]
    total_expenses = customer["total_cumulative_expenses"]
    total_time_spent = customer["total_time_spent"]

    if purchase_style not in purchase_style_expenses:
        purchase_style_expenses[purchase_style] = [0] * len(total_expenses)
        purchase_style_time_spent[purchase_style]  = [0] * len(total_time_spent)

    # Accumulate the expenses for each day
    for day in range(len(total_expenses)):
        purchase_style_expenses[purchase_style][day] += total_expenses[day]
        purchase_style_time_spent[purchase_style][day] += total_time_spent[day]

# Calculate the average daily expenses for each purchase style
for purchase_style in purchase_style_expenses:
    purchase_style_expenses[purchase_style] = [
        expense / len(data["customers"]) for expense in purchase_style_expenses[purchase_style]
    ]
    purchase_style_time_spent[purchase_style] = [
        time / len(data["customers"]) for time in purchase_style_time_spent[purchase_style]
    ]



# Define custom labels for the purchase styles
custom_labels = {
    "style_1": "Purchase on 3 gallons or less",
    "style_2": "Purchase on 7 gallons or less",
    "style_3": "Lowest cost purchase",
    # Add more styles as needed
}


# Plotting the data
chart.figure(figsize=(20, 12))

# Plot the average daily expenses for each purchase style
for purchase_style, avg_expenses in purchase_style_expenses.items():
    x = list(range(len(avg_expenses)))
    chart.plot(x, avg_expenses, marker='o', label=f"Style {purchase_style}")

# Plot the average daily time spent for each purchase style
for purchase_style, avg_time in purchase_style_time_spent.items():
    x = list(range(len(avg_time)))
    chart.plot(x, avg_time, marker='s', linestyle='--', label=f"Time - Style {purchase_style}")

# Plotting the data: Expenses
chart.figure(figsize=(10, 6))
for purchase_style, avg_expenses in purchase_style_expenses.items():
    x = list(range(len(avg_expenses)))
    label = custom_labels.get(purchase_style, purchase_style)
    chart.plot(x, avg_expenses, marker='o', label=label)

chart.title("Average Daily Expenses by Purchase Style")
chart.xlabel("Day Index")
chart.ylabel("Average Daily Expenses")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig("average_expenses.png")

# Plotting the data: Time Spent
chart.figure(figsize=(10, 6))
for purchase_style, avg_time in purchase_style_time_spent.items():
    x = list(range(len(avg_time)))
    chart.plot(x, avg_time, marker='s', label=f"Style {purchase_style}")

chart.title("Average Daily Time Spent by Purchase Style")
chart.xlabel("Days")
chart.ylabel("Average Daily Time Spent")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig("average_time_spent.png")
