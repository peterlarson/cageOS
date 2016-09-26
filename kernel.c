/*Alec Tiefenthal, Jacob Knispel, Peter Larson, TEAM: 2-F*/

#include "kernel.h"

int main()
{
	/* Initialize some variables */
	int index;
	int temp[2];
	currentProcess = -1;
	initializing = 1;
	exclusion = 0;
	raving = -1;

	/* Clear screen and set bg/text colors */
	fillScreen(' ');
	bgColor=0x10;
	textColor=0x07;
	setBackgroundAndTextColor(updateBGT(-1, -1), 0);


	/* Initialize process array */ 
	for (index = 0; index < 8; index++)
	{
		procArray[index].active = 0;
		procArray[index].sp = 0xFF00;
		procArray[index].name[0] = '\0';
	}



	/* Initialize interrupts */
	makeInterrupt21();
	makeTimerInterrupt();
	interrupt(0x21,4, "shell\0", 0, 0);
	currentProcess = 0;
	while(1);
}

void setBackgroundAndTextColor(int hexBgT, int effect)
{
	int i, j;


	/* Delay */
	if(effect == 1)
	{
		for(i=0; i<14000; i++)
		{
			for(j=0; j<99; j++)
			{
				/* Do nothing */
			}
		}
	}

	/* Fill the video memory with the color */
	for(i = 0; i < NUM_ROWS; i++)
	{
		for(j = 0; j < NUM_COLS; j++)
		{		
			putInMemory(0xB000, 0x8000 + NUM_COLS*(i)*2 + j*2 + 1, hexBgT);
		}
	}
}

void fillScreen(char fillWith)
{
	int i,j;
	for(i = 0; i < NUM_ROWS; i++)
	{
		for(j = 0; j < NUM_COLS; j++)
		{
			putInMemory(0xB000, 0x8000 + NUM_COLS*(i)*2 + j*2, fillWith);
		}
	}
}

void printString(char *chars)
{
	char al;
	char ah;
	int ax;
	int index=0;

	while (chars[index] != '\0')
	{
		al = chars[index];
		ah=0xe;
		ax = ah*256+al;
		interrupt(0x10, ax,0,0,0);

		if(chars[index]=='\n')
		{
			interrupt(0x10,0xe*256+'\r',0,0,0);
		}
		index++;
	}
}

void readString(char buffer[])
{
	char enter[2];
	char result;
	int ax=0;
	int index=0;

	while ( index < 78)
	{
		result = interrupt(0x16,ax,0,0,0);

		if(result == '\b')
		{
			interrupt(0x10,0xe*256+result,0,0,0);
			interrupt(0x10,0xe*256+' ',0,0,0);
			interrupt(0x10,0xe*256+result,0,0,0);
			index--;
			if(index < 0)
			{
				index =0;
			}
		} 
		else if (result =='\r')
		{
			enter[0] = '\n';
			enter[1] = '\0';
			interrupt(0x21, 0, enter, 0,0);
			break;
		} 
		else
		{
			interrupt(0x10,0xe*256+result,0,0,0);
			buffer[index]=result;
			index++;
		}
	}

	buffer[index++] = 0xa;
	buffer[index] = '\0';
}

/*
Integer division function
*/
int div(a, b)
{
	int quotient = 0;
	while ((quotient+1)*b <= a)
	  quotient  = quotient  + 1;
	return quotient;
}


/*
Modulus function
*/
int mod(a, b)
{
	while (a >= b)
	  a = a - b;
	return a;
}

/*
Reads the given sector into the given buffer
*/
void readSector(char buffer[], int sector)
{
	int cl;
	int dh;
	int ch;

	cl = mod(sector,18)+1;
	dh = mod(div(sector,18),2);
	ch = div(sector,36);

	interrupt(0x13,2*256+1,buffer,ch*256+cl,dh*256+0);
}

/*
Writes the given buffer into the given sector
*/
void writeSector(char buffer[], int sector)
{
	int cl;
	int dh;
	int ch;

	cl = mod(sector,18)+1;
	dh = mod(div(sector,18),2);
	ch = div(sector,36);

	interrupt(0x13,3*256+1,buffer,ch*256+cl,dh*256+0);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	int result;
	char pair[3];
	if(ax == 0)
	{
		/* Print String */
		printString((char *) bx);
	}	
	else if(ax == 1)
	{
		/* Read String */
		readString((char *) bx);
	}
	else if(ax == 2)
	{
		/* Read Sector */
		readSector((char *) bx, cx);
	} 
	else if(ax == 3)
	{
		/* Read File? */
		readFile((char *) bx, (char *) cx);
	}
	else if(ax == 4)
	{
		/* Load and execute program */
		executeProgram((char *) bx, cx);
	}
	else if(ax == 5)
	{
		/* Terminate */
		terminate();
	} 
	else if (ax == 6)
	{
		/* Writes to a sector */
		writeSector((char*)bx, cx);
	}
	else if (ax == 7)
	{
		/* Deletes file at the given address */
		deleteFile((char*)bx);
	}
	else if (ax == 8)
	{
		writeFile((char*) bx, (char*) cx, dx);
	} 
	else if (ax == 9)
	{
		/* BX = process_id, from 0 to 7 */
		killProcess(bx);
	}
	else if (ax == 10)
	{
		/* Clear the screen */
		fillScreen(' ');
	}
	else if (ax == 11)
	{
		/* Set text color */
		int bgt = updateBGT(-1, bx);
		setBackgroundAndTextColor(bgt, 0);
	}
	else if (ax == 12)
	{
		/* Set background color */
		int bgt = updateBGT(bx*0x10, -1);
		setBackgroundAndTextColor(bgt, 0);
	}
	else if (ax == 13)
	{
		/* For color cycling specifically */
		int bgt = updateBGT(bx*0x10, -1);
		setBackgroundAndTextColor(bgt, 1);
		bgt = updateBGT(-1, cx);
		setBackgroundAndTextColor(bgt, 1);
	}
	else if (ax == 14)
	{
		/* Fill screen */
		fillScreen(bx);
	}
	else if (ax == 15)
	{
		/* Needs to be run as soon as rave is called */
		initializeRave();
	}
	else if (ax == 16)
	{
		/* List all processes */
		listProcesses();
	}
	else
	{
		/* Error! Do nothing... */
	}
}

/* When bg=-1, don't update it, same for text*/
int updateBGT(int bg, int text)
{
	int newbg;
	int newtext;

	setKernelDataSegment();
	exclusion = 1;

	if(bg != -1)
		bgColor = bg;
	newbg = bgColor;	

	if(text != -1)
		textColor = text;
	newtext = textColor;

	exclusion = 0;
	restoreDataSegment();

	return newbg+newtext;
}

void initializeRave()
{
	/* Initialize rave */
	int deleteMe = -1;
	setKernelDataSegment();
	exclusion = 1;
	if(raving != -1)
		deleteMe = raving;

	exclusion = 0;
	restoreDataSegment();

	if(deleteMe != -1)
	{
		interrupt(0x21, 9, deleteMe, 0, 0);
		setKernelDataSegment();
		raving = -1;
		textColor = backuptext;
		bgColor = backupbg;
		setBackgroundAndTextColor(updateBGT(-1, -1), 0);
		restoreDataSegment();
		interrupt(0x21, 5, 0, 0, 0);
	}
	else
	{
		setKernelDataSegment();
		exclusion=1;
		raving = currentProcess;
		backuptext = textColor;
		backupbg = bgColor;
		exclusion=0;
		restoreDataSegment();
	}
}

/* 
Name must be six bytes
*/
int readFile(char name[], char buffer[])
{
	char directoryBuffer[512];
	char charBuffer[2];
	int foundIndex;
	int index;
	int subindex;
	int success = 0;

	readSector(directoryBuffer,2);

	for (index = 0; index <512 ; index += 32)
	{
		success = 1;

		for(subindex = 0; subindex < 6; subindex++)
		{
			if((directoryBuffer[index+subindex] == 0x0 && (name[subindex] == '\0'||name[subindex] == '\n') && subindex !=0))
				break;

			if(directoryBuffer[index+subindex] != name[subindex])
			{
				success = 0;
				break;
			}
		}
		if(success == 1)
		{
			foundIndex = index;
			break;
		}
	}

	if(success == 0)
	{
		buffer[0] = '\0';
		return 1;
	}


	index = 6;
	while((directoryBuffer[foundIndex+index]!=0x0) && (index < 20))
	{
		readSector(buffer+512*(index-6), directoryBuffer[foundIndex+index]);
		index++; 
	}

	return 0;
}

/*
Segment should be multiple of 0x1000
*/
void executeProgram(char* name, int wait)
{
	char buffer[13312];
	char c0, c1, c2, c3, c4, c5;
	int loaded;
	int index = 0;
	int nameIndex = 0;
	int segmentIndex = -1;
	setKernelDataSegment();
	exclusion = 1;
	restoreDataSegment();

	loaded = readFile(name, buffer);
	if(loaded == 1)
		return;

	for (index = 0; index < 8; index++)
	{	
		setKernelDataSegment();

		if(procArray[index].active == 0)
		{
			restoreDataSegment();
			segmentIndex = index;
			break;
		}

	restoreDataSegment();		
	}

	if(segmentIndex == -1)
	{	
		return;
	}

	/* Store the name into characters so it can
	   be accessed from the kernel (otherwise
	   the pointer to the name is irrelevant) */
	c0 = name[0];
	c1 = name[1];
	c2 = name[2];
	c3 = name[3];
	c4 = name[4];
	c5 = name[5];

	setKernelDataSegment();
	procArray[segmentIndex].active = 1;
	procArray[segmentIndex].sp = 0xFF00;
	procArray[segmentIndex].waitingOn = -1;

	procArray[segmentIndex].name[0] = c0;
	procArray[segmentIndex].name[1] = c1;
	procArray[segmentIndex].name[2] = c2;
	procArray[segmentIndex].name[3] = c3;
	procArray[segmentIndex].name[4] = c4;
	procArray[segmentIndex].name[5] = c5;
	procArray[segmentIndex].name[6] = '\0';
	restoreDataSegment();


	for(index=0; index<13312; index++)
		putInMemory(0x1000 * (segmentIndex + 2),index,buffer[index]);

	initializeProgram(0x1000 * (segmentIndex + 2));

	setKernelDataSegment();
	exclusion = 1;
	if(wait == 1)
	{
		procArray[currentProcess].waitingOn = segmentIndex;
	}
	exclusion = 0;
	restoreDataSegment();
}

void terminate()
{
	setKernelDataSegment();
	exclusion = 1;
	checkDependencies(currentProcess);
	procArray[currentProcess].active = 0;
	exclusion = 0;
	/*Questionable*/ 
	restoreDataSegment();  
	while(1);
}


void deleteFile(char* name)
{

	char directoryBuffer[512];
	char mapBuffer[512];
	int foundIndex;
	int index;
	int subindex;
	int success = 0;
	readSector(directoryBuffer,2);

	/* Code copied from readFile. */
	for (index = 0; index <512 ; index += 32)
	{
		success = 1;

		for(subindex = 0; subindex < 6; subindex++)
		{
			if((directoryBuffer[index+subindex] == 0x0 && (name[subindex] == '\0'||name[subindex] == '\n') && subindex !=0))
				break;

			if(directoryBuffer[index+subindex] != name[subindex])
			{
				success = 0;
				break;
			}
		}
		if(success == 1)
		{
			foundIndex = index;
			break;
		}
	}

	if(success == 0)
	{
		/* No file to delete */
		return;
	}
	readSector(mapBuffer,1);
	/* Code that deletes goes here. */
	/* Works by modifying mapBuffer and directoryBuffer then writing them back to disk. */

	index = 0;
	/*
	EXPLANATION OF LOGIC: 

		(directoryBuffer[foundIndex+index]!=0x0) || (index < 6)
			if index < 6 --> we are reading the name and should continue. 
			if index > 6 and dirB[f+i] !=0x0 we are reading the sectors from the buffer and should continue
			if index > 6 and dirb[f+i] ==0x0 we are done reading sectors and can stop. 

		(index < 20)
			if the file has a max-length name and a maximum size, this prevents us from overwriting the 
			directories of other files. 
	*/
	while(((directoryBuffer[foundIndex+index]!=0x0) || (index < 6)) && (index < 20))
	{
		if(index > 5) /*It is not the name field.*/
		{
			/*Set A byte of the map to 0x0. Byte comes from the directory*/
			mapBuffer[directoryBuffer[foundIndex+index]]=0x0; 	
		}
		/*Always overwrite the directory entry. Will overwrite untill bytes =0;*/
		directoryBuffer[foundIndex+index]=0x0;
		index++;

	}



	writeSector(mapBuffer,1);
	writeSector(directoryBuffer,2);
	return;

}

void writeFile(char* name,  char *buffer, int numberOfSectors)
{
	char directoryBuffer[512];
	char mapBuffer[512];
	int foundIndex;
	int index;
	int subindex;
	int success = 0;
	int numberFound=0;
	int sectorList[32];
	readSector(directoryBuffer,2);

	/*First check that the name is not taken*/

	/*Code copied from delete file*/
	for (index = 0; index <512 ; index += 32)
	{
		success = 1;

		for(subindex = 0; subindex < 6; subindex++)
		{
			if(directoryBuffer[index+subindex] == 0x0 && (name[subindex] == '\0'|| name[subindex] == '\n') && subindex != 0)
			{
				break;
			}

			if(directoryBuffer[index+subindex] != name[subindex])
			{
				success = 0;
				break;
			}
		}
		if(success == 1)
		{
			foundIndex = index;
			break;
		}
	}

	if(success == 1)
	{
		/* File already exists. */
		return;
	}

	/* Checks there is a space in the entry available. */
	for (index = 0; index < 512 ; index += 32)
	{
		success = 0; 
		if(directoryBuffer[index] == '\0')
		{
			success = 1;
			foundIndex = index;
			break;
		}
	}
	if(success == 0)
	{
		/* No entry in the directory available */
		return;
	}

	/* Check the map */
	readSector(mapBuffer, 1);	
	for (index = 0; index < 256; index++)
	{
		if (mapBuffer[index] == 0)
		{
			sectorList[numberFound]=index;
			numberFound++;
			if(numberFound == numberOfSectors)
			{
				/* We have found enough sectors */
				break;
			}
		}
	}

	if(numberFound < numberOfSectors)
	{
		/* There were not enough sectors to store this file */
		return;
	}
	/*Writes in all of the data, and updates the map*/
	for (index = 0; index < numberOfSectors; index++)
	{
		writeSector(buffer+index*512,sectorList[index]);
		mapBuffer[sectorList[index]]=0xFF;
	}

	/*Writes the name into the directory*/
	for (index = 0; (name[index] != '\0' && index < 6);index++)
	{
		directoryBuffer[foundIndex+index]=name[index];
	}
	/*Writes the sectors occupied into the directory*/
	for (index = 0; index < numberOfSectors; index++)
	{
		directoryBuffer[foundIndex+index+6] = sectorList[index];
	}

	writeSector(directoryBuffer,2);
	writeSector(mapBuffer,1);
	/*Done?*/
	return;
}

void handleTimerInterrupt(int segment, int sp)
{

	int newSp;
	int numberVisited;
	int procArrayIndex;
	int newSegmentIndex;

	if(currentProcess == -1 || exclusion == 1)
	{
		returnFromTimer(segment,sp);	
		return;
	}

	procArrayIndex = currentProcess;

	for (numberVisited = 1 ; numberVisited < 9 ; numberVisited++)
	{

		if( (procArray[mod(procArrayIndex + numberVisited, 8)].active == 1) && procArray[mod(procArrayIndex + numberVisited, 8)].waitingOn == -1)
		{
			break; 
		}

	}

	if(numberVisited == 9)
	{
		/* There are no running processes (except maybe the kernel) */
		return;
	}

	newSegmentIndex = mod(procArrayIndex+numberVisited,8);
	if(initializing == 1)
		initializing = 0;
	else
		procArray[currentProcess].sp = sp;

	currentProcess = newSegmentIndex;

	returnFromTimer(0x1000 * (2 + newSegmentIndex),procArray[newSegmentIndex].sp);
}

void killProcess(int number)
{
	setKernelDataSegment();
	checkDependencies(number);
	exclusion = 1;
	procArray[number].active = 0;
	exclusion = 0;
	restoreDataSegment();
}

void checkDependencies(int number)
{
	
	int index;

	for (index = 0 ; index < 8 ; index++)
	{
		exclusion = 1;
		if(procArray[index].waitingOn == number)
			procArray[index].waitingOn = -1;
		exclusion = 0;
	}
	

}

void listProcesses()
{
	int index;
	char pid[2];
	char endline[2];
	char pidText[8];
	char proc[7];
	endline[0] = '\n';
	pid[1] = '\0';
	endline[1] = '\0';
	pidText[0] = ' ';
	pidText[1] = '-';
	pidText[2] = ' ';
	pidText[3] = 'p';
	pidText[4] = 'i';
	pidText[5] = 'd';
	pidText[6] = ' ';
	pidText[7] = '\0';

	proc[0] = '\n';
	proc[1] = 'P';
	proc[2] = 'I';
	proc[3] = 'D';
	proc[4] = 's';
	proc[5] = '\n';
	proc[6] = '\0';

	interrupt(0x21,0,proc,0,0);
	for (index = 0; index < 8; index++)
	{
		setKernelDataSegment();
		if(procArray[index].active == 1)
		{
			restoreDataSegment();
			setKernelDataSegment();
			exclusion=1;
			interrupt(0x21,0,procArray[index].name,0,0);
			exclusion=0;
			restoreDataSegment();

			pid[0] = index+48;
			interrupt(0x21,0,pidText,0,0);
			interrupt(0x21,0,pid,0,0);
			interrupt(0x21,0,endline,0,0);
		} else {
			restoreDataSegment();
		}
	}
	interrupt(0x21,0,endline,0,0);
}