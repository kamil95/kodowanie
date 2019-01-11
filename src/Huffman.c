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
int get_maximum_count_index(int* table, int N);

int character_count[256];

int main(void) {
    clock_t start = clock();
	// printf("%i", sizeof(character_count)); zwróci sizeof(int) * 256
	//memset(character_count, 0, sizeof(character_count));

    FILE *f = fopen("notatki.txt", "r");    // otwiera plik do odczytu (musi istniec)

    if (f == NULL)
    {
        perror("Nie udalo sie otworzyc pliku notatki.txt");
        return 1;
    }
    puts("Plik otwarty pomyslnie!");



    char character = 0;
    do {
    	character = fgetc(f);
    	character_count[(uint8_t)character]++;
    } while (character != EOF);



    fseek(f, 0, 2); // (file, offset, mode) / mode=2 -> pos = eof - offset
    int x = ftell(f);
    printf("Rozmiar pliku %i Bytes.\n", x);
    fseek(f,0,0); // mode = 0 -> pos = offset
    int index = get_maximum_count_index(character_count, 256);
    printf("Ilosc: a: %i, Spacji: %i, z: %i, G: %i\n", character_count[(int)'a'], character_count[(int)' '], character_count[(int)'z'], character_count[(int)'G']);
    printf("Najwiecej jest: [%c/%i]\n", (char)index, character_count[index] );

    fclose(f);

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
