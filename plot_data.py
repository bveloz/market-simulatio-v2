import json
import matplotlib.pyplot as plot

with open("simulation_results.json", "r") as f:
    data = json.load(f)

plot.figure(figsize=(10,6))

for customer in data["customers"]:
    purchase_style = customer["purchase_style"]
    avg_cost_per_gallon = customer["average_cost_per_gallon"]

