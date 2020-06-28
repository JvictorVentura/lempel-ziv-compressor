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


void fillBuffer(char *buffer, FILE *arq, unsigned *bufferFilled){

	int c = ' ';
	
	for(unsigned i = *bufferFilled; i < BUFFER_SIZE; ++i){
		if((c = fgetc(arq))== EOF)
			break;
		else{
			buffer[i] = c;
			++(*bufferFilled);
		}
	}

}

void scrollWindow(char *window, unsigned *windowFilled, unsigned scrollSize, char *buffer){
	int k = 0;
	for (int i = scrollSize; i > 0; --i){

		if(*windowFilled == WINDOW_SIZE){
			for (int j = 0; j < WINDOW_SIZE; ++j){
				if( (j - i) >= 0)
					window[j - i] = window[j];
			}
			*windowFilled -= i;
		}else{
			window[(*windowFilled)++] = buffer[k++];
		}

	}
	
}

void scrollBuffer(char *buffer, unsigned *sizeOfBuffer, unsigned scrollSize, FILE *arq){
	for (int i = scrollSize; i < *sizeOfBuffer; ++i){
		buffer[i - scrollSize] = buffer[i];
	}

	*sizeOfBuffer -= scrollSize;

	fillBuffer(buffer, arq, sizeOfBuffer);
}


int main(){
	char buffer[BUFFER_SIZE];
	char window[WINDOW_SIZE];
	unsigned bufferFilled = 0;
	unsigned windowFilled = 0;
	unsigned scrollSize = 1;

	FILE *arq = fopen("fileTest", "r");

	if(arq != NULL){
		fillBuffer(buffer, arq, &bufferFilled);
	}

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);

//-------------------------------------------------------------
	scrollSize = 1;

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printf("\n\n");
	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);


//-------------------------------------------------------------
	scrollSize = 2;

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printf("\n\n");
	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);

//--------------------------------------------------------------

	scrollSize = 3;

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printf("\n\n");
	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);

//---------------------------------------------------------------

	scrollSize = 3;

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printf("\n\n");
	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);

//---------------------------------------------------------------

	scrollSize = 3;

	scrollWindow(window, &windowFilled, scrollSize, buffer);
	scrollBuffer(buffer, &bufferFilled, scrollSize, arq);

	printf("\n\n");
	printWindow(window, windowFilled);
	printf("\t\\\t");
	printBuffer(buffer, bufferFilled);

	fclose(arq);

	return 0;
}
