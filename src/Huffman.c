/*
 ============================================================================
 Name        : Huffman.c
 Author      : Kamil Trzebiatowski
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define MAX_CODE_LENGTH 128

int get_maximum_count_index(int* table, int N);

int byte_count[256];

typedef struct symbol
{
	struct symbol* upper;
	struct symbol* lower;
	uint8_t primitive_symbol;
	int is_primitive;
	double p;
	int code_length;
	char code[MAX_CODE_LENGTH];
};

struct symbol symbol_table[256];
int symbols_count;

int main(void) {
    clock_t start = clock();
	// printf("%i", sizeof(character_count)); zwróci sizeof(int) * 256
	//memset(character_count, 0, sizeof(character_count));

    FILE *f = fopen("notatki.txt", "rb");    // otwiera plik do odczytu, tryb binarny

    if (f == NULL) //  (musi istniec)
    {
        perror("Nie udalo sie otworzyc pliku notatki.txt");
        return 1;
    }
    puts("Plik otwarty pomyslnie!");


    uint8_t data_byte = 0;
    while (fread(&data_byte, 1, 1, f))
    {
    	byte_count[data_byte]++;
    }



    fseek(f, 0, 2); // (file, offset, mode) / mode=2 -> pos = eof - offset
    int x = ftell(f);
    printf("Rozmiar pliku %i Bytes.\n", x);
    fseek(f,0,0); // mode = 0 -> pos = offset
    int index = get_maximum_count_index(byte_count, 256);
    printf("Ilosc: a: %i, Spacji: %i, z: %i, G: %i\n", byte_count[(int)'a'], byte_count[(int)' '], byte_count[(int)'z'], byte_count[(int)'G']);
    printf("Najwiecej jest: [%c/%i]\n", (char)index, byte_count[index] );

    fclose(f);

    uint64_t sum = 0;
    for(int i = 0; i < 256; i++)
    {
    	//printf("[%c/%i]\n", (char)i, byte_count[i]);
    	sum = sum + byte_count[i];
    }

    printf("sum: %u\n", sum);

    double entropy = 0;
    double p = 0;
    int nonzero = 0;
    for(int i = 0; i < 256; i++)
    {
    	if(byte_count[i] != 0)
		{
			p =  (double)byte_count[i]/(double)sum;
		    printf("p: %c, %f\n", (char)i, p);
			entropy = entropy + log2(p)*p;
			nonzero++;
		}
    }
    entropy = -entropy;
    printf("Hmax: %f\n", log2(nonzero));
    printf("entropia: %f\n", entropy);



    clock_t end = clock();

    double cpu_time_used = ((double) (end - start)) / (double)CLOCKS_PER_SEC;
    printf("for loop took %f seconds to execute \n", cpu_time_used);

	return EXIT_SUCCESS;
}

int get_maximum_count_index(int* table, int N)
{
	int index = 0;
	int max = table[0];
	for(int i = 0; i < N; i++)
	{
		if(table[i] > max)
		{
			index = i;
			max = table[i];
		}
	}
	return index;
}
