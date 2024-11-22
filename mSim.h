#ifndef MSIM_H
#define MSIM_H


typedef enum PREFERENCE_T PREFERENCE_T;
typedef struct Customer Customer;
typedef struct Product Product;
typedef struct Wallet Wallet;
typedef struct ProductStatistics ProductStatistics;

char* change_to_lower(char*);
PREFERENCE_T string_to_preference(char*);
const char* preference_to_string(PREFERENCE_T);
Customer* parse_customers(const char *, int *);
void print_customer(Customer*);
Product* parse_products(const char *, int *);
void print_product(Product*);
Wallet* create_wallet(Customer*, Product*, int);
void free_wallet(Wallet*);
void set_adjusted_price(Product*);
float calculate_purchase_probability(Wallet*, Product*);
int consume_product(Wallet*, int);
int purchase_amount(Wallet*, double);
int purchase_product(Wallet*, Product*, int);
void restock_product(Product*, int);

#endif