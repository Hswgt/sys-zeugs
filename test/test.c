/*
 * test.c
 *
 *  Created on: 23.04.2011
 *      Author: steffen
 */

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* blah(int id){
	int random;
	while (1){
	if(pthread_mutex_trylock( &mutex1 )==0){
	printf("habe mutex %i \n", id);
	sleep(3);
	pthread_mutex_unlock( &mutex1 );
	printf("mutex freigegeben %i \n", id);
	break;
	}else{
		printf("kein mutex :-( also nochmal %i \n", id);
		random = rand()%20 +1;
		printf("Warte %i sekunden id: %i \n",random,id);
		sleep(random);
	}
	}
}

void* testthread1 (){


printf("hallo ich laufe 1 \n");

while(1){
	printf("versuche mutex zu holen(1) \n");
	blah(1);
	printf("pausiere 3sekunden(1) \n");
		sleep(3);
}
return;
}

void* testthread2 (){


printf("hallo ich laufe 2 \n");

while(1){
	printf("versuche mutex zu holen(2) \n");
	blah(2);
	printf("pausiere 2sekunden(2) \n");
		sleep(2);
}
return;
}

void* testthread3 (){

printf("hallo ich laufe 3 \n");

while(1){
	printf("versuche mutex zu holen(3) \n");
	blah(3);
	printf("pausiere 5sekunden(3) \n");
		sleep(5);
}
return;
}
void* testthread4 (){

printf("hallo ich laufe 4 \n");

while(1){
	printf("versuche mutex zu holen(4) \n");
	blah(4);
	printf("pausiere 1 sekunden(4) \n");
	sleep(1);
}
return;
}
int main(){
	pthread_t p1, p2, p3, p4;

	int  x;
	x = 1;
	pthread_create(&p1, NULL, testthread1, NULL);
	x = x++;
	pthread_create(&p2, NULL, testthread2, NULL);
	x = x++;
	pthread_create(&p3, NULL, testthread3, NULL);
	x = x++;
	pthread_create(&p4, NULL, testthread4, NULL);

pthread_join(p1,NULL);
pthread_join(p2,NULL);
pthread_join(p3,NULL);
pthread_join(p4,NULL);

return 0;
}
