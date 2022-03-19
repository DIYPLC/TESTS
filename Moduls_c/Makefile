all:
	gcc *.c #Build all
	./a.out #Run

asm:
	gcc -E main.c -o main.i #See preprocessor
	gcc -S main.i -o main.s #See assembler

obj:
	gcc -c *.c #Compile to object file
	gcc *.o    #Linker
	rm -rf *.o #Delete object file
	./a.out    #Run

del:
	rm -rf *.o #Delete object file
	rm -rf *.i #Delete preprocessor file
	rm -rf *.s #Delete assembler file
