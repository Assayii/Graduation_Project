
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

#include <vector>
#include <string>
#include <cstring>
#include <malloc.h>
#include <iostream>
#include "myassertions.h"
#include "var-info.h"

using namespace std;



string var_info :: get_a_string( int i, const char * prefix){
   string x;
   int j= (i >0)?i:-i ,rem;

   if (j==0)
      x=(char)'0' +x;
   
   while (j > 0){
      rem=j%10;
      x= (char)('0'+rem)+ x;
      j/=10;
   }
   
   x= prefix + x;
   return x;
}

var_info::var_info(){
     dimension=0;
     v= new vector<char *>();
     
}
/*
var_info::var_info( var_info f){
   v=f.get_vector();
   dimension=f.get_dimension();
}

*/

int var_info::get_dimension() const{
   return dimension;
}


var_info::var_info (var_info * f1, var_info * f2){
   // merge the two var-infos into a single info!
   
   int d1=f1->get_dimension(), d2=f2->get_dimension();
   dimension=0;
   v= new vector<char *>();
   int i;
   for (i=0;i<d1;++i){
      insert(f1->get_name(i));
   }
   
   for (i=0;i<d2;++i)
      insert(f2->get_name(i));

}


var_info::var_info(var_info * fr, vector<int> v1){
   // project out from fr based on v

   int n=fr->get_dimension();
   dimension=0;
   this->v=new vector<char * >();

   vector<int>::iterator vi;
   int i;
   for (i=0,vi=v1.begin(); vi < v1.end(); ++vi, ++i){
      PRECONDITION( ( (*vi) >=0 && (*vi) < n ) ,
                    "var_info::var_info asked to project out of range");

      insert(fr->get_name(*vi)); 
      
   }
   return;
}

var_info::var_info (var_info const * fr){
   int i;
   
   this->v =new vector<char *>();
   
   this->dimension=0;
   for (i=0; i<fr->dimension; ++i ) {
      this->insert(fr->get_name(i));
   }
}


vector<char *> * var_info::get_vector(){
   return v;
}

int var_info::insert(const char * what, int primed){
   char * c = (char *) malloc(strlen(what)+2);
   strcpy(c+primed,what);
   if (primed==1){
      c[0]='`';
   } else if (primed==2){
      c[0]='c';
      c[1]='_';
         }
   v->push_back(c);
   dimension++;
   return dimension-1;
}

int var_info::search (const char * what) const{
   vector<char*>::iterator i= v->begin();
   int j;
   for(j=0;i!=v->end();j++,i++){
      if (strcmp(*i,what)==0)
         return j;
      
   }

   return VAR_NOT_FOUND;
}

char * var_info::get_name(int dim  ) const{
   PRECONDITION( dim >=0 && dim < dimension, " Invalid dimension");
   return (*v)[dim];
}


var_info * var_info::dualize(){
   var_info * v1 = new var_info();
   int i;
    for (i=0;i<dimension;i++)
      v1->insert(get_name(i),2);
    v1->insert("d");
    
    return v1;
}



var_info * var_info::prime(){
    var_info * v1 = new var_info();
   int i;
   
    for (i=0;i<dimension;i++)
      v1->insert(get_name(i));
    for (i=0;i<dimension;i++)
      v1->insert(get_name(i),1);
    return v1;
}

void var_info::print(ostream & out) const{
   vector<char *>::iterator i;
   int j=0;
   out<<" [[ "<<endl;
   for(i=v->begin();i!=v->end();i++){
      out <<*i<<"  ,  ";      
      j++;
      if (j%20==0)
         out<<endl;
   }
   out<<"]]"<<endl;
 
}

int var_info::search_and_insert(const char *what){
   int index;
   index=search(what);
   if (index==VAR_NOT_FOUND){
      index=insert(what);
   }
   return index;
}

/*
int main(){
   var_info var;
   var.insert("papa");
   var.insert("murphy");
   var.insert("take");
   var.insert("and");
   var.insert("bake");
   var.insert("pizza");

   cout<< var.search("take")<<endl;
   cout<<var.get_dimension()<<endl;
   cout<< var.search("pAos")<<endl;
   
}
*/

void var_info::print_dimensions(ostream & out, set<int> const & what) const{
   set<int>::iterator vi;
   int j=0;
   out<<"[[  ";
   for (vi=what.begin(); vi !=what.end(); ++vi){
      PRECONDITION ( ((*vi)>=0 && (*vi) < dimension),
                     "var_info::print_dimensions --> asked to print out of range");
      
      out<< get_name((*vi)) <<"  ," ;
      ++j;
      if (j%20==0)
         out<<endl;
   }
   out<<" ]]"<<endl;
   return;
   
}

void var_info::resize_to(int what, const char * prefix){

   if (dimension > what) {
      v->erase(v->begin()+what,v->end());
      return;
   }
   if (dimension == what) return;

   // dimension < what

   int i;
   string x;
   i=dimension;
   for (;i < what;i++){
     x=get_a_string(i, prefix);
     insert(x.c_str()); 
   }


   return;
   
}

ostream & operator<< (ostream & out, var_info * const f){
   f->print(out);
   return out;
}
