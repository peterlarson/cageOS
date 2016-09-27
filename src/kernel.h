/*Alec Tiefenthal, Jacob Knispel, Peter Larson, TEAM: 2-F*/

// Constants
#define NUM_ROWS 25
#define NUM_COLS 80

struct process
{
	int active;
	int sp;
	int waitingOn;

	char name[7];
};

/* 
Index 0 - 0x2000
Index 1 - 0x3000
...
Index 7 - 0x9000
*/
struct process procArray[8];

int bgColor;
int textColor;
int backupbg;
int backuptext;


int currentProcess;
int initializing;
int exclusion;
int raving;

/*
	Sets the background color to upper(hex) and text to lower(text)

hexBgt: 1 byte hex value holds the background & text colors

effect: 0 - no effect
		1 - slow horizontal transition
		2 - slow vertical transition
*/
void setBackgroundAndTextColor(int hex, int effect);

/*
	Fills the console screen with the given character

	(e.g. fillWith=' ' clears the screen)
*/
void fillScreen(char fillWith);

/*
	Prints the given string to the display. String must be terminated by \0
*/
void printString(char *chars);

void initializeRave();

/*
	Reads a string from the keyboard
*/
void readString(char buffer[]);
int mod(a, b);
int div(a,b);
/*
Reads a sector. Has room to be improved. 
*/
void readSector(char buffer[], int sector);

/* 
Handles interrupt 21
*/
void handleInterrupt21(int ax, int bx, int cx, int dx);

int updateBGT();

/*
This function takes the name of a file, and reads it from the floopy disk.
Names are terminated by newline or by end of line character. 
*/
int readFile(char name[],char buffer[]);

/*
*/
void executeProgram(char* name, int wait);

/*
*/
void terminate();

/*
Writes to the specified sector on the disk.
Writes only one sector i believe. 
*/
void writeSector(char* data, int sector);

/*
Function deletes file from the disk. (erases reverences to it. )
Name lookup terminates on newline character. Is invalid for names. 
*/
void deleteFile(char* name);

/*
Creates a file of given name and size with the contents of buffer
*/
void writeFile(char* name, char *buffer, int numberOfSectors);

/*This function gets called on each timer interrupt*/
void handleTimerInterrupt(int segment, int sp);

/*This function kills processes. */
void killProcess(int number);

/*This function checks if any process are waiting on the current*/
void checkDependencies(int number);

/*This function prints all processes, their names, and pids*/
void listProcesses();