/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dfa.h"
#include "../BDD/bdd_internal.h"
#include "../Mem/mem.h"

void dfaAllocExceptions(DFABuilder* b, int n)
{
  int i;

  b->exceptions = mem_alloc(sizeof(struct path_descr) * n);
  b->no_exceptions = n;
  b->exception_index = 0;
  // Allocate the path field of each exception
  for (i = 0; i < b->no_exceptions; i++) {
    b->exceptions[i].path = mem_alloc(sizeof(char) * (b->offsets_size + 1));
  }
  b->bddpaths = mem_alloc(sizeof(bdd_ptr) * n);
}

void dfaStoreException(DFABuilder* b, int value, char *path)
{
  invariant(b->exception_index<b->no_exceptions);

  b->exceptions[b->exception_index].value = value;
  strcpy(b->exceptions[b->exception_index].path, path);
  
  b->exception_index++;
}

unsigned unite_leaf_fn(unsigned p_value, unsigned q_value, void *context)
{
  DFABuilder *b = (DFABuilder *)(context);
  if ((p_value == q_value) || (q_value == b->default_state))
    return p_value;
  else if (p_value == b->default_state)
    return q_value;
  else {
    printf("Error in unite (p_value: %d q_value: %d default: %d)\n",
           p_value, q_value, b->default_state);
    exit(-1);
  }
  return 0; /* avoid compiler warning */
}


/* Prerequisite: bddm->roots_array is non empty */
bdd_ptr unite_roots(DFABuilder *b, bdd_manager *bddm)
{
  int no_roots = bddm->roots_index;
  int i;
  bdd_ptr result;

  if ( !(result = bddm->roots_array[0]) ) {
    printf("Error in unite: no roots to unite.\n");
    exit (-1);
  }

  /* 
   * Number of iterations must be fixed/controlled, since
   * bdd_apply2_hashed add roots
   */
  for (i = 1; i < no_roots; i++)
    result = bdd_apply2_hashed(bddm, result, 
			       bddm, bddm->roots_array[i],
			       bddm, (void *)b, &unite_leaf_fn);
  
  return result;
}

bdd_ptr makepath(DFABuilder *b, bdd_manager *bddm, int n, unsigned leaf_value,
		 void (*update_bddpaths) (unsigned (*new_place) (unsigned node, bdd_manager *bddm_context),
                                          bdd_manager *bddm_context, void *context))
{
  bdd_ptr res, sub_res, default_state_ptr;
  unsigned index;

  while ((n < b->offsets_size) && (b->sorted_path[n] == 'X'))
    n++;

  if (n >= b->offsets_size)
    return (bdd_find_leaf_hashed(bddm, leaf_value, SEQUENTIAL_LIST(b->sub_results), (void *)b, update_bddpaths));

  sub_res = makepath(b, bddm, n+1, leaf_value, update_bddpaths);
  PUSH_SEQUENTIAL_LIST(b->sub_results, unsigned, sub_res);
  default_state_ptr = bdd_find_leaf_hashed(bddm, b->default_state, SEQUENTIAL_LIST(b->sub_results), (void *)b, update_bddpaths);
  POP_SEQUENTIAL_LIST(b->sub_results, unsigned, sub_res);

  index = b->global_offsets[b->sorted_indices[n]];
  
  if (b->sorted_path[n] == '0')
    res = bdd_find_node_hashed(bddm, sub_res, default_state_ptr, index, SEQUENTIAL_LIST(b->sub_results), (void *)b, update_bddpaths);
  else
    res = bdd_find_node_hashed(bddm, default_state_ptr, sub_res, index, SEQUENTIAL_LIST(b->sub_results), (void *)b, update_bddpaths);

  return res;
}

void update_bddpaths(unsigned (*new_place) (unsigned node, bdd_manager *bddm_context), bdd_manager *bddm_context, void *context) 
{
  DFABuilder *b = (DFABuilder *)(context); 
  int j;
  
  for (j = 0; j < b->exp_count; j++) 
    b->bddpaths[j] = new_place(b->bddpaths[j], bddm_context);
}

void makebdd(DFABuilder *b)
{
  bdd_manager *bddm = b->aut->bddm;
  bdd_manager *tmp_bddm;
  bdd_ptr united_bdds, default_ptr;
  int i;

  tmp_bddm = bdd_new_manager(8, 4);

  /*
  ** insert a leaf with value 'default_state' in tmp_bddm,
  ** if not already present
  */
  default_ptr = bdd_find_leaf_hashed(tmp_bddm,
                                     b->default_state,
                                     SEQUENTIAL_LIST(b->sub_results),
                                     (void *)b,
                                     &update_bddpaths); 

  for (b->exp_count = 0; b->exp_count < b->no_exceptions; b->exp_count++) {
    for (i = 0; i < b->offsets_size; i++)
      b->sorted_path[i] = b->exceptions[b->exp_count].path[b->sorted_indices[i]];

    /* clear the cache */
    bdd_kill_cache(tmp_bddm);
    bdd_make_cache(tmp_bddm, 8, 4);
    tmp_bddm->cache_erase_on_doubling = TRUE;

    b->bddpaths[b->exp_count] = makepath(b, tmp_bddm, 0, b->exceptions[b->exp_count].value, &update_bddpaths);
    PUSH_SEQUENTIAL_LIST(tmp_bddm->roots, unsigned, b->bddpaths[b->exp_count]);
  }    

  if (b->no_exceptions == 0)
    united_bdds = default_ptr;
  else if (b->no_exceptions == 1) 
    united_bdds = TOP_SEQUENTIAL_LIST(tmp_bddm->roots);
  else
    united_bdds = unite_roots(b, tmp_bddm);

  bdd_prepare_apply1(tmp_bddm);
  bdd_apply1(tmp_bddm, united_bdds, bddm, (void *)b, &fn_identity);       /* store the result in bddm->roots */

  bdd_kill_manager(tmp_bddm);
}

int offsets_cmp(const void *index1, const void *index2, void* context) 
{
  DFABuilder *b = (DFABuilder*)context;
  int o1, o2;
  
  o1 = b->global_offsets[*((int *)index1)];
  o2 = b->global_offsets[*((int *)index2)];
  
  if (o1 < o2) return -1;
  else if (o1 == o2) return 0;
  else return 1;
}

DFABuilder* dfaSetup(int ns, int os, unsigned int *offsets)
{
  int i;
  DFABuilder *b;

  b = mem_alloc(sizeof *b);
 
  MAKE_SEQUENTIAL_LIST(b->sub_results, unsigned, 64);
  
  b->no_states = ns;

  b->offsets_size = os;

  b->sorted_indices = mem_alloc(sizeof(unsigned int) * b->offsets_size);
  b->global_offsets = mem_alloc(sizeof(unsigned int) * b->offsets_size);
  b->sorted_path = mem_alloc(sizeof(char) * b->offsets_size);
  
  for (i = 0; i < b->offsets_size; i++) {
    b->sorted_indices[i] = i;
    b->global_offsets[i] = offsets[i];
  }

  qsort_r(b->sorted_indices, b->offsets_size, sizeof(int), offsets_cmp, (void*)b);

  // This can return null if table size is too large
  b->aut = dfaMake(b->no_states);

  if (b->aut != NULL) {
    b->aut->ns = b->no_states;
    b->aut->s = 0;
  }

  return b;
}

void dfaStoreState(DFABuilder *b, int ds)
{
  int i;

  if ((ds < 0) || (ds >= b->no_states)) {
    printf("Error dfaStoreState: default state larger than number of states! (ds: %d ns: %d)\n",
           ds, b->no_states);
    exit(-1);
  }

  b->default_state = ds;

  if (b->aut != NULL) {
    bdd_kill_cache(b->aut->bddm);
    bdd_make_cache(b->aut->bddm, 8, 4);
    makebdd(b);
  }

  // Free the allocated exceptions
  for (i = 0; i < b->no_exceptions; i++) {
    mem_free(b->exceptions[i].path);
  }
  mem_free(b->exceptions);
  mem_free(b->bddpaths);
}

DFA *dfaBuild(DFABuilder *b, char *finals)
{
  int        i;
  unsigned  *root_ptr;

  DFA* aut = b->aut;

  if (aut != NULL) {
    for (i=0, root_ptr = bdd_roots(aut->bddm); i < b->no_states; root_ptr++, i++) {
      aut->q[i] = *root_ptr;
      aut->f[i] = (finals[i] == '-') ? -1 : (finals[i] == '+' ? 1 : 0);
    }
  }

  // Free all components of DFABuilder
  FREE_SEQUENTIAL_LIST(b->sub_results);
  mem_free(b->sorted_indices);
  mem_free(b->global_offsets);
  mem_free(b->sorted_path);
  // Do *not* free the aut field as we are returning that
  mem_free(b);
  
  return aut;
}
