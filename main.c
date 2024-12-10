#include <stdio.h>
#include "headers/customer.h"
#include "headers/owner.h"

int customerMenu();
int ownerMenu();

int main(void) {
    int mode = -1;

    while (1) {
        printf("[1] Customer\n[2] Owner\n[3] Exit program\n");
        scanf("%d", &mode);

        if (mode == 1) customerMenu();
        else if (mode == 2) ownerMenu();
        else if (mode == 3) break;
        else printf("Invalid option. Please try again.\n");
    }
    printf("Exit\n");
    return 0;
}

int customerMenu() {
    int mode = -1;

    while (1) {
        printf("Customer Menu\n[1] Find Products\n[2] View Carts\n[3] Exit menu\n");
        scanf("%d", &mode);

        if (mode == 1) {}
        else if (mode == 2) {}
        else if (mode == 3) break;
        else printf("Invalid option. Please try again.\n");
            
    }
    return 0;
}

int ownerMenu() {
    int mode = -1;

    while (1) {
        printf("Owner Menu\n[1] Report\n[2] CRUD\n[3] Restock\n[4] Logging\n[5] Exit menu\n");
        scanf("%d", &mode);

        if (mode == 1) {}
        else if (mode == 2) {}
        else if (mode == 3) {}
        else if (mode == 4) {}
        else if (mode == 5) break;
            
        else printf("Invalid option. Please try again.\n");
            
    }
    return 0;
}
