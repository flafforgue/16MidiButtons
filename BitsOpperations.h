
#ifndef BITSOPPERATIONS
#define BITSOPPERATIONS

void ToggleMask(unsigned int *var, unsigned int mask) {
  *var = *var ^ mask;
}

void ToggleBit(unsigned int *var, byte nobit) {
  unsigned int mask = ( 1 << nobit );
  *var = *var | mask;
}

void SetBit(unsigned int *var, byte nobit) {
  unsigned int mask = ( 1 << nobit );
  *var = *var | mask;
}

void ClearBit(unsigned int *var, byte nobit) {
  unsigned int mask = ( 1 << nobit );
  *var = *var & ~mask;
}

#endif