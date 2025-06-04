#define NAME_SIZE 50
#define SUP_NAME_SIZE 50
#define BATCH_SIZE 50
#define HASH_SIZE 100
#define INVENTORY_SIZE 100


struct MEDNAME{
    char med_name[NAME_SIZE];
    int medindex;
};
struct Date{
    int day;
    int month;
    int year;
};
struct SupplierInfo{
    int sup_id;
    char sup_name[SUP_NAME_SIZE];
    int supplyQuantity;
    char PhnNO[10];
    float overallprice;
    int UniqueQuant;

    char uniqueMedicines[10][NAME_SIZE];
};
struct med{
    int medication_no[101];
    char med_name[NAME_SIZE];
    char batch_no[BATCH_SIZE];
    int Quantity;
    float price_unit;
    struct Date exp;
    struct SupplierInfo supplier;
    int reorder;
    float TotalSales;
};