//tentative version, maybe c++ is better than struct for classes


#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include "cJSON.h"
#include "mytime.h"
#include "mSim.h"
#include "mymath.h"


enum PREFERENCE_T
{
    HABITUAL, //default
    EFFICIENT, 
    BRAND
};

// customer 
struct Customer 
{
    char *name;
    double budget;
    PREFERENCE_T preference; 
};

// product 
struct Product
{
    int productId;
    char *name;
    char* seller;
    double price;
    double adjustedPrice;
    double quantity;
};

struct Wallet
{
    Customer* owner;
    Product* product;
    double capacity;
};

struct ProductStatistics
{
    int productId;
    double averagePrice;
    double minPrice;
    double maxPrice;
};



ProductStatistics gasStats;

//support function to duplicate a string to all lowercase
char* change_to_lower(char* str)
{
    if (str == NULL) return NULL;    
    char* lower_str = (char*)malloc(strlen(str) + 1);
    if (lower_str == NULL) return NULL;
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower((unsigned char)str[i]);
    lower_str[strlen(str)] = '\0';
    return lower_str;
}

//translate a string to preference type
PREFERENCE_T string_to_preference(char* str)
{
    char* temp = change_to_lower(str);
    PREFERENCE_T preference;
    if (strcmp(temp, "habitual") == 0)
        preference = HABITUAL;
    else if (strcmp(temp, "efficient") == 0)
        preference = EFFICIENT;
    else if (strcmp(temp, "brand") == 0)
        preference = BRAND;
    else
    {
        int random;
        random = rand() % 3;
        preference = random;
    }
    
    return preference;
}

//translate a preference type to string
const char* preference_to_string(PREFERENCE_T preference)
{
    static const char* preferenceNames[] = {"Habitual", "Efficient", "Brand"};
    if (preference >= HABITUAL && preference <= BRAND)
        return preferenceNames[preference];
    else 
        return "Uknown";
}

// Function to parse the customers.json file
Customer* parse_customers(const char *filename, int *customer_count) 
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        fprintf(stderr, "Error parsing JSON\n");
        free(data);
        return NULL;
    }
    
    *customer_count = cJSON_GetArraySize(json);
    Customer *customers = (Customer *)malloc(*customer_count * sizeof(Customer));
    
    for (int i = 0; i < *customer_count; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        customers[i].name = strdup(cJSON_GetObjectItem(item, "name")->valuestring);
        customers[i].budget = cJSON_GetObjectItem(item, "budget")->valuedouble;
        customers[i].preference = string_to_preference(cJSON_GetObjectItem(item, "preference")->valuestring);
    }
    
    cJSON_Delete(json);
    free(data);
    return customers;
}

// Function to print a customer
void print_customer(Customer* customer)
{
    printf("Name: %s, Budget: %.2f, Preference: %s\n",
               customer->name, customer->budget, preference_to_string(customer->preference));
}

// Function to parse the products.json file 
Product *parse_products(const char *filename, int *product_count) 
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        fprintf(stderr, "Error parsing JSON\n");
        free(data);
        return NULL;
    }
    
    *product_count = cJSON_GetArraySize(json);
    Product *products = (Product *)malloc(*product_count * sizeof(Product));
    
    for (int i = 0; i < *product_count; i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        products[i].productId = cJSON_GetObjectItem(item, "ProductId")->valueint;
        products[i].name = strdup(cJSON_GetObjectItem(item, "name")->valuestring);
        products[i].seller = strdup(cJSON_GetObjectItem(item, "seller")->valuestring);
        products[i].price = cJSON_GetObjectItem(item, "price")->valuedouble;
        products[i].adjustedPrice = products[i].adjustedPrice;
        products[i].quantity = cJSON_GetObjectItem(item, "quantity")->valueint;
    }
    
    cJSON_Delete(json);
    free(data);
    return products;
}

// Function that prints a product
void print_product(Product* product)
{
    printf("Name: %s, Baseline Price: %.2f, Adjusted Price: %.2f, Quantity: %.2f, ProductId: %d, Sold at: %s\n",
               product->name, product->price, product->adjustedPrice, product->quantity, product->productId, product->seller);
}

//function to create a wallet, customer begins with 0 items
Wallet* create_wallet(Customer* c, Product* p, int cap)
{
    Wallet* returnWallet = (Wallet*)malloc(sizeof(Wallet));
    returnWallet->owner = c;
    returnWallet->product = (Product*)malloc(sizeof(Product));
    memcpy(returnWallet->product, p, sizeof(Product));
    // by default 0 items
    returnWallet->product->quantity = 0;
    //set capacity
    returnWallet->capacity = cap;
    return returnWallet;
}

void free_wallet(Wallet* w)
{
    free(w->product);
    w->product = NULL;
    free(w);
    w = NULL;
}

// random price adjustment factor
void set_adjusted_price(Product* product)
{
    double random = random_bell_curve(gasStats.minPrice, gasStats.maxPrice, gasStats.averagePrice);
    product->adjustedPrice = random;
    print_product(product);
}

float calculate_purchase_probability(Wallet* wallet, Product* product)
{
    if (wallet->product->productId != product->productId)
    {
        printf("The product does not match the wallet\n");
        return 1;
    }
    float probability;

    
}

int consume_product(Wallet* consumer, int quantity)
{
    if (consumer->product->quantity < quantity)
    {
        printf("Attempting to consume more than is available\n");
        return 1;
    }
    else
    {
        consumer->product->quantity -= quantity;
        printf("%s consumed %d of %s\n", consumer->owner->name, quantity, consumer->product->name);
        return 0;
    }
}

int purchase_amount(Wallet* wallet, double price)
{
    int space = wallet->capacity - wallet->product->quantity;
    if (space <= 0)
        return 0;
    double affordable_amount = wallet->owner->budget / price;
    affordable_amount = space < affordable_amount ? space : affordable_amount;
    double sensitivity = price > gasStats.averagePrice ? 0.5 : 1.0;
    int amount = affordable_amount * sensitivity;
    return amount;
}

int purchase_product(Wallet* wallet, Product* product, int hour)
{
    if (wallet->product->productId != product->productId)
    {
        printf("The product does not match the wallet\n");
        return 1;
    }
    double gasPrice = product->adjustedPrice;
    int quantity = purchase_amount(wallet, gasPrice);
    double cost = quantity * gasPrice;

    if (cost > wallet->owner->budget || quantity == 0)
    {
        printf("The amount requested is out of budget or not needed \n");
        return 1;
    }
    else
    {
        wallet->owner->budget -= cost;
        wallet->product->quantity += quantity;
        printf("Purchased %d %s from %s at $%.2f per unit. Remaining budget: $%.2f\n",
           quantity, product->name ,product->seller, gasPrice, wallet->owner->budget);
        return 0;
    }
}

void restock_product(Product* product, int quantity)
{
    product->quantity += quantity;
}

// Main function to execute the program
int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s customers.json products.json\n", argv[0]);
        return 1;
    }
    int i;
    //srand(1);
    srand((unsigned int)time(NULL));
    int customer_count, product_count;
    Customer *customers = parse_customers(argv[1], &customer_count);
    Product *products = parse_products(argv[2], &product_count);
    
    if (!customers || !products) {
        fprintf(stderr, "Failed to parse JSON files\n");
        return 1;
    }


    gasStats.productId = 1;
    double average = 0.0, min = DBL_MAX, max = DBL_MIN;
    for (i = 0; i < product_count; i++) 
    {
        if (products[i].productId == 1)
        {
            average += products[i].price;
            min = min > products[i].price ? products[i].price : min;
            max = max < products[i].price ? products[i].price : max;
        }
    }
    gasStats.averagePrice = average / (double) product_count;
    gasStats.minPrice = min;
    gasStats.maxPrice = max;
    // Print parsed data
    printf("Customers:\n");
    for (i = 0; i < customer_count; i++) 
        print_customer(&customers[i]);
    
    printf("\nProducts:\n");
    for (i = 0; i < product_count; i++) 
        set_adjusted_price(&products[i]);
    
    
    Wallet* customer_wallet = create_wallet(customers, products, 15);
    printf("\nNew Wallet: %s, %s, %d\n", customer_wallet->owner->name, customer_wallet->product->name, customer_wallet->product->quantity, customer_wallet->capacity);
    //create a new time object
    Time* t = initialize_default_time();
    printf("Start of day:\n");
    //pass through a 24 hour cycle
    for (i = 0; i < MAX_HOURS; i++)
    {
        printf("\n");
        print_time(t);
        //random price adjustment every 2 hours
        int random = rand() % product_count;
        if (i % 2 == 0)
        {
            printf("Random price adjustment\n");
            set_adjusted_price(&products[random]);
        }
        increment_time(t, 1, HOUR);
        purchase_product(customer_wallet, &products[random], i);
        if (i >= 9 && i <= 17)
        {
            consume_product(customer_wallet, rand() % 2);
        }
    }


    // Free allocated memory
    for (int i = 0; i < customer_count; i++) {
        free(customers[i].name);
    }
    for (int i = 0; i < product_count; i++) {
        free(products[i].name);
    }
    
    free(customers);
    free(products);

    return 0;
}
