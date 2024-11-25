#!/bin/bash

make run

if [ $? -eq 0 ]; then
    if [ -f "simulation_results.json" ]; then
        python3 plot_data.py
    else
        echo "Simulation results not found"
    fi
else
    echo "Error running simulation"
fi