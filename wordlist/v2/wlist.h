#include <iostream>
#include <string>
#include <vector>
#include "../../runic/runic.h"  // runic
#include "superNode.h"

// preprocessor statements
#ifndef WLIST_H
#define WLIST_H

using namespace std;

int insert_base_val(runic_t &r, superNode node, string value);
void insert_item(runic_t &r, string value);
int lookup_next_val(superNode node, string value);
void lookup_item(runic_t r, string value);

#endif /* wlist.h */
