#include <stdlib.h>
#include <process.h>
#include <ipc.h>
#include <stdio.h>
#include <strings.h>
#include "pipeProdcons.h"

#define MAX_PRODUCERS 5
#define MAX_CONSUMERS 5
#define MAX_BUFFER 4096

static int producerPid[MAX_PRODUCERS];
static int consumerPid[MAX_CONSUMERS];

static int consumerIndex = 0;
static int producerIndex = 0;
static int index = 0;

int main(int argc, char *argv[]) {
	char c, aux;
	int control = 1;

	int pipe = openPipe("prodconsPipe");

	printf("Type:\n\tq to quit\n\tp to add producer\n\tk to kill producer\n\tc to add consumer\n\tx to kill consumer\n\ts for status\n\th for help\n");

  while(control != -1) {
    c = getchar();
    while(aux != '\n')
    	aux = getchar();
    aux = 0;
    control = processControl(c);
  }

  closePipe(pipe);

  return 0;
}

int processControl(char option) {

	char name[10];
	char num[11];
	char * arguments[] = {name, num};
	int ret = 1;

	switch(option) {
		case 'q':
			killAllProcesses();
			ret = -1;
			break;

		case 'p':
			producerIndex++;
			strncpy(name, "producer", 9);
			itoa(producerIndex, num, 10);
			printf("Added Producer\n");
			producerPid[producerIndex - 1] = runProcess(&producer, 2, arguments, 1);
			break;

		case 'k':
		  if(producerIndex > 0) {
        printf("Killed Producer %d\n", producerIndex);
        producerIndex--;
        kill(producerPid[producerIndex]);
      }
			break;

		case 'c':
			consumerIndex++;
			strncpy(name, "consumer", 9);
			itoa(consumerIndex, num, 10);
			printf("Added Consumer\n");
      consumerPid[consumerIndex - 1] = runProcess(&consumer, 2, arguments, 1);
			break;

		case 'x':
		  if(consumerIndex > 0) {
        printf("Killed Consumer %d\n", consumerIndex);
        consumerIndex--;
        kill(consumerPid[consumerIndex]);
      }
			break;

		case 's':
			printf("%d Producers, %d Consumers, %d/%d Used\n", producerIndex, consumerIndex, index, MAX_BUFFER);
			break;

		case 'h':
			printf("Type:\n\tq to quit\n\tp to add producer\n\tk to kill producer\n\tc to add consumer\n\tx to kill consumer\n\ts for status\n\th for help\n");
			break;

		default:
			printf("Unknown command\n");
			break;
	}

	return ret;
}

void killAllProcesses() {
	int i = consumerIndex;

	while(i-- >= 0) {
		kill(consumerPid[i]);
	}

	i = producerIndex;

	while(i-- >= 0) {
		kill(producerPid[i]);
	}
}

int producer(int argc, char * argv[]) {
	char product;
	int producerNumber;
	char * products = "abcdefg";
	size_t length = strlen(products);
	int productIndex = 0;
  int pipe = openPipe("prodconsPipe");

	if(argc < 2)
		return -1;

	parseInt(argv[1],&producerNumber);

	while(1) {

		if(productIndex == length)
			productIndex = 0;

		product = products[productIndex++];

		if(index == MAX_BUFFER) {
			printf("Full buffer\n");
		}
		write(pipe, &product, 1);
		index++;

		if(index % 1024 == 0)
		  printf("Producer %d: %c in %d\n", producerNumber, product, index - 1);
	}
}

int consumer(int argc, char * argv[]) {
	char consumed;
	int consumerNumber;
  int pipe = openPipe("prodconsPipe");

	if(argc < 2)
		return -1;

	parseInt(argv[1],&consumerNumber);


	while(1) {

		if(index == 0) {
			printf("Empty buffer\n");
		}
    read(pipe, &consumed,1);
		index--;

		printf("Consumer %d: %c in %d\n", consumerNumber, consumed, index);
	}
}
