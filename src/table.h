#ifndef CLOX_HASH_TABLE_C
#define CLOX_HASH_TABLE_C

#include "common.h"
#include "value.h"

typedef struct {
    ObjString *key;
    Value value;
} Entry;

typedef struct {
    usize count;
    usize capacity;
    Entry *entries;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
bool tableSet(Table *table, ObjString *key, Value value);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableDelete(Table *table, ObjString *key);
void tableAddAll(Table const *from, Table *to);
ObjString *tableFindString(Table *table, const char *chars, usize length, u32 hash);
void tableRemoveWhite(Table *);
void markTable(Table *table);

#endif
