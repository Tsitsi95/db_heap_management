#include "BF.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HEAP_INFO_BLOCK_NO 0
#define HEAP_FILE_IDENTIFIER 13

/*This Function is just a help function that
returns how many records fit in a Block of the File*/

int NoRec_Fit_Block(int rec_size)
{
    return ((rec_size == BLOCK_SIZE) ? 1 : BLOCK_SIZE/rec_size);
}

/*The function Create File creates a File opens it and
allocate memory in the file for the info block that include
the information of a heap file which for the sake of the exercise
is the HEAP_FILE_IDENTIFIER */

int HP_CreateFile(char *fileName)
{
    void* blk;
    const int heap_id = HEAP_FILE_IDENTIFIER;
    int identif;
    if(BF_CreateFile(fileName) < 0)
    {
        BF_PrintError("Something Went Wrong in File Creation! ");
        return -1;
    }
    else
    {
        identif = BF_OpenFile(fileName);
        if(identif < 0)
        {
            BF_PrintError("Function: HP_Create ");
        }
        else
        {
            switch (BF_AllocateBlock(identif))
            {
            case 0:
                BF_ReadBlock(identif,BF_GetBlockCounter(identif)-1,&blk);
                memcpy(blk,&heap_id,sizeof(int));
                BF_WriteBlock(identif,BF_GetBlockCounter(identif)-1);
                break;
            default:
                BF_PrintError("Function: HP_Create ");
                return -1;
            }
        }
        BF_CloseFile(identif);
    }
    return 0;
}
/*The function Open a Heap File that has been already created if creation wasn't corect then
returns error -1 or if the file that was inserted in the function is not a Heap File as has been
defined from the manufacturer in any other case an error flag is being arised*/

int HP_OpenFile(char *fileName)
{
    void* blk;
    int identif,heap_id;
    identif = BF_OpenFile(fileName);
    if(identif < 0)
    {
        BF_PrintError("Something went Wrong in File Opening! ");
        return -1;
    }
    else
    {
        switch(BF_ReadBlock(identif,HEAP_INFO_BLOCK_NO,&blk)) 				//Read the Info for the Heap File
        {
        case 0:
            memcpy(&heap_id,blk,sizeof(int));
            switch(heap_id)
            {
            case HEAP_FILE_IDENTIFIER:
                return identif;
            default:
                BF_PrintError("Function: HP_Open ");
                return -1;
            }
        default:
            BF_PrintError("Function : HP_Open ");
            return -1;
        }
    }
}
/*This Function Closes the Heap File that gets if an error occurs then an error flag is being arised*/

int HP_CloseFile(int fileDesc)
{
    switch(BF_CloseFile(fileDesc))
    {
    case 0:
        return 0;
    default:
        BF_PrintError("Problem in Closing File! ");
        return -1;
    }
}
/*This Function takes a record and according the situation of the Heap File and the emptiness or not
of the blocks of the Heap File either push a record in an block of the Heap File that fits in it or in other case if the block can't
fit more records -looks at the first integer which refers to the number of records that the block has- creates a new block and
inserts the record in the newly created block.A representation of the block is:
-------------------------------------------------------------
|    record	|	rec1	|	rec2 	|	...
|    counter	|		|		|
-------------------------------------------------------------
*/
int HP_InsertEntry(int fileDesc, Record record)
{
    int rec_no,counter_init = 1,blk_counter,rec_counter;
    void* blk;
    if(sizeof(record) <= 0 || sizeof(record) > BLOCK_SIZE)
    {
        return -1;
    }
    rec_no = NoRec_Fit_Block(sizeof(record));
    if((blk_counter = BF_GetBlockCounter(fileDesc)) >= 0)
    {
        switch(blk_counter)
        {
        case 1:
            switch(BF_AllocateBlock(fileDesc))
            {
            case 0:
                ++blk_counter;
                BF_ReadBlock(fileDesc,blk_counter-1,&blk);
                memcpy(blk,&counter_init,sizeof(int));
                memcpy(blk+sizeof(int),&record,sizeof(Record));
                BF_WriteBlock(fileDesc,blk_counter-1);
                return 0;
            default:
                BF_PrintError("Function: HP_Insert 1");
                return -1;
            }
        default:
            switch(BF_ReadBlock(fileDesc,blk_counter-1,&blk))
            {
            case 0:
                break;
            default:
                BF_PrintError("Function: HP_Insert 2");
                return -1;
            }
            memcpy(&rec_counter,blk,sizeof(int));
            if(rec_counter == rec_no)
            {
                switch(BF_AllocateBlock(fileDesc))
                {
                case 0:
                    ++blk_counter;
                    switch(BF_ReadBlock(fileDesc,blk_counter-1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Function: HP_insert 3");
                        return -1;
                    }
                    memcpy(blk,&counter_init,sizeof(int));
                    memcpy(blk+sizeof(int),&record,sizeof(Record));
                    switch(BF_WriteBlock(fileDesc,blk_counter-1))
                    {
                    case 0:
                        return 0;
                    default:
                        return -1;
                    }
                default:
                    BF_PrintError("Function: HP_Insert 4");
                    return -1;
                }
            }
            else
            {
                memcpy(blk+sizeof(int)+rec_counter*sizeof(Record),&record,sizeof(Record));
                ++rec_counter;
                memcpy(blk,&rec_counter,sizeof(int));
                switch(BF_WriteBlock(fileDesc,blk_counter-1))
                {
                case 0:
                    return 0;
                default:
                    return -1;
                }
            }
        }
    }
    else
    {
        BF_PrintError("Function HP_Insert 5");
        return -1;
    }
}
/*This Function get a fieldname and searches according that the Heap File in order to find these Records that have in this fieldname
the questioned value and then prints the result on the standard output.The function use the memcpy function to retrieve the records
from the disk and check the compatibility with the value that is being asked from the user.Due to the fact that the funciton is void
was being decided to exit as EXIT_FAILURE from the bad situations that can be occured!*/

void HP_GetAllEntries(int fileDesc, char* fieldName, void *value)
{
    int blk_counter,rec_counter,iter1,iter2,offset;
    int id_check;
    char name_check[15],sur_check[20],city_check[10];
    void* blk;
    if((blk_counter = BF_GetBlockCounter(fileDesc)) >= 0)
    {
        switch(blk_counter)
        {
        case 0:
            BF_PrintError("There is not Heap File in HP_GetAllEntries ");
            exit(EXIT_FAILURE);
        case 1:
            fprintf(stderr,"HP_GetAllEntries found none entry in Heap File\n");
            return;
        default:
            if(strcmp(fieldName,"id") == 0)
            {
                for(iter1 = 1; iter1 <= (blk_counter-1); iter1++)
                {
                    switch(BF_ReadBlock(fileDesc,iter1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Read Block Failed in HP_GetAllEntries ");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&rec_counter,blk,sizeof(int));
                    offset = 0;
                    for(iter2=1; iter2<=rec_counter; iter2++)
                    {
                        memcpy(&id_check,blk+sizeof(int)+offset,sizeof(int));
                        if(id_check == *((int*)value))
                        {
                            memcpy(name_check,blk+2*sizeof(int)+offset,15*sizeof(char));
                            memcpy(sur_check,blk+2*sizeof(int)+15*sizeof(char)+offset,20*sizeof(char));
                            memcpy(city_check,blk+2*sizeof(int)+35*sizeof(char)+offset,10*sizeof(char));
                            printf("%d,%s,%s,%s\n",id_check,name_check,sur_check,city_check);
                            printf("Record %d of Block Number: %d\n",iter2,iter1);
                        }
                        offset += sizeof(Record);
                    }
                }
            }
            else if(strcmp(fieldName,"name") == 0)
            {
                for(iter1 = 1; iter1 <= (blk_counter-1); iter1++)
                {
                    switch(BF_ReadBlock(fileDesc,iter1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Read Block Failed in HP_GetAllEntries ");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&rec_counter,blk,sizeof(int));
                    offset = 0;
                    for(iter2=1; iter2<=rec_counter; iter2++)
                    {
                        memcpy(name_check,blk+2*sizeof(int)+offset,15*sizeof(char));
                        if(strcmp(name_check,(char*)value) == 0)
                        {
                            memcpy(&id_check,blk+sizeof(int)+offset,sizeof(int));
                            memcpy(sur_check,blk+2*sizeof(int)+15*sizeof(char)+offset,20*sizeof(char));
                            memcpy(city_check,blk+2*sizeof(int)+35*sizeof(char)+offset,10*sizeof(char));
                            printf("%d,%s,%s,%s\n",id_check,name_check,sur_check,city_check);
                            printf("Record %d of Block Number: %d\n",iter2,iter1);
                        }
                        offset += sizeof(Record);
                    }
                }

            }
            else if(strcmp(fieldName,"surname") == 0)
            {
                for(iter1 = 1; iter1 <= (blk_counter-1); iter1++)
                {
                    switch(BF_ReadBlock(fileDesc,iter1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Read Block Failed in HP_GetAllEntries ");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&rec_counter,blk,sizeof(int));
                    offset = 0;
                    for(iter2=1; iter2<=rec_counter; iter2++)
                    {
                        memcpy(sur_check,blk+2*sizeof(int)+15*sizeof(char)+offset,20*sizeof(char));
                        if(strcmp(sur_check,(char*)value) == 0)
                        {
                            memcpy(&id_check,blk+sizeof(int)+offset,sizeof(int));
                            memcpy(name_check,blk+2*sizeof(int)+offset,15*sizeof(char));
                            memcpy(city_check,blk+2*sizeof(int)+35*sizeof(char)+offset,10*sizeof(char));
                            printf("%d,%s,%s,%s\n",id_check,name_check,sur_check,city_check);
                            printf("Record %d of Block Number: %d\n",iter2,iter1);
                        }
                        offset += sizeof(Record);
                    }
                }

            }
            else if(strcmp(fieldName,"city") == 0)
            {
                for(iter1 = 1; iter1 <= (blk_counter-1); iter1++)
                {
                    switch(BF_ReadBlock(fileDesc,iter1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Read Block Failed in HP_GetAllEntries ");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&rec_counter,blk,sizeof(int));
                    offset = 0;
                    for(iter2=1; iter2<=rec_counter; iter2++)
                    {
                        memcpy(city_check,blk+2*sizeof(int)+35*sizeof(char)+offset,10*sizeof(char));
                        if(strcmp(city_check,(char*)value) == 0)
                        {
                            memcpy(&id_check,blk+sizeof(int)+offset,sizeof(int));
                            memcpy(name_check,blk+2*sizeof(int)+offset,15*sizeof(char));
                            memcpy(sur_check,blk+2*sizeof(int)+15*sizeof(char)+offset,20*sizeof(char));
                            printf("%d,%s,%s,%s\n",id_check,name_check,sur_check,city_check);
                            printf("Record %d of Block Number: %d\n",iter2,iter1);
                        }
                        offset += sizeof(Record);
                    }
                }
            }
            else if(strcmp(fieldName,"all") == 0)
            {
                for(iter1 = 1; iter1 <= (blk_counter-1); iter1++)
                {
                    switch(BF_ReadBlock(fileDesc,iter1,&blk))
                    {
                    case 0:
                        break;
                    default:
                        BF_PrintError("Read Block Failed in HP_GetAllEntries ");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&rec_counter,blk,sizeof(int));
                    offset = 0;
                    for(iter2=1; iter2<=rec_counter; iter2++)
                    {
                        memcpy(&id_check,blk+sizeof(int)+offset,sizeof(int));
                        memcpy(name_check,blk+2*sizeof(int)+offset,15*sizeof(char));
                        memcpy(sur_check,blk+2*sizeof(int)+15*sizeof(char)+offset,20*sizeof(char));
                        memcpy(city_check,blk+2*sizeof(int)+35*sizeof(char)+offset,10*sizeof(char));
                        printf("%d %s %s %s\n",id_check,name_check,sur_check,city_check);
                        printf("Record %d of Block Number: %d\n",iter2,iter1);
                        offset += sizeof(Record);
                    }
                }
            }
            else
            {
                fprintf(stderr,"The field name arguments doesn't match those of Records\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        BF_PrintError("The BlockCounter failed in HP_GetAllEntries ");
        exit(EXIT_FAILURE);
    }
}
