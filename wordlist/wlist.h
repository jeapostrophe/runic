#include <iostream>
#include "../runic/runic.h"  // runic

using namespace std;

int insert_next_val(runic_t* r, const char* value);
void insert_item(runic_t* r, const char* value);
int lookup_next_val(runic_t r, const char* value);
void lookup_item(runic_t r, const char* value);
