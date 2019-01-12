/*
 ============================================================================
 Name        : Huffman.c
 Author      : Kamil Trzebiatowski
 Version     : 0.2
 Copyright   : GNU GPL
 Description : Huffman coding
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
	int upper_child_i; // child is to the left of the tree
	int lower_child_i;
    int parent_i; //parent is to the right of the tree
    double p; //probability of symbol
    uint8_t is_primitive;
	uint8_t primitive_symbol; //
	uint8_t code_length;
	//char code[MAX_CODE_LENGTH];
};

struct symbol symbols_table[256];
int symbols_count;

// Porównywacz
int compare_symbol_p (const void * a, const void * b)
{
    struct symbol *_a = (struct symbol*)a;
    struct symbol *_b = (struct symbol*)b;
    if(_a->p > _b->p) return -1;
    else return 1;
}

int main(void) {
    clock_t start = clock();
	// printf("%i", sizeof(character_count)); zwr�ci sizeof(int) * 256
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
    symbols_count = 0;

    for(int i = 0; i < 256; i++)
    {
    	if(byte_count[i] != 0)
		{
			p =  (double)byte_count[i]/(double)sum;
		    printf("p: %c, %f\n", (char)i, p);
			entropy = entropy + log2(p)*p;

            symbols_table[symbols_count].is_primitive = 1;
            symbols_table[symbols_count].primitive_symbol = (uint8_t)i;
            symbols_table[symbols_count].p = p;
            symbols_count++;
		}
    }
    entropy = -entropy;
    printf("Hmax: %f\n", log2(symbols_count));
    printf("entropia: %f\n", entropy);

    
    // KODOWANIE SYMBOLI: START
    int symbols_temp_count = symbols_count;

    qsort(symbols_table, symbols_count, sizeof(struct symbol), compare_symbol_p);
    //sortowanie od najwiekszych prawdopodobienstw do najmniejszych

    // KODOWANIE SYMBOLI: END


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
