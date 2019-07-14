#include <iostream>
#include <string>
#include "../../runic/runic.h"  // runic

// preprocessor statements
#ifndef WLIST_H
#define WLIST_H

using namespace std;

int insert_next_val(runic_t &r, string value);
void insert_item(runic_t &r, string value);
int lookup_next_val(runic_t r, string value);
void lookup_item(runic_t r, string value);

#endif /* wlist.h */