forallel
========

###NOTE:

	This tool accepts a srcML (http://www.sdml.info/projects/srcml/) filename as a parameter.
	Any number of for loops may be included in the srcML file.
	An optional -d flag will print the array references of loops as the tool executes.

	The tool will return a 0 if the loop(s) in question CAN be run in parallel.
	It will return a 1 if the loop(s) in question CANNOT be run in parallel.

	* The tool is based solely on array references, not scalar variables. *

###TESTS:

	The 'tests' folder contains examples to run from Uptal Banerjee's book
	in single loop form or all together (examples.cpp.for.xml).

###IN THE COMMAND LINE:

	To compile the program, type:
	
		make

	To run the program, type:

		./forallel <srcML filename>
	OR	./forallel -d <srcML filename>
	OR	./forallel <srcML filename> -d

	To delete all object code and executables, type:
	
		make clean

	To delete only object code, type:
	
		make cleano
		
