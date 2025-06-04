#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ExpirationAlertFile.h"
#define NAME_SIZE 50
#define SUP_NAME_SIZE 50
#define BATCH_SIZE 50
#define DATA_LINE_SIZE 150


int medicineseqnumber = 0;
int rand_order(){
    medicineseqnumber++;
    return medicineseqnumber;
}



typedef struct uniqueMedNode
{
   char uniquemedicine[NAME_SIZE];
   struct uniqueMedNode* next;
}uniqueMedNode;
uniqueMedNode* createuniqueMedNode(char medname[])
{
    uniqueMedNode*nptr=(uniqueMedNode*)malloc(sizeof(uniqueMedNode));
    if(nptr!=NULL)
    {
        strcpy(nptr->uniquemedicine,medname);
        nptr->next=NULL;
    }
    return nptr;
}
struct SupplierInfo{
    int sup_id;
    char sup_name[SUP_NAME_SIZE];
    int supplyQuantity;
    char PhnNO[10];
    float overallprice;
    int UniqueQuant;
    uniqueMedNode* med;  
};

struct med{
    int medication_no[100];
    char med_name[NAME_SIZE];
    char batch_no[BATCH_SIZE];
    int Quantity;
    float price_unit;
    struct Date exp;
    struct SupplierInfo supplier;
    int reorder;
    float TotalSales;
};
typedef struct MedNode
{
   struct med inventory;
   struct MedNode* next;
}MedNode;
typedef struct SupplierInfoNode
{
    struct SupplierInfo sup;
    struct SupplierInfoNode* next;
}SupplierInfoNode;

typedef struct pms
{
   struct MedNode* medicines;
   struct SupplierInfoNode* suppliers;
}pms;

MedNode* createmedNode(char newmed_name[], char btch_no[], int quant, float price_per_unit, int expday, int expmonth, int expyear, int reorderlvl, int supid, char supname[], int supquan, char phn[])
{
    MedNode* nptr=(MedNode*)malloc(sizeof(MedNode));
    if(nptr!=NULL)
    {
        strcpy(nptr->inventory.med_name, newmed_name);
        for (int j = 0; j < quant; j++)
        {
            nptr->inventory.medication_no[j] = rand_order();
        }
        strcpy(nptr->inventory.batch_no, btch_no);
        nptr->inventory.Quantity = quant;
        nptr->inventory.price_unit = price_per_unit;
        nptr->inventory.exp.day = expday;
        nptr->inventory.exp.month = expmonth;
        nptr->inventory.exp.year = expyear;
        nptr->inventory.supplier.sup_id = supid;
        strcpy(nptr->inventory.supplier.sup_name, supname);
        nptr->inventory.supplier.supplyQuantity = supquan;
        strcpy(nptr->inventory.supplier.PhnNO, phn);
        nptr->inventory.reorder = reorderlvl;
        nptr->next=NULL;
    }
    return nptr;
}
SupplierInfoNode* createSuppinfoNode(int supid, char supname[], int supquan, char no[])
{
    SupplierInfoNode* nptr=(SupplierInfoNode*)malloc(sizeof(SupplierInfoNode));
    nptr->sup.sup_id = supid;
    strcpy(nptr->sup.sup_name, supname);
    strcpy(nptr->sup.PhnNO, no);
    nptr->sup.supplyQuantity = supquan;
    nptr->sup.UniqueQuant=0;
    nptr->sup.med=NULL;  //to initialise each uniquemedNode
    nptr->next=NULL;
    return nptr;
}

SupplierInfoNode* AddSupplier(SupplierInfoNode* suphead,int supid, char supname[], int supquan, char pno[])
{
    int found=0;
    SupplierInfoNode* nptr,*ptr=suphead,*prev=NULL;
    nptr=createSuppinfoNode(supid, supname, supquan, pno);
    if(suphead==NULL)
    {
        suphead=nptr;
    }
    else
    {
        while (ptr && !found)
        {
            if(ptr->sup.sup_id==nptr->sup.sup_id){
                found=1;
            }
            else{
                prev=ptr;
                ptr=ptr->next;
            }
        }
        if(!found){   //found==0
            prev->next=nptr;
        }
        else
        {
           ptr->sup.supplyQuantity+=supquan;  
           strcpy(ptr->sup.PhnNO,pno);
           strcpy(ptr->sup.sup_name,supname);
           free(nptr);
        }
    }
    return suphead;
}

void AddMedicineToSupplier(SupplierInfoNode* suphead, int supid, char medname[]) 
{
    SupplierInfoNode* sptr=suphead;
    int supplierFound = 0;
    while(sptr && !supplierFound)
    {
        if (sptr->sup.sup_id == supid)
        {
            supplierFound = 1; 
            uniqueMedNode* uptr=sptr->sup.med,*nptr,*prev=NULL;;
            int medicineFound=0;
            while(uptr && !medicineFound)
            {
                if (strcmp(uptr->uniquemedicine, medname) == 0) {
                        //already present
                        medicineFound = 1;
                }
                prev=uptr;
                uptr=uptr->next;
            }
            if (! medicineFound) {
                sptr->sup.UniqueQuant++;
                nptr=createuniqueMedNode(medname);  //add uniquemed to supp
                if(sptr->sup.med==NULL)
                {
                    sptr->sup.med=nptr;
                }
                else
                {
                    prev->next=nptr;
                }
            }
        }
        sptr=sptr->next;
    }
}

void writetoFile(MedNode* medhead, SupplierInfoNode* suphead) {
    FILE* file = fopen("medicines.txt", "w");
    if (file == NULL) {
        printf("\n---ERROR while opening file!---");
        exit(1);
    }

    for(MedNode* curr = medhead; curr != NULL; curr = curr->next ) {
        fprintf(file, "%s,", curr->inventory.med_name);
        fprintf(file, "%s,", curr->inventory.batch_no);
        fprintf(file, "%d,", curr->inventory.Quantity);
        fprintf(file, "(");
        for(int j=0;j<curr->inventory.Quantity;j++){
            fprintf(file, "%d,",curr->inventory.medication_no[j]);
        }
        fprintf(file, ")");
        fprintf(file,"%f,", curr->inventory.price_unit);
        fprintf(file,"%d,%d,%d,", curr->inventory.exp.day, curr->inventory.exp.month, curr->inventory.exp.year);
        fprintf(file,"%d,", curr->inventory.reorder);
        fprintf(file,"%d,", curr->inventory.supplier.sup_id);
        fprintf(file,"%s,", curr->inventory.supplier.sup_name);
        fprintf(file,"%s,", curr->inventory.supplier.PhnNO);
        fprintf(file,"%d", curr->inventory.supplier.supplyQuantity);
        fprintf(file,"\n");
    }

    /*
    fprintf(file,"END OF MEDICATION INVENTORY\n");
 
    
   for(SupplierInfoNode* curr = suphead; curr != NULL; curr = curr->next ) {
        fprintf(file,"%s,", curr->sup.sup_name);
        fprintf(file,"%d,", curr->sup.sup_id);
        fprintf(file,"%s,", curr->sup.PhnNO);
        fprintf(file,"%d", curr->sup.UniqueQuant);
        fprintf(file,"\n");
    }
    fprintf(file,"END OF SUPPLIER DETAILS\n");
    */
    fclose(file);

}


MedNode* AddandUpdateMedication(MedNode* medhead,SupplierInfoNode** suphead,char newmed_name[], 
char btch_no[], int quant, float price_per_unit, int expday, int expmonth, int expyear, 
int reorderlvl, int supid, char supname[], int supquan, char phn[], int medNo[])
{
    MedNode* ptr=medhead,*nptr;
    SupplierInfoNode* sptr=*suphead,*sprev=NULL;
    int found=0;
    while(ptr && !found)
    {
        if (strcmp(ptr->inventory.med_name, newmed_name) == 0 && strcmp(ptr->inventory.batch_no, btch_no) == 0)
        {
            found = 1;
        }
        else
        {
            ptr=ptr->next;
        }
    }
    if(found)
    {
        strcpy(ptr->inventory.med_name, newmed_name);
        for (int j = 0; j < quant; j++)
        {
            if (!medNo) {
                ptr->inventory.medication_no[j] = rand_order();
            } else {    
                ptr->inventory.medication_no[j] = medNo[j];
            }
        }
        strcpy(ptr->inventory.batch_no, btch_no);
        ptr->inventory.Quantity += quant;
        ptr->inventory.price_unit = price_per_unit;
        ptr->inventory.exp.day = expday;
        ptr->inventory.exp.month = expmonth;
        ptr->inventory.exp.year = expyear;
        ptr->inventory.supplier.sup_id = supid;
        strcpy(ptr->inventory.supplier.sup_name, supname);
        ptr->inventory.supplier.supplyQuantity += supquan;
        strcpy(ptr->inventory.supplier.PhnNO, phn);
        ptr->inventory.reorder = reorderlvl;
    }
    else
    {
        nptr= createmedNode(newmed_name,btch_no,quant,price_per_unit,expday,expmonth,expyear,reorderlvl,supid,supname,supquan,phn);
        if(nptr!=NULL)
        {
            if(medhead==NULL)
            {
                medhead=nptr;
            }
            else
            {
                MedNode* temp=medhead;
                while(temp->next)
                {
                    temp=temp->next;
                }
                temp->next=nptr;
            }
        }
    }
    *suphead=AddSupplier(*suphead,supid,supname,supquan,phn);
    AddMedicineToSupplier(*suphead,supid,newmed_name);

    if (!medNo) {
        //write only when you are not reading from file
        writetoFile(medhead, *suphead);
    }
    return medhead;
}


pms* readFromFile(pms* pmsNode) {
    FILE* file = fopen("medicines.txt", "r");
    if (file == NULL) {
        printf("\n---No file to read or error while reading file!---");
        return pmsNode;
    }

    char newmed_name[NAME_SIZE];
    char btch_no[BATCH_SIZE];
    int quant;
    float price_per_unit;
    int expday;
    int expmonth;
    int expyear;
    int reorderlvl;
    int supid;
    char supname[NAME_SIZE];
    int supquan;
    char phn[11];

    MedNode* inventory = pmsNode->medicines;
    SupplierInfoNode* supplier_db = pmsNode->suppliers;


    while( fscanf(file, "%50[^,],", newmed_name) > 0) {

        char medQuantities[150];
        fscanf(file, "%50[^,],", btch_no);
        fscanf(file, "%d,", &quant);
        fscanf(file, "(%100[^)])", medQuantities);

        int medNo[quant];

        char* token = strtok(medQuantities, ",");

        int i=0;
       while(token != 0 && i < quant) {
            medNo[i] = atoi(token);
            token = strtok(0, ",");
            i++;
        }

        fscanf(file, "%f,%d,%d,%d,%d,%d,%50[^,],%50[^,],%d", 
        &price_per_unit, &expday, &expmonth,  &expyear, &reorderlvl, &supid, supname, phn, &supquan);
   
        fscanf(file, "\n");

        inventory = AddandUpdateMedication(inventory, &supplier_db, newmed_name, btch_no, quant, price_per_unit,
         expday, expmonth, expyear, reorderlvl, supid, supname, supquan, phn, medNo);

    }
    fclose(file);

    pmsNode->medicines = inventory;
    pmsNode->suppliers = supplier_db;

    return pmsNode;

}

MedNode* DeleteMedication(MedNode* head, SupplierInfoNode* supplierhead, char newmed_name[], char btch_no[])
{
    int i = 0, found = 0;MedNode*ptr=head,*prev=NULL;
    if(head==NULL){
        printf("Medication database is Empty\n");
    }
    else if(head->next==NULL)  // single node 
    {
        if (strcmp(head->inventory.med_name, newmed_name)==0 && strcmp(head->inventory.batch_no, btch_no) == 0) 
        {
            free(head);
            head=NULL;
        }
        else
        {
            printf("medicine is not found\n");
        }
    }
    else
    {
        while (ptr && !found)
        {
            if (strcmp(ptr->inventory.med_name, newmed_name)==0 && strcmp(ptr->inventory.batch_no, btch_no) == 0)  //IMPPPPPPP
            {
                found=1;
            }
            else
            {
                prev=ptr;
                ptr=ptr->next;
            }
        }
        if(found)
        {
            if(ptr==head){
                head=head->next;  
            }
            else{
                prev->next=ptr->next;
            }
            free(ptr);
        }
        else
        {
           printf("medicine is not found\n");
        }
    }
    writetoFile(head, supplierhead);
    return head;
}
SupplierInfoNode* DeleteSupplier(SupplierInfoNode* suphead, int supid)
{
    int found = 0;
    SupplierInfoNode* ptr=suphead,*prev=NULL;
    if(suphead==NULL)
    {
        printf("Supplier database is empty\n");
    }
    else if(suphead->next==NULL)
    {
        if (ptr->sup.sup_id == supid)
        {
            free(suphead);
            suphead=NULL;
        }
        else
        {
            printf("supplier not found\n");
        }
    }
    else
    {
        while (ptr && !found)
        {
            if (ptr->sup.sup_id == supid)
            {
                found=1;
            }
            else
            {
                prev=ptr;
                ptr=ptr->next;
            }
        }
        if(found)
        {
            if(ptr==suphead)
            {
                suphead=suphead->next;
            }
            else
            {
                prev->next=ptr->next;
            }
            free(ptr);
        }
        else
        {
            printf("supplier not found\n");
        }
    }
    return suphead;
}
void searchMedication(MedNode* head,int ch)
{
    MedNode *ptr=head;
    int flag = 1;
    int med_id;
    char medc_name[NAME_SIZE];
    int supid;
    switch (ch)
    {
    case 1:        
        
        printf("Enter required medication id: ");
        scanf("%d", &med_id);
        while(ptr && flag)
        {
            for (int j = 0; j < ptr->inventory.Quantity && flag; j++)
            {
                if (ptr->inventory.medication_no[j] == med_id)
                {
                    printf("Name of medication: %s \n", ptr->inventory.med_name);
                    printf("BATCH NO: %s \n", ptr->inventory.batch_no);
                    printf("Quantity of medication in stock: %d \n", ptr->inventory.Quantity);
                    printf("Price per unit: %f \n", ptr->inventory.price_unit);
                    printf("Expiry Date of medication: %d %d %d", ptr->inventory.exp.day, ptr->inventory.exp.month, ptr->inventory.exp.year);
                    printf("\nSUPPLIER DETAILS: \n");
                    printf("Name of supplier: %s \n", ptr->inventory.supplier.sup_name);
                    printf("Supplier ID: %d \n", ptr->inventory.supplier.sup_id);
                    printf("Contact number of supplier: %s\n", ptr->inventory.supplier.PhnNO);
                    printf("Quantity supplied: %d\n\n", ptr->inventory.supplier.supplyQuantity);
                    flag = 0;
                }
            }
            ptr=ptr->next;
        }
        if (flag == 1)
        {
            printf("Not in inventory \n");
        }
        break;

    case 2:
        printf("Enter required medication name: ");
        scanf("%s", medc_name);
        while(ptr)
        {
            if (strcmp(ptr->inventory.med_name, medc_name) == 0)
            {
                printf("Name of medication: %s \n", ptr->inventory.med_name);
                for (int j = 0; j < ptr->inventory.Quantity; j++)
                {
                    printf("Medication ID: %d ,", ptr->inventory.medication_no[j]);
                }
                printf("\nBATCH NO: %s \n", ptr->inventory.batch_no);
                printf("Quantity of medication in stock: %d \n", ptr->inventory.Quantity);
                printf("Price per unit: %f \n", ptr->inventory.price_unit);
                printf("Expiry Date of medication: %d %d %d", ptr->inventory.exp.day, ptr->inventory.exp.month, ptr->inventory.exp.year);
                printf("\nSUPPLIER DETAILS: \n");
                printf("Name of supplier: %s \n", ptr->inventory.supplier.sup_name);
                printf("Supplier ID: %d \n", ptr->inventory.supplier.sup_id);
                printf("Contact number of supplier: %s\n", ptr->inventory.supplier.PhnNO);
                printf("Quantity supplied: %d\n\n", ptr->inventory.supplier.supplyQuantity);
                flag = 0;
            }
            ptr=ptr->next;
        }
        if (flag == 1) {
            printf("Not in inventory \n");
        }
        break;

    case 3:
        
        printf("Enter required supplier id: ");
        scanf("%d", &supid);
        while(ptr)
        {
            if (ptr->inventory.supplier.sup_id == supid)
            {
                printf("Name of medication: %s \n", ptr->inventory.med_name);
                printf("BATCH NO: %s \n", ptr->inventory.batch_no);
                printf("Quantity of medication in stock: %d \n", ptr->inventory.Quantity);
                printf("Price per unit: %f \n", ptr->inventory.price_unit);
                printf("Expiry Date of medication: %d-%d-%d \n", ptr->inventory.exp.day, ptr->inventory.exp.month, ptr->inventory.exp.year);
                printf("\nSUPPLIER DETAILS: \n");
                printf("Name of supplier: %s \n", ptr->inventory.supplier.sup_name);
                printf("Supplier ID: %d \n", ptr->inventory.supplier.sup_id);
                printf("Contact number of supplier: %s\n", ptr->inventory.supplier.PhnNO);
                printf("Quantity supplied: %d\n\n", ptr->inventory.supplier.supplyQuantity);
                flag = 0;
            }
            ptr=ptr->next;
        }
        if (flag == 1) {
            printf("Not in inventory \n");
        }
        break;

    default:
        printf("Enter a valid searching method! \n");
        break;
    }
}
void checkExpirationDate(MedNode* head, int cur_day, int cur_month, int cur_year)
{
    MedNode* ptr=head;
    struct Date current;
    current.day = cur_day;
    current.month = cur_month;
    current.year = cur_year;
    while(ptr)
    {
        if (daysBetween(current, ptr->inventory.exp) <= 30)
        {
            for (int j = 0; j < ptr->inventory.Quantity; j++)
            {
                printf("%s with medication id %d is expiring in %ld days \n", ptr->inventory.med_name, ptr->inventory.medication_no[j], daysBetween(current, ptr->inventory.exp));
            }
        }
        ptr=ptr->next;
    }
}
MedNode* DivideList(MedNode* head){
    MedNode* nptr,*fast,*slow;
    slow=head;
    fast=head->next->next;
    while(fast){
        slow=slow->next;
        fast=fast->next;
        if(fast){
            fast=fast->next;
        }
    }
    nptr=slow->next;
    slow->next=NULL;
    return nptr;
}
MedNode* MergeList(MedNode* head1,MedNode* head2){
    MedNode* result,*tail,*ptr1=head1,*ptr2=head2;
    if (totalDays(head1->inventory.exp) < totalDays(head2->inventory.exp))
    {
        result=tail=head1;
        ptr1=ptr1->next;
    }
    else{
        result=tail=head2;
        ptr2=ptr2->next;
    }
    while (ptr1 && ptr2)
    {
        if(totalDays(ptr1->inventory.exp) < totalDays(ptr2->inventory.exp)){
            tail->next=ptr1;
            tail=tail->next;
            ptr1=ptr1->next;
        }
        else{
            tail=tail->next=ptr2;
            ptr2=ptr2->next;
        }
    }
    if(ptr1){
        tail->next=ptr1;
    }
    else{
        tail->next=ptr2;
    }
    return result;
}
MedNode* SortByExpDate(MedNode* head){
    MedNode* nptr,*list_ptr;
    list_ptr=head;
    if(head && head->next){
        nptr=DivideList(list_ptr);
        list_ptr=SortByExpDate(list_ptr);
        nptr=SortByExpDate(nptr);
        head=MergeList(list_ptr,nptr);
    }
    return head;
}
MedNode* Sell(MedNode* head, int Qsold, char name[])
{
    int i = 0, found = 0;
    MedNode* ptr=head;
    while (ptr && !found)
    {
        if (strcmp(ptr->inventory.med_name, name) == 0 && ptr->inventory.Quantity >= Qsold)
        {
            ptr->inventory.Quantity -= Qsold;
            found = 1;
            float totalprice = (ptr->inventory.price_unit) * Qsold;
            printf("batch name=%s\nmedication name=%s\nAmount earned from this batch=%f\n\n", ptr->inventory.batch_no, ptr->inventory.med_name, totalprice);
        }
        else if (strcmp(ptr->inventory.med_name, name) == 0 && ptr->inventory.Quantity < Qsold)
        {
            Qsold = Qsold - ptr->inventory.Quantity;
            float totalprice = (ptr->inventory.price_unit) *(ptr->inventory.Quantity) ;
            printf("batch name=%s\nmedication name=%s\nAmount earned from this batch=%f\n\n", ptr->inventory.batch_no, ptr->inventory.med_name, totalprice);
            ptr->inventory.Quantity = 0;
        }
        ptr=ptr->next;
    }
    if(!found)
    {
       printf("!! Not enough medicine of this name\n");
    }
    return head;
}

MedNode* MedRestockAlert(MedNode* head, char name[])
{
    MedNode* ptr=head;
    int TotalQuant = 0;
    int found = 0;
    while(ptr )
    {
        if (strcmp(ptr->inventory.med_name, name) == 0)
        {
            TotalQuant += ptr->inventory.Quantity;
        }
        ptr=ptr->next;
    }

    ptr=head;
    int reqreorderalert;
    while(ptr && !found) {
        if (strcmp(ptr->inventory.med_name, name) == 0) 
        {
            found = 1;
            reqreorderalert=ptr->inventory.reorder;
        }
        ptr=ptr->next;
    }
    if(found==0)
    {
        printf(" %s medicine is not found !!\n", name);
    }
    else
    {
        if (TotalQuant <= reqreorderalert)
        {
            printf("Restock needed for %s !!\n", name);
        }
    }
    return head;
}
SupplierInfoNode* DivideList2(SupplierInfoNode* head){
    SupplierInfoNode* nptr,*fast,*slow;
    slow=head;
    fast=head->next->next;
    while(fast){
        slow=slow->next;
        fast=fast->next;
        if(fast){
            fast=fast->next;
        }
    }
    nptr=slow->next;
    slow->next=NULL;
    return nptr;
}
SupplierInfoNode* MergeListForTurnover(SupplierInfoNode* head1,SupplierInfoNode* head2){
    SupplierInfoNode* result,*tail,*ptr1=head1,*ptr2=head2;
    if ((head1->sup.overallprice) < (head2->sup.overallprice))
    {
        result=tail=head1;
        ptr1=ptr1->next;
    }
    else{
        result=tail=head2;
        ptr2=ptr2->next;
    }
    while (ptr1 && ptr2)
    {
        if((ptr1->sup.overallprice) < (ptr2->sup.overallprice)){
            tail=tail->next=ptr1;
            ptr1=ptr1->next;
        }
        else{
            tail=tail->next=ptr2;
            ptr2=ptr2->next;
        }
    }
    if(ptr1){
        tail->next=ptr1;
    }
    else{
        tail->next=ptr2;
    }
    return result;
}
SupplierInfoNode* SortByTurnover(SupplierInfoNode* head){
    SupplierInfoNode* nptr,*list_ptr;
    list_ptr=head;
    if(head && head->next){
        nptr=DivideList2(list_ptr);
        list_ptr=SortByTurnover(list_ptr);
        nptr=SortByTurnover(nptr);
        head=MergeListForTurnover(list_ptr,nptr);
    }
    return head;
}
SupplierInfoNode* TurnOver(SupplierInfoNode* suphead,MedNode* medhead)
{
    SupplierInfoNode* supptr=suphead;
    MedNode* medptr=medhead;
    float overallprice;
    int count=0;
    if(suphead && medhead)
    {
        while(supptr)
        {
            medptr=medhead;
            overallprice = 0;
            while(medptr)
            {
                if (supptr->sup.sup_id == medptr->inventory.supplier.sup_id)
                {
                    overallprice = overallprice + (medptr->inventory.price_unit) * (medptr->inventory.Quantity);
                }
                medptr=medptr->next;
            }
            supptr->sup.overallprice = overallprice;
            supptr=supptr->next;
        }
        suphead=SortByTurnover(suphead);
        printf("First 10 turnovers are: \n");
        supptr=suphead;
        while(supptr && count<10)
        {
            printf("%d) %s \n",count+1, supptr->sup.sup_name);
            supptr=supptr->next;
            count++;
        }
    }
    return suphead;
}
SupplierInfoNode* MergeListForUnique(SupplierInfoNode* head1,SupplierInfoNode* head2){
    SupplierInfoNode* result,*tail,*ptr1=head1,*ptr2=head2;
    if ((head1->sup.UniqueQuant) < (head2->sup.UniqueQuant))
    {
        result=tail=head1;
        ptr1=ptr1->next;
    }
    else{
        result=tail=head2;
        ptr2=ptr2->next;
    }
    while (ptr1 && ptr2)
    {
        if((ptr1->sup.UniqueQuant) < (ptr2->sup.UniqueQuant)){
            tail=tail->next=ptr1;
            ptr1=ptr1->next;
        }
        else{
            tail=tail->next=ptr2;
            ptr2=ptr2->next;
        }
    }
    if(ptr1){
        tail->next=ptr1;
    }
    else{
        tail->next=ptr2;
    }
    return result;
}
SupplierInfoNode* SortByUnique(SupplierInfoNode* head)
{
    SupplierInfoNode* nptr,*list_ptr;
    list_ptr=head;
    if(head && head->next)
    {
        nptr=DivideList2(list_ptr);
        list_ptr=SortByUnique(list_ptr);
        nptr=SortByUnique(nptr);
        head=MergeListForUnique(list_ptr,nptr);  
    }
    return head;
}
SupplierInfoNode* AllrounderSupplier(SupplierInfoNode* suphead)
{
    SupplierInfoNode* supptr;
    int count=0;
    suphead=SortByUnique(suphead);
    supptr=suphead;
    printf("First 10 allrounder Supplier are: \n");
    while(supptr && count<10)
    {
        printf("%d) %s \n",count+1, supptr->sup.sup_name);
        supptr=supptr->next;
        count++;
    }
    return suphead;
}

void Display(MedNode* inventory,SupplierInfoNode* supplier_db){
    int i;
    printf("MEDICATION INVENTORY\n");
    MedNode* tempNode = inventory;

    while(tempNode){
        if (tempNode->inventory.price_unit != 0) {
            printf("\n");
            printf("Name of medication: %s \n", tempNode->inventory.med_name);
            printf("BATCH NO: %s \n", tempNode->inventory.batch_no);
            printf("Quantity of medication in stock: %d \n", tempNode->inventory.Quantity);
            for(int j=0;j<tempNode->inventory.Quantity;j++){
                printf("Medication id: %d \n",tempNode->inventory.medication_no[j]);
            }
            printf("Price per unit: %f \n", tempNode->inventory.price_unit);
            printf("Expiry Date of medication: %d %d %d \n", tempNode->inventory.exp.day, tempNode->inventory.exp.month, tempNode->inventory.exp.year);
            printf("SUPPLIER DETAILS:\n");
            printf("Name of supplier: %s \n", tempNode->inventory.supplier.sup_name);
            printf("Supplier ID: %d \n", tempNode->inventory.supplier.sup_id);
            printf("Contact number of supplier: %s \n", tempNode->inventory.supplier.PhnNO);
        }
        tempNode=tempNode->next;
    }
    printf("END OF MEDICATION INVENTORY\n");
    printf("\nSUPPLIER DATABASE\n");
    while(supplier_db){
        if (supplier_db->sup.sup_id != 0) {
            printf("Name of supplier: %s \n", supplier_db->sup.sup_name);
            printf("Supplier ID: %d \n", supplier_db->sup.sup_id);
            printf("Contact number of supplier: %s\n", supplier_db->sup.PhnNO);
            printf("unique medicines: %d\n", supplier_db->sup.UniqueQuant);
        }
        supplier_db =  supplier_db->next;
    }
    printf("END OF SUPPLIER DATABASE\n\n");
}

pms* PrePopulateMedicine(pms* pmsNode) {
    MedNode* inventory = pmsNode->medicines;
    SupplierInfoNode* supplier_db = pmsNode->suppliers;

    inventory = AddandUpdateMedication(inventory, &supplier_db, "crocin", "c1", 5, 2.0, 24, 10, 2025, 3, 1, "sss", 5, "9999988888", NULL );
    inventory = AddandUpdateMedication(inventory, &supplier_db, "relent", "r1", 5, 10.0, 24, 10, 2027, 3, 1, "sss", 5, "9999988888" , NULL);
    inventory = AddandUpdateMedication(inventory, &supplier_db, "limcee", "l1", 5, 10.0, 24, 10, 2027, 3, 1, "sss", 5, "9999988888" , NULL);
    inventory = AddandUpdateMedication(inventory, &supplier_db, "vita", "v1", 5, 10.0, 24, 10, 2027, 3, 1, "sss", 5, "9999988888" , NULL);
    
    inventory = AddandUpdateMedication(inventory, &supplier_db, "koflet", "k1", 5, 20.0, 14, 9, 2027, 3, 2, "sss2", 5, "9999988888", NULL );
    inventory = AddandUpdateMedication(inventory, &supplier_db, "strepsil", "s1", 4, 20.0, 14, 9, 2027, 3, 2, "sss2", 5, "9999988888", NULL );
    inventory = AddandUpdateMedication(inventory, &supplier_db, "vitac", "vc1", 5, 10.0, 24, 10, 2027, 3, 1, "sss", 5, "9999988888", NULL );

    inventory = AddandUpdateMedication(inventory, &supplier_db, "crocin", "c2", 5, 20.0, 14, 9, 2027, 3, 3, "sss3", 5, "9999988888", NULL );

    inventory = AddandUpdateMedication(inventory, &supplier_db, "koflet", "k3", 5, 20.0, 14, 9, 2027, 3, 3, "sss3", 5, "9999988888" , NULL);
    inventory = AddandUpdateMedication(inventory, &supplier_db, "nycil", "n3", 5, 20.0, 14, 9, 2027, 3, 3, "sss3", 5, "9999988888", NULL );

    pmsNode-> medicines = inventory;
    pmsNode->suppliers = supplier_db;
    return pmsNode;
     
}


int main()
{
    struct Date currentDate;
    
    pms* pmsnode=(pms*)malloc(sizeof(pms));
    pmsnode->medicines = NULL;
    pmsnode->suppliers = NULL;

    int choice;
    int flag = 1;
    char newmed_name[NAME_SIZE];
    char btch_no[BATCH_SIZE];
    int quant;
    float price_per_unit;
    int expday;
    int expmonth;
    int expyear;
    int reorderlvl;
    int supid;
    char supname[NAME_SIZE];
    int supquan;
    char phn[11];

    printf("--------------WELCOME TO PHARMACY MANAGEMENT--------------\n");
    printf("Enter today's date as DD MM YYYY: ");
    scanf("%d %d %d", &currentDate.day, &currentDate.month, &currentDate.year);

    printf("Reading the Medicines Inventory from File");
    pmsnode = readFromFile(pmsnode);
    printf("Completed the file reading");

    do{
        printf("\nPlease enter your action to proceed: \n");
        printf("1.Updating or adding medication information\n");
        printf("2.Deleteing medication information from database\n");
        printf("3.Search medication information\n");
        printf("4.Sell medicine to a consumer\n");
        printf("5.Sort database by expiry date\n");
        printf("6.Supplier Database management\n");
        printf("7.To find suppliers with largest turnovers\n");
        printf("8.Finding the all-rounder suppliers list\n");
        printf("9.Display records of inventory\n");
        printf("10.Prepoulated inventory\n");
        printf("11.Exit\n");
        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            printf("Enter the medicine name: ");
            scanf("%s", newmed_name);
            printf("Enter the batch number: ");
            scanf("%s", btch_no);
            printf("Enter the new Quantity: ");
            scanf("%d", &quant);
            printf("Enter the new price per unit: ");
            scanf("%f", &price_per_unit);
            printf("Enter the new Expiry day: ");
            scanf("%d", &expday);
            printf("Enter the new Expiry month: ");
            scanf("%d", &expmonth);
            printf("Enter the new Expiry year: ");
            scanf("%d", &expyear);
            printf("Enter the quantity after which reorder alert must be generated: ");
            scanf("%d", &reorderlvl);
            printf("Enter the new Supplier id: ");
            scanf("%d", &supid);
            printf("Enter the new Supplier name: ");
            scanf("%s", supname);
            printf("Enter the new supplied quantity: ");
            scanf("%d", &supquan);
            printf("Enter the new supplier contact number: ");
            scanf("%s", phn);

            pmsnode->medicines = AddandUpdateMedication(pmsnode->medicines, &pmsnode->suppliers, newmed_name, btch_no, quant, price_per_unit, expday, expmonth, expyear, reorderlvl, supid, supname, supquan, phn, NULL);
            checkExpirationDate(pmsnode->medicines, currentDate.day, currentDate.month, currentDate.year);
            break;

        case 2:
            printf("Enter medicine name and batch number to be deleted: ");
            scanf("%s", newmed_name);
            scanf("%s", btch_no);
            DeleteMedication(pmsnode->medicines, pmsnode->suppliers, newmed_name, btch_no);
            MedRestockAlert(pmsnode->medicines, newmed_name);
            break;

        case 3:
            int ch, med_id;
            printf("Enter 1 to search by ID,2 to seach by name and 3 to search by supplier ID");
            scanf("%d", &ch);
            searchMedication(pmsnode->medicines, ch);
            break;

        case 4:
            int Qsold;
            printf("Enter the quantity sold and medicine sold: ");
            scanf("%d", &Qsold);
            scanf("%s", newmed_name);
            pmsnode->medicines=Sell(pmsnode->medicines, Qsold, newmed_name);
            pmsnode->medicines=MedRestockAlert(pmsnode->medicines, newmed_name);
            break;

        case 5:
            SortByExpDate(pmsnode->medicines);
            printf("\nSorting done!\n");
            break;

        case 6:
            int supp_ch;
            printf("Enter 1 to add supplier, 2 to remove supplier and 3 to update any supplier details: ");
            scanf("%d", &supp_ch);
            switch (supp_ch)
            {
            case 1:
                printf("Enter the Supplier id: ");
                scanf("%d", &supid);
                printf("Enter the Supplier name: ");
                scanf("%s", supname);
                printf("Enter the supplier contact number: ");
                scanf("%s", phn);
                printf("Enter the supplier quantity: ");
                scanf("%d",&supquan);
                pmsnode->suppliers = AddSupplier(pmsnode->suppliers, supid, supname, supquan, phn);
                break;

            case 2:
                printf("Enter supplier id to delete: ");
                scanf("%d", &supid);
                pmsnode->suppliers = DeleteSupplier(pmsnode->suppliers, supid);
                break;

            case 3:
                printf("Enter the Supplier id: ");
                scanf("%d", &supid);
                printf("Enter the new Supplier name: ");
                scanf("%s", supname);
                printf("Enter the new supplier contact number: ");
                scanf("%s", phn);
                printf("Enter the supplier quantity: ");
                scanf("%d",&supquan);
                pmsnode->suppliers = AddSupplier(pmsnode->suppliers, supid, supname, supquan, phn);
                break;

            case 4:
                int supid, i1;
                printf("Enter required supplier id: ");
                scanf("%d", &supid);
                printf("Medications supplied by this supplier:");
                MedNode* medPtr = pmsnode->medicines;
                while(medPtr)                 
                {
                    if (medPtr->inventory.supplier.sup_id == supid)
                    {
                        puts(medPtr->inventory.med_name);
                    }
                    medPtr = medPtr->next;
                }
                break;
            default:
                printf("Enter a valid operation!\n");
                break;
            }
            break;

        case 7:
            pmsnode->suppliers= TurnOver(pmsnode->suppliers, pmsnode->medicines);
            break;
        
        case 8:
            pmsnode->suppliers=AllrounderSupplier(pmsnode->suppliers);
            break;

        case 9:
            Display(pmsnode->medicines,pmsnode->suppliers);
            break;
        
        case 10:
            //TO DO prepopulate inventory for demo
            pmsnode = PrePopulateMedicine(pmsnode);
            checkExpirationDate(pmsnode->medicines, currentDate.day, currentDate.month, currentDate.year);
            break;
        
        case 11:
            free(pmsnode);
            flag=0;
            break;

        default:
            free(pmsnode);
            flag=0;
            break;
        }

    }while(flag==1);
    printf("\n---Thank you for using the system!---\n");
}
