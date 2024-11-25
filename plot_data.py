import json
import matplotlib
import matplotlib.pyplot as chart

with open("simulation_results.json", "r") as f:
    data = json.load(f)

chart.figure(figsize=(10,6))

for customer in data["customers"]:
    purchase_style = customer["purchase_style"]
    avg_cost_per_gallon = customer["average_cost_per_gallon"]

    x = list(range(len(avg_cost_per_gallon)))

    chart.plot(x, avg_cost_per_gallon, marker='o',  label=f"Style {purchase_style}")

chart.title("Average Cost Per Gallon by Purchase Style")
chart.xlabel("Index")
chart.ylabel("Average Cost Per Gallon")
chart.legend(title="Purchase Style")
chart.grid(True)
chart.tight_layout()
chart.savefig("output.png")