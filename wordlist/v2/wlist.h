#include <iostream>
#include <string>
#include "../../runic/runic.h"  // runic
#include "superNode.h" // superNode

// preprocessor statements
#ifndef WLIST_H
#define WLIST_H

using namespace std;

void insert_item(runic_t &r, string value);
int insert_base_val(runic_t &r, superNode node, string value);
void lookup_item(runic_t r, string value);
int lookup_next_val(superNode node, string value);

#endif /* wlist.h */
