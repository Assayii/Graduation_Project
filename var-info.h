
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

#ifndef __VAR__INFO__H_
#define  __VAR__INFO__H_
#include <set>
#include<vector>
#include <string>

using namespace std;

#define VAR_NOT_FOUND (-1)
#define NULL_VAR_INFO ((var_info *) 0)
#define NULL_VAR_LIST ((var_info *) 0)

class var_info{

   
  protected:


   string get_a_string(int i, const char * prefix);
   
   vector<char*> * v;

   
   int dimension;
   
   
  public:
  
   var_info();
   
   var_info(var_info const *  f);
   
   var_info(var_info * fr, vector<int> v);
   
   // project and just provide the dimensions that are in v
   
   var_info (var_info * fr1, var_info * fr2);

   // merge the two var_infos

   
   int get_dimension() const;

   virtual vector<char *> * get_vector();
   
   virtual int insert(const char * what, int primed=0);

   virtual int search(const char * what)const ;

   virtual char * get_name(int dimension) const;
   
   virtual var_info * prime();

   virtual var_info * dualize();
   
   virtual int search_and_insert(const char * what);

   virtual void print_info(ostream & out) const {
      print(out);
   }
   
   virtual void print(ostream & out) const;

   virtual void print_dimensions(ostream & out, set<int> const & what) const;
   
   virtual void resize_to(int what, const char  * prefix );
};

// a simple check
ostream & operator<< (ostream & out, var_info * const f);
   
#endif
