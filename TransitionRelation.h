
/*
 * lsting: Invariant Generation using Constraint Solving. 
 * Copyright (C) 2005 Sriram Sankaranarayanan < srirams@theory.stanford.edu>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*
 * Filename: Location
 * Author: Sriram Sankaranarayanan< srirams@theory.stanford.edu>
 * Comments:
 *    Contains transition relation information. Computes duals, posts, and such things.
 * Post-Comments: SACRED CODE. Reimplemented from scratch for the DUCHESS project.
 * Copyright: see README file for the copyright.
 */

#ifndef __TRANSITION_RELATION__H_
#define __TRANSITION_RELATION__H_

#include "var-info.h"
#include <iostream>
#include "Clump.h"
#include "Location.h"
#include "SparseLinExpr.h"
#include "LinTransform.h"
#include "Expression.h"
#include "ExpressionStore.h"
#include "Context.h"
#include <ppl.hh>

using namespace std;
using namespace Parma_Polyhedra_Library;
using namespace Parma_Polyhedra_Library::IO_Operators;

class DualTransitionRelation;

class TransitionRelation{
  private:
   int n;
   var_info *f, *fd, *fm, *fp; // fm is the var_info for non-linear multipliers
   Location * preloc, * postloc;
   // the actual transition relation as a 2n dimensional polyhedron
   C_Polyhedron * rel;

   // structure of the transition relation
   // the structure in rel has been factored into , guard, update and preserved
   // for faster post computation.
   C_Polyhedron * guard, * update;
   set<int>preserved;

   //

   int mult_left, mult_right,nc; //the range multipliers in its dual and the number of constraint variables
   int index; // The index of the non-linear multiplier in fm

   string name;

   
   int fired;
   void initialize(int n, var_info *f, var_info * fd, var_info * fm,
                   Location * preloc, Location * postloc, C_Polyhedron *rel, string name);
   void initialize(int n, var_info *f, var_info *fd,
                                       var_info * fm, string name);

   void initialize_without_populating(int n, var_info *f, var_info * fd, var_info * fm,
                   Location * preloc, Location * postloc, C_Polyhedron *rel, string name, int index);
   void initialize_without_populating(int n, var_info *f, var_info *fd,
                                       var_info * fm, string name, int index);

   bool is_preserved(int i) const;
   bool add_guard(Constraint const & cc);
   bool add_preservation_relation( Constraint const & cc);
   bool split_relation(); // if expression is of the for 'x-x , then add 

   void dualize_standard(C_Polyhedron & result) const;
   
  public:
   
   
   TransitionRelation(int n, var_info * f, var_info * fd, var_info * fm, Location * preloc,
                      Location * postloc, C_Polyhedron * rel,string name);


   TransitionRelation(int n, var_info *f, var_info *fd, var_info *fm, string name);

   
   TransitionRelation(int n, var_info * f, var_info * fd, var_info * fm, Location * preloc,
                      Location * postloc, C_Polyhedron * rel,string name, int index);

   TransitionRelation(int n, var_info *f, var_info *fd, var_info *fm, string name, int index);

   void set_locs(Location * preloc, Location * postloc);
   void set_relation(C_Polyhedron * rel);

   void compute_nc();
   
   void strengthen(const C_Polyhedron * p);

   void compute_post(const C_Polyhedron * p, C_Polyhedron & q) const;
   void compute_post_new(const C_Polyhedron * p, C_Polyhedron & q) const;

   int get_firing_count();
   //TransitionRelation * compose( TransitionRelation * t);
 
   
   
   void compute_consecution_constraints( Context & c);

   void compute_consecution_constraints(vector<Clump> & vcl, C_Polyhedron & initp);

   void compute_consecution_01(vector<Clump> & vcl, C_Polyhedron & initp);
   
   void populate_multipliers(); // Compute the number of multipliers required and add them to the constraint store

   bool fire();
   
   int get_range_left() const;
   int get_range_right() const;

   int get_mult_index() const {
      return index;
   }
   bool matches(string & f) const;
   const string & get_name() const;
   
   const string & get_preloc_name() const;
   const string & get_postloc_name() const;

   void add_preloc_invariant();
   
   DualTransitionRelation get_dual_relation() const;
   
   const C_Polyhedron & get_relation() const;
   C_Polyhedron & get_non_const_relation();
   
   const var_info * get_varinfo() const;

   C_Polyhedron const & get_guard_poly() const{
      return (*guard);
   }

   C_Polyhedron const & get_update_poly() const{
      return (*update);
   }

   set<int> const & get_preserved_set() const{
      return preserved;
   }

   void check_map();

   int get_dimension() const{
      return n;
   };
   
   C_Polyhedron & get_relation_byme() const{
      return *rel;
   };
   C_Polyhedron & get_guard_byme() const{
      return *guard;
   };
   C_Polyhedron & get_update_byme() const{
      return *update;
   };
   void set_guard_byme(C_Polyhedron * guard){
      this->guard=guard;
   }
   void set_update_byme(C_Polyhedron * update){
      this->update=update;
   }
   var_info & get_varinfo_f() const{
      return *f;
   };
   var_info & get_varinfo_fd() const{
      return *fd;
   };
   var_info & get_varinfo_fm() const{
      return *fm;
   };
   Location & get_preloc() const{
      return *preloc;
   };
   Location & get_postloc() const{
      return *postloc;
   };
};

ostream & operator << (ostream & in, TransitionRelation const & t);

#endif
