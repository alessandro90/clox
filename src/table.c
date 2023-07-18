#include "table.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>

#define TABLE_MAX_LOAD 0.75F

void initTable(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

static Entry *findEntry(Entry *entries, usize capacity, ObjString *key) {
    usize index = key->hash % capacity;
    Entry *tombstone = NULL;
    while (true) {
        Entry *entry = &entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
            }
            if (tombstone == NULL) { tombstone = entry; }
        } else if (entry->key == key) {
            return entry;
        }
        index = (index + 1U) % capacity;
    }
}

static void adjustCapacity(Table *table, usize capacity) {
    Entry *entries = ALLOCATE(Entry, capacity);
    for (usize i = 0; i < capacity; ++i) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }
    table->count = 0;
    for (usize i = 0; i < table->capacity; ++i) {
        Entry *entry = &table->entries[i];
        if (entry->key == NULL) {
            continue;
        }
        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        ++table->count;
    }
    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

void freeTable(Table *table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

bool tableSet(Table *table, ObjString *key, Value value) {
    if ((float)(table->count + 1U) > (float)table->capacity * TABLE_MAX_LOAD) {
        usize const capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }
    Entry *entry = findEntry(table->entries, table->capacity, key);
    bool const isNewKey = entry->key == NULL;
    if (isNewKey && IS_NIL(entry->value)) { ++table->count; }

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool tableGet(Table *table, ObjString *key, Value *value) {
    if (table->count == 0) { return false; }
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry == NULL) { return false; }
    *value = entry->value;
    return true;
}


bool tableDelete(Table *table, ObjString *key) {
    if (table->count == 0) { return false; }
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == NULL) { return false; }
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
}

void tableAddAll(const Table *from, Table *to) {
    for (usize i = 0; i < from->capacity; ++i) {
        Entry *entry = &from->entries[i];
        if (entry->key != NULL) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

ObjString *tableFindString(Table *table, const char *chars, usize length, u32 hash) {
    if (table->count == 0) { return NULL; }

    usize index = hash % table->capacity;
    while (true) {
        Entry *entry = &table->entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) { return NULL; }
        } else if (entry->key->length == length
                   && entry->key->hash == hash
                   && memcmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }
        index = (index + 1U) % table->capacity;
    }
}

void tableRemoveWhite(Table *table) {
    for (usize i = 0; i < table->capacity; ++i) {
        Entry *entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.isMarked) {
            tableDelete(table, entry->key);
        }
    }
}

void markTable(Table *table) {
    for (usize i = 0; i < table->capacity; ++i) {
        Entry *entry = &table->entries[i];
        markObject((Obj *)entry->key);
        markValue(entry->value);
    }
}
