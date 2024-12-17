#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAXPRODUCT 100
#define MAXCART 10

typedef struct
{
    char name[20];
    char description[50];
    float price;
    int stock;
    int max;
    int min;
} Product;

typedef struct 
{
    char name[20];
    float price;
    int amount;
} Cart;

typedef struct 
{
    float Income;
    int SellAmount;
    int RestockAmount;
    char MostSell[30];
    int MostSellAmout; 
} Report;

Product productCache[MAXPRODUCT];
int productCount = 0;

Cart carts[MAXCART];
int cartCount = 0;

Report currentReport = {0, 0, 0, "", 0};

int customerMenu();
int ownerMenu();

Product product_create(const char* name, const char* description, float price, int stock, int maxStock, int minThreshold);
void product_init();
Product* product_search(char* keyword, int* size);
void product_reducestock();
void product_view();
void product_restock(int id, int count);
void product_restockFromFile();
void product_insert(Product product);
Product product_getProduct();
void product_save();
bool product_autoRestock(char* productName);
void product_delete(int id);
int product_getIndex(char* name);
int find_product_id_by_name(const char *name);

void cart_insert(char* name, float price, int amount);
void cart_insertfromFile();
void cart_view(float* returnPrice);
void cart_reset();

float coupon_checker(float price, char* text);
void coupon_insert(char* coupon, float percent, char* date);

void log_restock(char* name, int stock);
void log_purchase(float totalCost);

void CRUD_create();
void CRUD_read();
void CRUD_update();
void CRUD_delete();

void UpdateReport();
void GetReport();

int main(void) {
    product_init();
    int mode = -1;

    while (1) {
        printf("[1] Customer\n[2] Owner\n[3] Exit program\n");
        scanf("%d", &mode);

        if (mode == 1) customerMenu();
        else if (mode == 2) ownerMenu();
        else if (mode == 3) break;
        else printf("Invalid option. Please try again.\n");
    }
    UpdateReport();
    printf("Exit\n");
    product_save();
    return 0;
}

int customerMenu() {
    int mode = -1;

    while (1) {
        printf("Customer Menu\n[1] Find Products\n[2] View Carts\n[3] Add to cart from file\n[4] Exit menu\n");
        scanf("%d", &mode);

        if (mode == 1) {
            int choice;
            char keyword[20];

            while (1) {
                printf("Search: ");
                scanf("%s", keyword);

                int returnSize;
                Product* searchList = product_search(keyword, &returnSize);

                if (returnSize == 0) {
                    printf("No result found, 1 to re-search or 0 to cancel: ");
                    scanf("%d", &choice);

                    if (choice == 1) {
                        continue;
                    } else if (choice == 0) {
                        printf("Returning to the menu.\n");
                        break;
                    } else {
                        printf("Invalid choice. Returning to the menu.\n");
                        break;
                    }
                } else {
                    for (int i = 0; i < returnSize; i++) {
                        printf("[%d] %s\n", i + 1, searchList[i].name);
                    }

                    printf("Select a product (0 to cancel): ");
                    scanf("%d", &choice);

                    if (choice == 0) {
                        printf("Returning to the menu.\n");
                        break;
                    } else if (choice > 0 && choice <= returnSize) {
                        printf("Product: %s\nDetail: %s\nPrice: %g\nStock: %d\n", searchList[choice-1].name, searchList[choice-1].description, searchList[choice-1].price, searchList[choice-1].stock);
                        int count;
                        printf("Quantity (0 for cancel): ");
                        scanf("%d", &count);

                        if(count == 0){
                            printf("Back to searching...\n");
                            continue;
                        }
                        else if(count > 0) cart_insert(searchList[choice-1].name, searchList[choice-1].price, count);
                        else if(count < 0){
                            printf("\nInvalid input, try again\n");
                            continue;
                        }
                    } else {
                        printf("Invalid selection. Returning to the menu.\n");
                        break;
                    }
                }
                free(searchList);
            }
        }
        else if (mode == 2){
            int choice;
            float totalPrice;
            cart_view(&totalPrice);
            printf("[1] Pay\n[2] Clear\n");
            scanf("%d", &choice);

            if(choice == 1){
                int use;
                printf("[1] Have coupon\n[2] No\n");
                scanf("%d", &use);

                if(use == 1){
                    char coupon[10];
                    int confirm;

                    scanf("%s", coupon);

                    float newPrice = coupon_checker(totalPrice, coupon);
                    printf("You saved %.2f from using %s\nTotal cost: %.2f\n", totalPrice-newPrice, coupon, newPrice);
                    printf("[1] Confirm\n[2] Cancel\n");
                    scanf("%d", &confirm);

                    if(confirm == 1){
                        printf("Successfully place order\n");
                        currentReport.Income += newPrice; // Add to total income
                        currentReport.SellAmount += cartCount; // Add to total items sold
                        log_purchase(newPrice);
                        product_reducestock(); 
                    }else if(confirm == 2) break;
                    else {
                        printf("Invalid Choice!\n");
                        continue;
                    }

                }else if(use == 2){
                    printf("Successfully place order\n");
                    for (int i = 0; i < cartCount; i++) currentReport.SellAmount += carts[i].amount; // Add to total items sold
                    currentReport.Income += totalPrice; // Add to total income
                    log_purchase(totalPrice);
                    product_reducestock();
                }else {
                    printf("Invalid choice! return");
                    continue;
                }
            }else if(choice == 2){
                cart_reset();

            }else {
                printf("Invalid choice.\n");
                continue;
            }
        } 
        else if(mode == 3) cart_insertfromFile();
        else if (mode == 4) break;
        else {
            printf("Invalid option. Please try again.\n");
        }
    }

    return 0;
}

int ownerMenu() {
    int mode = -1;

    while (1) {
        printf("Owner Menu\n[1] Report\n[2] CRUD\n[3] Restock\n[4] Discount\n[5] Exit menu\n");
        scanf("%d", &mode);

        if (mode == 1) {
            GetReport();
        }
        else if (mode == 2) {
            int action;
            printf("CRUD Menu\n[1] Create\n[2] Read\n[3] Update\n[4] Delete\n>>");
            scanf("%d", &action);

            if(action == 1)CRUD_create();
            else if(action == 2) CRUD_read();
            else if(action == 3) CRUD_update();
            else if(action == 4) CRUD_delete();
            else {
                printf("Invalid input! try again\n");
                continue;
            }
        }
        else if (mode == 3) {
            int choice;

            printf("Restock Menu\n[1] Manual restock\n[2] Read from file\n>> ");
            scanf("%d", &choice);

            if(choice == 1){
                int id, count;
                product_view();

                printf("Choice (Number ID): ");
                scanf("%d", &id);

                if(id < 1 || id > productCount){
                    printf("\nInvalid choice!\n");
                    continue;
                }else {
                    printf("\nQuantity: ");
                    scanf("%d", &count);

                    if(count < 0){
                        printf("Invalid Input!\n");
                        continue;
                    }else product_restock(id-1, count);
                }
            }else if(choice == 2) product_restockFromFile();
            else {
                printf("Invalid choice! Try again\n");
            }
        }
        else if (mode == 4) {
            printf("Create new discount\n");

            char coupon[10], expiration_date[11];
            float percent;

            printf("code discount date: ");
            scanf("%s %f %s", coupon, &percent,expiration_date);

            if(percent < 0){
                printf("Invalid input!");
                break;
            }else coupon_insert(coupon, percent, expiration_date);
        }
        else if (mode == 5) break;
            
        else printf("Invalid option. Please try again.\n");
            
    }
    return 0;
}

Product product_create(const char* name, const char* description, float price, int stock, int maxStock, int minThreshold) {
    Product p;
    strncpy(p.name, name, sizeof(p.name) - 1);
    strncpy(p.description, description, sizeof(p.description) - 1);
    p.price = price;
    p.stock = stock;
    p.max = maxStock;
    p.min = minThreshold;
    return p;
}

void product_init(void) {
    FILE *inv;
    inv = fopen("./db/inventory.csv", "r");

    if (inv == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[100];
    while(fgets(line, sizeof(line), inv)){
        char name[20], description[50];
        float price;
        int stock, max, min;

        int items = sscanf(line, "%49[^,],%199[^,],%f,%d,%d,%d", name, description, &price, &stock, &max, &min);

        if(items == 6){
            productCache[productCount] = product_create(name, description, price, stock, max, min);
            productCount++;
        }
    }

    fclose(inv); 
}

Product* product_search(char* keyword, int* size) {
    int c = 0;
    
    Product* result = malloc(MAXPRODUCT * sizeof(Product));

    for (int i = 0; i < productCount; i++) {
        if (strstr(productCache[i].name, keyword) != NULL || 
            strstr(productCache[i].description, keyword) != NULL) {
            result[c++] = productCache[i];
        }
    }
    *size = c;

    return result;
}

void product_reducestock() {
    static int mostSellIndex = -1;

    for (int i = 0; i < cartCount; i++) {
        bool restockSuccess = product_autoRestock(carts[i].name);

        for (int j = 0; j < productCount; j++) {
            if (strcmp(carts[i].name, productCache[j].name) == 0) {
                // Check if restock succeeded or not
                if (!restockSuccess && productCache[j].stock < carts[i].amount) {
                    printf("Not enough stock for %s. Available: %d, Required: %d\n",
                           productCache[j].name, productCache[j].stock, carts[i].amount);
                    carts[i].amount = productCache[j].stock; // Reduce cart amount to what's available
                    productCache[j].stock = 0; // All stock exhausted
                } else if (productCache[j].stock >= carts[i].amount) {
                    productCache[j].stock -= carts[i].amount; // Deduct stock normally
                }

                // Update most sold product
                if (mostSellIndex == -1 || carts[i].amount > currentReport.MostSellAmout) {
                    mostSellIndex = j;
                    strncpy(currentReport.MostSell, productCache[j].name, sizeof(currentReport.MostSell) - 1);
                    currentReport.MostSellAmout = carts[i].amount;
                }

                break;
            }
        }
    }
    cart_reset();
}


void product_view(){
    for(int i = 0; i < productCount; i++){
        if(productCache[i].stock < productCache[i].min) printf("%d. %s (In stock: %d) [Below minimum]\n", i+1, productCache[i].name, productCache[i].stock); 
        else printf("%d. %s (In stock: %d)\n", i+1, productCache[i].name, productCache[i].stock);
    }
}

void product_restock(int id, int count){
    productCache[id].stock += count;
    currentReport.RestockAmount += count;

    if(productCache[id].stock > productCache[id].max) productCache[id].stock = productCache[id].max;

    printf("Successfully restock for %s, In stock %d\n", productCache[id].name, productCache[id].stock);
    log_restock(productCache[id].name, productCache[id].stock);
}

void product_save(){
    FILE *inv;
    inv = fopen("db/inventory.csv", "w");

    if(inv == NULL) printf("Error open file\n");

    for (int i = 0; i < productCount; i++) {
        fprintf(inv, "%s,%s,%.2f,%d,%d,%d\n",
            productCache[i].name,
            productCache[i].description,
            productCache[i].price,
            productCache[i].stock,
            productCache[i].max,
            productCache[i].min);
    }
    fclose(inv);

    printf("Product data saved successfully.\n");
}

bool product_autoRestock(char* productName) {
    FILE *warehouse, *tempFile;
    char line[100];
    char warehouseProductName[50];
    int warehouseQuantity;

    bool productFound = false;
    bool canRestock = false;

    warehouse = fopen("db/warehouse.csv", "r");
    tempFile = fopen("db/temp.csv", "w");

    if (warehouse == NULL || tempFile == NULL) {
        printf("Error opening file.\n");
        return false;
    }

    while (fgets(line, sizeof(line), warehouse)) {
        if (sscanf(line, "%49[^,], %d", warehouseProductName, &warehouseQuantity) != 2) {
            fprintf(tempFile, "%s", line);
            continue;
        }

        if (strcmp(warehouseProductName, productName) == 0) {
            productFound = true;

            for (int id = 0; id < MAXPRODUCT; id++) {
                if (strcmp(productCache[id].name, productName) == 0) {
                    int neededStock = productCache[id].min - productCache[id].stock;

                    if (neededStock > 0) {
                        int restockAmount;

                        if (warehouseQuantity >= neededStock)restockAmount = neededStock; 
                        else restockAmount = warehouseQuantity;

                        if (restockAmount > 0) {
                            productCache[id].stock += restockAmount;
                            warehouseQuantity -= restockAmount;
                            canRestock = true;
                            printf("[Auto Restock] %s: Added %d units, Current Stock: %d\n", 
                                   productName, restockAmount, productCache[id].stock);
                        }
                    }
                    break;
                }
            }
        }

        fprintf(tempFile, "%s, %d\n", warehouseProductName, warehouseQuantity);
    }

    fclose(warehouse);
    fclose(tempFile);
    remove("db/warehouse.csv");
    rename("db/temp.csv", "db/warehouse.csv");

    if (!productFound) {
        printf("Product '%s' not found in warehouse.\n", productName);
        return false;
    }

    return canRestock;
}

void product_insert(Product product) {
    if (productCount < MAXPRODUCT) {
        productCache[productCount++] = product;
    } else {
        printf("Product list is full. Cannot insert more products.\n");
    }
}

Product product_getProduct() {
    int id;

    while (1) {
        for (int i = 0; i < productCount; i++) {
            printf("[%d] %s\n", i + 1, productCache[i].name);
        }
        printf("Enter your choice: ");
        scanf("%d", &id);

        if (id > 0 && id <= productCount) {
            return productCache[id - 1];
        } else {
            printf("Invalid choice! Please try again.\n");
        }
    }
}

int product_getIndex(char* name){
    for(int i = 0; i < productCount; i++){
        if(strcmp(name, productCache[i].name) == 0) return i;
    }
    return -1;
}

void product_delete(int id){
    char deleted_product[20];
    strcpy(deleted_product, productCache[id].name);

    for (int i = id; i < productCount - 1; i++) {
        productCache[i] = productCache[i + 1];
    }

    memset(productCache[productCount - 1].name, 0, sizeof(productCache[productCount - 1].name));
    memset(productCache[productCount - 1].description, 0, sizeof(productCache[productCount - 1].description));
    productCache[productCount-1].price = 0;
    productCache[productCount-1].stock = 0;
    productCache[productCount-1].max = 0;
    productCache[productCount-1].min = 0;

    productCount--; 
    printf("%s has been deleted successfully.\n", deleted_product);
}

int find_product_id_by_name(const char *name) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(productCache[i].name, name) == 0) return i;
    }
    return -1;
}

void product_restockFromFile() {
    FILE *restockFile = fopen("command/restock.csv", "r");
    if (restockFile == NULL) {
        printf("Cannot open file\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), restockFile)) {
        char productName[20];
        int restockCount;

        if (sscanf(line, "%19[^,], %d", productName, &restockCount) == 2) {
            int productID = find_product_id_by_name(productName);
            if (productID != -1) {
                product_restock(productID, restockCount);
            } else {
                printf("Warning: Product '%s' not found in database.\n", productName);
            }
        } else {
            printf("Warning: Invalid input: %s\n", line);
        }
    }

    fclose(restockFile);
}

void cart_insert(char* name, float price, int amount) {
    strncpy(carts[cartCount].name, name, sizeof(carts[cartCount].name) - 1);
    carts[cartCount].price = price * amount;
    carts[cartCount].amount = amount;

    cartCount++;

    printf("Sucessfully add %s to cart!\n", name);
}

float find_product_price_by_name(const char *name) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(productCache[i].name, name) == 0) return productCache[i].price;
    }
    return -1;
}

void cart_insertfromFile() {
    FILE *purchase = fopen("command/purchase.csv", "r");
    if (purchase == NULL) {
        printf("Error: Cannot open file 'command/purchase.csv'.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), purchase)) {
        char productName[20];
        int quantity;

        if (sscanf(line, "%19[^,], %d", productName, &quantity) == 2) {
            float productPrice = find_product_price_by_name(productName);
            if (productPrice >= 0) cart_insert(productName, productPrice, quantity);
            else printf("Warning: Product '%s' not found in product cache.\n", productName);
        } else printf("Warning: Invalid line format: %s", line);
    }

    fclose(purchase);
}

void cart_view(float* returnPrice){
    float totalPrice = 0.0;
    for(int i = 0; i < cartCount; i++){
        printf("[%d] Product: %s x%d for $%g\n", i+1, carts[i].name, carts[i].amount, carts[i].price);
        totalPrice += carts[i].price;
    }
    printf("Total cost: %.2f\n", totalPrice);
    *returnPrice = totalPrice;
}

void cart_reset() {
    cartCount = 0;

    for (int i = 0; i < MAXCART; i++) {
        carts[i].name[0] = '\0';
        carts[i].price = 0;
        carts[i].amount = 0;
    }
}

float coupon_checker(float price, char* text) {
    FILE *coupon;
    coupon = fopen("db/coupon.csv", "r");

    if (coupon == NULL) {
        printf("Error opening coupon file.\n");
        return price;
    }

    char line[256];
    char coupon_code[50];
    float discount_percentage;
    char expiration_date[11];

    time_t now;
    struct tm tm_now;

    time(&now);

    tm_now = *localtime(&now);
    while (fgets(line, sizeof(line), coupon)) {
        int items = sscanf(line, "%49[^,],%f,%10s", coupon_code, &discount_percentage, expiration_date);

        if (items == 3) {
            if (strcmp(coupon_code, text) == 0) {
                struct tm tm_expiry = {0};
                sscanf(expiration_date, "%4d-%2d-%2d", &tm_expiry.tm_year, &tm_expiry.tm_mon, &tm_expiry.tm_mday);
                tm_expiry.tm_year -= 1900;
                tm_expiry.tm_mon -= 1;

                time_t expiry_time = mktime(&tm_expiry);

                if (difftime(expiry_time, now) >= 0) {
                    float discount = price * (discount_percentage / 100);
                    float new_price = price - discount;
                    fclose(coupon);
                    return new_price;
                }
                else {
                    printf("Coupon expired.\n");
                    fclose(coupon);
                    return price;
                }
            }
        }
    }

    printf("Coupon not found.\n");
    fclose(coupon);
    return price; 
}

void coupon_insert(char* coupon, float percent, char* date){
    FILE *couponf;

    couponf = fopen("db/coupon.csv", "a");

    if (couponf == NULL) {
        printf("Error: Could not open coupon file for writing.\n");
        return;
    }

    fseek(couponf, 0, SEEK_END);
    long file_size = ftell(couponf);

    if (file_size > 0) fprintf(couponf, "\n");

    fprintf(couponf, "%s,%.2f,%s", coupon, percent, date);
    fclose(couponf);

    printf("Coupon added successfully: %s, %.2f, %s\n", coupon, percent, date);
}

void log_restock(char* name, int stock){
    FILE* log;
    log = fopen("log/log.txt", "a");

    if(log == NULL) {
        printf("Error open file!\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fseek(log, 0, SEEK_END);
    long file_size = ftell(log);
    if(file_size > 0) fprintf(log, "\n");

    fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d Restock %s. In stock %d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, name, stock);

    fclose(log);
}

void log_purchase(float totalCost){
    FILE* log;
    log = fopen("log/log.txt", "a");

    if(log == NULL) {
        printf("Error open file!\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fseek(log, 0, SEEK_END);
    long file_size = ftell(log);
    if(file_size > 0) fprintf(log, "\n");

    fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d Customer buy ",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    for (int i = 0; i < cartCount; i++) {
        fprintf(log, "%s x%d", carts[i].name, carts[i].amount);
        if (i < cartCount - 1) fprintf(log, ", ");
    }

    fprintf(log, " for %.2f", totalCost);

    fclose(log);
}

void CRUD_create() {
    char name[20], detail[50];
    float price;
    int stock, max, min;

    printf("Enter (name, detail, price, stock, max, min): ");
    scanf("%s", name);
    getchar();
    fgets(detail, sizeof(detail), stdin);
    detail[strcspn(detail, "\n")] = 0;

    scanf("%f %d %d %d", &price, &stock, &max, &min);

    Product product = product_create(name, detail, price, stock, max, min);
    product_insert(product);

    printf("Successfully insert %s to database\n", name);
}

void CRUD_read() {
    Product product = product_getProduct();

    printf("Product: %s\n", product.name);
    printf("Description: %s\n", product.description);
    printf("Price: %.2f\n", product.price);
    printf("In Stock: %d\n", product.stock);
    printf("Max Stock: %d\n", product.max);
    printf("Minimum Threshold: %d\n", product.min);
}

void CRUD_update() {
    Product product = product_getProduct();
    int choice;
    int index = product_getIndex(product.name);

    printf("%s", product.name);

    printf("What to update?\n[1] Name\n[2] Description\n[3] Price\n[4] Stock\n[5] Max stock\n[6] Minimum threshold\n>> ");
    scanf("%d", &choice);

    if(choice == 1){
        char name[20];
        scanf("%s", name);
        strcpy(productCache[index].name, name);
        printf("Successfully change product name to %s\n", name);

    }else if(choice == 2){
        char detail[50];
        scanf("%s", detail);
        strcpy(productCache[index].description, detail);
        printf("Successfully change product description to %s\n", detail);
    }else if(choice == 3){
        float price;
        scanf("%f", &price);
        productCache[index].price = price;
        printf("Successfully change product price to %.2f\n", price);
    }else if(choice == 4){
        int stock;
        scanf("%d", &stock);
        productCache[index].stock = stock;
        printf("Successfully change product stock to %d\n", stock);
    }else if(choice == 5){
        int max;
        scanf("%d", &max);
        productCache[index].max = max;
        printf("Successfully change product maximum stock to %d\n", max);
    }else if(choice == 6){
        int min;
        scanf("%d", &min);
        productCache[index].min = min;
        printf("Successfully change product minimum threshold to %d\n", min);
    }else {
        printf("Invalid input! try again\n");
        return;
    }
}

void CRUD_delete() {
    int id;
    for(int i = 0; i < productCount; i++){
        printf("[%d] %s\n", i+1, productCache[i].name);
    }
    
    printf("Remove choice: ");
    scanf("%d", &id);
    
    if(id < 1 || id > productCount){
        printf("Invalid choice! Try again.\n");
    }else product_delete(id-1);
}

void UpdateReport() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char filename[40];

    sprintf(filename, "Report/%02d-%02d-%04d.csv", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    FILE *reportFile = fopen(filename, "w");
    if (reportFile == NULL) {
        printf("Error creating report file.\n");
        return;
    }

    fprintf(reportFile, "Income,%.2f\n", currentReport.Income);
    fprintf(reportFile, "SellAmount,%d\n", currentReport.SellAmount);
    fprintf(reportFile, "RestockAmount,%d\n", currentReport.RestockAmount);
    fprintf(reportFile, "MostSell,%s\n", currentReport.MostSell);
    fprintf(reportFile, "MostSellAmount,%d\n", currentReport.MostSellAmout);

    fclose(reportFile);
    printf("Report updated successfully: %s\n", filename);

    memset(&currentReport, 0, sizeof(currentReport));
}

void GetReport(){

    FILE *inventory;
    char name[30];
    printf("What date you want to see report? (dd-mm-yyyy) : ");
    scanf(" %s", name);

    char filename[30] = "Report/";

    strcat(filename, name);
    strcat(filename, ".csv");

    printf(filename);
    printf("\n");

    if((inventory = fopen(filename, "r")) == NULL) {
        puts("File could not be opened, May be wrong formatt for file name");
    }else {

        printf("-----------------------------\n");

        char line[256];
        int isFirstLine = 1;
        char Topic[20];
        char data[20];

        while (fgets(line, sizeof(line), inventory)) {
            if (isFirstLine) {
                isFirstLine = 0;
                continue;
            }

            sscanf(line, "%[^,],%s", &Topic, &data);
            printf("%-14s %-9s\n", Topic, data);
        }
    }

    fclose(inventory);

    printf("-----------------------------\n");
}