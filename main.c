#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE* file;

typedef struct Agent {
    char name[100];
    int id;
    struct Agent* left;
    struct Agent* right;
    int commission;
} Agent;

Agent* createAgent(char* name, int id) {
    Agent* newAgent = (Agent*)malloc(sizeof(Agent));
    if (newAgent == NULL) {
        printf("\t\tMemory allocation failed!\n");
        exit(1);
    }
    strcpy(newAgent->name, name);
    newAgent->id = id;
    newAgent->left = NULL;
    newAgent->right = NULL;
    newAgent->commission = 0;
    return newAgent;
}

void freeAgent(Agent* agent) {
    if (agent == NULL) {
        return;
    }
    freeAgent(agent->left);
    freeAgent(agent->right);
    free(agent);
}

void saveHierarchyToFile(Agent* agent, FILE* file, int level) {
    if (agent == NULL) {
        return;
    }
    for (int i = 0; i < level; i++) {
        fprintf(file, "\t");
    }
    fprintf(file, "%s (ID: #%d, Commission: %d)\n", agent->name, agent->id, agent->commission);

    saveHierarchyToFile(agent->left, file, level + 1);
    saveHierarchyToFile(agent->right, file, level + 1);
}

Agent* findAgent(Agent* agent, char* name, int id) {
    if (agent == NULL) {
        return NULL;
    }
    if (strcasecmp(agent->name, name) == 0 && agent->id == id) {
        return agent;
    }

    Agent* found = findAgent(agent->left, name, id);
    if (found == NULL) {
        found = findAgent(agent->right, name, id);
    }
    return found;
}

Agent* loadHierarchyFromFile(FILE* file) {
    char line[100];
    if (fgets(line, sizeof(line), file) == NULL) {
        return NULL;
    }

    char name[100];
    int id, commission;
    sscanf(line, "%s %d %d", name, &id, &commission);

    Agent* agent = createAgent(name, id);
    agent->commission = commission;

    agent->left = loadHierarchyFromFile(file);
    agent->right = loadHierarchyFromFile(file);

    return agent;
}

int isIdTaken(Agent* root, int id) {
    if (root == NULL) {
        return 0;
    }
    if (root->id == id) {
        return 1;
    }
    return isIdTaken(root->left, id) || isIdTaken(root->right, id);
}

void hireEmployee(Agent* root) {
    char search[50];
    int employeeId;

    printf("\n\t     Who's Hiring the Employee/s? (Enter Name): ");
    scanf("%s", search);
    printf("\t     Enter ID: #");
    scanf("%d", &employeeId);

    Agent* hiringAgent = findAgent(root, search, employeeId);

    if (hiringAgent == NULL) {
        printf("\n\t     ERROR! %s NOT FOUND in the List of Employees.\n", search);
        return;
    }

    if (hiringAgent->left != NULL && hiringAgent->right != NULL) {
        printf("\n\t     %s has already hired TWO Employees.", hiringAgent->name);
        printf("\n\t     Cannot hire employees anymore because\n\t     he/she has already reached the limit.\n");
        return;
    }

    printf("\n\t     How many Employees would %s Hire? (1 or 2): ", hiringAgent->name);
    int choice;
    scanf("%d", &choice);

    if (choice < 1 || choice > 2) {
        printf("\n\t\t     INVALID NUMBER of Employees.\n");
        printf("\t     An employee can only hire ONE TO TWO employees.");
        printf("\n\t\t     Please enter  1 or 2 only.");
        return;
    }

    if (hiringAgent->left == NULL) {
        char left[50];
        int leftId;
        printf("\n\t\t     Enter Name of Employee 1 (Left): ");
        scanf("%s", &left);
        do {
            printf("\t\t     Enter ID for Employee 1: #");
            scanf("%d", &leftId);
            if (isIdTaken(root, leftId)) {
                printf("\t\t     ERROR! ID #%d is already taken. Please enter a different ID.\n", leftId);
            }
        } while (isIdTaken(root, leftId));
        hiringAgent->left = createAgent(left, leftId);
        hiringAgent->commission += 100;
        printf("\t\t     %s with the ID of #%d is OFFICIALLY HIRED by %s!\n", left, leftId, hiringAgent->name);
        choice--;
    }

   if (choice > 0) {
        if (choice == 1 && hiringAgent->right == NULL) {
        char right[50];
        int rightId;
        printf("\n\t\t     Enter name of Employee 2 (Right): ");
        scanf("%s", &right);
        do {
            printf("\t\t     Enter ID for Employee 2: #");
            scanf("%d", &rightId);
            if (isIdTaken(root, rightId)) {
                printf("\t\t     ERROR! ID #%d is already taken. Please enter a different ID.\n", rightId);
            }
        } while (isIdTaken(root, rightId));
        hiringAgent->right = createAgent(right, rightId);
        hiringAgent->commission += 100;
        printf("\t\t     %s with the ID of #%d is OFFICIALLY HIRED by %s!\n", right, rightId, hiringAgent->name);

    } else {
        printf("\n\t\t     %s already hired one employee.", hiringAgent->name);
        printf("\n\t\t     He/she can only hire ONE more employee.\n");
      }
    }

    FILE* file = fopen("HierarchyStructure.txt", "w");
    if (file == NULL) {
        printf("\t     ERROR: Unable to Open File.\n");
        return;
    }
    saveHierarchyToFile(root, file, 0);
    fclose(file);
    printf("\n\t\t     Hierarchy Saved to 'HierarchyStructure.txt'.\n");
}

Agent* removeEmployee(Agent* root, const char* name, int id, int* isFound) {
    if (root == NULL) {
        return NULL;
    }

    if (strcasecmp(root->name, name) == 0 && root->id == id) {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            *isFound = 1;
            return NULL;
        } else {
            *isFound = 1;
            return root;
        }
    }

    root->left = removeEmployee(root->left, name, id, isFound);
    root->right = removeEmployee(root->right, name, id, isFound);
    return root;
}

void handleRemoveEmployee(Agent* root) {
    char name[50];
    int id;
    int isFound = 0;

    printf("\n\t     Enter the Name of the Employee to Remove: ");
    scanf("%s", name);
    printf("\t     Enter ID of the Employee: #");
    scanf("%d", &id);

    Agent* finding = findAgent(root, name, id);

    if (finding == NULL) {
        printf("\n\t     ERROR! Employee '%s' with ID %d NOT FOUND in the Hierarchy.\n", name, id);
    } else if (finding == root) {
        printf("\n\t\t     ERROR! CANNOT REMOVE MANAGER.\n");
    } else if (finding->left != NULL || finding->right != NULL) {
        printf("\n\t     ERROR! CANNOT REMOVE EMPLOYEE %s with ID #%d because he/she has Subordinate/s.\n", name, id);
    } else {
        root = removeEmployee(root, name, id, &isFound);
        printf("\n\t   Employee '%s' with the ID #%d REMOVED SUCCESSFULLY!\n", name, id);

        FILE* file = fopen("HierarchyStructure.txt", "w");
        if (file == NULL) {
            printf("\t     ERROR! Cannot open file.\n");
            return;
        }
        saveHierarchyToFile(root, file, 0);
        fclose(file);
        printf("\n\t     Hierarchy Updated and Saved to 'Hierarchy.txt'.\n");
    }
}

void searchEmployee(Agent* root) {
    char name[50];
    int id;
    printf("\n\t     Enter the Name of the Employee to Search: ");
    scanf("%s", name);
    printf("\t     Enter ID of the Employee: #");
    scanf("%d", &id);

    Agent* employee = findAgent(root, name, id);
    if (employee == NULL) {
        printf("\n\t     ERROR! Employee '%s' with ID #%d NOT FOUND in the List of Employees.\n", name, id);
        return;
    }

    if (employee == root) {
        printf("\n\t     Employee '%s' with the ID #%d is the MANAGER with a COMMISSION of %d.\n", root->name, root->id, root->commission);
    } else {
        printf("\n\t     Employee '%s' FOUND with the ID #%d and a COMMISSION of %d.\n", employee->name, employee->id, employee->commission);
    }

    if (employee->left != NULL) {
        if (employee->right != NULL) {
            printf("\t\t     Employees Hired: %s (ID: #%d) and %s (ID: #%d)\n", employee->left->name, employee->left->id, employee->right->name, employee->right->id);
        } else {
            printf("\t\t     Employee Hired: %s (ID: #%d)\n", employee->left->name, employee->left->id);
        }
    } else if (employee->right != NULL) {
        printf("\t\t     Employee Hired: %s (ID: #%d)\n", employee->right->name, employee->right->id);
    } else {
        printf("\t\t     No Employees Hired.\n");
    }
}

void displayHierarchy(FILE* file) {
    printf("\n\n");
    printf("\t\tCALL CENTER AGENCY EMPLOYEE HIERARCHY\n\n");

    file = fopen("HierarchyStructure.txt", "r");

    char content[1000];

    if (file != NULL) {
        while (fgets(content, 1000, file)) {
            printf("\t\t%s", content);
        }
        fclose(file);
    } else {
        printf("\t     ERROR: Unable to Open File.\n");
    }
}


void displayMenu() {
    printf("\n\n\t   +-------------------------------------------------+\n");
    printf("\t   |                                                 |\n");
    printf("\t   |              CALL CENTER AGENCY                 |\n");
    printf("\t   |                                                 |\n");
    printf("\t   +-------------------------------------------------+\n");
    printf("\t   |                                                 |\n");
    printf("\t   |     1. Display Hierarchy                        |\n");
    printf("\t   |     2. Hire Employee                            |\n");
    printf("\t   |     3. Remove Employee                          |\n");
    printf("\t   |     4. Search Employee                          |\n");
    printf("\t   |     5. Exit                                     |\n");
    printf("\t   |                                                 |\n");
    printf("\t   +-------------------------------------------------+\n");
}

int main() {
    int choice;
    Agent* root = NULL;

    FILE* file = fopen("HierarchyStructure.txt", "r");
    if (file != NULL) {
        root = loadHierarchyFromFile(file);
        fclose(file);
    }

    char rootName[50];
    int rootID;
    printf("\n\t         Enter name for the Manager: ");
    scanf("%s", rootName);
    printf("\t         Enter ID for the Manager: #");
    scanf("%d", &rootID);
    root = createAgent(rootName, rootID);

    file = fopen("HierarchyStructure.txt", "w");
    if (file != NULL) {
        saveHierarchyToFile(root, file, 0);
        fclose(file);
    }

    while (1) {
        displayMenu();
        printf("\n\t\t   Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayHierarchy(file);
                break;
            case 2:
                hireEmployee(root);
                break;
            case 3:
                handleRemoveEmployee(root);
                break;
            case 4:
                searchEmployee(root);
                break;
            case 5:
                printf("\n\t\tThank you for participating!");
                printf("\n\t\t   Exiting the program...\n");
                freeAgent(root);
                exit(0);
            default:
                printf("\n\t\tInvalid choice. Please try again.\n");
        }
    }
    return 0;
}
