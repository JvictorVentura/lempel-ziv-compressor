#include <stdio.h>
#include <stdlib.h>
#include<stdint.h>
#define WINDOW_SIZE /*8*/2048
#define BUFFER_SIZE /*4*/29
#define MAX_FILENAME_LENGTH 55
#include <string.h>

//TODO clean later

void printWindow(char *window, uint16_t sizeOfwindow_usage_count){
	for(uint16_t i = 0; i < sizeOfwindow_usage_count; ++i)
		printf("%c", window[i]);

}

void printBuffer(char *buffer , uint16_t sizeOfBuffer){
	for(uint16_t i = 0; i < sizeOfBuffer; ++i)
		printf("%c", buffer[i]);

}


void fillBuffer(char *buffer, FILE *arq, uint16_t *bufferFilled){//fill the buffer when there is space

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

void fillWindow(char *window, char *buffer, uint16_t *window_usage_count, int sizeToFill){//fill the window with a specified number of characters
	for(int i = 0; i < sizeToFill; ++i){
		window[(*window_usage_count)++] = buffer[i];
	}
}

void scrollWindow(char *window, uint16_t *window_usage_count, int scrollSize){//push the characters to the begining of the window by a especified number of characters
	for (int i = 0; i < WINDOW_SIZE; ++i){
		if( (i - scrollSize) >= 0)
			window[i - scrollSize] = window[i];
	}
	*window_usage_count -= scrollSize;
}

void scrollAndFillWindow(char *window, uint16_t *window_usage_count, int scrollSize, char *buffer, uint16_t bufferFilled){
	if(scrollSize > bufferFilled)//check to see if the scrollSize is incorrect
		scrollSize = bufferFilled;

	
	if( (WINDOW_SIZE - *window_usage_count) < scrollSize)//if there is no space left in the window it scrolls to make space to fill with new charaters from the buffer
		scrollWindow(window, window_usage_count, scrollSize - (WINDOW_SIZE - *window_usage_count));
	
	fillWindow(window, buffer, window_usage_count, scrollSize);//put a specified number of characters of buffer on window
}

void scrollBuffer(char *buffer, uint16_t *sizeOfBuffer, uint16_t scrollSize){//scroll the buffer array by a determined number of characters
	for (int i = scrollSize; i < *sizeOfBuffer; ++i){
		buffer[i - scrollSize] = buffer[i];
	}

	//subtract the size scrolled from the size of the buffer, to keep track of it
	if(scrollSize > *sizeOfBuffer)
		*sizeOfBuffer = 0;
	else
		*sizeOfBuffer -= scrollSize;
}

void scrollAndFillBuffer(char *buffer, uint16_t *sizeOfBuffer, uint16_t scrollSize, FILE *arq){
	scrollBuffer(buffer, sizeOfBuffer, scrollSize);

	fillBuffer(buffer, arq, sizeOfBuffer);
}


int8_t searchOnDictionary(char *window, char *buffer, uint16_t *bufferFilled, uint16_t *window_usage_count, int *length, uint16_t *location){
	uint16_t longest_sequence = 0;
	uint16_t location_longest_sequence = 0;

	uint16_t current_Sequence = 0;
	uint16_t location_current_sequence = 0;

	int buffer_index = 0;

	for(int window_index = 0 ; window_index < *window_usage_count; ++window_index){

		if( buffer_index == (BUFFER_SIZE -1) )
			break;

		if(current_Sequence == 0){
			if(window[window_index] == buffer[buffer_index]){
				location_current_sequence = window_index;
				++current_Sequence;
				++buffer_index;
			}
		}else{
			if(window[window_index] == buffer[buffer_index]){
				++current_Sequence;
				++buffer_index;
			}else{
				if(current_Sequence > longest_sequence){
					longest_sequence = current_Sequence;
					location_longest_sequence = location_current_sequence;
				}
				current_Sequence = 0;
				buffer_index = 0;
			}
		}

	}	

	if(current_Sequence > longest_sequence){
		longest_sequence = current_Sequence;
		location_longest_sequence = location_current_sequence;
	}

	*length = longest_sequence;
	*location = location_longest_sequence;

	if(longest_sequence > 0)
		return 1;
	else
		return 0;
}

void newWriteLocationAndLength(uint16_t length, uint16_t location, FILE *arq){
	uint16_t bitMask = 1024;
	uint16_t output = length << 11;

	for(int8_t i = 3; i > 0; --i){
		output = output | (bitMask & location);
		bitMask >>= 1;
	}

	fputc(output >> 8, arq);

	output = location;

	fputc(output, arq);

}

void writeOriginalExtensionOnFile(FILE *compressedFile, char *fileName, int sizeOfExtension, int  extensionLocation){

	fputc(sizeOfExtension, compressedFile);
	if(sizeOfExtension > 0)
		for(int i = 0; i < sizeOfExtension; ++i)
			fputc(fileName[extensionLocation + i], compressedFile);
		
	
}

void writeMagicNumberOnFile(FILE *compressedFile){
	char magicNumber[2] = {'L', 'Z'};
	for(int i = 0; i < 4; ++i)
		fputc(magicNumber[i], compressedFile);
}

char *add_extension_filename(char *current_filename){
	uint8_t filename_length = strlen(current_filename);
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


void compress(char *base_file_filename, char *output_file_filename){

	char *buffer = malloc(BUFFER_SIZE);
	char *window = malloc(WINDOW_SIZE);
	uint16_t bufferFilled = 0;
	uint16_t window_usage_count = 0;

	int length = 0;
	uint16_t location = 0;

	FILE *base_file = fopen(base_file_filename, "r");

	if(base_file != NULL){
		fillBuffer(buffer, base_file, &bufferFilled);
	}else{
		printf("File does not exist\n");
		return;
	}

	FILE *output_file = NULL; 
	if(output_file_filename != NULL){
		output_file = fopen(output_file_filename, "w");
	}else{
		output_file = fopen(add_extension_filename(base_file_filename), "w");
	}

	if(output_file == NULL){
		printf("Cannot create file\n");
			return;
	}


	while(bufferFilled > 0){
		
		if(searchOnDictionary(window, buffer, &bufferFilled, &window_usage_count, &length, &location)){

			newWriteLocationAndLength(length, location, output_file);
			if(length < bufferFilled)
				fputc(buffer[length++], output_file);

			scrollAndFillWindow(window, &window_usage_count, length , buffer, bufferFilled);
			scrollAndFillBuffer(buffer, &bufferFilled, length , base_file);
		

		}else{
			fputc(0, output_file);
			if(length < bufferFilled)
				fputc(buffer[length], output_file);

			length = 1;
			scrollAndFillWindow(window, &window_usage_count, 1, buffer, bufferFilled);
			scrollAndFillBuffer(buffer, &bufferFilled, 1, base_file);


		}

	}

	free(buffer);
	free(window);
	fclose(base_file);
	fclose(output_file);
}



void extractLengthAndLocation(int rawByte, uint16_t *length, uint16_t *location){
	int rawMask = 32768;
	int extractedMask = 16;

	while( rawMask > 1024 ){
		if( (rawByte & rawMask) == rawMask )
			*length += extractedMask;

		rawMask >>= 1;
		extractedMask >>= 1;
	}

	while( rawMask > 0 ){
		if( (rawByte & rawMask) == rawMask )
			*location += rawMask;

		rawMask >>= 1;
	}

}

void decompress(char *base_file_filename, char *output_file_filename){
	if(base_file_filename == NULL) return;

	FILE *base_file = fopen(base_file_filename, "r");
	
	if(base_file == NULL){
		printf("File does not exist\n");
		return;
	}

	FILE *output_file; 
	if(output_file_filename != NULL){
		output_file = fopen(output_file_filename, "w");
	}else{
		output_file = fopen("decompression_output", "w");
	}

	if(output_file == NULL){
		printf("Could not create file\n");
		return;
	}

	
	char *window = malloc(WINDOW_SIZE);
	char *buffer = malloc(BUFFER_SIZE);
	uint16_t window_usage_count = 0;
	int bufferFilled = 0;

	uint16_t length = 0;
	uint16_t location = 0;
	int charBuffer = 0;

	while( (charBuffer = fgetc(base_file)) != EOF ){
	
		if(charBuffer == 0){


			charBuffer = fgetc(base_file);
			length = 1;
			bufferFilled = 1;

			buffer[0] = charBuffer;
			fputc(charBuffer, output_file);

		}else{
		
			charBuffer <<= 8;
			charBuffer += fgetc(base_file);

			extractLengthAndLocation( charBuffer, &length, &location);


			for(int i = 0; i < length; ++i){
				buffer[i] = window[location + i];
			}

			
			if( (charBuffer = fgetc(base_file)) != EOF)	//check if there is a next character and its not the EOF, because some files end with only refences to the window
				buffer[length++] = charBuffer;

			bufferFilled = length;
			

			for(int i = 0; i < bufferFilled; ++i){
				//printf("%c", buffer[i]);
				fputc(buffer[i], output_file);
			}
		}

		scrollAndFillWindow( window, &window_usage_count, length, buffer, length);
		
		bufferFilled = 0;
		location = 0;
		length = 0;

				
	}

	free(window);
	free(buffer);
	fclose(base_file);
	fclose(output_file);
}






int main(int argc, char *argv[]){
	if(argc <= 2)
		printf("Invalid command\n");
	else{

		if(strcmp(argv[1], "-c") == 0){
			if(argc > 3)
				compress(argv[2], argv[3]);
			else
				compress(argv[2], NULL);
		}else if(strcmp(argv[1], "-d") == 0){
			if(argc > 3)
				decompress(argv[2], argv[3]);
			else
				decompress(argv[2], NULL);
		}

	}
	return 0;
	
}
