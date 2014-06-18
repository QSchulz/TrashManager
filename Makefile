all:
	gcc -c fonction.c -Wall -Werror -g
	gcc -o lo41 main.c fonction.o -Wall -Werror -lpthread -g

clean:
	rm -rf *.o ./lo41 *~
