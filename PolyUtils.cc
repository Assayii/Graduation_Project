
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

#include "myassertions.h"
#include "PolyUtils.h"


void print_generators(ostream & out, Generator_System const & gg, var_info  * f){

   Generator_System::const_iterator vi=gg.begin();
   int n=(*vi).space_dimension();
   
   int nf=f->get_dimension();
   
   PRECONDITION((nf >=n), "var-info not informative enough to print");

   int i;
   int j;
   LinExpr l(n,f);
   bool flag;
   for(vi=gg.begin();vi !=gg.end();++vi){
      flag=true;
      
      for (i=0;i<n;i++){
         if (!handle_integers((*vi).coefficient(Variable(i)),j))
            flag = false;
         l[i]=j;
      }
      l[n]=0;
      if (flag){
         out<<l;
         
         if ((*vi).type()==Generator::RAY)
            out <<"  = Ray "<<endl;
         else if ((*vi).type()==Generator::LINE)
            out << " =LINE"<<endl;
         else if ((*vi).type()==Generator::POINT)
            out <<" = POINT" <<endl;
         else
            out <<"= CLOSURE_POINT"<<endl;
      }
   }

   return;

}

ostream &  print_polyhedron(ostream & in, C_Polyhedron const & np, const var_info * f){

   if (np.is_universe()){
      in << "True"<<endl;
      return in;
   }

   if (np.is_empty()){
      in << "false"<<endl;
      return in;
   }
   
   // Assume that f->dimension < the dimension of the polyhedron and that
   // f names the first f->dimension() dimensions of the polyhedron
   // The remaining will just receive some fake name, say "__A"-->"__Z"
   int n=np.space_dimension(), nf=f->get_dimension(),i;
   var_info * f2;
   char  a[4] = {'_','_','A','\0'};
   if (nf>=n){
      f2=new var_info();
      for (i=0;i<n;i++)
         f2->insert(f->get_name(i));
   } else {
      f2=new var_info();
      for ( i=0;i<nf;i++)
         f2->insert(f->get_name(i));
      for(i=nf;i<n;i++){
         a[2]='A'+i-nf;
         f2->insert(a);
      }
      
   }
   bool flag=true;
   int j;
   
   LinExpr  l(n,f2);

   Constraint_System const &  cs = np.minimized_constraints();

   Constraint_System::const_iterator vi;

   for(vi=cs.begin();vi !=cs.end();++vi){
      flag =true;
      
      for (i=0;i<n;i++){
         if (!handle_integers((*vi).coefficient(Variable(i)), j))
            flag = false;
         
         l[i]=j;
      }
      
      if (!handle_integers((*vi).inhomogeneous_term(),j))
         flag = false;
      
      l[n]=j;
      
      if (flag) {
         in<<l;

      
         if ((*vi).type()==Constraint::EQUALITY)
            in <<"  = 0 "<<endl;
         else if ((*vi).type()==Constraint::NONSTRICT_INEQUALITY)
            in << " >= 0"<<endl;
         else
            in <<" > 0" <<endl;
      }
   }
   
   
   delete(f2);

   return in;
   
}



bool handle_integers(Coefficient const & t, int & res){
   bool ret=true;
   
   if (!t.fits_sint_p()){
      cout<<"Fatal Warning from PolyUtils::handle_integers-- gmp integer overflow"<<endl;
      ret=false;
   }
   
   res= (int) t.get_si();

   return ret;
}

int handle_integers(Coefficient const & t){

   if (!t.fits_sint_p()){
      cout<<"Fatal Warning from PolyUtils::handle_integers-- gmp integer overflow"<<endl;
      exit(1);
   }
 
   return (int) t.get_si();
         
}

ostream & print_lin_expression(ostream & in, Linear_Expression const & lp, const var_info * f){
   // print the linear expression lp using var_info f
   int n=lp.space_dimension(),nf=f->get_dimension(),i;
   var_info * f2;
   char  a[4] = {'_','_','A','\0'};
   if (nf>=n){
      f2=new var_info();
      for (i=0;i<n;i++)
         f2->insert(f->get_name(i));
   } else {
      f2=new var_info();
      for ( i=0;i<nf;i++)
         f2->insert(f->get_name(i));
      for(i=nf;i<n;i++){
         a[2]='A'+i-nf;
         f2->insert(a);
      }
      
   }
   LinExpr  l(n,f2);
   bool flag=true;
   int j;
   for (i=0;i<n;i++){
      if (!handle_integers(lp.coefficient(Variable(i)),j))
         flag = false;
      
      l[i]=j;
   }
   
   if (!handle_integers(lp.inhomogeneous_term(),j))
      flag = false;
   
   l[n] = j;

   if (flag )
      in<<l;

   delete(f2);
   return in;
}


ostream & print_constraint( ostream & in, Constraint const & cc, var_info * f){
   unsigned n=f->get_dimension();
   unsigned i;
   if (cc.space_dimension()>n){
      in<<"Space dimension exceeds var_info dimension in ostream & print_constraint(ostream &.....)"<<endl;
      return in;
   }
   LinExpr  l(n,f);
   bool flag=true;
   int j;
   for (i=0;i<n;i++){
      if (!handle_integers(cc.coefficient(Variable(i)),j))
         flag = false;
      l[i]=j;
   }
   
   if (!handle_integers(cc.inhomogeneous_term(),j))
      flag=false;
   
   l[n]=j;
   
   if (flag){
      in<<l;
   
      if (cc.type()==Constraint::EQUALITY)
         in <<"  = 0 "<<endl;
      else if (cc.type()==Constraint::NONSTRICT_INEQUALITY)
         in << " >= 0"<<endl;
      else
         in <<" > 0" <<endl;
   }
   return in;

}



void dualize (C_Polyhedron const & p, C_Polyhedron & result){

   int n=p.space_dimension();


   if (p.is_empty()){
      result=C_Polyhedron(n+1,UNIVERSE);
      return;
   }

   Constraint_System  cs = p.minimized_constraints();
   Constraint_System::const_iterator vi ;

   
   
   int i,j,nc;

   //count the number of multiplier variables required
   nc=0;
   
   for (vi=cs.begin();vi!=cs.end(); ++vi)
      nc++;

   
   result=C_Polyhedron(n+1+nc, UNIVERSE);
   
   Linear_Expression lin(0);

   // Now build the constraints
   for (i=0;i<n;i++){
      
      lin=Linear_Expression(0);
      lin=lin - Variable(i); // add -c_i to the constraint
      j=0;

      for (vi=cs.begin();vi!=cs.end(); ++vi){
         lin=lin + (*vi).coefficient(Variable(i)) * Variable(n+1+j);
         j++;
      }

      result.add_constraint(lin == 0); // Add the constraint lin==0 to the result
      
   }
   
   // Now the constraints on the constant
   lin=Linear_Expression(0);
   lin=lin- Variable(n); // add -d to the constraint
   j=0;
   for (vi=cs.begin();vi!=cs.end(); ++vi){
      lin=lin + (*vi).inhomogeneous_term() * Variable(n+1+j);
      j++;
   }

   result.add_constraint(lin <= 0);

   // Now the constraints on the multipliers

   j=0;
   for (vi=cs.begin();vi!=cs.end(); ++vi){
      if ((*vi).type()==Constraint::NONSTRICT_INEQUALITY){
         result.add_constraint(Variable(n+1+j) >= 0);
      } else if ((*vi).type()==Constraint::STRICT_INEQUALITY){
         cerr<<"Location::compute_dual_constraints -- Warning: Encountered Strict Inequality"<<endl;
         cerr<<"                "<<(*vi)<<endl;
         result.add_constraint(Variable(n+1+j) >= 0); // Just handle it as any other inequality 
      }
      
      j++;
   }

   result.remove_higher_space_dimensions(n+1);// Remove the excess dimensions to obtain a new Polyhedron

      
   return;
}

void primal(C_Polyhedron const & what, C_Polyhedron & result){

   int n= (int) what.space_dimension() -1 ;

   // compute the generators
   Generator_System gs= what.generators();
   Generator_System::const_iterator vi;
   
   // create a result polyhedron 
   result= C_Polyhedron (n, UNIVERSE);
   
   // traverse through the generators
   int i;
   Linear_Expression lin(0);

   for (vi=gs.begin(); vi != gs.end(); ++vi){
      lin*=0;
      lin+=(*vi).coefficient(Variable(n));

      for (i=0;i<n;++i)
         lin += (*vi).coefficient(Variable(i)) * Variable(i);

      if ((*vi).is_point() || (*vi).is_ray()){

          result.add_constraint( lin >=0);
          
       } else if ((*vi).is_line()) {

          result.add_constraint(lin ==0 );
          
       } else {
          result.add_constraint(lin > 0);
       }
   }
   
}


void test_and_add_generator(int n, Generator const & a, C_Polyhedron const & test, C_Polyhedron & result)
{
   if (test.relation_with(a)==Poly_Gen_Relation::subsumes()){                   
      result.add_generator(a);                                                  
   }                                                                            
}


void H79_narrow(C_Polyhedron & result, C_Polyhedron const & what) {

   //
   // take all the generators of the what and
   // reaffirm them
   //
   
   if (what.is_universe())
      return;
   
   
   int n=result.space_dimension();
   int i;
   
   C_Polyhedron ret(n,UNIVERSE);

   for (i=0;i<n;++i)
      ret.add_constraint(Variable(i)==0);
   
   Generator_System gs=what.generators();

   Generator_System::const_iterator vi;

   
   for (vi=gs.begin(); vi !=gs.end(); ++vi){
      if (!(*vi).is_line()){
         test_and_add_generator(n, (*vi), result, ret);
      } else {
         Linear_Expression lin(0);
         for (i=0;i<n;++i)
            lin+= (*vi).coefficient(Variable(i)) * Variable(i);
         
         test_and_add_generator(n,ray(lin),result,ret);
         test_and_add_generator(n,ray(0-lin),result,ret);
      }
   }

    

   result=ret;
}

void H79_narrow(C_Polyhedron & result, C_Polyhedron const & what, vector<Generator> & frames ) {

   //
   // take all the generators of the what and
   // reaffirm them
   //
   
   if (what.is_universe())
      return;
   
   
   int n=result.space_dimension();
   int i;
   
   C_Polyhedron ret(n,UNIVERSE);

   for (i=0;i<n;++i)
      ret.add_constraint(Variable(i)==0);
   
   Generator_System gs=what.generators();

   Generator_System::const_iterator vi;

   
   for (vi=gs.begin(); vi !=gs.end(); ++vi){
      if (!(*vi).is_line()){
         test_and_add_generator(n, (*vi), result, ret);
      } else {
         Linear_Expression lin(0);
         for (i=0;i<n;++i)
            lin+= (*vi).coefficient(Variable(i)) * Variable(i);
         
         test_and_add_generator(n,ray(lin),result,ret);
         test_and_add_generator(n,ray(0-lin),result,ret);
      }
   }

   // now for each member of frames check if it is common to both result and what,
   // if so, add it to ret.

   vector<Generator>::iterator vj;

   for (vj=frames.begin(); vj != frames.end(); ++vj){

      if (ret.relation_with((*vj))== Poly_Gen_Relation::subsumes())
         continue;

      if (result.relation_with((*vj))==Poly_Gen_Relation::subsumes())
         ret.add_generator((*vj));
      else {
         vj=frames.erase(vj);
         --vj;
      }
   }

   result=ret;
}

void set_up_affine_transform(int n, Constraint const & cc, Linear_Expression & left, Linear_Expression & right){
   
   // decompose the expression into two parts, the right has the expression from
   // 0.. n-1 the left has expression n.. 2n-1
   // returns true if left ==0
   
   int i;
   PRECONDITION(cc.space_dimension()== (unsigned) 2*n,
                "std::set_up_affine_transform mismatched space dimension");

   left *= 0;
   right *= 0;

   for (i=0;i<n;++i){
      right -= cc.coefficient(Variable(i)) * Variable(i);
      left += cc.coefficient(Variable(i+n)) * Variable(i+n);
   }
   right -= cc.inhomogeneous_term();

   return;
   
}
