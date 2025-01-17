#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define NUM_CUSTOMERS 1500
#define NUM_DAYS 60

#define GAS_TANK_CAPACITY 15.0
#define DAILY_TRAVEL_HOURS 2.0
#define GAS_CONSUMPTION_RATE 2.0
#define MAX_GAS_STATIONS 10
#define RESULTS_FOLDER "results/"
#define JSON_FILE "simulation_results.json"
#define FULL_PATH RESULTS_FOLDER JSON_FILE

// Gas station struct
typedef struct {
    double base_price;
    double adjusted_price;
    int travel_time_from_a;
    int travel_time_from_b;
} GasStation;

// Customer struct
typedef struct {
    double gas_tank_quantity;
    int purchase_style;
    double total_expenses_per_day[NUM_DAYS];
    double total_cumulative_expenses[NUM_DAYS];
    double average_cost_per_gallon[NUM_DAYS];
    double cumulative_cost_per_gallon[NUM_DAYS];
    double total_time_spent[NUM_DAYS];
} Customer;

// HELPER FUNCTIONS
// Function to generate random prices based on a bell curve
double bell_curve_price(double base_price) {
    double random_value = ((double)rand() / RAND_MAX - 0.5) * 2.0;
    double factor = exp(-pow(random_value, 2));
    return base_price + random_value * factor * base_price * 0.1;
}

// GAS AND CUSTOMER FUNCTIONS
// Function to simulate a single customer's trip
void simulate_customer_trip(Customer *customer, GasStation *stations, int num_stations, int day) {
    double gas_used = DAILY_TRAVEL_HOURS * GAS_CONSUMPTION_RATE;
    double current_gas = customer->gas_tank_quantity - gas_used;
    if (current_gas < 0) {
        printf("Error: Gas consumption exceeds tank capacity.\n");
        exit(EXIT_FAILURE);
    }
    for (int trip = 0; trip < 2; trip++)   
    {
        // Determine if refueling is needed
        if ((customer->purchase_style == 0 && current_gas <= 3) || // Low tank (3 gallons)
            (customer->purchase_style == 1 && current_gas < GAS_TANK_CAPACITY / 2) || // Half tank
            (customer->purchase_style == 2)) { // Low cost check
            int selected_station = -1;
            double min_cost = 1e9;
            double min_time = 1e9;
            int trip_travel_time;
            for (int i = 0; i < num_stations; i++) {
                
                stations[i].adjusted_price = bell_curve_price(stations[i].base_price);

                int temp_travel_time = trip % 2 == 0 ? stations[i].travel_time_from_a : stations[i].travel_time_from_b; //alternate trip times between trips
                double cost = stations[i].adjusted_price * (GAS_TANK_CAPACITY - current_gas);
                double time = temp_travel_time;

                if (customer->purchase_style == 0 && time < min_time) {
                    min_time = time;
                    trip_travel_time = temp_travel_time;
                    selected_station = i;
                } else if (customer->purchase_style == 1 && time < min_time) {
                    min_time = time;
                    trip_travel_time = temp_travel_time;
                    selected_station = i;
                } else if (customer->purchase_style == 2 && cost < min_cost) {
                    min_cost = cost;
                    trip_travel_time = temp_travel_time;
                    selected_station = i;
                }
            }

            // Refuel at the selected station
            if (selected_station != -1) {
                customer->gas_tank_quantity = GAS_TANK_CAPACITY;
                customer->total_expenses_per_day[day] += stations[selected_station].adjusted_price * (GAS_TANK_CAPACITY - current_gas);
                customer->total_time_spent[day] += trip_travel_time;
            }
        }
        else
        {
            customer->gas_tank_quantity = current_gas;
        }
    }
    

    // Record daily metrics
    customer->average_cost_per_gallon[day] = customer->total_expenses_per_day[day] / (GAS_TANK_CAPACITY - current_gas);
}

// Function to generate random gas stations
void generate_gas_stations(GasStation *stations, int num_stations) {
    for (int i = 0; i < num_stations; i++) {
        stations[i].base_price = 2.0 + ((double)rand() / RAND_MAX) * 1.0;
        stations[i].travel_time_from_a = rand() % 16 + 5; // 5 to 20 minutes
        stations[i].travel_time_from_b = rand() % 16 + 5; // 5 to 20 minutes
    }
}

// Main simulation function
void run_simulation() {
    srand(time(NULL));
    
    Customer customers[NUM_CUSTOMERS];
    FILE *file = fopen(FULL_PATH, "w");

    if (!file) {
        printf("Error: Could not open file for writing.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "{\n  \"customers\": [\n");

    // Initialize customers
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customers[i].gas_tank_quantity = GAS_TANK_CAPACITY;
        customers[i].purchase_style = i % 3;
        memset(customers[i].total_expenses_per_day, 0, sizeof(customers[i].total_expenses_per_day));
        memset(customers[i].total_cumulative_expenses, 0, sizeof(customers[i].total_cumulative_expenses));
        memset(customers[i].average_cost_per_gallon, 0, sizeof(customers[i].average_cost_per_gallon));
        memset(customers[i].total_time_spent, 0, sizeof(customers[i].total_time_spent));
    }

    // Run simulation
    for (int i = 0; i < NUM_CUSTOMERS; i++) 
    {
        // generate stations on a per customer basis
        int rand_num_stations = rand() % MAX_GAS_STATIONS;
        GasStation stations[rand_num_stations];
        generate_gas_stations(stations, rand_num_stations);

        // simulate customer on the same gas Station configuration for 90 days
        for(int day = 0; day < NUM_DAYS; day++)
        {
            simulate_customer_trip(&customers[i], stations, rand_num_stations, day);
            double sum = 0; 
            if (day > 0)
                sum = customers[i].total_cumulative_expenses[day - 1];
            sum += customers[i].total_expenses_per_day[day];
            customers[i].total_cumulative_expenses[day] = sum;
            sum = 0;
            if (day > 0)
                sum = customers[i].cumulative_cost_per_gallon[day - 1];
            sum += customers[i].average_cost_per_gallon[day];
            customers[i].cumulative_cost_per_gallon[day] = sum;
        }
    }

    // Write results to JSON file
    for (int i = 0; i < NUM_CUSTOMERS; i++) 
    {
        fprintf(file, "    {\n      \"customer_id\": %d,\n      \"purchase_style\": %d,\n      \"total_expenses_per_day\": [",
                i, customers[i].purchase_style);
        for (int day = 0; day < NUM_DAYS; day++) 
        {
            fprintf(file, "%.2f%s", customers[i].total_expenses_per_day[day], day == NUM_DAYS - 1 ? "" : ", ");
        }
        fprintf(file, "],\n      \"total_cumulative_expenses\": [");
        for (int day = 0; day < NUM_DAYS; day++) 
        {
            fprintf(file, "%.2f%s", customers[i].total_cumulative_expenses[day], day == NUM_DAYS - 1 ? "" : ", ");
        }
        fprintf(file, "],\n      \"average_cost_per_gallon\": [");
        for (int day = 0; day < NUM_DAYS; day++) 
        {
            fprintf(file, "%.2f%s", customers[i].average_cost_per_gallon[day], day == NUM_DAYS - 1 ? "" : ", ");
        }
        fprintf(file, "],\n      \"cumulative_cost_per_gallon\": [");
        for (int day = 0; day < NUM_DAYS; day++) 
        {
            fprintf(file, "%.2f%s", customers[i].cumulative_cost_per_gallon[day], day == NUM_DAYS - 1 ? "" : ", ");
        }
        fprintf(file, "],\n      \"total_time_spent\": [");
        for (int day = 0; day < NUM_DAYS; day++) 
        {
            fprintf(file, "%.2f%s", customers[i].total_time_spent[day], day == NUM_DAYS - 1 ? "" : ", ");
        }
        fprintf(file, "]\n    }%s\n", i == NUM_CUSTOMERS - 1 ? "" : ",");
    }

    fprintf(file, "  ]\n}\n");
    fclose(file);
    printf("Simulation complete. Results saved to %s\n", FULL_PATH);
}

// Entry point
int main() {
    run_simulation();
    return 0;
}
