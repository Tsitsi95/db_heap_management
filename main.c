#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"
#include "BF.h"
#define MAX_NAME_SIZE 256
#define BUFFER_SIZE 200
#define NAME_SIZE 15
#define SURNAME_SIZE 20
#define CITY_SIZE 10
#define FILE_NAME 50
#define NUMBER_OF_OPENED_HEAP 10

static int number_of_heap = 0;

void printLabel ();		//Gui Functions in the end of main file
void printMenu1 ();
void printMenu2 ();
void Parse_Insert (char *parse_file, int recognition);	//Parser and Insert take a given csv file and writes in an Opened File the

int main ()
{
	//Variables Declerations
	int recognition = -1, i, user_id, choice, choice2, retur;
	int opened_heap_files[NUMBER_OF_OPENED_HEAP];	//A Queue of Opened Heap Files containing the recognition number of each one
	char user_name[NAME_SIZE], user_surname[SURNAME_SIZE], user_city[CITY_SIZE],
		 file[FILE_NAME], parse_file[MAX_NAME_SIZE];
	//Initializing and Opening Heap File
	BF_Init ();
	for (i = 0; i < NUMBER_OF_OPENED_HEAP; i++)
	{
		opened_heap_files[i] = -1;
	}
	/*This is the menu of Heap Files Management */
	while (1)
	{
		printMenu1 ();
		scanf (" %d", &choice);
		switch (choice)
		{
		case 1:
			//Heap File Creation according to a file name
			printf
			("Please insert the Name of the new Heap File (existing files with the same name will be overwrited!)\n");
			scanf (" %s", file);
			HP_CreateFile (file);
			break;
		case 2:
			//Open Files if there is sufficient Space in Queue - The size can be defined as the user wants
			printf ("Please insert the name of an Existing File to Open!\n");
			scanf (" %s", file);
			if (number_of_heap < NUMBER_OF_OPENED_HEAP)
			{
				recognition = HP_OpenFile (file);
				opened_heap_files[number_of_heap] = recognition;
				number_of_heap++;
			}
			else
			{
				fprintf (stderr,
						 "Opened Heap Files Limit Reached!-If you want to open-manage more heap files\
							define a new magnitude for the Queue in the main-has been set as default to 10!!!!\n");
			}
			break;
		case 3:
			//Parse the csv file and insert the records in Heap File that is Opened and that is showed currently in the Queue
			if (number_of_heap <= 0)
			{
				fprintf (stderr,
						 "Unable to locate opened Heap File to store the loaded parsed File!\n");
				break;
			}
			printf
			("Please give the name of the file you want to parse and load in the latest opened Heap File !\n");
			scanf (" %s", parse_file);
			Parse_Insert (parse_file, opened_heap_files[number_of_heap - 1]);
			break;
		case 4:
			//Opens the Search Managment Tool for the Current Heap File in the Queue
			if (number_of_heap <= 0)
			{
				printf ("Unable to locate an Open Heap File\n");
				break;
			}
			retur = 1;
			while (retur)
			{
				printLabel ();	//Gui informations
				printMenu2 ();
				scanf (" %d", &choice2);
				switch (choice2)
				{
				case 0:
					//Return to the menu box above
					retur = 0;
					break;
				case 1:
					//Search in the records of the Heap File according the id
					printf ("Give me the id you want to lookup for:\n");
					scanf (" %d", &user_id);
					HP_GetAllEntries (opened_heap_files[number_of_heap - 1],
									  "id", &user_id);
					break;
				case 2:
					//Search in the records of the Heap File according the Name
					printf ("Give me the Name you want to lookup for:\n");
					scanf (" %s", user_name);
					HP_GetAllEntries (opened_heap_files[number_of_heap - 1],
									  "name", user_name);
					break;
				case 3:
					//Search in the records of the Hep File according the Surname
					printf ("Give me the Surname you want to lookup for:\n");
					scanf (" %s", user_surname);
					HP_GetAllEntries (opened_heap_files[number_of_heap - 1],
									  "surname", user_surname);
					break;
				case 4:
					//Search in the records of the Heap File according the City
					printf ("Give me the City you want to lookup for:\n");
					scanf (" %s", user_city);
					HP_GetAllEntries (opened_heap_files[number_of_heap - 1],
									  "city", user_city);
					break;
				case 5:
					//Print all the records of the Heap File
					HP_GetAllEntries (opened_heap_files[number_of_heap - 1],
									  "all", NULL);
					break;
				default:
					//If user give another choice warning message
					fprintf (stderr,
							 "Given Choice of User in Menu doesn't exist!\n");
					break;
				}
			}
			break;
		case 5:
			//Closes the current Heap File that is opened in the Queue
			HP_CloseFile (opened_heap_files[number_of_heap - 1]);
			for (i = number_of_heap - 1; i <= NUMBER_OF_OPENED_HEAP - 2; i++)
			{
				//Shift all the next in the Queue with one step
				opened_heap_files[i] = opened_heap_files[i + 1];
			}
			opened_heap_files[NUMBER_OF_OPENED_HEAP - 1] = -1;	//and make the last in the Queue again Closed
			number_of_heap--;
			break;
		case 6:
			//Moves to the previous Opened Heap File in the Queue
			if ((opened_heap_files[number_of_heap - 2] != -1)
					&& (number_of_heap - 2 >= 0))
			{
				printf ("Moved to the previous Opened Heap File!\n");
				number_of_heap--;
			}
			else
			{
				printf ("There is no previous opened file in the Queue!\n");
			}
			break;
		case 7:
			//Moves to the next Opened Heap File in the Queue
			if ((opened_heap_files[number_of_heap] != -1)
					&& (number_of_heap < NUMBER_OF_OPENED_HEAP))
			{
				printf ("Moved to the next Opened Heap File!\n");
				number_of_heap++;
			}
			else
			{
				printf ("There is no next opened file in the Queue!\n");
			}
			break;
		default:
			//When programm is Closed then All still Open Heap Files should close!
			for (i = 0; i < number_of_heap; i++)
			{
				if (opened_heap_files[i] != -1)
				{
					HP_CloseFile (opened_heap_files[i]);
				}
			}
			printf ("All Opened Heap Files were closed!\n");
			printf ("Thank you for playing with me!\n");
			exit (EXIT_SUCCESS);
			break;
		}
	}
	return 0;
}


void
printLabel ()
{
	printf ("***************************************\n\
*                                     *\n\
*         Heap File Search Panel      *\n\
*                                     *\n\
***************************************\n");
}

void
printMenu1 ()
{
	printf ("\n1.Create a New Heap File\n\
2.Open an existed Heap File\n\
3.Load a parse file in the last opened Heap File\n\
4.Search Menu of A Heap File\n\
5.Close last Open Heap File\n\
6.Move to the previous Opened Heap File if exists\n\
7.Move to the next Opened Heap File if exists\n\
Type Something else to exit the Programm\n");
}

void
printMenu2 ()
{
	printf ("\n\nChoose what to do:\n1.Print the records according to id\n\
2.Print the records according Name!\n\
3.Print the records according Surname\n\
4.Print the records according City\n\
5.Print all the records\n\
0.Return to previous Menu\n");
}

void
Parse_Insert (char *parse_file, int recognition)
{
	FILE *fp;
	char *checker;
	Record rec;
	char buffer[BUFFER_SIZE];
	if ((fp = fopen (parse_file, "r")) == NULL)
	{
		fprintf (stderr, "Unknown File for Parsing!\n");
		HP_CloseFile (recognition);
		exit (EXIT_FAILURE);
	}
	while (!feof (fp))
	{
		//Read in the line and make sure it was successful
		if (fgets (buffer, 200, fp) != NULL)
		{
			checker = strtok (buffer, ",\"");
			rec.id = atoi (checker);
			strncpy (rec.name, strtok (NULL, ",\""), 14);	//14 because \0 should be included
			rec.name[14] = '\0';
			strncpy (rec.surname, strtok (NULL, ",\""), 19);	//same
			rec.surname[19] = '\0';
			strncpy (rec.city, strtok (NULL, ",\""), 9);	//same
			rec.city[9] = '\0';
			if (HP_InsertEntry (recognition, rec) == -1)
			{
				HP_CloseFile (recognition);
				exit (EXIT_FAILURE);
			}
		}
	}
	fclose (fp);
}
