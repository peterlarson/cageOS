/*Alec Tiefenthal, Jacob Knispel, Peter Larson, TEAM: 2-F*/






				   NICK CAGE OPERATING SYSTEM
					USAGE INSTRUCTION GUIDE








----------------------------------------------- BUILD INSTRUCTIONS -----------------------------------------------

To run our OS simply run compileOS.sh and then run runOS.sh from the ./m5 directory (these compile and run the OS). Requires Bochs to run. Exact commands listed below:
./compileOS.sh
./runOS.sh

Running the operating system initially runs the shell, which will continuously accept user input. Most commands available for running in the shell run concurrently with each other (up to 8 processes) and will not interfere with one another (although running multiple functions which output to the console may result in jumbled output)










----------------------------------------------- LIST OF SHELL COMMANDS -----------------------------------------------


To run a command like "getpids" in the shell, simply type "getpids" and hit ENTER when the terminal looks something like this:
CAGE> getpids

In the shell, you can use the following commands, which are all case-insensitive (except for inputs like filenames and the like):


	------------ FILE SYSTEM FUNCTIONS ------------

	type <file-name>: Prints the contents of the given file to the console

	copy <name 1> <name 2>: Copies the file with name one into a new file with name two and allocates the proper amount of space for it

	dir : lists the contents of the directory and their sizes in sectors

	create <filename>: creates a file with the given name, and 
		fills it with user input. To stop entering input press 
		-return- on an enpty line. 

	delete <filename>: deletes the file with the given filename.


	-------------- PROCESS FUNCTIONS -------------

	execute <function-in-memory>: Executes the function in memory with the given name as a new process

	execforeground <function-in-memory>: Executes the function in memory with the given name and forces the calling function (the shell) to wait for that function to finish before continuing

	getpids: lists the names and pids of processes currently executing

	kill <pid>: Terminates the function with the given pid. May result in the program ending in a bad state, since it didn't finish naturally.

	------------------- OTHER ---------------------

	fill <character>: Fills the screen with the given character (not very useful, very obnoxious)

	textcolor <integer>: Sets all text color to a color corresponding to teh given integer. Possible values listed below:
		0 - Black
		1 - Blue
		2 - Green
		3 - Cyan
		4 - Red
		5 - Magenta
		6 - Orange
		7 - Grey
		8 - Dark grey

	bgcolor <integer>: Fills the background with a color corresponding to the given integer. Uses the same colors as textcolor except it has no color corresponding with the number 8 (no dark grey)

	rave: Toggles rave mode, where background and text color flash cheerfully.

	clear: Clears the screen of all text

	quit: Kills all processes

	-----------------------------------------------


	(inputting a command not found above will print a picture of Nicolas Cage)











----------------------------------------------- KNOWN BUGS OR DEFICIENCIES -----------------------------------------------


- It is possible that the quit command will not quit every process if the shell has a non-zero pid and there is a process running at a lower pid than it (this is difficult to accomplish, however)

- CAGE> still prints after the shell is terminated via the kill or quit commands for some reason

- CAGE> prints before the title is displayed and had to be hardcoded into the end of the title program as a result

- The cursor is not reset by the clear command

- Killing the "rave" process while it is running will result in the next call to rave failing (it will reset the screen/text color to the way they were before you called rave the first time), since it tries to terminate the rave process that was already terminated manually by kill











----------------------------------------------- LIST OF ADDITIONAL FEATURES -----------------------------------------------

- copy allocates the minimum necessary amount of space for the new file

- dir prints the size of the files in sectors

- the fill command

- the clear command

- the textcolor command

- the bgcolor command

- the rave command

- the quit command

- the getpids command

- the cool ASCII art of Nicolas Cage and "CAGE OS"

- commands aren't case sensative, but the arguments are