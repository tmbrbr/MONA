/*
 * MONA
 * Copyright (C) 1997-2013 Aarhus University.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the  Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335,
 * USA.
 */

#include <stdlib.h>
#include "dfa.h"
#include "../Mem/mem.h"

DFA *dfaTrue()
{
  DFABuilder *b = dfaSetup(2, 0, NULL);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  /* state 1 */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  return dfaBuild(b, "0+");
}

DFA *dfaFalse()
{
  DFABuilder *b = dfaSetup(2, 0, NULL);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  /* state 1 */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  return dfaBuild(b, "0-");
}

DFA *dfaSingleton(int i)
{
  int var_index[1];
  var_index[0] = i;
  
  DFABuilder *b = dfaSetup(4, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  /* state 1 */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 2, "1");
  dfaStoreState(b, 1);

  /* state 2 */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 3, "1");
  dfaStoreState(b, 2);

  /* state 3 */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 3);

  return dfaBuild(b, "0-+-");
}

DFA *dfaEq2(int i, int j)
{
  if (i == j) 
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(3, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);

    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 1, "11");
    dfaStoreState(b, 2);

    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
}

DFA *dfaSubset(int i, int j)
{
  if (i == j) 
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(3, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);

    /* state 1 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 2, "10");
    dfaStoreState(b, 1);

    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
}

DFA *dfaEmpty(int i)
{
  int var_index[1];
  var_index[0] = i;

  DFABuilder *b = dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);
  
  /* state 1 */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "0");
  dfaStoreState(b, 2);
    
  /* state 2 */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);

  return dfaBuild(b, "0+-");
}

DFA *dfaPlus2(int i, int j)
{
  if (i == j)
    return dfaEmpty(i); /* Pi = Pi+1 iff Pi = empty */
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(4, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 2, "01");
    dfaStoreState(b, 3);
    
    /* state 2 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 3, "0X");
    dfaStoreException(b, 1, "10");
    dfaStoreState(b, 2);
    
    /* state 3 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    return dfaBuild(b, "0+--");
  }    
}

DFA *dfaMinus2(int i, int j)
{
  if (i == j) {
    int var_index[1];
    var_index[0] = i;

    DFABuilder *b = dfaSetup(4, 1, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    /* state 2 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 2, "0");
    dfaStoreState(b, 3);
    
    /* state 3 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    return dfaBuild(b, "0++-");
  }
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;    

    DFABuilder *b = dfaSetup(6, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 3, "00");
    dfaStoreException(b, 4, "10");
    dfaStoreException(b, 2, "11");
    dfaStoreState(b, 5);
        
    /* state 2 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 4, "1X");
    dfaStoreState(b, 3);
        
    /* state 3 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 3, "00");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 5);
    
    /* state 4 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 4, "11");
    dfaStoreException(b, 3, "01");
    dfaStoreState(b, 5);
            
    /* state 5 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 5);
    
    return dfaBuild(b, "0+++--");
  }
}

DFA *dfaUnion(int i, int j, int k)
{
  if (i == j)
    return dfaSubset(k, i);
  else if (i == k)
    return dfaSubset(j, i);
  else if (j == k)
    return dfaEq2(i, j);
  else {
    int var_index[3];
    var_index[0] = i;
    var_index[1] = j;
    var_index[2] = k;

    DFABuilder *b = dfaSetup(3, 3, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 1, "000");
    dfaStoreException(b, 1, "1X1");
    dfaStoreException(b, 1, "110");
    dfaStoreState(b, 2);
    
    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
}

DFA *dfaInter(int i, int j, int k)
{
  if (i == j)
    return dfaSubset(i, k);
  else if (i == k)
    return dfaSubset(i, j);
  else if (j == k)
    return dfaEq2(i, j);
  else {
    int var_index[3];
    var_index[0] = i;
    var_index[1] = j;
    var_index[2] = k;

    DFABuilder *b = dfaSetup(3, 3, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 1, "111");
    dfaStoreException(b, 1, "00X");
    dfaStoreException(b, 1, "010");
    dfaStoreState(b, 2);
    
    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
}

DFA *dfaSetminus(int i, int j, int k)
{
  if (j == k || i == k)
    return dfaEmpty(i);
  else if (i == j) {    /* make: k inter i = empty */
    int var_index[2];
    var_index[0] = i;
    var_index[1] = k;

    DFABuilder *b = dfaSetup(3, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1,"0X");
    dfaStoreException(b, 1,"10");
    dfaStoreState(b, 2);

    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
  else {
    int var_index[3];
    var_index[0] = i;
    var_index[1] = j;
    var_index[2] = k;

    DFABuilder *b = dfaSetup(3, 3, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 1, "00X");
    dfaStoreException(b, 1, "110");
    dfaStoreException(b, 1, "011");
    dfaStoreState(b, 2);
    
    /* state 2 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    return dfaBuild(b, "0+-");
  }
}

DFA *dfaBoolvar(int b)
{
  int var_index[1];
  var_index[0] = b;

  DFABuilder *builder = dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(builder, 1);
  dfaStoreException(builder, 2, "0");
  dfaStoreState(builder, 1);      

  /* state 1 */
  dfaAllocExceptions(builder, 0);
  dfaStoreState(builder, 1);

  /* state 2 */
  dfaAllocExceptions(builder, 0);
  dfaStoreState(builder, 2);

  return dfaBuild(builder, "0+-");
}

DFA *dfaFirstOrder(int i)
{
  int var_index[1];
  var_index[0] = i;

  DFABuilder *b = dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);
  
  /* state 1 */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "0");
  dfaStoreState(b, 2);
  
  /* state 2 */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);
  
  return dfaBuild(b, "0-+");
}

/* an automaton that expresses a non-WS1S property: it accepts iff it
   reads an empty string or if first-order variable i assumes the
   value n-1, where n is the length of the string */
DFA *dfaLastPos(int i)
{
  int var_index[1];
  var_index[0] = i;

  DFABuilder *b = dfaSetup(5, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 4);

  /* state 1, rejecting */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "0");
  dfaStoreState(b, 2); /* on "1" go to transitory accepting state */
  
  /* state 2, accepting */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 3);
    
  /* state 3, rejecting */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 3);

  /* state 4, accepting so as to accept word that describes only
     Booleans (the empty word when Boolean part is removed) */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "0");
  dfaStoreState(b, 2); /* on "1" go to transitory accepting state */
    
  return dfaBuild(b, "00+0+");
}

/* an automaton that expresses a non-WS1S property: it accepts iff it
   never reads a 0 on the i'th track and don't-cares otherwise */
DFA *dfaAllPos(int i)
{
  int var_index[1];
  var_index[0] = i;
  DFABuilder *b = dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);

  /* state 1, accepting */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 2, "0");
  dfaStoreState(b, 1);
  
  /* state 2, rejecting */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);

  return dfaBuild(b, "0+0");
}

DFA *dfaIn(int i, int j)
{
  int var_index[2];
  var_index[0] = i;
  var_index[1] = j;
  invariant(i != j);

  DFABuilder *b = dfaSetup(4, 2, var_index);

  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);
  
  /* state 1 */
  dfaAllocExceptions(b, 2);
  dfaStoreException(b, 3, "10");
  dfaStoreException(b, 2, "11");
  dfaStoreState(b, 1);
  
  /* state 2 accept */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);
  
  /* state 3 reject */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 3);
    
  return dfaBuild(b, "0-+-");
}

DFA *dfaEq1(int i, int j)
{
  if (i == j) 
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(4, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
  
    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 2, "11");
    dfaStoreState(b, 3);
  
    /* state 2 accept */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
  
    /* state 3 reject */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    return dfaBuild(b, "0-+-");
  }
}

DFA *dfaLess(int i, int j)
{
  if (i == j) 
    return dfaFalse();
  else {
    int var_index[2];
    var_index[0] = i,
    var_index[1] = j;

    DFABuilder *b = dfaSetup(5, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 2, "10");
    dfaStoreState(b, 3);
    
    /* state 2 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 2, "X0");
    dfaStoreState(b, 4);

    /* state 3 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);

    /* state 4 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 4);

    return dfaBuild(b, "0---+");
  }
}

DFA *dfaLesseq(int i, int j)
{
  if (i == j) 
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(5, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 2, "10");
    dfaStoreException(b, 4, "11");
    dfaStoreState(b, 3);
    
    /* state 2 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 2, "X0");
    dfaStoreState(b, 4);

    /* state 3 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);

    /* state 4 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 4);

    return dfaBuild(b, "0---+");
  }
}

DFA *dfaConst(int n, int i)
{
  DFA *aut;
  int var_index[1];
  int state_no;
  char *finals;
  
  var_index[0] = i;
  
  DFABuilder *b = dfaSetup(n+4, 1, var_index);
  
  /* boolvar */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 3);
  
  /* state 1  - Accept */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);
  
  /* state 2 - Reject */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);
  
  /* states 3 .. (n+2) */
  for (state_no = 3; state_no < n+3; state_no++) {
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, state_no+1, "0");
    dfaStoreState(b, 2);
  }
  
  /* state n+3 */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "1");
  dfaStoreState(b, 2);
  
  {
    int k;
    
    finals = (char *) mem_alloc((n+4) * (sizeof(char *)));
    for (k = 0; k < n+4; k++)
      finals[k] = '-';
    finals[0] = '0';
    finals[1] = '+';
  }
  
  aut = dfaBuild(b, finals);
  mem_free(finals);  
  return aut;
}

DFA *dfaPlus1(int i, int j, int n)
{
  if (n == 0)
    return dfaEq1(i, j);
  else if (i == j)
    return dfaFalse();  
  else {
    DFA *aut;
    int var_index[2];
    int state_no;
    char *finals;

    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(n+4, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);

    /* state 1 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 1, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreState(b, 2);

    /* state 2 - Reject */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);

    /* state 3 .. (n+1) */
    for (state_no = 3; state_no <= n+1; state_no++) {
      dfaAllocExceptions(b, 1);
      dfaStoreException(b, state_no+1,"0X");
      dfaStoreState(b, 2);
    }
    
    /* state n+2 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, n+3, "1X");
    dfaStoreState(b, 2);

    /* state n+3 - Accept */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, n+3);

    {
      int k;

      finals = (char *) mem_alloc((n+4) * (sizeof(char *)));
      for (k = 0; k < n+4; k++)
	finals[k] = '-';
      finals[0] = '0';
      finals[n+3] = '+';
    }

    aut = dfaBuild(b, finals);
    mem_free(finals);
    return aut;
  }
}

DFA *dfaMinus1(int i, int j)
{
  if (i == j) { /* <=> pi=0 */
    int var_index[1];
    var_index[0] = i;
    
    DFABuilder *b = dfaSetup(4, 1, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3,"1");
    dfaStoreState(b, 2);

    /* state 2 - Reject */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    /* state 3 - Accept */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    return dfaBuild(b, "0--+");
  }
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(6, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);

    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 5);

    /* state 2 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 3);

    /* state 3 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);

    /* state 4 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "X0");
    dfaStoreState(b, 5);

    /* state 5 */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 5);

    return dfaBuild(b, "0----+");
  }
}

DFA *dfaMax(int i, int j)
{
  if (i == j)
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(5, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 2, "0X");
    dfaStoreState(b, 3);

    /* state 2 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 2, "0X");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 3);

    /* state 3 - Accept */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "X0");
    dfaStoreState(b, 4);
    
    /* state 4 - All reject */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 4);

    return dfaBuild(b, "0--+-");
  }
}

/* is i (first-order) lowest element in j?; i=0 if j is empty */
DFA *dfaMin(int i, int j)
{
  if (i == j)
    return dfaTrue();
  else {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;

    DFABuilder *b = dfaSetup(6, 2, var_index);

    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 5);

    /* state 2 */
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 5, "11");
    dfaStoreState(b, 3);

    /* state 3 - Reject */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);

    /* state 4 - Accept */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 4, "X0");
    dfaStoreState(b, 3);
    
    /* state 5 - Accept */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 5);
    
    return dfaBuild(b, "0---++");
  }
}

DFA *dfaPlusModulo1(int i, int j, int k)  /* see plusmodulo.mona */
{
  if (i == j) {
    if (i == k) {
      int var_index[1];
      var_index[0] = i;
      DFABuilder *b = dfaSetup(4, 1, var_index);

      /* boolvar */
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 1);
      
      /* state 1 */ 
      dfaAllocExceptions(b, 1);
      dfaStoreException(b, 2, "0");
      dfaStoreState(b, 3);
      
      /* state 2 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 2);
      
      /* state 3 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 3);
      
      return dfaBuild(b, "0--+");
    }
    else {
      int var_index[2];
      var_index[0] = i;
      var_index[1] = k;

      DFABuilder *b = dfaSetup(5, 2, var_index);

      /* boolvar */
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 1);
      
      /* state 1 */ 
      dfaAllocExceptions(b, 2);
      dfaStoreException(b, 2, "0X");
      dfaStoreException(b, 3, "10");
      dfaStoreState(b, 4);
      
      /* state 2 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 2);
      
      /* state 3 */ 
      dfaAllocExceptions(b, 1);
      dfaStoreException(b, 2, "X0");
      dfaStoreState(b, 4);
      
      /* state 4 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 4);
      
      return dfaBuild(b, "0---+");
    }
  }
  else if (j == k) {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = k;
    
    DFABuilder *b = dfaSetup(7, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 5);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 6, "10");
    dfaStoreState(b, 3);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "X0");
    dfaStoreState(b, 5);
    
    /* state 5 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 5);
    
    /* state 6 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 6, "X0");
    dfaStoreState(b, 5);
    
    return dfaBuild(b, "0----+-");
  }
  else if (i == k) {
    int var_index[2];
    var_index[0] = k;
    var_index[1] = j;
    
    DFABuilder *b = dfaSetup(5, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 2, "0X");
    dfaStoreException(b, 3, "10");
    dfaStoreState(b, 4);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "X0");
    dfaStoreState(b, 4);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 4);
    
    return dfaBuild(b, "0---+");
  }
  else {
    int var_index[3];
    var_index[0] = i;
    var_index[1] = j;
    var_index[2] = k;
    
    DFABuilder *b = dfaSetup(13, 3, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 6);
    dfaStoreException(b, 2, "000");
    dfaStoreException(b, 3, "0X1");
    dfaStoreException(b, 4, "010");
    dfaStoreException(b, 5, "100");
    dfaStoreException(b, 6, "101");
    dfaStoreException(b, 7, "110");
    dfaStoreState(b, 8);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 9, "000");
    dfaStoreException(b, 4, "010");
    dfaStoreException(b, 10, "100");
    dfaStoreState(b, 3);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 11, "1X0");
    dfaStoreState(b, 3);
    
    /* state 5 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 12, "X00");
    dfaStoreException(b, 6, "X01");
    dfaStoreException(b, 7, "X10");
    dfaStoreState(b, 8);
    
    /* state 6 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 6, "X0X");
    dfaStoreState(b, 8);
    
    /* state 7 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 8, "XX1");
    dfaStoreState(b, 3);
    
    /* state 8 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 8);
    
    /* state 9 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 9, "000");
    dfaStoreException(b, 4, "010");
    dfaStoreState(b, 3);
    
    /* state 10 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 10, "X00");
    dfaStoreException(b, 8, "X11");
    dfaStoreState(b, 3);
    
    /* state 11 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 11, "XX0");
    dfaStoreState(b, 8);
    
    /* state 12 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 12, "X00");
    dfaStoreException(b, 6, "X01");
    dfaStoreException(b, 7, "X10");
    dfaStoreState(b, 3);
    
    return dfaBuild(b, "0-------+----");
  }
}

DFA *dfaMinusModulo1(int i, int j, int k)  /* see minusmodulo.mona */
{
  if (i == j) {
    if (i == k) {
      int var_index[1];
      var_index[0] = i;
    
      DFABuilder *b = dfaSetup(4, 1, var_index);
      
      /* boolvar */
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 1);
      
      /* state 1 */ 
      dfaAllocExceptions(b, 1);
      dfaStoreException(b, 2, "0");
      dfaStoreState(b, 3);
    
      /* state 2 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 2);
    
      /* state 3 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 3);
    
      return dfaBuild(b, "0--+");
    }
    else {
      int var_index[2];
      var_index[0] = i;
      var_index[1] = k;
    
      DFABuilder *b = dfaSetup(5, 2, var_index);
      
      /* boolvar */
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 1);
      
      /* state 1 */ 
      dfaAllocExceptions(b, 2);
      dfaStoreException(b, 2, "0X");
      dfaStoreException(b, 3, "10");
      dfaStoreState(b, 4);
    
      /* state 2 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 2);
    
      /* state 3 */ 
      dfaAllocExceptions(b, 1);
      dfaStoreException(b, 2, "X0");
      dfaStoreState(b, 4);
    
      /* state 4 */ 
      dfaAllocExceptions(b, 0);
      dfaStoreState(b, 4);
    
      return dfaBuild(b, "0---+");
    }
  }
  else if (j == k) {
    int var_index[2];
    var_index[0] = i;
    var_index[1] = j;
    
    DFABuilder *b = dfaSetup(6, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 5);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 2, "00");
    dfaStoreException(b, 4, "10");
    dfaStoreState(b, 3);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "X0");
    dfaStoreState(b, 5);
    
    /* state 5 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 5);
    
    return dfaBuild(b, "0----+");
  }
  else if (i == k) {
    int var_index[2];
    var_index[0] = j;
    var_index[1] = k;
    
    DFABuilder *b = dfaSetup(5, 2, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 3, "01");
    dfaStoreException(b, 4, "11");
    dfaStoreState(b, 2);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 2);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 3, "0X");
    dfaStoreState(b, 4);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 4);
    
    return dfaBuild(b, "0---+");
  }
  else {
    int var_index[3];
    var_index[0] = i;
    var_index[1] = j;
    var_index[2] = k;
    
    DFABuilder *b = dfaSetup(12, 3, var_index);
    
    /* boolvar */
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 1);
    
    /* state 1 */ 
    dfaAllocExceptions(b, 6);
    dfaStoreException(b, 2, "000");
    dfaStoreException(b, 4, "010");
    dfaStoreException(b, 5, "100");
    dfaStoreException(b, 6, "101");
    dfaStoreException(b, 7, "110");
    dfaStoreException(b, 8, "111");
    dfaStoreState(b, 3);
    
    /* state 2 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 2, "000");
    dfaStoreException(b, 9, "100");
    dfaStoreState(b, 3);
    
    /* state 3 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 3);
    
    /* state 4 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 4, "0X0");
    dfaStoreException(b, 7, "1X0");
    dfaStoreState(b, 3);
    
    /* state 5 */ 
    dfaAllocExceptions(b, 3);
    dfaStoreException(b, 10, "X00");
    dfaStoreException(b, 6, "X01");
    dfaStoreException(b, 11, "X10");
    dfaStoreState(b, 8);
    
    /* state 6 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 6, "X0X");
    dfaStoreState(b, 8);
    
    /* state 7 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 8, "XX1");
    dfaStoreState(b, 3);
    
    /* state 8 */ 
    dfaAllocExceptions(b, 0);
    dfaStoreState(b, 8);
    
    /* state 9 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 11, "X10");
    dfaStoreException(b, 8, "X11");
    dfaStoreState(b, 3);
    
    /* state 10 */ 
    dfaAllocExceptions(b, 2);
    dfaStoreException(b, 10, "X00");
    dfaStoreException(b, 6, "X01");
    dfaStoreState(b, 3);
    
    /* state 11 */ 
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 11, "XX0");
    dfaStoreState(b, 8);
    
    return dfaBuild(b, "0-------+---");
  }
}

DFA *dfaPresbConst(int i, int n)
{
  /* the following code constructs an automaton for the Presburger 
     relation 'p_i = n' where (non-negative) numbers are encoded 
     with least-significant bit first */
  
  int var_index[1];  /* array of indices of the free variables */
  int bits;  /* total number of bits required to represent 'n' */ 
  char *status;  /* array used for state kinds (-1/0/1) */
  DFA *res;
  int t, s;
  
  /* fill 'var_index', only one variable in this case */
  var_index[0] = i; 
  
  /* calculate 'bits' */
  for (t = n, bits = 0; t != 0; t >>= 1)
    bits++;

  /* prepare construction of automaton with
     'bits + 3' states and  1 variable */ 
  status = (char *) mem_alloc(bits + 3);
  DFABuilder *b = dfaSetup(bits + 3, 1, var_index);
  
  /* now create the states on at a time,
     always start with the initial state (state 0),
     state 0: */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 2);
  status[0] = '0';  /* '0' denotes "don't care" */
  /* these two lines read: there are 0 exceptions for
     going to state 2, this is what we want since the first
     symbol being read encodes the values of the Boolean
     variables of which there are none in this case */
  
  /* we choose to use state 1 as the 'all reject' state,
     state 1: */
  dfaAllocExceptions(b, 0);
  dfaStoreState(b, 1);
  status[1] = '-';  /* '-' denotes "reject" */
  
  /* now generate one state for each bit in 'n' */
  for (t = n, s = 2; s <= bits+1; s++, t >>= 1) {
    /* state 's' goes to state 's+1' or all reject
       depending on the next bit */
    dfaAllocExceptions(b, 1);
    dfaStoreException(b, 1, (t&1) ? "0" : "1");
    dfaStoreState(b, s+1);
    status[s] = '-'; /* '-' denotes "reject" */
  }

  /* the last state accepts and loops on '0' */
  dfaAllocExceptions(b, 1);
  dfaStoreException(b, 1, "1");
  dfaStoreState(b, bits+2);
  status[bits+2] = '+'; /* '+' denotes "accept" */

  /* finalize the construction */
  res = dfaBuild(b, status);
  mem_free(status);  /* deallocate 'status' */

  return res;
}
