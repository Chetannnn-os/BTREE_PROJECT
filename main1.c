#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BATCH_SIZE 50
#define NAME_SIZE 100
#define EXP_DATE 9
#define CONTACT_SIZE 15
#define MAX_SUPPLIERS 50

typedef struct ExpiryDate {
    int day;
    int month;
    int year;
} ExpiryDate;

typedef struct SupplierData {
    unsigned long Supplier_ID;
    char Supplier_Name[NAME_SIZE];
    unsigned int Quantity_of_stock_bysupplier;
    char Contact[CONTACT_SIZE];
} SupplierData;

typedef struct MedicationData {
    unsigned long Medication_ID;
    char Medicine_Name[NAME_SIZE];
    unsigned int Quantity_in_stock;
    unsigned int Price_per_Unit;
    int Reorderlevel;

    struct {
        char Batch[BATCH_SIZE];
        ExpiryDate Expiration_Date;
        int Total_sales;
    } Batch_details;

    struct SupplierTree *Suppliers;
} MedicationData;

typedef struct BTreeMedication {
    MedicationData *keys;                   
    int order;                              
    int cursize;                             
    struct BTreeMedication **children;      
    bool isLeaf;                             
    struct BTreeMedication *parent;          
} BTreeMedication;

typedef struct MedicationNameNode {
    char Medicine_Name[NAME_SIZE];
    unsigned long Medication_ID;  // Reference to the original medication
} MedicationNameNode;

typedef struct BTreeMedicationName {
    MedicationNameNode *keys;
    int order;
    int cursize;
    struct BTreeMedicationName **children;
    bool isLeaf;
    struct BTreeMedicationName *parent;
} BTreeMedicationName;

typedef struct BTree {
    BTreeMedication *root;
    int order;
} BTree;

typedef struct BTreeByName {
    BTreeMedicationName *root;
    int order;
} BTreeByName;

typedef struct SupplierTree {
    SupplierData *keys;                      
    int order;                                
    int cursize;                              
    struct SupplierTree **children;          
    bool isLeaf;                              
    struct SupplierTree *parent;             
} SupplierTree;

typedef struct MedicationExpiryNode {
    ExpiryDate expiry_date;       // Expiration date for sorting
    unsigned long Medication_ID;   // Reference to the original medication
} MedicationExpiryNode;

typedef struct BTreeMedicationExpiry {
    MedicationExpiryNode *keys;
    int order;
    int cursize;
    struct BTreeMedicationExpiry **children;
    bool isLeaf;
    struct BTreeMedicationExpiry *parent;
} BTreeMedicationExpiry;

typedef struct BTreeByExpiry {
    BTreeMedicationExpiry *root;
    int order;
} BTreeByExpiry;

// Function prototypes
void Initialise(BTree *medicationTree, BTreeByName *medicationNameTree, int ord);
BTreeMedication *create_Medication(int order);
BTreeMedicationName *create_MedicationName(int order);
SupplierTree *create_Supplier(int order);
bool CheckMEdicIDExist(unsigned long Medication_ID, BTreeMedication *root);
bool checkMedicNameExist(const char *name, BTreeMedicationName *root);
bool checkSuppID(unsigned long SupID, SupplierTree *supplierTree);
void splitMedicationNode(BTreeMedication *parent, int index, BTreeMedication *fullNode);
void splitMedicationNameNode(BTreeMedicationName *parent, int index, BTreeMedicationName *fullNode);
void splitSupplierNode(SupplierTree *parent, int index, SupplierTree *fullNode);
void addMedicationToNode(BTreeMedication *node, MedicationData med);
void addMedicationNameToNode(BTreeMedicationName *node, MedicationNameNode medName);
void addMedicationName(BTreeByName *nameTree, MedicationNameNode medName);
void addSupplier(SupplierTree **root, SupplierData supplier, int order);
void printMedication(MedicationData med);
void printMedicationWithExpiryStatus(MedicationData med, ExpiryDate today);
void printSuppliers(SupplierTree *suppliers);
void printAllMedications(BTreeMedication *root);
void AddNewMedication(BTree *medicationTree, BTreeByName *medicationNameTree);
void cleanupResources(BTree *medicationTree, BTreeByName *medicationNameTree);
int isLeapYear(int year);
int noOfDaysTillday(ExpiryDate date);
int difference(ExpiryDate today, ExpiryDate expiryDay);
bool Valid(int day, int month, int year);
void checkExpiredMedications(BTreeMedication *root, ExpiryDate today);
void searchMedicationById(BTreeMedication *root, unsigned long searchId);
void searchMedicationByName(BTreeMedicationName *nameRoot, BTreeMedication *medRoot, const char *searchName);
void searchMedicationBySupplier(BTreeMedication *root, unsigned long supplierId);
void searchMedication(BTree *medicationTree, BTreeByName *medicationNameTree);
BTreeMedicationExpiry *create_MedicationExpiry(int order);
int compareExpiryDates(ExpiryDate date1, ExpiryDate date2);
void splitMedicationExpiryNode(BTreeMedicationExpiry *parent, int index, BTreeMedicationExpiry *fullNode);
void addMedicationExpiryToNode(BTreeMedicationExpiry *node, MedicationExpiryNode medExpiry);
void addMedicationExpiry(BTreeByExpiry *expiryTree, MedicationExpiryNode medExpiry);
void InitialiseWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree, int ord);
void AddNewMedicationWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree);
void displayMedicationsByExpiry(BTreeMedicationExpiry *root, ExpiryDate today, int daysThreshold);
void cleanupExpiryTree(BTreeMedicationExpiry *root);
void cleanupResourcesWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree);
void listMedicationsExpiringWithinDays(BTreeByExpiry *expiryTree, BTreeMedication *medRoot, ExpiryDate today);
void trackSalesForAllMedications(BTreeMedication *root);
void trackSalesById(BTreeMedication *root, unsigned long medicationId);
void sales_Tracking(BTree *medicationTree);
void stock_Alerts(BTree *medicationTree);
void updateMedicationDetails(BTree *medicationTree);
void deleteMedicationById(BTreeMedication *root, unsigned long medication_id);
void deleteMedicationByName(BTreeMedicationName *root, unsigned long medication_id);
void deleteMedicationByExpiry(BTreeMedicationExpiry *root, unsigned long medication_id);
void deleteMedication(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree);
MedicationData* findMedicationById(BTreeMedication *root, unsigned long medId);
SupplierData* findSupplierById(SupplierTree *root, unsigned long supplierId);
bool deleteSupplierFromTree(SupplierTree **root, unsigned long supplierId);
void addSupplierToMedication(BTree *medicationTree);
void updateSupplierInformation(BTree *medicationTree);
void deleteSupplier(BTree *medicationTree);
void viewSuppliersForMedication(BTree *medicationTree);
void supplier_management(BTree *medicationTree);
void supplier_analysis(BTree *medicationTree);

// Ensure SupplierMedicationCount is defined before usage
typedef struct SupplierMedicationCount {
    unsigned long Supplier_ID;
    char Supplier_Name[NAME_SIZE];
    int unique_medication_count;
    double total_turnover;
} SupplierMedicationCount;

// Forward declaration to resolve implicit declaration warning
void collect_suppliers_from_tree(SupplierTree *supplier_tree, MedicationData med, 
                                SupplierMedicationCount suppliers[], int *supplier_count, int max_suppliers);

void Initialise(BTree *medicationTree, BTreeByName *medicationNameTree, int ord) {
    medicationTree->order = ord;
    medicationTree->root = create_Medication(ord);
    
    medicationNameTree->order = ord;
    medicationNameTree->root = create_MedicationName(ord);
}

BTreeMedication *create_Medication(int order) {
    BTreeMedication *temp = (BTreeMedication *)malloc(sizeof(BTreeMedication));
    if (temp != NULL) {
        temp->order = order;
        temp->cursize = 0;
        temp->keys = (MedicationData *)malloc((order - 1) * sizeof(MedicationData));
        temp->children = (BTreeMedication **)malloc(order * sizeof(BTreeMedication *));
        temp->isLeaf = true;
        temp->parent = NULL;

        for (int i = 0; i < order - 1; i++) {
            temp->keys[i].Medication_ID = 0;
            temp->keys[i].Medicine_Name[0] = '\0';
            temp->keys[i].Quantity_in_stock = 0;
            temp->keys[i].Price_per_Unit = 0;
            temp->keys[i].Reorderlevel = 0;
            temp->keys[i].Batch_details.Batch[0] = '\0';
            temp->keys[i].Batch_details.Expiration_Date.day = 0;
            temp->keys[i].Batch_details.Expiration_Date.month = 0;
            temp->keys[i].Batch_details.Expiration_Date.year = 0;
            temp->keys[i].Batch_details.Total_sales = 0; // Initialize Total_sales to zero
            temp->keys[i].Suppliers = NULL;
        }
    }
    return temp;
}

BTreeMedicationName *create_MedicationName(int order) {
    BTreeMedicationName *temp = (BTreeMedicationName *)malloc(sizeof(BTreeMedicationName));
    if (temp != NULL) {
        temp->order = order;
        temp->cursize = 0;
        temp->keys = (MedicationNameNode *)malloc((order - 1) * sizeof(MedicationNameNode));
        temp->children = (BTreeMedicationName **)malloc(order * sizeof(BTreeMedicationName *));
        temp->isLeaf = true;
        temp->parent = NULL;

        for (int i = 0; i < order - 1; i++) {
            temp->keys[i].Medicine_Name[0] = '\0';
            temp->keys[i].Medication_ID = 0;
        }
    }
    return temp;
}

SupplierTree *create_Supplier(int order) {
    SupplierTree *temp = (SupplierTree *)malloc(sizeof(SupplierTree));
    if (temp != NULL) {
        temp->order = order;
        temp->cursize = 0;
        temp->keys = (SupplierData *)malloc((order - 1) * sizeof(SupplierData));
        temp->children = (SupplierTree **)malloc(order * sizeof(SupplierTree *));
        temp->isLeaf = true;
        temp->parent = NULL;

        for (int i = 0; i < order - 1; i++) {
            temp->keys[i].Supplier_ID = 0;
            temp->keys[i].Supplier_Name[0] = '\0';
            temp->keys[i].Quantity_of_stock_bysupplier = 0;
            temp->keys[i].Contact[0] = '\0';
        }
    }
    return temp;
}

bool CheckMEdicIDExist(unsigned long Medication_ID, BTreeMedication *root) {
    if (root == NULL) return false; // If the tree is empty, the ID does not exist

    for (int i = 0; i < root->cursize; i++) {
        if (root->keys[i].Medication_ID == Medication_ID) {
            return true;
        }
    }

    // Recur for children
    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            if (CheckMEdicIDExist(Medication_ID, root->children[i])) {
                return true;
            }
        }
    }

    return false; // ID does not exist
}

bool checkMedicNameExist(const char *name, BTreeMedicationName *root) {
    if (root == NULL) return false;

    for (int i = 0; i < root->cursize; i++) {
        if (strcmp(root->keys[i].Medicine_Name, name) == 0) {
            return true;
        }
    }

    // Recur for children
    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            if (checkMedicNameExist(name, root->children[i])) {
                return true;
            }
        }
    }

    return false;
}

bool checkSuppID(unsigned long SupID, SupplierTree *supplierTree) {
    if (supplierTree == NULL) return true; // If the tree is empty, the ID does not exist

    for (int i = 0; i < supplierTree->cursize; i++) {
        if (supplierTree->keys[i].Supplier_ID == SupID) {
            return false;
        }
    }

    return true; // ID does not exist
}

void splitMedicationNode(BTreeMedication *parent, int index, BTreeMedication *fullNode) {
    int t = (fullNode->order + 1) / 2; // Calculate the split point

    // Create a new node to store the second half of keys
    BTreeMedication *newNode = create_Medication(fullNode->order);
    if (newNode == NULL) {
        printf("Memory allocation failed during node split\n");
        return;
    }
    
    newNode->isLeaf = fullNode->isLeaf;
    
    // Copy the last half of keys from fullNode to newNode
    for (int i = 0; i < fullNode->order - t; i++) {
        if (t + i < fullNode->cursize) {
            newNode->keys[i] = fullNode->keys[t + i];
            newNode->cursize++;
        }
    }

    // Copy the last half of children from fullNode to newNode (if not a leaf)
    if (!fullNode->isLeaf) {
        for (int i = 0; i < fullNode->order - t + 1; i++) {
            if (t + i <= fullNode->cursize) {
                newNode->children[i] = fullNode->children[t + i];
                if (newNode->children[i] != NULL) {
                    newNode->children[i]->parent = newNode;
                }
            }
        }
    }

    // Update the current size of fullNode
    fullNode->cursize = t - 1;

    // Shift children of parent to make space for newNode
    for (int i = parent->cursize; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    // Link newNode to parent
    parent->children[index + 1] = newNode;
    newNode->parent = parent;

    // Shift keys of parent to make space for the middle key of fullNode
    for (int i = parent->cursize - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }

    // Copy the middle key of fullNode to parent
    parent->keys[index] = fullNode->keys[t - 1];

    // Increment the number of keys in parent
    parent->cursize++;
}

void splitMedicationNameNode(BTreeMedicationName *parent, int index, BTreeMedicationName *fullNode) {
    int t = (fullNode->order + 1) / 2; // Calculate the split point

    // Create a new node to store the second half of keys
    BTreeMedicationName *newNode = create_MedicationName(fullNode->order);
    if (newNode == NULL) {
        printf("Memory allocation failed during name node split\n");
        return;
    }
    
    newNode->isLeaf = fullNode->isLeaf;
    
    // Copy the last half of keys from fullNode to newNode
    for (int i = 0; i < fullNode->order - t; i++) {
        if (t + i < fullNode->cursize) {
            newNode->keys[i] = fullNode->keys[t + i];
            newNode->cursize++;
        }
    }

    // Copy the last half of children from fullNode to newNode (if not a leaf)
    if (!fullNode->isLeaf) {
        for (int i = 0; i < fullNode->order - t + 1; i++) {
            if (t + i <= fullNode->cursize) {
                newNode->children[i] = fullNode->children[t + i];
                if (newNode->children[i] != NULL) {
                    newNode->children[i]->parent = newNode;
                }
            }
        }
    }

    // Update the current size of fullNode
    fullNode->cursize = t - 1;

    // Shift children of parent to make space for newNode
    for (int i = parent->cursize; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    // Link newNode to parent
    parent->children[index + 1] = newNode;
    newNode->parent = parent;

    // Shift keys of parent to make space for the middle key of fullNode
    for (int i = parent->cursize - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }

    // Copy the middle key of fullNode to parent
    parent->keys[index] = fullNode->keys[t - 1];

    // Increment the number of keys in parent
    parent->cursize++;
}

void splitSupplierNode(SupplierTree *parent, int index, SupplierTree *fullNode) {
    int t = (fullNode->order + 1) / 2; // Calculate the split point

    // Create a new node to store the second half of keys
    SupplierTree *newNode = create_Supplier(fullNode->order);
    if (newNode == NULL) {
        printf("Memory allocation failed during supplier node split\n");
        return;
    }
    
    newNode->isLeaf = fullNode->isLeaf;
    
    // Copy the last half of keys from fullNode to newNode
    for (int i = 0; i < fullNode->order - t; i++) {
        if (t + i < fullNode->cursize) {
            newNode->keys[i] = fullNode->keys[t + i];
            newNode->cursize++;
        }
    }

    // Copy the last half of children from fullNode to newNode (if not a leaf)
    if (!fullNode->isLeaf) {
        for (int i = 0; i < fullNode->order - t + 1; i++) {
            if (t + i <= fullNode->cursize) {
                newNode->children[i] = fullNode->children[t + i];
                if (newNode->children[i] != NULL) {
                    newNode->children[i]->parent = newNode;
                }
            }
        }
    }

    // Update the current size of fullNode
    fullNode->cursize = t - 1;

    // Shift children of parent to make space for newNode
    for (int i = parent->cursize; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    // Link newNode to parent
    parent->children[index + 1] = newNode;
    newNode->parent = parent;

    // Shift keys of parent to make space for the middle key of fullNode
    for (int i = parent->cursize - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }

    // Copy the middle key of fullNode to parent
    parent->keys[index] = fullNode->keys[t - 1];

    // Increment the number of keys in parent
    parent->cursize++;
}

void addMedicationToNode(BTreeMedication *node, MedicationData med) {
    int i = node->cursize - 1;

    if (node->isLeaf) {
        // Find the correct position for the new medication
        while (i >= 0 && med.Medication_ID < node->keys[i].Medication_ID) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        // Insert the new medication
        node->keys[i + 1] = med;
        node->cursize++;
    } else {
        // Find the child that will receive the new medication
        while (i >= 0 && med.Medication_ID < node->keys[i].Medication_ID) {
            i--;
        }
        i++;

        // If the child is full, split it
        if (node->children[i]->cursize == node->order - 1) {
            splitMedicationNode(node, i, node->children[i]);

            // After splitting, determine which child will receive the new medication
            if (med.Medication_ID > node->keys[i].Medication_ID) {
                i++;
            }
        }

        addMedicationToNode(node->children[i], med);
    }
}

void addMedicationNameToNode(BTreeMedicationName *node, MedicationNameNode medName) {
    int i = node->cursize - 1;

    if (node->isLeaf) {
        // Find the correct position for the new medication name
        while (i >= 0 && strcmp(medName.Medicine_Name, node->keys[i].Medicine_Name) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        // Insert the new medication name
        node->keys[i + 1] = medName;
        node->cursize++;
    } else {
        // Find the child that will receive the new medication name
        while (i >= 0 && strcmp(medName.Medicine_Name, node->keys[i].Medicine_Name) < 0) {
            i--;
        }
        i++;

        // If the child is full, split it
        if (node->children[i]->cursize == node->order - 1) {
            splitMedicationNameNode(node, i, node->children[i]);

            // After splitting, determine which child will receive the new medication name
            if (strcmp(medName.Medicine_Name, node->keys[i].Medicine_Name) > 0) {
                i++;
            }
        }

        addMedicationNameToNode(node->children[i], medName);
    }
}

void addMedicationName(BTreeByName *nameTree, MedicationNameNode medName) {
    if (nameTree->root == NULL) {
        nameTree->root = create_MedicationName(nameTree->order);
    }

    BTreeMedicationName *root = nameTree->root;

    if (root->cursize == nameTree->order - 1) {
        BTreeMedicationName *newRoot = create_MedicationName(nameTree->order);
        newRoot->isLeaf = false;
        newRoot->children[0] = root;

        splitMedicationNameNode(newRoot, 0, root);

        int i = (strcmp(medName.Medicine_Name, newRoot->keys[0].Medicine_Name) > 0) ? 1 : 0;
        addMedicationNameToNode(newRoot->children[i], medName);

        nameTree->root = newRoot;
    } else {
        addMedicationNameToNode(root, medName);
    }
}

void addSupplier(SupplierTree **root, SupplierData supplier, int order) {
    if (*root == NULL) {
        *root = create_Supplier(order);
    }

    SupplierTree *node = *root;

    if (node->cursize == node->order - 1) {
        // If the root is full, split it
        SupplierTree *newRoot = create_Supplier(order);
        newRoot->isLeaf = false;
        newRoot->children[0] = node;

        splitSupplierNode(newRoot, 0, node);

        int i = (supplier.Supplier_ID > newRoot->keys[0].Supplier_ID) ? 1 : 0;
        addSupplier(&(newRoot->children[i]), supplier, order);

        *root = newRoot;  // Update the root
    } else {
        // Insert into a non-full node
        int i = node->cursize - 1;
        if (node->isLeaf) {
            while (i >= 0 && supplier.Supplier_ID < node->keys[i].Supplier_ID) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }

            node->keys[i + 1] = supplier;
            node->cursize++;
        } else {
            while (i >= 0 && supplier.Supplier_ID < node->keys[i].Supplier_ID) {
                i--;
            }
            i++;

            if (node->children[i]->cursize == node->order - 1) {
                splitSupplierNode(node, i, node->children[i]);

                if (supplier.Supplier_ID > node->keys[i].Supplier_ID) {
                    i++;
                }
            }

            addSupplier(&(node->children[i]), supplier, order);
        }
    }
}

void AddNewMedication(BTree *medicationTree, BTreeByName *medicationNameTree) {
    if (medicationTree->root == NULL) {
        medicationTree->root = create_Medication(medicationTree->order);
    }

    BTreeMedication *root = medicationTree->root;
    MedicationData newMed;
    MedicationNameNode newMedName;

    printf("Enter Medication ID: ");
    unsigned long newID;
    scanf("%lu", &newID);

    while (CheckMEdicIDExist(newID, medicationTree->root)) {
        printf("Medication ID already exists. Enter a new Medication ID: ");
        scanf("%lu", &newID);
    }
    newMed.Medication_ID = newID;
    newMedName.Medication_ID = newID;

    printf("Enter Medication Name: ");
    scanf("%s", newMed.Medicine_Name);
    strcpy(newMedName.Medicine_Name, newMed.Medicine_Name);

    printf("Enter Quantity in Stock: ");
    scanf("%u", &newMed.Quantity_in_stock);

    printf("Enter Price per Unit: ");
    scanf("%u", &newMed.Price_per_Unit);

    printf("Enter the Batch Name: ");
    scanf("%s", newMed.Batch_details.Batch);

    // Validate reorder level
    
        printf("Enter Reorder Level (between 10 and 100): ");
        scanf("%d", &newMed.Reorderlevel);
        if (newMed.Reorderlevel < 10 || newMed.Reorderlevel > 100) {
            printf("Invalid reorder level! Please enter a value between 10 and 100.\n");
            scanf("%d", &newMed.Reorderlevel);
        }
    

    printf("Enter Expiration Date in DD-MM-YYYY\n");
    do {
        printf("Day: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.day);
        printf("Month: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.month);
        printf("Year: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.year);

        if (!Valid(newMed.Batch_details.Expiration_Date.day, 
                   newMed.Batch_details.Expiration_Date.month, 
                   newMed.Batch_details.Expiration_Date.year)) {
            printf("Invalid date! Please enter a valid date.\n");
        }
    } while (!Valid(newMed.Batch_details.Expiration_Date.day, 
                    newMed.Batch_details.Expiration_Date.month, 
                    newMed.Batch_details.Expiration_Date.year));

    newMed.Batch_details.Total_sales = 0; // Initialize Total_sales to zero

    int numSuppliers;
    printf("Enter the number of suppliers: ");
    scanf("%d", &numSuppliers);

    newMed.Suppliers = NULL;  // Initialize supplier tree

    for (int j = 0; j < numSuppliers; j++) {
        SupplierData supplier;
        printf("Enter Supplier-%d ID: ", j + 1);
        scanf("%lu", &supplier.Supplier_ID);

        while (!checkSuppID(supplier.Supplier_ID, newMed.Suppliers)) {
            printf("Supplier ID already exists. Enter a new Supplier ID: ");
            scanf("%lu", &supplier.Supplier_ID);
        }

        printf("Enter Supplier-%d Name: ", j + 1);
        scanf("%s", supplier.Supplier_Name);

        printf("Enter Quantity Supplied by Supplier-%d: ", j + 1);
        scanf("%u", &supplier.Quantity_of_stock_bysupplier);
        newMed.Quantity_in_stock += supplier.Quantity_of_stock_bysupplier;

        printf("Enter Contact details (10 digits): ");
        scanf("%s", supplier.Contact);

        // Insert supplier correctly into SupplierTree
        addSupplier(&(newMed.Suppliers), supplier, medicationTree->order);
    }

    // Add to ID-based tree
    if (root->cursize == medicationTree->order - 1) {
        BTreeMedication *newRoot = create_Medication(medicationTree->order);
        newRoot->isLeaf = false;
        newRoot->children[0] = root;

        splitMedicationNode(newRoot, 0, root);

        int i = (newMed.Medication_ID > newRoot->keys[0].Medication_ID) ? 1 : 0;
        addMedicationToNode(newRoot->children[i], newMed);

        medicationTree->root = newRoot;
    } else {
        addMedicationToNode(root, newMed);
    }

    // Add to name-based tree
    addMedicationName(medicationNameTree, newMedName);
    
    printf("Medication added successfully!\n");
}

void printMedication(MedicationData med) {
    printf("\nMedication ID: %lu\n", med.Medication_ID);
    printf("Medication Name: %s\n", med.Medicine_Name);
    printf("Batch Name: %s\n", med.Batch_details.Batch);
    printf("Expiration Date: %02d-%02d-%04d\n", med.Batch_details.Expiration_Date.day, med.Batch_details.Expiration_Date.month, med.Batch_details.Expiration_Date.year);
    printf("Quantity in Stock: %u\n", med.Quantity_in_stock);
    printf("Price Per Unit: %u\n", med.Price_per_Unit);
    printf("Reorder Level: %d\n", med.Reorderlevel);
    printf("Total Sales: %d\n", med.Batch_details.Total_sales);
}

void printSuppliers(SupplierTree *suppliers) {
    if (suppliers == NULL) {
        printf("No suppliers available for this medication.\n");
        return;
    }

    for (int i = 0; i < suppliers->cursize; i++) {
        if (!suppliers->isLeaf) {
            printSuppliers(suppliers->children[i]);
        }

        printf("\nSupplier ID: %lu\n", suppliers->keys[i].Supplier_ID);
        printf("Supplier Name: %s\n", suppliers->keys[i].Supplier_Name);
        printf("Quantity Supplied: %u\n", suppliers->keys[i].Quantity_of_stock_bysupplier); // Ensure this prints correctly
        printf("Contact: %s\n", suppliers->keys[i].Contact);
    }

    if (!suppliers->isLeaf) {
        printSuppliers(suppliers->children[suppliers->cursize]);
    }
}

void printAllMedications(BTreeMedication *root) {
    if (root == NULL) return;

    // Traverse the left subtree
    for (int i = 0; i < root->cursize; i++) {
        if (!root->isLeaf) {
            printAllMedications(root->children[i]);
        }

        // Print the current medication
        printMedication(root->keys[i]);

        // Print the suppliers for the current medication
        printSuppliers(root->keys[i].Suppliers);
    }

    // Traverse the rightmost child
    if (!root->isLeaf) {
        printAllMedications(root->children[root->cursize]);
    }
}

void cleanupNameTree(BTreeMedicationName *root) {
    if (root == NULL) return;

    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            cleanupNameTree(root->children[i]);
        }
    }

    free(root->keys);
    free(root->children);
    free(root);
}

void cleanupResources(BTree *medicationTree, BTreeByName *medicationNameTree) {
    if (medicationTree != NULL && medicationTree->root != NULL) {
        // Free memory for the B-tree
        free(medicationTree->root->keys);
        free(medicationTree->root->children);
        free(medicationTree->root);
        medicationTree->root = NULL;
    }
    
    if (medicationNameTree != NULL && medicationNameTree->root != NULL) {
        // Free memory for the name-based B-tree
        cleanupNameTree(medicationNameTree->root);
        medicationNameTree->root = NULL;
    }
}

// Check if a year is a leap year
int isLeapYear(int year) {
    if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))
        return 1;
    return 0;
}

int noOfDaysTillday(ExpiryDate date) {
    // Month days accounting for leap years later
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Convert to days since Jan 1, 2000 (arbitrary reference point)
    int days = 0;
    
    // Add days for years
    for (int y = 2000; y < date.year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    
    // Add days for months in current year
    for (int m = 1; m < date.month; m++) {
        days += daysInMonth[m];
        if (m == 2 && isLeapYear(date.year)) {
            days += 1; // Add leap day if February in leap year
        }
    }
    
    // Add days in current month
    days += date.day;
    
    return days;
}

// Calculate the difference in days between two dates
int difference(ExpiryDate today, ExpiryDate expiryDay) {
    return noOfDaysTillday(expiryDay) - noOfDaysTillday(today);
}

// Validate if a given date is valid
bool Valid(int day, int month, int year) {
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Basic validation
    if (year < 2000 || month < 1 || month > 12 || day < 1)
        return false;
    
    // Check for February in leap years
    if (month == 2 && isLeapYear(year))
        return day <= 29;
    
    // Check against days in month
    return day <= daysInMonth[month];
}

void printMedicationWithExpiryStatus(MedicationData med, ExpiryDate today) {
    // Print basic medication info
    printMedication(med);
    
    // Calculate and print expiry status
    int daysToExpiry = difference(today, med.Batch_details.Expiration_Date);
    if (daysToExpiry < 0) {
        printf("Expiration Status: EXPIRED (%d days ago)\n", -daysToExpiry);
    } else if (daysToExpiry <= 30) {
        printf("Expiration Status: WARNING - EXPIRING SOON (%d days remaining)\n", daysToExpiry);
    } else {
        printf("Expiration Status: Valid (%d days remaining)\n", daysToExpiry);
    }
}

void checkExpiredMedications(BTreeMedication *root, ExpiryDate today) {
    if (root == NULL) return;

    // Traverse the left subtree
    for (int i = 0; i < root->cursize; i++) {
        if (!root->isLeaf) {
            checkExpiredMedications(root->children[i], today);
        }
        // Debug: Print medication ID and expiration date
        printf("Checking Medication ID: %lu, Expiry Date: %02d-%02d-%04d\n",
               root->keys[i].Medication_ID,
               root->keys[i].Batch_details.Expiration_Date.day,
               root->keys[i].Batch_details.Expiration_Date.month,
               root->keys[i].Batch_details.Expiration_Date.year);

        // Check if the medication is expired
        int daysToExpiry = difference(today, root->keys[i].Batch_details.Expiration_Date);
        printf("Days to Expiry: %d\n", daysToExpiry); // Debug: Print days to expiry

        if (daysToExpiry < 0) {
            printf("\n--- EXPIRED MEDICATION ---\n");
            printMedicationWithExpiryStatus(root->keys[i], today);
        } else if (daysToExpiry <= 30) {
            printf("\n--- EXPIRING SOON ---\n");
            printMedicationWithExpiryStatus(root->keys[i], today);
        }
    }

    // Traverse the rightmost child
    if (!root->isLeaf) {
        checkExpiredMedications(root->children[root->cursize], today);
    }
}

void searchMedicationById(BTreeMedication *root, unsigned long searchId) {
    if (root == NULL) return;
    
    int flag = 1;  // Flag to track if item was found
    
    // Search in current node
    for (int i = 0; i < root->cursize; i++) {
        if (root->keys[i].Medication_ID == searchId) {
            printf("\n--- Medication Found ---\n");
            printMedication(root->keys[i]);
            printSuppliers(root->keys[i].Suppliers);
            flag = 0;
            return;  // Found the medication, exit function
        }
    }
    
    // If not found and not a leaf, search in appropriate child
    if (!root->isLeaf) {
        int i = 0;
        while (i < root->cursize && searchId > root->keys[i].Medication_ID) {
            i++;
        }
        searchMedicationById(root->children[i], searchId);
    }
}
void searchMedicationByName(BTreeMedicationName *nameRoot, BTreeMedication *medRoot, const char *searchName) {
    if (nameRoot == NULL) return;

    // Search in current node
    for (int i = 0; i < nameRoot->cursize; i++) {
        if (strcmp(nameRoot->keys[i].Medicine_Name, searchName) == 0) {
            printf("\n--- Medication Found ---\n");
            searchMedicationById(medRoot, nameRoot->keys[i].Medication_ID);
            return;  // Found the medication, exit function
        }
    }

    // If not found and not a leaf, search in appropriate child
    if (!nameRoot->isLeaf) {
        int i = 0;
        while (i < nameRoot->cursize && strcmp(searchName, nameRoot->keys[i].Medicine_Name) > 0) {
            i++;
        }
        searchMedicationByName(nameRoot->children[i], medRoot, searchName);
    }
}

bool finsupplierInTree(SupplierTree *supplierTree, unsigned long supplierId) {
    if (supplierTree == NULL) return false; // If the tree is empty, the ID does not exist

    for (int i = 0; i < supplierTree->cursize; i++) {
        if (supplierTree->keys[i].Supplier_ID == supplierId) {
            return true;
        }
    }

    // Recur for children
    if (!supplierTree->isLeaf) {
        for (int i = 0; i <= supplierTree->cursize; i++) {
            if (finsupplierInTree(supplierTree->children[i], supplierId)) {
                return true;
            }
        }
    }

    return false; // ID does not exist
}

void searchMedicationBySupplier(BTreeMedication *root, unsigned long supplierId) {
    if (root == NULL) return;

    // Traverse the left subtree
    for (int i = 0; i < root->cursize; i++) {
        if (!root->isLeaf) {
            searchMedicationBySupplier(root->children[i], supplierId);
        }

        // Check if the medication has the supplier
        if (finsupplierInTree(root->keys[i].Suppliers, supplierId)) {
            printf("\n--- Medication Found ---\n");
            printMedication(root->keys[i]);
            printSuppliers(root->keys[i].Suppliers);
        }
    }

    // Traverse the rightmost child
    if (!root->isLeaf) {
        searchMedicationBySupplier(root->children[root->cursize], supplierId);
    }
}

void searchMedication(BTree *medicationTree, BTreeByName *medicationNameTree) {
    int choice;
    printf("Search by:\n1. Medication ID\n2. Medication Name\n3. Supplier ID\nEnter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            unsigned long searchId;
            printf("Enter Medication ID to search: ");
            scanf("%lu", &searchId);
            searchMedicationById(medicationTree->root, searchId);
            break;
        }
        case 2: {
            char searchName[50];
            printf("Enter Medication Name to search: ");
            scanf("%s", searchName);
            searchMedicationByName(medicationNameTree->root, medicationTree->root, searchName);
            break;
        }
        case 3: {
            unsigned long supplierId;
            printf("Enter Supplier ID to search: ");
            scanf("%lu", &supplierId);
            searchMedicationBySupplier(medicationTree->root, supplierId);
            break;
        }
        default:
            printf("Invalid choice!\n");
    }
}

BTreeMedicationExpiry *create_MedicationExpiry(int order) {
    BTreeMedicationExpiry *temp = (BTreeMedicationExpiry *)malloc(sizeof(BTreeMedicationExpiry));
    if (temp != NULL) {
        temp->order = order;
        temp->cursize = 0;
        temp->keys = (MedicationExpiryNode *)malloc((order - 1) * sizeof(MedicationExpiryNode));
        temp->children = (BTreeMedicationExpiry **)malloc(order * sizeof(BTreeMedicationExpiry *));
        temp->isLeaf = true;
        temp->parent = NULL;

        for (int i = 0; i < order - 1; i++) {
            temp->keys[i].expiry_date.day = 0;
            temp->keys[i].expiry_date.month = 0;
            temp->keys[i].expiry_date.year = 0;
            temp->keys[i].Medication_ID = 0;
        }
    }
    return temp;
}

int compareExpiryDates(ExpiryDate date1, ExpiryDate date2) {
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    } else if (date1.month != date2.month) {
        return date1.month - date2.month;
    } else {
        return date1.day - date2.day;
    }
}

void splitMedicationExpiryNode(BTreeMedicationExpiry *parent, int index, BTreeMedicationExpiry *fullNode) {
    int t = (fullNode->order + 1) / 2; // Calculate the split point

    // Create a new node to store the second half of keys
    BTreeMedicationExpiry *newNode = create_MedicationExpiry(fullNode->order);
    if (newNode == NULL) {
        printf("Memory allocation failed during expiry node split\n");
        return;
    }
    
    newNode->isLeaf = fullNode->isLeaf;
    
    // Copy the last half of keys from fullNode to newNode
    for (int i = 0; i < fullNode->order - t; i++) {
        if (t + i < fullNode->cursize) {
            newNode->keys[i] = fullNode->keys[t + i];
            newNode->cursize++;
        }
    }

    // Copy the last half of children from fullNode to newNode (if not a leaf)
    if (!fullNode->isLeaf) {
        for (int i = 0; i < fullNode->order - t + 1; i++) {
            if (t + i <= fullNode->cursize) {
                newNode->children[i] = fullNode->children[t + i];
                if (newNode->children[i] != NULL) {
                    newNode->children[i]->parent = newNode;
                }
            }
        }
    }

    // Update the current size of fullNode
    fullNode->cursize = t - 1;

    // Shift children of parent to make space for newNode
    for (int i = parent->cursize; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    // Link newNode to parent
    parent->children[index + 1] = newNode;
    newNode->parent = parent;

    // Shift keys of parent to make space for the middle key of fullNode
    for (int i = parent->cursize - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }

    // Copy the middle key of fullNode to parent
    parent->keys[index] = fullNode->keys[t - 1];

    // Increment the number of keys in parent
    parent->cursize++;
}

void addMedicationExpiryToNode(BTreeMedicationExpiry *node, MedicationExpiryNode medExpiry) {
    int i = node->cursize - 1;

    if (node->isLeaf) {
        // Find the correct position for the new medication expiry
        while (i >= 0 && compareExpiryDates(medExpiry.expiry_date, node->keys[i].expiry_date) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        // Insert the new medication expiry
        node->keys[i + 1] = medExpiry;
        node->cursize++;
    } else {
        // Find the child that will receive the new medication expiry
        while (i >= 0 && compareExpiryDates(medExpiry.expiry_date, node->keys[i].expiry_date) < 0) {
            i--;
        }
        i++;

        // If the child is full, split it
        if (node->children[i]->cursize == node->order - 1) {
            splitMedicationExpiryNode(node, i, node->children[i]);

            // After splitting, determine which child will receive the new medication expiry
            if (compareExpiryDates(medExpiry.expiry_date, node->keys[i].expiry_date) > 0) {
                i++;
            }
        }

        addMedicationExpiryToNode(node->children[i], medExpiry);
    }
}

void addMedicationExpiry(BTreeByExpiry *expiryTree, MedicationExpiryNode medExpiry) {
    if (expiryTree->root == NULL) {
        expiryTree->root = create_MedicationExpiry(expiryTree->order);
    }

    BTreeMedicationExpiry *root = expiryTree->root;

    if (root->cursize == expiryTree->order - 1) {
        BTreeMedicationExpiry *newRoot = create_MedicationExpiry(expiryTree->order);
        newRoot->isLeaf = false;
        newRoot->children[0] = root;

        splitMedicationExpiryNode(newRoot, 0, root);

        int i = (compareExpiryDates(medExpiry.expiry_date, newRoot->keys[0].expiry_date) > 0) ? 1 : 0;
        addMedicationExpiryToNode(newRoot->children[i], medExpiry);

        expiryTree->root = newRoot;
    } else {
        addMedicationExpiryToNode(root, medExpiry);
    }
}

void InitialiseWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree, int ord) {
    Initialise(medicationTree, medicationNameTree, ord);
    expiryTree->order = ord;
    expiryTree->root = create_MedicationExpiry(ord);
}

void AddNewMedicationWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree) {
    if (medicationTree->root == NULL) {
        medicationTree->root = create_Medication(medicationTree->order);
    }

    BTreeMedication *root = medicationTree->root;
    MedicationData newMed;
    MedicationNameNode newMedName;
    MedicationExpiryNode newMedExpiry;

    printf("Enter Medication ID: ");
    unsigned long newID;
    scanf("%lu", &newID);

    while (CheckMEdicIDExist(newID, medicationTree->root)) {
        printf("Medication ID already exists. Enter a new Medication ID: ");
        scanf("%lu", &newID);
    }
    newMed.Medication_ID = newID;
    newMedName.Medication_ID = newID;
    newMedExpiry.Medication_ID = newID;

    printf("Enter Medication Name: ");
    scanf("%s", newMed.Medicine_Name);
    strcpy(newMedName.Medicine_Name, newMed.Medicine_Name);


    printf("Enter Price per Unit: ");
    scanf("%u", &newMed.Price_per_Unit);

    printf("Enter the Batch Name: ");
    scanf("%s", newMed.Batch_details.Batch);

    printf("Enter Reorder Level: ");
    scanf("%d", &newMed.Reorderlevel);
    if(newMed.Reorderlevel < 10 || newMed.Reorderlevel > 100) {
        printf("Invalid reorder level! Please enter a value between 10 and 100.\n");
        scanf("%d", &newMed.Reorderlevel);
    }

    printf("Enter Expiration Date in DD-MM-YYYY\n");
    do {
        printf("Day: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.day);
        printf("Month: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.month);
        printf("Year: ");
        scanf("%d", &newMed.Batch_details.Expiration_Date.year);

        if (!Valid(newMed.Batch_details.Expiration_Date.day, 
                   newMed.Batch_details.Expiration_Date.month, 
                   newMed.Batch_details.Expiration_Date.year)) {
            printf("Invalid date! Please enter a valid date.\n");
        }
    } while (!Valid(newMed.Batch_details.Expiration_Date.day, 
                    newMed.Batch_details.Expiration_Date.month, 
                    newMed.Batch_details.Expiration_Date.year));

    newMed.Batch_details.Total_sales = 0; // Initialize Total_sales to zero

    newMedExpiry.expiry_date = newMed.Batch_details.Expiration_Date;

    int numSuppliers;
    printf("Enter the number of suppliers: ");
    scanf("%d", &numSuppliers);

    newMed.Suppliers = NULL;  // Initialize supplier tree

    for (int j = 0; j < numSuppliers; j++) {
        SupplierData supplier;
        printf("Enter Supplier-%d ID: ", j + 1);
        scanf("%lu", &supplier.Supplier_ID);

        while (!checkSuppID(supplier.Supplier_ID, newMed.Suppliers)) {
            printf("Supplier ID already exists. Enter a new Supplier ID: ");
            scanf("%lu", &supplier.Supplier_ID);
        }

        printf("Enter Supplier-%d Name: ", j + 1);
        scanf("%s", supplier.Supplier_Name);

        printf("Enter Quantity Supplied by Supplier-%d: ", j + 1);
        scanf("%u", &supplier.Quantity_of_stock_bysupplier);
        
        newMed.Quantity_in_stock += supplier.Quantity_of_stock_bysupplier;

        printf("Enter Contact details (10 digits): ");
        scanf("%s", supplier.Contact);

        // Insert supplier correctly into SupplierTree
        addSupplier(&(newMed.Suppliers), supplier, medicationTree->order);
    }

    // Add to ID-based tree
    if (root->cursize == medicationTree->order - 1) {
        BTreeMedication *newRoot = create_Medication(medicationTree->order);
        newRoot->isLeaf = false;
        newRoot->children[0] = root;

        splitMedicationNode(newRoot, 0, root);

        int i = (newMed.Medication_ID > newRoot->keys[0].Medication_ID) ? 1 : 0;
        addMedicationToNode(newRoot->children[i], newMed);

        medicationTree->root = newRoot;
    } else {
        addMedicationToNode(root, newMed);
    }

    // Add to name-based tree
    addMedicationName(medicationNameTree, newMedName);

    // Add to expiry-based tree
    addMedicationExpiry(expiryTree, newMedExpiry);
    
    printf("Medication added successfully!\n");
}
void AddNewMedicationWithExpiry2(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree  , MedicationData newMed) {
    if (medicationTree->root == NULL) {
        medicationTree->root = create_Medication(medicationTree->order);
    }

    BTreeMedication *root = medicationTree->root;
    MedicationNameNode Medic;
    MedicationExpiryNode newMedExpiry;

    Medic.Medication_ID = newMed.Medication_ID;
    strcpy(Medic.Medicine_Name, newMed.Medicine_Name);
    newMedExpiry.Medication_ID = newMed.Medication_ID;
    
    newMedExpiry.expiry_date = newMed.Batch_details.Expiration_Date;

    if (root->cursize == medicationTree->order - 1) {
        BTreeMedication *newRoot = create_Medication(medicationTree->order);
        newRoot->isLeaf = false;
        newRoot->children[0] = root;

        splitMedicationNode(newRoot, 0, root);

        int i = (newMed.Medication_ID > newRoot->keys[0].Medication_ID) ? 1 : 0;
        addMedicationToNode(newRoot->children[i], newMed);

        medicationTree->root = newRoot;
    } else {
        addMedicationToNode(root, newMed);
    }
    
    addMedicationName(medicationNameTree, Medic);

    addMedicationExpiry(expiryTree, newMedExpiry);
    
    printf("Medication added successfully!\n");
}

void displayMedicationsByExpiry(BTreeMedicationExpiry *root, ExpiryDate today, int daysThreshold) {
    if (root == NULL) return;

    // Traverse the left subtree
    for (int i = 0; i < root->cursize; i++) {
        if (!root->isLeaf) {
            displayMedicationsByExpiry(root->children[i], today, daysThreshold);
        }

        // Check if the medication is expiring within the threshold
        int daysToExpiry = difference(today, root->keys[i].expiry_date);
        if (daysThreshold == -1 || daysToExpiry <= daysThreshold) {
            printf("\n--- Medication  ---\n");
            printf("Medication ID: %lu\n", root->keys[i].Medication_ID);
            printf("Expiration Date: %02d-%02d-%04d\n",
                   root->keys[i].expiry_date.day,
                   root->keys[i].expiry_date.month,
                   root->keys[i].expiry_date.year);
            printf("Days to Expiry: %d\n", daysToExpiry);
        }
    }

    // Traverse the rightmost child
    if (!root->isLeaf) {
        displayMedicationsByExpiry(root->children[root->cursize], today, daysThreshold);
    }
}

void cleanupExpiryTree(BTreeMedicationExpiry *root) {
    if (root == NULL) return;

    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            cleanupExpiryTree(root->children[i]);
        }
    }

    free(root->keys);
    free(root->children);
    free(root);
}

void cleanupResourcesWithExpiry(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree) {
    cleanupResources(medicationTree, medicationNameTree);
    
    if (expiryTree != NULL && expiryTree->root != NULL) {
        // Free memory for the expiry-based B-tree
        cleanupExpiryTree(expiryTree->root);
        expiryTree->root = NULL;
    }
}

void listMedicationsExpiringWithinDays(BTreeByExpiry *expiryTree, BTreeMedication *medRoot, ExpiryDate today) {
    int daysThreshold;
    printf("Enter the number of days to check for expiring medications: ");
    scanf("%d", &daysThreshold);
    displayMedicationsByExpiry(expiryTree->root, today, daysThreshold);
}


void trackSalesForAllMedications(BTreeMedication* root) {
    if (root == NULL) return;
    
    // Process all medications in this node
    for (int i = 0; i < root->cursize; i++) {
        int sales;
        printf("Enter the sales of medication ID - %lu and name - %s\n", 
               root->keys[i].Medication_ID, 
               root->keys[i].Medicine_Name);
        scanf("%d", &sales);
        
        // Check if sufficient stock is available
        if (root->keys[i].Quantity_in_stock >= sales) {
            root->keys[i].Quantity_in_stock -= sales;
            
            // Add a total_sales field to MedicationData if it doesn't exist
            root->keys[i].Batch_details.Total_sales += sales;
            
            printf("Sale recorded successfully! Remaining stock: %u\n", 
                   root->keys[i].Quantity_in_stock);
                   
            // Check if reorder level is reached after the sale
            if (root->keys[i].Quantity_in_stock <= root->keys[i].Reorderlevel) {
                printf("WARNING: Medication ID %lu has reached reorder level!\n", 
                       root->keys[i].Medication_ID);
            }
        } else {
            printf("Insufficient stock! Available: %u, Requested: %d\n", 
                   root->keys[i].Quantity_in_stock, sales);
        }
    }
    
    // If not a leaf, process all children
    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            trackSalesForAllMedications(root->children[i]);
        }
    }
}

// Function to track sales for a specific medication by ID
void trackSalesById(BTreeMedication* root, unsigned long medicationId) {
    if (root == NULL) return;
    
    int i = 0;
    while (i < root->cursize && medicationId > root->keys[i].Medication_ID) {
        i++;
    }
    
    // Found the medication
    if (i < root->cursize && medicationId == root->keys[i].Medication_ID) {
        int sales;
        printf("Enter the sales of medication ID - %lu and name - %s\n", 
               root->keys[i].Medication_ID, 
               root->keys[i].Medicine_Name);
        scanf("%d", &sales);
        
        // Check if sufficient stock is available
        if (root->keys[i].Quantity_in_stock >= sales) {
            root->keys[i].Quantity_in_stock -= sales;
            
            // Add a total_sales field to MedicationData if it doesn't exist
            root->keys[i].Batch_details.Total_sales += sales;
            
            printf("Sale recorded successfully! Remaining stock: %u\n", 
                   root->keys[i].Quantity_in_stock);
                   
            // Check if reorder level is reached after the sale
            if (root->keys[i].Quantity_in_stock <= root->keys[i].Reorderlevel) {
                printf("WARNING: Medication ID %lu has reached reorder level!\n", 
                       root->keys[i].Medication_ID);
            }
        } else {
            printf("Insufficient stock! Available: %u, Requested: %d\n", 
                   root->keys[i].Quantity_in_stock, sales);
        }
        return;
    }
    
    // If this is a leaf node and we didn't find the key
    if (root->isLeaf) {
        printf("Medication with ID %lu not found!\n", medicationId);
        return;
    }
    
    // Continue searching in the appropriate child
    trackSalesById(root->children[i], medicationId);
}

// Main sales tracking function with options
void sales_Tracking(BTree* medicationTree) {
    printf("\n--- Sales Tracking ---\n");
    printf("1. Track sales for all medications\n");
    printf("2. Track sales for a specific medication\n");
    printf("Enter your choice: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            trackSalesForAllMedications(medicationTree->root);
            break;
        case 2: {
            unsigned long medicationId;
            printf("Enter medication ID: ");
            scanf("%lu", &medicationId);
            trackSalesById(medicationTree->root, medicationId);
            break;
        }
        default:
            printf("Invalid choice!\n");
    }
}

void checkNode(BTreeMedication *node, int *alertsFound) {
    if (node == NULL) return;

    // Check medications in current node
    for (int i = 0; i < node->cursize; i++) {
        MedicationData *med = &node->keys[i];
        if (med->Quantity_in_stock <= med->Reorderlevel) {
            printf("ALERT: Medication '%s' (ID: %lu)\n", med->Medicine_Name, med->Medication_ID);
            printf("  Current Stock: %u\n", med->Quantity_in_stock); // Fixed type to unsigned
            printf("  Reorder Level: %d\n", med->Reorderlevel);
            printf("  Action Required: Place new order\n\n");
            *alertsFound = 1; // Correctly update the value pointed to by alertsFound
        }
    }

    // If not leaf, check all child nodes
    if (!node->isLeaf) {
        for (int i = 0; i <= node->cursize; i++) {
            checkNode(node->children[i], alertsFound); // Pass by reference to update the flag
        }
    }
}

void stock_Alerts(BTree *medicationTree) {
    int alertsFound = 0;  // Flag to track if any alerts were found

    printf("\nStock Alert Report:\n");
    printf("------------------\n");

    // Helper function to check nodes recursively
    
    // Start checking from the root
    checkNode(medicationTree->root, &alertsFound);

    if (!alertsFound) {
        printf("No stock alerts at this time. All medication quantities are above reorder levels.\n");
    }
}

void updateMedicationDetails(BTree *medicationTree) {
    unsigned long medication_id;
    printf("Enter the ID of the medication to update: ");
    scanf("%lu", &medication_id);

    BTreeMedication *node = medicationTree->root;
    MedicationData *required_medication = NULL;

    // Traverse the B-Tree to find the required medication by ID
    while (node != NULL) {
        int i = 0;
        while (i < node->cursize && medication_id > node->keys[i].Medication_ID) {
            i++;
        }
        if (i < node->cursize && medication_id == node->keys[i].Medication_ID) {
            required_medication = &node->keys[i];
            break;
        }
        if (node->isLeaf) {
            break;
        }
        node = node->children[i];
    }

    if (required_medication == NULL) {
        printf("Medication with ID %lu not found.\n", medication_id);
        return;
    }

    int choice;
    printf("Enter which details you want to modify (1-3):\n");
    printf("1. Price\n2. Supplier Information\n3. Stock\n");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter the new price: ");
        scanf("%u", &required_medication->Price_per_Unit);
        printf("Price updated successfully.\n");
    } else if (choice == 2) {
        unsigned long supplier_id;
        SupplierData *required_supplier = NULL;
        SupplierTree *supplier_node = required_medication->Suppliers;

        printf("Enter the supplier ID to modify details: ");
        scanf("%lu", &supplier_id);

        // Traverse the supplier tree to find the required supplier by ID
        while (supplier_node != NULL) {
            int i = 0;
            while (i < supplier_node->cursize && supplier_id > supplier_node->keys[i].Supplier_ID) {
                i++;
            }
            if (i < supplier_node->cursize && supplier_id == supplier_node->keys[i].Supplier_ID) {
                required_supplier = &supplier_node->keys[i];
                break;
            }
            if (supplier_node->isLeaf) {
                break;
            }
            supplier_node = supplier_node->children[i];
        }

        if (required_supplier == NULL) {
            printf("Supplier with ID %lu not found.\n", supplier_id);
            return;
        }

        printf("Enter the new supplier name: ");
        scanf("%s", required_supplier->Supplier_Name);
        printf("Enter the new quantity supplied: ");
        scanf("%u", &required_supplier->Quantity_of_stock_bysupplier);
        printf("Enter the new contact information: ");
        scanf("%s", required_supplier->Contact);

        // Recalculate the total quantity of stock
        unsigned int total_quantity = 0;
        supplier_node = required_medication->Suppliers;
        while (supplier_node != NULL) {
            for (int i = 0; i < supplier_node->cursize; i++) {
                total_quantity += supplier_node->keys[i].Quantity_of_stock_bysupplier;
            }
            if (supplier_node->isLeaf) {
                break;
            }
            supplier_node = supplier_node->children[0];
        }
        required_medication->Quantity_in_stock = total_quantity;

        printf("Supplier details updated successfully.\n");
    } else if (choice == 3) {
        printf("Enter the new stock quantity: ");
        scanf("%u", &required_medication->Quantity_in_stock);
        printf("Stock updated successfully.\n");
    } else {
        printf("Invalid choice.\n");
    }
}

void deleteMedicationById(BTreeMedication *root, unsigned long medication_id) {

    if (root == NULL) return;



    // Search for the medication ID in the current node

    int i = 0;

    while (i < root->cursize && medication_id > root->keys[i].Medication_ID) {

        i++;

    }



    // If found, delete it

    if (i < root->cursize && medication_id == root->keys[i].Medication_ID) {

        // Shift keys to remove the medication

        for (int j = i; j < root->cursize - 1; j++) {

            root->keys[j] = root->keys[j + 1];

        }

        root->cursize--;

    } else if (!root->isLeaf) {

        // If not found and not a leaf, search in the appropriate child

        deleteMedicationById(root->children[i], medication_id);

    }

}

void deleteMedicationByName(BTreeMedicationName *root, unsigned long medication_id) {

    if (root == NULL) return;



    // Search for the medication ID in the current node

    int i = 0;

    while (i < root->cursize && medication_id > root->keys[i].Medication_ID) {

        i++;

    }



    // If found, delete it

    if (i < root->cursize && medication_id == root->keys[i].Medication_ID) {

        // Shift keys to remove the medication

        for (int j = i; j < root->cursize - 1; j++) {

            root->keys[j] = root->keys[j + 1];

        }

        root->cursize--;

    } else if (!root->isLeaf) {

        // If not found and not a leaf, search in the appropriate child

        deleteMedicationByName(root->children[i], medication_id);

    }

}

void deleteMedicationByExpiry(BTreeMedicationExpiry *root, unsigned long medication_id) {

    if (root == NULL) return;



    // Search for the medication ID in the current node

    int i = 0;

    while (i < root->cursize && medication_id > root->keys[i].Medication_ID) {

        i++;

    }



    // If found, delete it

    if (i < root->cursize && medication_id == root->keys[i].Medication_ID) {

        // Shift keys to remove the medication

        for (int j = i; j < root->cursize - 1; j++) {

            root->keys[j] = root->keys[j + 1];

        }

        root->cursize--;

    } else if (!root->isLeaf) {

        // If not found and not a leaf, search in the appropriate child

        deleteMedicationByExpiry(root->children[i], medication_id);

    }

}



void deleteMedication(BTree *medicationTree, BTreeByName *medicationNameTree, BTreeByExpiry *expiryTree) {

    unsigned long medication_id;

    printf("Enter the ID of the medication to delete: ");

    scanf("%lu", &medication_id);



    // Delete from ID-based tree

    deleteMedicationById(medicationTree->root, medication_id);



    // Delete from name-based tree

    deleteMedicationByName(medicationNameTree->root, medication_id);



    // Delete from expiry-based tree

    deleteMedicationByExpiry(expiryTree->root, medication_id);

}

MedicationData* findMedicationById(BTreeMedication *root, unsigned long medId) {
    if (root == NULL) {
        return NULL;
    }
    int i;
    for (i = 0; i < root->cursize; i++) {
        if (medId == root->keys[i].Medication_ID) {
            return &(root->keys[i]);
        }
        if (medId < root->keys[i].Medication_ID) {
            break;
        }
    }
    if (root->isLeaf) {
        return NULL;
    }
    return findMedicationById(root->children[i], medId);
}

// Add missing function: findSupplierById
SupplierData* findSupplierById(SupplierTree *root, unsigned long supplierId) {
    if (root == NULL) {
        return NULL;
    }
    for (int i = 0; i < root->cursize; i++) {
        if (supplierId == root->keys[i].Supplier_ID) {
            return &(root->keys[i]);
        }
        if (supplierId < root->keys[i].Supplier_ID) {
            if (!root->isLeaf) {
                return findSupplierById(root->children[i], supplierId);
            }
            break;
        }
    }
    if (!root->isLeaf) {
        return findSupplierById(root->children[root->cursize], supplierId);
    }
    return NULL;
}

// Add missing function: deleteSupplierFromTree
bool deleteSupplierFromTree(SupplierTree **root, unsigned long supplierId) {
    if (*root == NULL) {
        return false;
    }
    SupplierTree *node = *root;
    int i = 0;
    while (i < node->cursize && supplierId > node->keys[i].Supplier_ID) {
        i++;
    }
    if (i < node->cursize && supplierId == node->keys[i].Supplier_ID) {
        for (int j = i; j < node->cursize - 1; j++) {
            node->keys[j] = node->keys[j + 1];
        }
        node->cursize--;
        return true;
    }
    if (!node->isLeaf) {
        return deleteSupplierFromTree(&(node->children[i]), supplierId);
    }
    return false;
}

// Function to add a supplier to a medication
void addSupplierToMedication(BTree *medicationTree) {
    unsigned long medId;
    printf("\n===== ADD SUPPLIER TO MEDICATION =====\n");
    printf("Enter Medication ID: ");
    scanf("%lu", &medId);
    MedicationData *medication = findMedicationById(medicationTree->root, medId);
    if (medication == NULL) {
        printf("Medication ID %lu not found.\n", medId);
        return;
    }
    printf("Found Medication: %s (ID: %lu)\n", medication->Medicine_Name, medication->Medication_ID);
    SupplierData newSupplier;
    printf("Enter Supplier ID: ");
    scanf("%lu", &newSupplier.Supplier_ID);
    if (medication->Suppliers != NULL && checkSuppID(newSupplier.Supplier_ID, medication->Suppliers)) {
        printf("Error: Supplier ID %lu already exists for this medication.\n", newSupplier.Supplier_ID);
        return;
    }
    printf("Enter Supplier Name: ");
    scanf(" %[^\n]", newSupplier.Supplier_Name);
    printf("Enter Quantity Supplied: ");
    scanf("%u", &newSupplier.Quantity_of_stock_bysupplier);
    printf("Enter Contact Information: ");
    scanf(" %[^\n]", newSupplier.Contact);
    if (medication->Suppliers == NULL) {
        medication->Suppliers = create_Supplier(medicationTree->order);
    }
    addSupplier(&(medication->Suppliers), newSupplier, medicationTree->order);
    medication->Quantity_in_stock += newSupplier.Quantity_of_stock_bysupplier;
    printf("Supplier added successfully!\n");
}

// Function to update supplier information
void updateSupplierInformation(BTree *medicationTree) {
    unsigned long medId;
    printf("\n===== UPDATE SUPPLIER INFORMATION =====\n");
    printf("Enter Medication ID: ");
    scanf("%lu", &medId);
    MedicationData *medication = findMedicationById(medicationTree->root, medId);
    if (medication == NULL) {
        printf("Medication ID %lu not found.\n", medId);
        return;
    }
    if (medication->Suppliers == NULL) {
        printf("No suppliers found for this medication.\n");
        return;
    }
    printf("Medication: %s (ID: %lu)\n", medication->Medicine_Name, medication->Medication_ID);
    printSuppliers(medication->Suppliers);
    unsigned long supplierId;
    printf("Enter Supplier ID to update: ");
    scanf("%lu", &supplierId);
    SupplierData *supplier = findSupplierById(medication->Suppliers, supplierId);
    if (supplier == NULL) {
        printf("Supplier ID %lu not found for this medication.\n", supplierId);
        return;
    }
    printf("Current Supplier Name: %s\n", supplier->Supplier_Name);
    printf("Enter new Supplier Name (or press Enter to keep current): ");
    char newName[NAME_SIZE];
    scanf(" %[^\n]", newName);
    if (strlen(newName) > 0) {
        strcpy(supplier->Supplier_Name, newName);
    }
    printf("Current Quantity: %u\n", supplier->Quantity_of_stock_bysupplier);
    printf("Enter new Quantity (or 0 to keep current): ");
    unsigned int newQuantity;
    scanf("%u", &newQuantity);
    if (newQuantity > 0) {
        medication->Quantity_in_stock -= supplier->Quantity_of_stock_bysupplier;
        supplier->Quantity_of_stock_bysupplier = newQuantity;
        medication->Quantity_in_stock += newQuantity;
    }
    printf("Current Contact: %s\n", supplier->Contact);
    printf("Enter new Contact (or press Enter to keep current): ");
    char newContact[CONTACT_SIZE];
    scanf(" %[^\n]", newContact);
    if (strlen(newContact) > 0) {
        strcpy(supplier->Contact, newContact);
    }
    printf("Supplier information updated successfully!\n");
}

// Function to delete a supplier
void deleteSupplier(BTree *medicationTree) {
    unsigned long medId;
    printf("\n===== DELETE SUPPLIER =====\n");
    printf("Enter Medication ID: ");
    scanf("%lu", &medId);
    MedicationData *medication = findMedicationById(medicationTree->root, medId);
    if (medication == NULL) {
        printf("Medication ID %lu not found.\n", medId);
        return;
    }
    if (medication->Suppliers == NULL) {
        printf("No suppliers found for this medication.\n");
        return;
    }
    printf("Medication: %s (ID: %lu)\n", medication->Medicine_Name, medication->Medication_ID);
    printSuppliers(medication->Suppliers);
    unsigned long supplierId;
    printf("Enter Supplier ID to delete: ");
    scanf("%lu", &supplierId);
    SupplierData *supplier = findSupplierById(medication->Suppliers, supplierId);
    if (supplier == NULL) {
        printf("Supplier ID %lu not found for this medication.\n", supplierId);
        return;
    }
    unsigned int quantityToRemove = supplier->Quantity_of_stock_bysupplier;
    if (deleteSupplierFromTree(&(medication->Suppliers), supplierId)) {
        medication->Quantity_in_stock -= quantityToRemove;
        printf("Supplier deleted successfully!\n");
    } else {
        printf("Failed to delete supplier. Please try again.\n");
    }
}

// Function to view all suppliers for a medication
void viewSuppliersForMedication(BTree *medicationTree) {
    unsigned long medId;
    printf("\n===== VIEW SUPPLIERS FOR MEDICATION =====\n");
    printf("Enter Medication ID: ");
    scanf("%lu", &medId);
    MedicationData *medication = findMedicationById(medicationTree->root, medId);
    if (medication == NULL) {
        printf("Medication ID %lu not found.\n", medId);
        return;
    }
    printf("\nMEDICATION DETAILS:\n");
    printf("ID: %lu\n", medication->Medication_ID);
    printf("Name: %s\n", medication->Medicine_Name);
    printf("Quantity in Stock: %u\n", medication->Quantity_in_stock);
    printf("Price per Unit: %u\n", medication->Price_per_Unit);
    printf("Reorder Level: %d\n", medication->Reorderlevel);
    printf("Batch: %s\n", medication->Batch_details.Batch);
    printf("Expiration Date: %d/%d/%d\n", 
           medication->Batch_details.Expiration_Date.day,
           medication->Batch_details.Expiration_Date.month,
           medication->Batch_details.Expiration_Date.year);
    printf("Total Sales: %d\n", medication->Batch_details.Total_sales);
    printf("\nSUPPLIERS:\n");
    if (medication->Suppliers == NULL || medication->Suppliers->cursize == 0) {
        printf("No suppliers found for this medication.\n");
    } else {
        printSuppliers(medication->Suppliers);
    }
}

void supplier_management(BTree *medicationTree) {
    int choice;
    printf("\n===== SUPPLIER MANAGEMENT MENU =====\n");
    printf("1. Add Supplier to Medication\n");
    printf("2. Update Supplier Information\n");
    printf("3. Delete Supplier\n");
    printf("4. View All Suppliers for a Medication\n");
    printf("0. Return to Main Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            addSupplierToMedication(medicationTree);
            break;
        case 2:
            updateSupplierInformation(medicationTree);
            break;
        case 3:
            deleteSupplier(medicationTree);
            break;
        case 4:
            viewSuppliersForMedication(medicationTree);
            break;
        case 0:
            printf("Returning to main menu...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }
}

int compare_by_med_count(const void *a, const void *b) {
    const SupplierMedicationCount *sa = (const SupplierMedicationCount *)a;
    const SupplierMedicationCount *sb = (const SupplierMedicationCount *)b;
    return sb->unique_medication_count - sa->unique_medication_count;
}

int compare_by_turnover(const void *a, const void *b) {
    const SupplierMedicationCount *sa = (const SupplierMedicationCount *)a;
    const SupplierMedicationCount *sb = (const SupplierMedicationCount *)b;
    return (sb->total_turnover > sa->total_turnover) - (sb->total_turnover < sa->total_turnover);
}
void collect_suppliers_from_tree(SupplierTree *supplier_tree, MedicationData med, 
                                SupplierMedicationCount suppliers[], int *supplier_count, int max_suppliers) {
    if (supplier_tree == NULL) return;

    for (int i = 0; i < supplier_tree->cursize; i++) {
        SupplierData supplier = supplier_tree->keys[i];
        unsigned long supplier_id = supplier.Supplier_ID;

        int found = -1;
        for (int k = 0; k < *supplier_count; k++) {
            if (suppliers[k].Supplier_ID == supplier_id) {
                found = k;
                break;
            }
        }

        if (found == -1 && *supplier_count < max_suppliers) {
            suppliers[*supplier_count].Supplier_ID = supplier_id;
            strncpy(suppliers[*supplier_count].Supplier_Name, supplier.Supplier_Name, NAME_SIZE - 1);
            suppliers[*supplier_count].unique_medication_count = 1;
            suppliers[*supplier_count].total_turnover = med.Price_per_Unit * supplier.Quantity_of_stock_bysupplier;
            (*supplier_count)++;
        } else if (found != -1) {
            suppliers[found].unique_medication_count++;
            suppliers[found].total_turnover += med.Price_per_Unit * supplier.Quantity_of_stock_bysupplier;
        }
    }

    if (!supplier_tree->isLeaf) {
        for (int i = 0; i <= supplier_tree->cursize; i++) {
            collect_suppliers_from_tree(supplier_tree->children[i], med, suppliers, supplier_count, max_suppliers);
        }
    }
}

void collect_supplier_data(BTreeMedication *root, SupplierMedicationCount suppliers[], int *supplier_count, int max_suppliers) {
    if (root == NULL) return;

    for (int i = 0; i < root->cursize; i++) {
        MedicationData med = root->keys[i];
        collect_suppliers_from_tree(med.Suppliers, med, suppliers, supplier_count, max_suppliers);
    }

    if (!root->isLeaf) {
        for (int i = 0; i <= root->cursize; i++) {
            collect_supplier_data(root->children[i], suppliers, supplier_count, max_suppliers);
        }
    }
}


void find_all_rounder_suppliers(BTree *medicationTree) {
    printf("\n========== ALL-ROUNDER SUPPLIERS ANALYSIS ==========\n");

    SupplierMedicationCount suppliers[MAX_SUPPLIERS];
    int supplier_count = 0;

    collect_supplier_data(medicationTree->root, suppliers, &supplier_count, MAX_SUPPLIERS);

    printf("Total suppliers found: %d\n", supplier_count);

    qsort(suppliers, supplier_count, sizeof(SupplierMedicationCount), compare_by_med_count);

    if (supplier_count > 0) {
        printf("\nBest All-Rounder Supplier:\n");
        printf("Supplier ID: %lu\n", suppliers[0].Supplier_ID);
        printf("Supplier Name: %s\n", suppliers[0].Supplier_Name);
        printf("Unique Medications Supplied: %d\n", suppliers[0].unique_medication_count);

        printf("\nTop 10 All-Rounder Suppliers:\n");
        printf("%-6s %-30s %-25s\n", "Rank", "Supplier Name", "Unique Medications Count");
        printf("----------------------------------------------------------------\n");

        int display_count = supplier_count < 10 ? supplier_count : 10;
        for (int i = 0; i < display_count; i++) {
            printf("%-6d %-30s %-25d\n", i + 1, suppliers[i].Supplier_Name, suppliers[i].unique_medication_count);
        }
    } else {
        printf("No supplier data available.\n");
    }
}

void find_suppliers_with_largest_turnover(BTree *medicationTree) {
    printf("\n========== SUPPLIERS WITH LARGEST TURNOVER ==========\n");

    SupplierMedicationCount suppliers[MAX_SUPPLIERS];
    int supplier_count = 0;

    collect_supplier_data(medicationTree->root, suppliers, &supplier_count, MAX_SUPPLIERS);

    printf("Total suppliers found: %d\n", supplier_count);

    qsort(suppliers, supplier_count, sizeof(SupplierMedicationCount), compare_by_turnover);

    if (supplier_count > 0) {
        printf("\nSupplier with Largest Turnover:\n");
        printf("Supplier ID: %lu\n", suppliers[0].Supplier_ID);
        printf("Supplier Name: %s\n", suppliers[0].Supplier_Name);
        printf("Total Turnover: $%.2f\n", suppliers[0].total_turnover);

        printf("\nTop 10 Suppliers by Turnover:\n");
        printf("%-6s %-30s %-25s\n", "Rank", "Supplier Name", "Turnover ($)");
        printf("----------------------------------------------------------------\n");

        int display_count = supplier_count < 10 ? supplier_count : 10;
        for (int i = 0; i < display_count; i++) {
            printf("%-6d %-30s $%-24.2f\n", i + 1, suppliers[i].Supplier_Name, suppliers[i].total_turnover);
        }
    } else {
        printf("No supplier data available.\n");
    }
}

void supplier_analysis(BTree *medicationTree) {
    int choice;

    do {
        printf("\n========== SUPPLIER ANALYSIS ==========\n");
        printf("1. Find All-Rounder Suppliers\n");
        printf("2. Find Suppliers with Largest Turnover\n");
        printf("3. Run All Analyses\n");
        printf("0. Return to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                find_all_rounder_suppliers(medicationTree);
                break;
            case 2:
                find_suppliers_with_largest_turnover(medicationTree);
                break;
            case 3:
                find_all_rounder_suppliers(medicationTree);
                find_suppliers_with_largest_turnover(medicationTree);
                break;
            case 0:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void ReadFileAndStoreData(const char* filename, BTree* tree, BTreeByName* medicationNameTree, BTreeByExpiry* expiryTree) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    char Buffer[256];

    while (fgets(Buffer, sizeof(Buffer), file)) {
        if (Buffer[0] == '\n' || Buffer[0] == '\r') {
            continue;
        }

        MedicationData medication;
        unsigned long medicationID;
        char medicineName[NAME_SIZE];
        unsigned int pricePerUnit, reorderLevel;
        char batch[BATCH_SIZE];
        int day, month, year, numSuppliers;

        sscanf(Buffer, "%lu", &medicationID);

        // Read medication name
        fgets(medicineName, sizeof(medicineName), file);
       

        fscanf(file, "%u", &medication.Quantity_in_stock);
        fscanf(file, "%u", &pricePerUnit);
        fscanf(file, "%s", batch);
        fscanf(file, "%d %d %d", &day, &month, &year);
        fscanf(file, "%d", &numSuppliers);

        medication.Suppliers = create_Supplier(tree->order); // Create a new supplier tree for this medication
        

        for (int j = 0; j < numSuppliers; j++) {
            SupplierData supplier;
            unsigned long supplierID;
            char supplierName[NAME_SIZE];
            unsigned long long quantityBySupplier;
            char contact[CONTACT_SIZE];

            // Read supplier data
            fscanf(file, "%lu %s %llu %s", &supplierID, supplierName, &quantityBySupplier, contact);

            supplier.Supplier_ID = supplierID;
            strcpy(supplier.Supplier_Name, supplierName);
            supplier.Quantity_of_stock_bysupplier = quantityBySupplier;
            strcpy(supplier.Contact, contact);

            // Add supplier to the supplier tree
            addSupplier(&(medication.Suppliers), supplier, tree->order);

            // Add supplier's quantity to medication's total stock
            medication.Quantity_in_stock += quantityBySupplier;
        }

        fscanf(file, "%d", &reorderLevel);

        medication.Medication_ID = medicationID;
        strcpy(medication.Medicine_Name, medicineName);
        medication.Price_per_Unit = pricePerUnit;
        medication.Quantity_in_stock = medication.Quantity_in_stock; // Set total stock from suppliers
        strcpy(medication.Batch_details.Batch, batch);
        medication.Batch_details.Expiration_Date.day = day;
        medication.Batch_details.Expiration_Date.month = month;
        medication.Batch_details.Expiration_Date.year = year;
        medication.Batch_details.Total_sales = 0;
        medication.Reorderlevel = reorderLevel;

        AddNewMedicationWithExpiry2(tree, medicationNameTree, expiryTree, medication);
    }

    fclose(file);
}
        

// Update main function to include delete medication
int main() {
    BTree medicationTree;
    BTreeByName medicationNameTree;
    BTreeByExpiry expiryTree;
    const char* filename = "medications.txt";   

    // Initialize all three trees with order 4
    InitialiseWithExpiry(&medicationTree, &medicationNameTree, &expiryTree, 3);

    // Load database at startup
    //loadDatabase(&medicationTree, &medicationNameTree, &expiryTree);

    ReadFileAndStoreData( filename, &medicationTree , &medicationNameTree , &expiryTree);
    int choice;
    do {
        printf("\nMenu:\n");
        printf("1. Add New Medication\n");
        printf("2. Search Medication\n");
        printf("3. Print All Medications\n");
        printf("4. Check Expired Medications\n");
        printf("5. List Medications by Expiry Date\n");
        printf("6. Sales Tracking\n");
        printf("7. Stock Alerts\n");
        printf("8. Update Medication Details\n");
        printf("9. Delete Medication\n");
        printf("10. Supplier Management\n");
        printf("11. Supplier Analysis\n");
        printf("12. Cleanup and Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                AddNewMedicationWithExpiry(&medicationTree, &medicationNameTree, &expiryTree);
                break;
            case 2:
                searchMedication(&medicationTree, &medicationNameTree);
                break;
            case 3:
                printAllMedications(medicationTree.root);
                break;
            case 4: {
                ExpiryDate today;
                printf("Enter today's date (DD-MM-YYYY): ");
                scanf("%d-%d-%d", &today.day, &today.month, &today.year);
                checkExpiredMedications(medicationTree.root, today);
                break;
            }
            case 5: {
                ExpiryDate today;
                printf("Enter today's date (DD-MM-YYYY): ");
                scanf("%d-%d-%d", &today.day, &today.month, &today.year);
                listMedicationsExpiringWithinDays(&expiryTree, medicationTree.root, today);
                break;
            }
            case 6:
                sales_Tracking(&medicationTree);
                break;
            case 7:
                stock_Alerts(&medicationTree);
                break;
            case 8:
                updateMedicationDetails(&medicationTree);
                break;
            case 9:
                deleteMedication(&medicationTree, &medicationNameTree, &expiryTree);
                break;
            case 10:
                supplier_management(&medicationTree);
                break;
            case 11:
                supplier_analysis(&medicationTree);
                break;      

        
            case 12:
                // Save database before exiting
                // saveDatabase(&medicationTree);
                cleanupResourcesWithExpiry(&medicationTree, &medicationNameTree, &expiryTree);
                printf("Database saved. Cleaning up resources and exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 12);

    return 0;
}
