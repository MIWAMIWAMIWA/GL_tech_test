

Filefinder : main.o lib_filefind.a
	gcc -std=c11 main.o  -lpthread -L. lib_filefind.a -o $@ 

main.o : main.c
	gcc -std=c11 -c main.c -lpthread -o $@

filefind.o : filefind.c
	gcc -std=c11 -c filefind.c -lpthread -o $@

lib_filefind.a : filefind.o
	ar rcs $@ $^

clean :
	rm -f Filefinder *.o *.a