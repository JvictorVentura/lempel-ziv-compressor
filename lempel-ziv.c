#include <stdio.h>
#include <stdlib.h>
#define WINDOW_SIZE 8/*2048*/
#define BUFFER_SIZE 4/*29*/
#define MAX_FILENAME_LENGTH 55
#include <stdint.h>
#include <string.h>
//enum Command { COMPRESS, DECOMPRESS};

void printWindow(char *window, uint16_t sizeOfWindowFilled){
	for(uint16_t i = 0; i < sizeOfWindowFilled; ++i)
		printf("%c", window[i]);

}

void printBuffer(char *buffer , uint16_t sizeOfBuffer){
	for(uint16_t i = 0; i < sizeOfBuffer; ++i)
		printf("%c", buffer[i]);

}


void fillBuffer(char *buffer, FILE *base_file, uint16_t *bufferFilled){

	int c = ' ';
	
	for(uint16_t i = *bufferFilled; i < BUFFER_SIZE; ++i){
		if((c = fgetc(base_file))== EOF)
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

void scrollBuffer(char *buffer, uint16_t *sizeOfBuffer, uint16_t scrollSize, FILE *base_file){
	for (int i = scrollSize; i < *sizeOfBuffer; ++i){
		buffer[i - scrollSize] = buffer[i];
	}

	
	if(scrollSize > *sizeOfBuffer)
		*sizeOfBuffer = 0;
	else
		*sizeOfBuffer -= scrollSize;

	fillBuffer(buffer, base_file, sizeOfBuffer);
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

void write_location_and_length(uint16_t length, uint16_t location, FILE *base_file){
	uint8_t uint8_tWritten = 0;
	for(int i = 16; i > 0; i >>= 1){
		uint8_tWritten += length & i;
	}

	uint8_tWritten <<= 3;
	int index = 4;

	for(int i = 1024; i > 0; i >>= 1){

		if(index == 0){
			fputc(uint8_tWritten, base_file);
			index = 128;
			uint8_tWritten = 0;
		}

		if( (location & i) == i){
			uint8_tWritten = index;
		}

		index >>= 1;
	}

	fputc(uint8_tWritten, base_file);
}

char *add_extension_filename(char *current_filename){
	uint8_t filename_length = strlen(current_filename);
	printf("p1\n");
	int8_t dot_position = -1;		//-1 means it does not have a dot on the filename
	
	for(uint8_t i = 0; i < filename_length; ++i){
		if(current_filename[i] == '.'){
			dot_position = i;
			break;
		}
	}

	static char new_filename[MAX_FILENAME_LENGTH];

	if(dot_position >= 1){
		strncpy(new_filename, current_filename, dot_position);
	}else{
		strcpy(new_filename, current_filename);
	}
	
	strcat(new_filename, ".lz");

	return new_filename;

}

void compress(char *file_to_be_compressed, char *name_of_compressed_file){

	char *buffer = malloc(BUFFER_SIZE);
	char *window = malloc(WINDOW_SIZE);
	uint16_t bufferFilled = 0;
	uint16_t windowFilled = 0;

	uint16_t length = 0;
	uint16_t location = 0;

	FILE *base_file = fopen(file_to_be_compressed, "r");

	if(base_file != NULL){
		fillBuffer(buffer, base_file, &bufferFilled);
	}else{
		printf("Arquivo não existe\n");
		return;
	}

	printf("%s\n\n", name_of_compressed_file);	
	FILE *compressed_file = NULL; 
	if(name_of_compressed_file != NULL){
		compressed_file = fopen(name_of_compressed_file, "w");
	}else{
		compressed_file = fopen(add_extension_filename(file_to_be_compressed), "w");
	}

	if(compressed_file == NULL){
		printf("Erro ao criar base_fileuivo\n");
			return;
	}


	while(bufferFilled > 0){

		/*
		printf("\n");
		printWindow(window, windowFilled);
		printf("\t|\t");
		printBuffer(buffer, bufferFilled);

		printf("\t (%d,%d)\n", length, location);
		*/

		if(searchOnDictionary(window, buffer, &bufferFilled, &windowFilled, &length, &location)){

			write_location_and_length(length, location, compressed_file);
			if(length < bufferFilled)
				fputc(buffer[length], compressed_file);

			scrollWindow(window, &windowFilled, length + 1, buffer, bufferFilled);
			scrollBuffer(buffer, &bufferFilled, length + 1, base_file);
		
		}else{
			fputc(0, compressed_file);
			if(length < bufferFilled)
				fputc(buffer[length], compressed_file);

			scrollWindow(window, &windowFilled, 1, buffer, bufferFilled);
			scrollBuffer(buffer, &bufferFilled, 1, base_file);
		}


	}

	free(buffer);
	free(window);
	fclose(base_file);
	fclose(compressed_file);
}

// WTF i thought i implemented the decompressed function, maybe i lost that version, damn
void decompress(char *file_to_be_decompressed, char *name_of_decompressed_file){

}

int main(int argc, char *argv[]){
	if(argc <= 2)		printf("Comando inválido\n");
	else{

		if(strcmp(argv[1], "-c") == 0){
			if(argc > 3)
				compress(argv[2], argv[3]);
			else
				compress(argv[2], NULL);
		}

		if(strcmp(argv[1], "-d") == 0){
			//todo
		}

	}

	

	return 0;
	
}
