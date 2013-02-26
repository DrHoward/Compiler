By Jacob Howard, Alexander Darino, Ryan McConnell, Diego Velasquez, and Jorge Rivas

Program tested to compile and run on the Olympus server without problems.

To compile the program: Compile all 4 .c files together, eg. "gcc main.c scanner.c parser.c vm.c". This command will produce an a.out file - the executable version of our program.

This program takes one command-line argument: the name of the input file to process, eg. "./a.out input.txt". (In Dev-C++, you can specify command-line arguments by clicking on the main menue Execute > Parameters...)

The program produces a number of intermediate output files including lexemelist.txt and vma.out. These are used as part of the compiling process. vma.out represents the final output of the compiler: a program that is directly executable in the PM/0 Virtual Machine.