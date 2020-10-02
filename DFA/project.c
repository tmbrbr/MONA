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

#include <stdint.h>
#include "dfa.h"
#include "../BDD/hash.h"
#include "../Mem/mem.h"

#define SET_BDD_NOT_CALCULATED (unsigned)-1

struct set {
   int size;
   int *elements;
   unsigned sq;     /* bddm->SEQUENTIAL_LIST(roots)[sq]  is the
		       BDD node of the subset state; if equal
		       SET_BDD_NOT_CALCULATED then not calculated yet */
   int decomp1, decomp2; /* union these to get this set */
   int permanent;        /* state in final automata, -1 = none */
}; 

struct set_list {
    int n_ssets;
    struct set *ssets;
    int next_sset;
};

struct set_list init_ssets(unsigned int sz)
{
   struct set_list slist;
   slist.n_ssets = sz;
   slist.ssets = mem_alloc((sizeof(struct set)) * sz);
   slist.next_sset = 0;
   return slist;
}

int make_sset(struct set_list *slist, hash_tab htbl_set, int sz, int *elem, unsigned sq, int d1, int d2)
{
  if (slist->next_sset == slist->n_ssets) {
    struct set *new_ssets = mem_alloc((sizeof(struct set)) * slist->n_ssets * 2);
    
    mem_copy(new_ssets, slist->ssets, (sizeof(struct set)) * slist->n_ssets);
    mem_free(slist->ssets);
    slist->ssets = new_ssets;
    slist->n_ssets *= 2;
  }
  slist->ssets[slist->next_sset].size = sz;
  slist->ssets[slist->next_sset].elements = elem;
  slist->ssets[slist->next_sset].sq = sq;
  slist->ssets[slist->next_sset].decomp1 = d1;
  slist->ssets[slist->next_sset].decomp2 = d2;
  slist->ssets[slist->next_sset].permanent = -1;
  insert_in_hash_tab(htbl_set, (long)elem, 0, (void *)(uintptr_t)(slist->next_sset+1));  
  /* htbl maps to ++id, since 0 = not_found */ 

  return(slist->next_sset++);
}       


struct sslist_ {
  int sset_id;
  struct sslist_ *next;  
};

typedef struct sslist_ *sslist;

sslist new_sslist(int si, sslist nxt)
{  
  sslist sl = mem_alloc(sizeof *sl);
  
  sl->sset_id = si;
  sl->next = nxt;
  
  return(sl);
}

/* These global because used in proj_term.  */

struct proj_context {
  struct set_list *slist;
  hash_tab htbl_set;
  int *next_state;
  sslist lt;
  int nest;
};

/* Fn to create pairs */
unsigned proj_term1(unsigned state1, unsigned  state2, void *context)
{
  struct proj_context *ptc = (struct proj_context *)(context);
  int res;
  int *s;
  int size;

  if (state1 == state2) {
    size = 1;
    s = mem_alloc((sizeof *s) * 2);
    s[0] = state1; s[1] = -1;         
  } 
  else {
    size = 2;
    s = mem_alloc((sizeof *s) * 3);
    if(state1<state2) {
      s[0] = state1;
      s[1] = state2; 
      s[2] = -1;
    }
    else {
      s[0] = state2; 
      s[1] = state1; 
      s[2] = -1;
    }
  }
   
  /* res = 0 or id+1 */
  if ( (res = (int)(uintptr_t) lookup_in_hash_tab(ptc->htbl_set, (long) s, 0)) ) {
    mem_free(s); /* it was already there */  
    return (--res);
  }
  else {
    res = make_sset(ptc->slist, ptc->htbl_set,
                    size, s, SET_BDD_NOT_CALCULATED, state1, state2); /* optimize if equal? */
    return (res);
  }
}

/* Fn to union leaves */
bdd_ptr proj_term2(unsigned set_index1,  unsigned set_index2, void *context)
{
  struct proj_context *ptc = (struct proj_context *)(context);
  struct set *ssets = ptc->slist->ssets;
  int res;
  int *s;
  struct set *ss1, *ss2;
  int *e1, *e2, *e3;
  ss1 = &(ssets[set_index1]);
  ss2 = &(ssets[set_index2]);
  s = mem_alloc((ss1->size + ss2->size + 1) * (sizeof *s));
  
  /* Union the sets */      
  for (e1 = ss1->elements, e2 = ss2->elements, e3 = s; 
      (*e1 >= 0) && (*e2 >= 0);) {
    if (*e1 < *e2)
      *e3++ = *e1++;
    else if (*e1 == *e2) {
      *e3++ = *e1++; 
      e2++;
    }
    else
	*e3++ = *e2++;
  }
  if(*e1 >= 0) 
    do {
      *e3++ = *e1++;
    } while(*e1 >= 0);
  else if (*e2 >= 0) 
    do {
      *e3++ = *e2++;
    } while(*e2 >= 0);
  
  *e3 = -1;   /* Terminate the new set */
  
  /* res = 0 or id+1 */
  if ( (res = (int)(uintptr_t) lookup_in_hash_tab(ptc->htbl_set, (long) s, 0)) ) {
    mem_free(s); /* it was already there */
    return (--res);
  }
  else {
    res = make_sset(ptc->slist, ptc->htbl_set,
                    (e3-s), s, SET_BDD_NOT_CALCULATED, set_index1, set_index2);
    return (res);
  }
}



/* Fn to insert leaves and return permanent "q" */
bdd_ptr proj_term3(unsigned p, void *context)
{
  struct proj_context *ptc = (struct proj_context *)(context);
  if(ptc->slist->ssets[p].permanent < 0) {
    ptc->lt->next = new_sslist(p, 0);   /* Put in queue */
    ptc->lt = ptc->lt->next;
    ptc->slist->ssets[p].permanent = (*ptc->next_state)++;
  }

  return (ptc->slist->ssets[p].permanent);
}

int eval_bdd(unsigned ss, bdd_manager *bddm_res, struct set_list *slist, hash_tab htbl_set, int nest)
{
  unsigned root1, root2;
  unsigned res;
  
  // Null pointer checks
  if (!slist) {
    return -1;
  }
  // Array bounds check
  if (ss >= slist->n_ssets) {
    return -1;
  }

  if (slist->ssets[ss].sq == SET_BDD_NOT_CALCULATED) {

    struct proj_context ptc;
    ptc.slist = slist;
    ptc.htbl_set = htbl_set;
    ptc.nest = nest;

    root1 = eval_bdd(slist->ssets[ss].decomp1, bddm_res, slist, htbl_set, nest + 1);
    root2 = eval_bdd(slist->ssets[ss].decomp2, bddm_res, slist, htbl_set, nest + 1);
    if ((root1 < 0) || (root1 >= bdd_roots_length(bddm_res)) ||
        (root2 < 0) || (root2 >= bdd_roots_length(bddm_res))) {
      return -1;
    }
    res = bdd_apply2_hashed(bddm_res, bdd_roots(bddm_res)[root1],
                            bddm_res, bdd_roots(bddm_res)[root2],
                            bddm_res, (void *)(&ptc), &proj_term2);
    // bdd_apply2_hashed can double the BDD table, which can lead to
    // table size exceeding the maximum. Could also check the result
    // of res.
    if (bddm_res->table_total_size > BDD_MAX_TOTAL_TABLE_SIZE) {
      return -1;
    }

    slist->ssets[ss].sq = bdd_roots_length(bddm_res) - 1;
  }

  return(slist->ssets[ss].sq);
} 

DFA *dfaProject(DFA *a, unsigned var_index) 
{
  if (!a) {
    return NULL;
  }

  int i,*e;
  DFA *res;
  int error = 0;
  sslist lst, lh, lt, lnxt;
  unsigned size_estimate = 2 * bdd_size(a->bddm);
  bdd_manager *bddm_res;

  bddm_res = bdd_new_manager(size_estimate, size_estimate/8 + 2);
  bdd_make_cache(bddm_res, size_estimate, size_estimate/8 + 2);    
  bddm_res->cache_erase_on_doubling = TRUE;

  struct set_list slist = init_ssets(a->ns * 2);
  hash_tab htbl_set = new_hash_tab(hashlong, eqlong);
  int next_state = 0; 
  
  for(i = 0; i < a->ns; i++) {  /* Allocate singletons, ssets[i] = {i} */
    int *s = mem_alloc(2 * (sizeof *s));
    
    s[0] = i; s[1] = -1;
    make_sset(&slist, htbl_set, 1, s, SET_BDD_NOT_CALCULATED, -1, -1);
  }

  for (i = 0; i < a->ns; i++) {  /* Update bdd's */
    struct proj_context pc;
    pc.slist = &slist;
    pc.htbl_set = htbl_set;

    (void) bdd_project(a->bddm, a->q[i], var_index, bddm_res, (void *)(&pc), &proj_term1);
    slist.ssets[i].sq = bdd_roots_length(bddm_res) - 1; /* bdd_roots_length(bddm_res) - 1 == i */
    /* bdd_roots(bddm_res)[ssets[i].sq] now contains 
       place where a node index is to be found*/
  }

  /* Create a list of reachable sets. */
  lst = lh = lt = new_sslist(a->s, 0);   /* start singleton */
  slist.ssets[a->s].permanent = next_state++;  /* Should be 0 */
  {
    unsigned root_place;
    bdd_manager *bddm_res_ = bdd_new_manager(size_estimate,
					     size_estimate / 8 + 2);
    bdd_make_cache(bddm_res_, size_estimate, size_estimate /8 + 2);
    bdd_kill_cache(bddm_res);
    bdd_make_cache(bddm_res, size_estimate, size_estimate/8 + 2);    
    bddm_res->cache_erase_on_doubling = TRUE;
    
    bdd_prepare_apply1(bddm_res);
    while (lh) {
      root_place = eval_bdd(lh->sset_id, bddm_res, &slist, htbl_set, 0);
      // The BDD maximum entries is BDD_MAX_TOTAL_TABLE_SIZE (=0x1000000)
      if ((root_place >= BDD_MAX_TOTAL_TABLE_SIZE) || (root_place < 0)) {
        printf("Error in %s: root_place (= %d) is invalid (> %d)!\n",
               __func__, root_place, BDD_MAX_TOTAL_TABLE_SIZE);
        error = 1;
        break;
      }
      /* Insert leaves */
      struct proj_context pc;
      pc.slist = &slist;
      pc.next_state = &next_state;
      pc.lt = lt;
      bdd_apply1(bddm_res, bdd_roots(bddm_res)[root_place], bddm_res_, (void *)(& pc), &proj_term3);
      /*evaluate bdd_roots(bddm_res) at each iteration since bdd_apply1 is called*/
      lt = pc.lt;
      lh = lh -> next;
      i = 0;
    }
   
    {
      unsigned *new_roots;
      
      res = dfaMakeNoBddm(next_state);
      res->bddm = bddm_res_;
      new_roots = bdd_roots(bddm_res_);
      
      for (i = 0; i < next_state; i++) {   /* Walk through list */
        if (!error) {
          int non_bottom_found = 0;
          int plus_one_found = 0;
          res->q[i] = new_roots[i];
          for (e = slist.ssets[lst->sset_id].elements; *e >= 0; e++) {
	    non_bottom_found += (a->f[*e] != 0);
	    plus_one_found += (a->f[*e] == 1);
          }
          if (!non_bottom_found)
            res->f[i] = 0;
          else
            if (plus_one_found)
              res->f[i] = 1;
            else
              res->f[i] = -1;
          res->s = slist.ssets[a->s].permanent;  /* Move to out of loop */
	}
	lnxt = lst -> next;
	mem_free(lst);          /* Free the list */
	lst = lnxt;
      }
    
      for(i = 0; i < slist.next_sset; i++) 
	mem_free(slist.ssets[i].elements);
      
      mem_free(slist.ssets);
      free_hash_tab(htbl_set);  
      bdd_update_statistics(bddm_res, (unsigned)PROJECT);
      bdd_update_statistics(bddm_res_, (unsigned)PROJECT);
      bdd_kill_manager(bddm_res);
      if (error) {
        dfaFree(res);
        res = NULL;
      }
      return(res);
    }
  }
}
