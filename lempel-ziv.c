#include <stdio.h>
#include <stdlib.h>
#define WINDOW_SIZE 8/*2048*/
#define BUFFER_SIZE 4/*29*/
#include <stdint.h>

void printWindow(char *window, uint16_t sizeOfWindowFilled){
	for(uint16_t i = 0; i < sizeOfWindowFilled; ++i)
		printf("%c", window[i]);

}

void printBuffer(char *buffer , uint16_t sizeOfBuffer){
	for(uint16_t i = 0; i < sizeOfBuffer; ++i)
		printf("%c", buffer[i]);

}


void fillBuffer(char *buffer, FILE *arq, uint16_t *bufferFilled){

	int c = ' ';
	
	for(uint16_t i = *bufferFilled; i < BUFFER_SIZE; ++i){
		if((c = fgetc(arq))== EOF)
			break;
		else{
			buffer[i] = c;
			++(*bufferFilled);
		}
	}

}

void scrollWindow(char *window, uint16_t *windowFilled, uint16_t scrollSize, char *buffer, uint16_t bufferFilled){
	if(scrollSize > bufferFilled)
		scrollSize = bufferFilled;

	int k = 0;
	int i = scrollSize;

	while( i > 0 ){
		if(*windowFilled == WINDOW_SIZE){
			for (int j = 0; j < WINDOW_SIZE; ++j){
				if( (j - i) >= 0)
					window[j - i] = window[j];
			}
			*windowFilled -= i;
		}else{
			window[(*windowFilled)++] = buffer[k++];
			--i;
		}

	}
	
}

void scrollBuffer(char *buffer, uint16_t *sizeOfBuffer, uint16_t scrollSize, FILE *arq){
	for (int i = scrollSize; i < *sizeOfBuffer; ++i){
		buffer[i - scrollSize] = buffer[i];
	}

	
	if(scrollSize > *sizeOfBuffer)
		*sizeOfBuffer = 0;
	else
		*sizeOfBuffer -= scrollSize;

	fillBuffer(buffer, arq, sizeOfBuffer);
}


uint8_t searchOnDictionary(char *window, char *buffer, uint16_t *bufferFilled, uint16_t *windowFilled, uint16_t *length, uint16_t *location){
	uint16_t longestSequence = 0;
	uint16_t sequenceLocation = 0;

	uint16_t actualSequence = 0;
	uint16_t actualLocation = 0;

	int i2 = 0;

	for(int i = 0 ; i < *windowFilled; ++i){

		if( i2 == (BUFFER_SIZE -1) )
			break;

		if(actualSequence == 0){
			if(window[i] == buffer[i2]){
				actualLocation = i;
				++actualSequence;
				++i2;
			}
		}else{
			if(window[i] == buffer[i2]){
				++actualSequence;
				++i2;
			}else{
				if(actualSequence > longestSequence){
					longestSequence = actualSequence;
					sequenceLocation = actualLocation;
				}
				actualSequence = 0;
				i2 = 0;
			}
		}

	}	

	if(actualSequence > longestSequence){
		longestSequence = actualSequence;
		sequenceLocation = actualLocation;
	}

	*length = longestSequence;
	*location = sequenceLocation;

	if(longestSequence > 0)
		return 1;
	else
		return 0;
}

void write_location_and_length(uint16_t length, uint16_t location, FILE *arq){
	uint8_t uint8_tWritten = 0;
	for(int i = 16; i > 0; i >>= 1){
		uint8_tWritten += length & i;
	}

	uint8_tWritten <<= 3;
	int index = 4;

	for(int i = 1024; i > 0; i >>= 1){

		if(index == 0){
			fputc(uint8_tWritten, arq);
			index = 128;
			uint8_tWritten = 0;
		}

		if( (location & i) == i){
			uint8_tWritten = index;
		}

		index >>= 1;
	}

	fputc(uint8_tWritten, arq);
}

void compress(char *filename){

	char *buffer = malloc(BUFFER_SIZE);
	char *window = malloc(WINDOW_SIZE);
	uint16_t bufferFilled = 0;
	uint16_t windowFilled = 0;

	uint16_t length = 0;
	uint16_t location = 0;

	FILE *arq = fopen(filename, "r");

	if(arq != NULL){
		fillBuffer(buffer, arq, &bufferFilled);
	}

	FILE *arq2 = fopen("fileCompressed.lz", "w");


	while(bufferFilled > 0){

		/*
		printf("\n");
		printWindow(window, windowFilled);
		printf("\t|\t");
		printBuffer(buffer, bufferFilled);

		printf("\t (%d,%d)\n", length, location);
		*/

		if(searchOnDictionary(window, buffer, &bufferFilled, &windowFilled, &length, &location)){

			write_location_and_length(length, location, arq2);
			if(length < bufferFilled)
				fputc(buffer[length], arq2);

			scrollWindow(window, &windowFilled, length + 1, buffer, bufferFilled);
			scrollBuffer(buffer, &bufferFilled, length + 1, arq);
		
		}else{
			fputc(0, arq2);
			if(length < bufferFilled)
				fputc(buffer[length], arq2);

			scrollWindow(window, &windowFilled, 1, buffer, bufferFilled);
			scrollBuffer(buffer, &bufferFilled, 1, arq);
		}


	}

	free(buffer);
	free(window);
	fclose(arq);
	fclose(arq2);
}

int main(){
	
	compress("fileTest");

	return 0;
	
}
