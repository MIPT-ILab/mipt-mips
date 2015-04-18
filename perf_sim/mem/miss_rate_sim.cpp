#include <iostream>
#include <stdio.h>
#include <cache_tag_array.h>
#include <math.h>

#define COLUMNS 11
#define ROWS 6
#define Kb 1024

float test_cache(char* filename, CacheTagArray * cache){
    FILE *input = fopen(filename,"r");
    float counter = 0;
    float hits = 0;
    unsigned int address;

    while (fscanf(input, "%x ", &address) != EOF) {
        if(cache->read(address)) hits ++;
        counter ++;
    }
    return (1 - hits/counter);
}

int main(int argc, char* argv[])
{
    int counter = 0;
    unsigned int address;

    CacheTagArray * Cache[ROWS][COLUMNS];
    unsigned int Hits[ROWS][COLUMNS];

    FILE* output;

    if(argc == 3) {
        output = fopen(argv[2],"w");
    } else {
        return 0;
    }

    int sizes[COLUMNS];
    sizes[0] = Kb;
    for(int i = 1; i < COLUMNS; i ++) sizes[i] = sizes[i-1]*2;

    int ways[ROWS - 1];
    ways[0] = 1;
    for(int i = 1; i < ROWS - 1; i ++) ways[i] = ways[i-1]*2;

    for(int i = 0; i < ROWS - 1; i ++){
        for(int j = 0; j < COLUMNS; j ++){
            float miss_rate = test_cache(argv[1], new CacheTagArray(sizes[j], ways[i], 4, 32));
            fprintf(output, "%f, ", miss_rate);
        }
        fprintf(output, "\n");
    }

    for(int i = 0; i < COLUMNS; i ++){
        float miss_rate = test_cache(argv[1], new CacheTagArray(sizes[i], sizes[i]/4, 4, 32));
        fprintf(output, "%f, ", miss_rate);
    }

    return 0;
}
