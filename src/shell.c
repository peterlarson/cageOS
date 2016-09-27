/*Alec Tiefenthal, Jacob Knispel, Peter Larson, TEAM: 2-F*/

int main()
{
	enableInterrupts();
	interrupt(0x21,4,"title\0", 1, 0);
	while(1)
	{

		char line[80];
		int toKill;
		int i;
		i = 0;
		while(i < 80)
		{
			line[i] = 0;
			i++;
		}
		/* Print prompt */
		interrupt(0x21,0,"CAGE> ",0,0);

		/* Read line */
		interrupt(0x21,1,line,0,0);

		/*Make just the command (not arguments) non case sensative*/
		i = 0;
		while((((int)line[i]) != 0) && (line[i] != ' '))
		{
			if(((int)line[i]) < 91 && ((int)line[i]) > 64)
			{
				line[i] = ((char)((int)line[i]) + 32);
			}
			i++;
		}

		/*If line starts with type ...*/
		if(line[0] == 't' && line[1] == 'y' && line[2] == 'p' && line[3] == 'e' && line[4] == ' ')
		{
			/*void handleInterrupt21(int ax, int bx, int cx, int dx)
			readFile((char *) bx, (char *) cx);*/
			char name[7];
			char buffer[13312];
			int i = 5;
			while(i < 11 && line[i-1] != '\0')
			{
				name[i-5] = line[i];
				i++;
			}
			name[i-5]='\0';

			interrupt(0x21,0,"\n\0",0,0);
			interrupt(0x21, 3, name, buffer, 0);
			if(buffer[0] != '\0')
			{
				interrupt(0x21, 0, buffer, 0, 0);
			} 
			else
			{
				interrupt(0x21, 0, "Did not load file\n", 0, 0);
			}

		/*If line starts with execute ...*/
		} else if(line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && line[3] == 'c' && line[4] == 'u'
			&& line[5] == 't' && line[6] == 'e' && line[7] == ' ')
		{
			char name[7];
			char buffer[13312];
			int i = 8;
			while(i < 14 && line[i-1] != '\0')
			{
				name[i-8] = line[i];
				i++;
			}
			name[i-8]='\0';
			interrupt(0x21,0,"\n",0,0);
			interrupt(0x21, 4, name, 0, 0);
		} 
		/*If the line starts with delete ...*/
		else if(line[0] == 'd' && line[1] == 'e' && line[2] == 'l' && line[3] == 'e' && line[4] == 't'
			&& line[5] == 'e' && line[6] == ' ' )
		{
			
			interrupt(0x21,0,"\n",0,0);
			interrupt(0x21,0,"Deleting file: ",0,0);
			interrupt(0x21,0,line+7,0,0);
			interrupt(0x21,7,line+7);

		}
		/*If the line starts with copy ...*/
		else if(line[0] == 'c' && line[1] == 'o' && line[2] == 'p' && line[3] == 'y' && line[4] == ' ')
		{

			char pair[3];
			char copyBuffer[13312];
			char name1[6];
			char name2[6];
			char directoryBuffer[512];
			int size = 0;
			int found = 0;
			int index;
			int subindex;
			int nameIndex =0;
			int bothExist =0;
			int result[2];
			int nameStart =5;
			int success = 1;
			int sectors = 0;
			name1[0]='\0';
			name2[0]='\0';
			for(index = 5; line[index] != '\n'&&line[index] != '\0'&&index<nameStart+6; index++)
			{
				if(line[index]==' ')
				{
					break;
				}
				name1[nameIndex]=line[index];
				nameIndex++;				
			}
			nameIndex = 0;
			index++;
			nameStart = index;
			for(;line[index] != '\n'&&line[index] != '\0'&&index<nameStart+6; index++)
			{
				if(line[index]==' ')
				{
					break;
				}
				name2[nameIndex]=line[index];
				nameIndex++;
			}

			interrupt(0x21,3,name1,copyBuffer,result);
			interrupt(0x21,2,directoryBuffer,2,0);

			/* This ensures that copy doesn't copy an unessecarily large amount of data.
			EG only copies data that actually exists in the file */
			sectors = 0;
			for (index = 0; index < 512; index += 32)
			{
				if(directoryBuffer[index] != '\0')
				{
					success = 1;
					for(subindex = 0; subindex < 6; subindex++)
					{
						if(name1[subindex] == '\0' || name1[subindex] == '\n' || name1[subindex] == 0x0)
							break;
						if(directoryBuffer[index+subindex] != name1[subindex])
						{
							success = 0;
							break;
						}
					}
					if(success == 1)
					{
						for(subindex = 6; subindex < 32; subindex++)
						{
							if(directoryBuffer[index+subindex] == '\0')
								break;
							sectors++;
						}
						break;
					}
				}
			}
			if(success == 1)
				interrupt(0x21,8,name2,copyBuffer,sectors);
		}
		/*If the line starts with dir ...*/
		else if(line[0] == 'd' && line[1] == 'i' && line[2] == 'r')
		{
			int index;
			int subindex;
			int sectors;
			int mod;

			char printMe[7];
			char size[3];
			char directoryBuffer[512];
			printMe[6] = '\0';
			interrupt(0x21, 2, directoryBuffer, 2, 0);
			sectors = 0;

			for (index = 0; index < 512; index += 32)
			{
				if(directoryBuffer[index] != '\0')
				{
					/* Get the filename to be printed */
					for(subindex = 0; subindex < 6; subindex++)
					{
						printMe[subindex] = directoryBuffer[index+subindex];
					}

					/* Count the number of sectors */
					sectors = 0;
					for(subindex = 6; subindex < 32; subindex++)
					{
						if(directoryBuffer[index+subindex] == '\0')
							break;

						sectors++;
					}

					/* Convert the number of sectors to char vals */
					mod = sectors;
					while (mod >= 10)
						mod = mod - 10;
					if((sectors - mod)/10 != 0){
						size[0] = ((sectors - mod)/10) + 48;
						size[1] = mod + 48;
						size[2] = '\0';
					} else {
						size[0] = mod + 48;
						size[1] = '\0';
					}

					/* Print information */
					interrupt(0x21, 0, "\n",0,0);
					interrupt(0x21, 0, printMe,0,0);
					interrupt(0x21, 0, " - ",0,0);
					interrupt(0x21, 0, size,0,0);
					interrupt(0x21, 0, " sector",0,0);

					if(sectors != 1)
						interrupt(0x21, 0, "s",0,0);
				}
			}
			interrupt(0x21, 0, "\n\n",0,0);	
		}
		else if (line[0] == 'c' && line[1] == 'r' && line[2] == 'e' && line[3] == 'a' && line[4] == 't' && line[5] == 'e' && line[6] == ' ' && line[7] != '\0' && line[7] != '\n')
		{
			char file[512*26];
			char fileLine[80];
			int index = 0;
			int subindex = 0;
			char name[7];
			int i = 7;

			/* Extract the name of the file into name[] */
			while(i < 13 && line[i] != '\0' && line[i] != '\n')
			{
				name[i-7] = line[i];
				i++;
			}
			name[i-7]='\0';

			/* Prompt the user */
			interrupt(0x21, 0, "\n",0,0);
			interrupt(0x21,1,fileLine,0,0);
			while(fileLine[0] != '\0' && fileLine[0] != '\n')
			{
				for(subindex = 0; subindex < 80; subindex++)
				{
					if(fileLine[subindex] == '\0')
						break;

					file[index] = fileLine[subindex];
					index++;
				}
				interrupt(0x21, 0, "\n",0,0);
				interrupt(0x21,1,fileLine,0,0);
			}
			file[index] = '\0';
			interrupt(0x21,8,name,file,(int)(index/512) + 1);
		}
		else if(line[0] == 'k' && line[1] == 'i' && line[2] == 'l' && line[3] == 'l' && line[4] == ' ')
		{

			if(0 <= (((int)line[5]) - 48) && 8 >= (((int)line[5]) - 48))
			{
				interrupt(0x21,0,"\nKilled the process.\n",0,0);
				interrupt(0x21,9, ((int)line[5]) - 48,0,0);
				/*interrupt(0x21,9, toKill,0,0);*/
			} 
			else
				interrupt(0x21,0,"\nInvalid\n",0,0);
		}
		else if(line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && line[3] == 'c' && line[4] == 'f'
			&& line[5] == 'o' && line[6] == 'r' && line[7] == 'e' && line[8] == 'g' && line[9] == 'r'
			&& line[10] == 'o' && line[11] == 'u' && line[12] == 'n' && line[13] == 'd' && line[14] == ' ')
		{
			char name[7];
			char buffer[13312];
			int i = 15;
			while(i < 21 && line[i-1] != '\0')
			{
				name[i-15] = line[i];
				i++;
			}
			name[i-15]='\0';
			interrupt(0x21,0,"\n",0,0);
			interrupt(0x21, 4, name, 1, 0);
		} 
		else if(line[0] == 'c' && line[1] == 'l' && line[2] == 'e' && line[3] == 'a' && line[4] == 'r')
		{
			interrupt(0x21, 10, 0, 0, 0);
		} 
		else if(line[0] == 't' && line[1] == 'e' && line[2] == 'x' && line[3] == 't' && line[4] == 'c' && line[5] == 'o'
			&& line[6] == 'l' && line[7] == 'o' && line[8] == 'r' && line[9] == ' ')
		{
			int color = ((int) line[10])-48;
			if(color >= 9 || color < 0)
				interrupt(0x21,0,"\nBad text color!\n",0,0);
			else
				interrupt(0x21,11,((int) line[10])-48,0,0);
		} 
		else if(line[0] == 'b' && line[1] == 'g' && line[2] == 'c' && line[3] == 'o'
			&& line[4] == 'l' && line[5] == 'o' && line[6] == 'r' && line[7] == ' ')
		{
			int color = ((int) line[8])-48;
			if(color >= 8 || color < 0)
				interrupt(0x21,0,"\nBad background color!\n",0,0);
			else
				interrupt(0x21,12,color,0,0);
		} 
		else if(line[0] == 'f' && line[1] == 'i' && line[2] == 'l' && line[3] == 'l' && line[4] == ' ')
		{
			interrupt(0x21,14,line[5],0,0);
		} 
		else if(line[0] == 'q' && line[1] == 'u' && line[2] == 'i' && line[3] == 't')
		{
			interrupt(0x21,10,0,0,0);
			interrupt(0x21,0,"Goodbye!\n",0,0);
			interrupt(0x21,9,7,0,0);
			interrupt(0x21,9,6,0,0);
			interrupt(0x21,9,5,0,0);
			interrupt(0x21,9,4,0,0);
			interrupt(0x21,9,3,0,0);
			interrupt(0x21,9,2,0,0);
			interrupt(0x21,9,1,0,0);
			interrupt(0x21,9,0,0,0);
		} 
		else if(line[0] == 'h' && line[1] == 'e' && line[2] == 'l' && line[3] == 'p')
		{
			interrupt(0x21,0,"No\n",0,0);
		} 
		else if(line[0] == 'r' && line[1] == 'a' && line[2] == 'v' && line[3] == 'e' && line[4] == '\n')
		{
			line[4] = '\0';
			interrupt(0x21,4,line,0,0);
		} 
		else if(line[0] == 'g' && line[1] == 'e' && line[2] == 't' && line[3] == 'p' && line[4] == 'i' && line[5] == 'd' && line[6] == 's' && line[7] == '\n')
		{
			interrupt(0x21,16,0,0,0);
		} 
		else
		{
			/*interrupt(0x21,0,"\n\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xb0\xb0\xdf\xdf\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdc\xdb\xdb\xdf\xdc\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdb\xdb\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xdf\xdf\xdb\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xdc\xdc\xdc\xb0\xb0\xdf\xdb\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xb0\xb0\xb0\xb0\xdc\xdf\xb0\xdc\xb0\xb0\xdb\xdc\xdb\xdb\xdf\xdc\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xdb\xb0\xb0\xb0\xb0\xdf\xb0\xb0\xb0\xdb\xb0\xb0\xb0\xdf\xdf\xdf\xdf\xdf\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdf\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdf\xb0\xb0\xb0\xb0\xdf\xdf\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xdc\xdc\xb0\xdf\xb0\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xdb\xdc\xdb\xdf\xb0\xdc\xb0\xb0\xdb\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdc\xdb\xdb\xdb\xdb\xdb\xdf\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xdf\xdb\xdb\xdb\xdb\xdf\xb0\xdf\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdf\xb0\xb0\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xb0\xb0\xb0\xdc\xdf\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"             You Don't Say?\n",0,0);*/
			//interrupt(0x21,0,"\nBad Command!",0,0);*/
			interrupt(0x21,0,"\n\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdc\xdc\xdc\xdc\xdc",0,0);
			interrupt(0x21,0,"\xdc\xdc\xdc\xdc\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xdf\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xdf\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xb0\xb0\xdf\xdf\xdb\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdc\xdb\xdb\xdf\xdc\xb0\xb0\xb0\xb0\xb0\xdc\xdb",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xdf\xdf\xdb\xdb\xdf\xb0\xb0\xb0\xb0\xb0\xdc\xdc",0,0);
			interrupt(0x21,0,"\xdc\xb0\xb0\xdf\xdb\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xb0\xb0\xb0\xb0\xdc\xdf\xb0\xdc\xb0\xb0\xdb\xdc\xdb",0,0);
			interrupt(0x21,0,"\xdb\xdf\xdc\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xdb\xb0\xb0\xb0\xb0\xdf\xb0\xb0\xb0\xdb\xb0\xb0\xb0\xdf\xdf",0,0);
			interrupt(0x21,0,"\xdf\xdf\xdf\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdb\xdc\xb0\xb0\xb0\xb0\xb0\xdc\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdf\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdf\xb0\xb0\xb0\xb0\xdf\xdf\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdb\xdb\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xdc\xdc\xb0\xdf\xb0\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xdb\xdc\xdb\xdf\xb0\xdc\xb0\xb0\xdb\xdb\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdb\xdc\xdb\xdb\xdb\xdb\xdb\xdf\xb0\xb0\xb0\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xdf\xdb\xdb\xdb\xdb\xdf\xb0\xdf\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc\xdf\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xdc\xb0\xb0\xb0\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdc",0,0);
			interrupt(0x21,0,"\xdb\xdf\xb0\xb0\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdc\xdc\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xb0\xdf\xb0",0,0);
			interrupt(0x21,0,"\xb0\xb0\xdc\xdf\xdc\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n",0,0);
			interrupt(0x21,0,"             You Don't Say?\n",0,0);
			
			interrupt(0x21,0,"\n",0,0);
			
		}
	}
}