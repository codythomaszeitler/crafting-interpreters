#ifndef CHUNK_HEADER
#define CHUNK_HEADER
typedef struct Chunk {
    uint8_t* code;  
    int count;
    int capacity;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t value);
void freeChunk(Chunk* chunk);

#endif