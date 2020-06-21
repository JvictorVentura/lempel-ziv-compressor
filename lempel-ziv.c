#include <stdio.h>
#include <stdlib.h>
typedef char byte;
#define WINDOW_SIZE 8/*2048*/
#define BUFFER_SIZE 4/*29*/

void printWindow(char *window, unsigned sizeOfWindowFilled){
	for(unsigned i = 0; i < sizeOfWindowFilled; ++i)
		printf("%c", window[i]);

}

void printBuffer(char *buffer , unsigned sizeOfBuffer){
	for(unsigned i = 0; i < sizeOfBuffer; ++i)
		printf("%c", buffer[i]);

}


void fillBuffer(char *buffer, FILE *arq, unsigned *bufferSize){

	int c = ' ';
	
	for(unsigned i = 0; i < BUFFER_SIZE; ++i){
		if((c = fgetc(arq))== EOF)
			break;
		else{
			buffer[i] = c;
			++(*bufferSize);
		}
	}

}

int main(){

	return 0;
}
