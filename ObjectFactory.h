#ifndef _H_ObjectFactory
#define _H_ObjectFactory

#include "ObjectCell.h"

// This was moved to scope.h
// enum { CT_Trinkets = 0, CT_Stones, CT_Flock, CT_Spyro, CT_Peppers, CT_Flowers, CT_Fruits, CT_Vegies, kNbrCellTypes };
class ScopeApp;
ObjectCell *getObjectCell(int cellType, ScopeApp *parent);



#endif

