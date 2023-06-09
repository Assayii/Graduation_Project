
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

#include "DisequalityStore.h"


void DisequalityStore::initialize(int n, var_info * f){

   //
   // initialize the store.
   // with n dimensions and var-info f
   // Question to self: why on earth could this not have been written in the
   // constructor itself. Learnt costly lesson :-)
   //
   
   this->n=n;
   this->f=f;
   vp=new vector<Linear_Expression>();
   ineqs= new C_Polyhedron(n, UNIVERSE);

   // Automatically set all the variables to be positive
   // This is not part of the general semantics for this class
   // but this is useful anyway.
   
   int i;
   for (i=0;i<n;i++)
      ineqs->add_constraint(Variable(i)>=0);

   // initial store is consistent
   incon=false;
}

DisequalityStore::DisequalityStore(int n, var_info * f){
   initialize(n,f);
}

void DisequalityStore::check_consistent(){

   
   if (incon) return;
   // first check if ineqs is non-empty
   if (ineqs->is_empty()){
      //set inconsistent flag
      incon=true; 
      // Npthing more to be done
      return;
   }


   // now check for each relation e<>0 in vp
   
   Poly_Con_Relation rel1=Poly_Con_Relation::is_included(),rel(rel1);

   vector<Linear_Expression>::iterator vi=vp->begin();

   // iterate through vp
   
   for(vi=vp->begin();vi!=vp->end();++vi){
      // What is the relation between the ineqs and (*vi)==0
      rel=ineqs->relation_with((*vi)==0); 

      if (rel==rel1){
         //subsumed?
         incon=true;
         // nothing more to be done
         break;
      }
   }
}

bool DisequalityStore::check_consistent(C_Polyhedron & t){

   // just do the same as in check_consistent()..
   // two changes.. 1. do not set ineqs
   //               2. do not use ineqs member
   //  Post comment== I could have factored check_consistent() into this. I suck
   

   // first check if t is non-empty
   if (t.is_empty()){
      return false;
   }

   
   Poly_Con_Relation rel1=Poly_Con_Relation::is_included(),rel(rel1);
   vector<Linear_Expression>::iterator vi=vp->begin();
   
   for(vi=vp->begin();vi!=vp->end();++vi){
      // What is the relation between the ineqs and (*vi)==0
      rel=t.relation_with((*vi)==0);
      
      if (rel==rel1){
         return false;
         
      }
   }

   return true;
}


void DisequalityStore::add_constraint(SparseLinExpr const & p, int ineq_type){
   // add a constraint

   // if inconsistent .. then nothing to be done here.
   if (incon) return;

   // if the inequality is a disequality .. puch it into
   // vp after converting it to a linear expression a la PPL
   
   if (ineq_type==TYPE_DIS){
      vp->push_back(p.to_lin_expression());
      
   } else {
      ineqs->add_constraint(p.get_constraint(ineq_type));
   }

   // check if the whole new mess is consistent
   check_consistent();
   // return.
}

bool DisequalityStore::is_consistent() const{
   // not inconsistent
   return !incon;
}


int DisequalityStore::get_dimension() const{
   return n;
}


const var_info * DisequalityStore::get_var_info() const{
   return f;
}


void DisequalityStore::print_constraints(ostream & in) const{
   // print the whole thing out using ostream

   if (incon) {
      in <<" Inconsistent Store" <<endl;

#ifdef __DEBUG__D_
      in << "Test Message--"<<endl;
      print_polyhedron(in, *ineqs,f);
      vector<Linear_Expression>::iterator vi;
      
      for(vi=vp->begin();vi<vp->end();vi++){
         print_lin_expression(in, (*vi), f);
         in <<" <> 0"<<endl;
      }
      in <<"Test message ends"<<endl;      
#endif
      return;
   }
   
   in<< "Consistent Store"<<endl;

   // print the polyhedron
   
   print_polyhedron(in, *ineqs,f);

   vector<Linear_Expression>::iterator vi;
   // print the disequalities
   for(vi=vp->begin();vi<vp->end();vi++){
      print_lin_expression(in, (*vi), f);
      in <<" <> 0"<<endl;
   }
   // done
   
   in<<endl;
}

bool DisequalityStore::add_transform(LinTransform const & l){

   // add l==0 after changing l to a linear expression a la PPL
   Linear_Expression l1 = l.to_lin_expression();
   ineqs->add_constraint(l1==0);
   check_consistent();
   return incon;
}

bool DisequalityStore::add_transform_inequality(LinTransform const & l){
   //a la PPL
   Linear_Expression l1 = l.to_lin_expression();
   // add l>=0
   ineqs->add_constraint(l1>=0);
   check_consistent();
   return incon;
}

bool DisequalityStore::add_transform_negated(LinTransform const & l){
   // a la PPL
   Linear_Expression l1 = l.to_lin_expression();
   // add l1<> 0
   vp->push_back(l1);
   check_consistent();
   return incon;
}


ostream & operator << (ostream & in , DisequalityStore const & ds){
   // print
   ds.print_constraints(in);
   return in;
}


DisequalityStore * DisequalityStore::clone() const{
   // clone the disequality store
   // create a new one
   DisequalityStore * ret = new DisequalityStore(n,f);
   // force set inequalities to ineqs (by cloning ineqs)
   
   ret->set_inequalities( ineqs);
   // clone vp and force set
   ret->set_disequalities(vp);

   // recheck the consistency of ret or dump core
   ret->check_consistent();

   // return the clone
   return ret;
}

void DisequalityStore::set_inequalities(C_Polyhedron const *p){
   // force set the inequalities
   // Post comment: Why not use the copy constructor? 

   // iterate through constraints
   
   Constraint_System cs = p->minimized_constraints();
   Constraint_System::const_iterator vi;
   
   // add the constraints one by one
   for (vi=cs.begin();vi!= cs.end(); vi++)
      ineqs->add_constraint(*vi);

   // unnecessary actually.. but will do it anyways
   check_consistent();
   return;
}

void DisequalityStore::set_disequalities(vector<SparseLinExpr>  * vp){
   // each sparselinexpr requires conversion before addition
   
   vector<SparseLinExpr>::iterator vi;
   // add the constraints one by one
   for (vi=vp->begin();vi<vp->end();vi++)
      add_constraint((*vi),TYPE_DIS);

   check_consistent();
}




void DisequalityStore::set_disequalities(vector<Linear_Expression> const * vp1){

   // a different format.. just directly clone
   
   //vector<Linear_Expression>::iterator vi;
   delete(vp);
   vp=new vector<Linear_Expression>(*vp1);

   check_consistent();
}



bool DisequalityStore::check_status_equalities(LinTransform & lt){

   // check if adding lt==0 will create inconsistencies
   // First add lt==0 to the polyhedron and then check consistency

   // create a polyhedron t with ineqs /\ l1 ==0
   
   C_Polyhedron t(*ineqs);

   Linear_Expression l1=lt.to_lin_expression();
   t.add_constraint(l1 ==0);

   // check if things will remain consistent.
   return check_consistent(t);
   
}


DisequalityStore::~DisequalityStore(){
   // the destructor
   delete(vp);
}
