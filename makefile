SubgateUniverse:			SubgateUniverse.o
							gcc -o SubgateUniverse SubgateUniverse.o -lGL -lGLU -lSDL -lGLEW

SubgateUniverse.o:			SubgateUniverse.c suGameRoutines.h suFileHandling.h suNetworking.h suTypes.h
							gcc -Wall -c SubgateUniverse.c

#Clean Rule removes executables and object files
clean:					
							rm -f -r SubgateUniverse SubgateUniverse.o
