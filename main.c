#include <signal.h>
#include <stdio.h>

void onInterrupt2(int sig) {
	printf("Second alarm received\n");
}

void onInterrupt(int sig) {
	printf("Alarm received\n");
	signal(SIGALRM, onInterrupt2);
	alarm(2);
	sleep(10 - 5);
}

int main(int argc, char* argv[]) {
	signal(SIGALRM, onInterrupt);
	
	alarm(5);
	sleep(10);
	printf("Hello World !\n");
	
	return 0;
}
