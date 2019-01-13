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

#define MAX_CODE_LENGTH 32
#define BITS_8  2048

int get_maximum_count_index(int* table, int N);
void reverse_string(char *str);
double calculate_codes();

int byte_count[BITS_8];

typedef struct symbol
{
	int upper_child_i; // child is to the left of the tree
	int lower_child_i;
    int parent_i; //parent is to the right of the tree
    double p; //probability of symbol
    uint8_t is_primitive;
	uint8_t primitive_symbol; //
	uint8_t code_length;
    uint32_t primitive_code;
	char primitive_code_string[MAX_CODE_LENGTH];
};

struct symbol symbols_table[BITS_8];
int symbols_count;

// Porównywacz
int compare_symbol_p (const void * a, const void * b)
{
    struct symbol *_a = (struct symbol*)a;
    struct symbol *_b = (struct symbol*)b;
    if(_a->p > _b->p) return -1;
    else return 1;
}

int compare_symbol_index_p (const void * a, const void * b)
{
    int* _a = (int*)a;
    int* _b = (int*)b;
    struct symbol *sa = &symbols_table[*_a];
    struct symbol *sb = &symbols_table[*_b];
    if(sa->p > sb->p) return -1;
    else return 1;
}

int main(int argc, char **argv) {
    clock_t start = clock();
	// printf("%i", sizeof(character_count)); zwr�ci sizeof(int) * BITS_8
	//memset(character_count, 0, sizeof(character_count));
    FILE * f_in;
    if(argc > 1)
    {
        f_in = fopen(argv[1], "rb");
    } else 
    {
        f_in = fopen("notatki.txt", "rb");    // otwiera plik do odczytu, tryb binarny
    }

    if (f_in == NULL) //  (musi istniec)
    {
        perror("Nie udalo sie otworzyc pliku");
        return 1;
    }

    printf("Plik otwarty pomyslnie!");

    uint8_t data_byte = 0;
    while (fread(&data_byte, 1, 1, f_in))
    {
    	byte_count[data_byte]++;
    }   //count number of occurences of all characters

    fseek(f_in, 0, 2); // (file, offset, mode) / mode=2 -> pos = eof - offset
    int x = ftell(f_in); //find size of file
    printf("Rozmiar pliku %i Bytes.\n", x);

    fseek(f_in,0,0); // mode = 0 -> pos = offset
    int index = get_maximum_count_index(byte_count, BITS_8); //find maximum count
    printf("Najwiecej jest: [%c/%i]\n", (char)index, byte_count[index] );

    

    /*Calculate Huffman codes of characters*/
    /*uses global variables:
        struct symbol symbols_table[BITS_8];
        int symbols_count;
        int byte_count[BITS_8];
    */
    double Lsr = calculate_codes();

    //use table: symbols_table, and var symbols_count to translate file and make a new file
    FILE *f_out;
    if(argc > 2)
    {
        f_out = fopen(argv[2], "wb");
    } else 
    {
        f_out = fopen("out.txt", "wb");    // otwiera plik do odczytu, tryb binarny
    }

    if (f_out == NULL) //  (musi istniec)
    {
        perror("Nie udalo sie utworzyc nowego pliku");
        return 1;
    }

    printf("Plik otwarty pomyslnie!");

    data_byte = 0;
    uint32_t read_size = 0;
    uint32_t write_buffer = 0;
    uint8_t buffer_i = 31;
    uint8_t symbol_index = 0;
    struct symbol *s;
    

    //laduj bajty z f_in, translatuj i wpisuj do f_out
    while (fread(&data_byte, 1, 1, f_in))
    {
        symbol_index = get_symbol_index(data_byte); //TODO
        s = &symbols_table[symbol_index];
    	write_buffer += (s->primitive_code << (buffer_i - s->code_length));
        //bedzie zapisywac 4bajty do pliku gdy zapelni sie 4 bajtowy bufor
        //byte_count[data_byte]++;
        read_size = fread(&data_byte,1,1,f_out);

    }   //count number of occurences of all characters

    fseek(f_out, 0, 2); // (file, offset, mode) / mode=2 -> pos = eof - offset
    x = ftell(f_out); //find size of file
    printf("Rozmiar pliku wyjsciowego %i Bytes.\n", x);

    fseek(f_out,0,0); // mode = 0 -> pos = offset

    fclose(f_out);
    fclose(f_in);

    printf("Lsr: %f\n", Lsr);
    clock_t end = clock();

    double cpu_time_used = ((double) (end - start)) / (double)CLOCKS_PER_SEC;
    printf("for loop took %f seconds to execute \n", cpu_time_used);

	return EXIT_SUCCESS;
}

double calculate_codes()
{
    /*find sum of all character occurences*/
    uint64_t sum = 0;
    for(int i = 0; i < BITS_8; i++)
    {
    	//printf("[%c/%i]\n", (char)i, byte_count[i]);
    	sum = sum + byte_count[i];
    }

    printf("sum: %u\n", sum);

    double entropy = 0;
    double p = 0; //probability
    symbols_count = 0; //how much various symbols

    /*table with indexes of symbols in symbols_table, used for sorting and joining symbols
    for Huffman algorithm*/
    int symbols_index_table[BITS_8]; 

    /*Find symbols probability and symbols count with nonzero probability*/
    for(int i = 0; i < BITS_8; i++)
    {
    	if(byte_count[i] != 0)
		{
			p =  (double)byte_count[i]/(double)sum;
		    //printf("p: %c, %f\n", (char)i, p);
			entropy = entropy + log2(p)*p;

            symbols_table[symbols_count].is_primitive = 1;
            symbols_table[symbols_count].primitive_symbol = (uint8_t)i;
            symbols_table[symbols_count].p = p;
            symbols_index_table[symbols_count] = symbols_count;
            symbols_count++;
		}
    }
    entropy = -entropy;
    printf("Hmax: %f\n", log2(symbols_count));
    printf("entropia: %f\n", entropy);

    
    // KODOWANIE SYMBOLI: START

    //sortowanie od najwiekszych prawdopodobienstw do najmniejszych
    //qsort(symbols_table, symbols_count, sizeof(struct symbol), compare_symbol_p);
    int symbols_new_index = symbols_count;
    int symbols_temp_count = symbols_count;

    while(symbols_temp_count > 1)
    {
        qsort(symbols_index_table, symbols_temp_count, sizeof(int), compare_symbol_index_p);
        
        struct symbol * s = &symbols_table[symbols_new_index];

        s->is_primitive = 0;
        s->upper_child_i = symbols_index_table[symbols_temp_count-2];
        symbols_table[s->upper_child_i].parent_i = symbols_new_index;
        s->lower_child_i = symbols_index_table[symbols_temp_count-1];
        symbols_table[s->lower_child_i].parent_i = symbols_new_index;

        s->p = symbols_table[s->upper_child_i].p + symbols_table[s->lower_child_i].p;
        symbols_index_table[symbols_temp_count-2] = symbols_new_index; 
        symbols_temp_count--;
        symbols_new_index++;
    }

    /*for(int i = 0; i < symbols_new_index; i++)
    {
        printf("p: %f\n", symbols_table[i].p);
    }*/
    // KODOWANIE SYMBOLI: END

    int child_i;
    struct symbol *primitive, *s, *parent;
    double Lsr = 0;
    for(int i = 0; i < symbols_count; i++)
    {
        s = &symbols_table[i]; 
        primitive = s;
        printf("%c[%f]: ", s->primitive_symbol, s->p);
        p = s->p;
        child_i = i;
        while(s->parent_i != 0)
        {
            parent = &symbols_table[s->parent_i];
            
            if(parent->upper_child_i == child_i)
            {
                primitive->primitive_code_string[primitive->code_length] = '1';
                primitive->primitive_code+= (1 << primitive->code_length);
            }
            else if(parent->lower_child_i == child_i)
            {
                primitive->primitive_code_string[primitive->code_length] = '0';
            }

            child_i = s->parent_i;
            primitive->code_length++;
            s = parent;
        }
        reverse_string(primitive->primitive_code_string);
        printf("%s\n",primitive->primitive_code_string);
        Lsr=Lsr+primitive->code_length*p;
    }

    return Lsr;
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

void reverse_string(char *str)
{
    /* skip null */
    if (str == 0)
    {
        return;
    }

    /* skip empty string */
    if (*str == 0)
    {
        return;
    }

    /* get range */
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    /* reverse */
    while (end > start)
    {
        /* swap */
        temp = *start;
        *start = *end;
        *end = temp;

        /* move */
        ++start;
        --end;
    }
}