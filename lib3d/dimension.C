// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <iostream>
#include "dimension.h"

using namespace std;

/************************************************************************/
ostream& operator<<(ostream& out, const dimension& d) {
  out << d.value << ":" << d.prop;
  if (d.prop != dimension::unique)
    return out << ":" << *(d.refDim);
  else 
    return out << "  ";
}
