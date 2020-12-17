/*****************************************************************************************
* Copyright (c) 2018 Artois University
*
* This file is part of CEGAR-RCC8
*
* CEGAR-RCC8 is free software: you can redistribute it and/or modify it 
* under the terms of the GNU General Public License as published by the Free Software Foundation, 
* either version 3 of the License, or (at your option) any later version.
*
* CEGAR-RCC8 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with CEGAR-RCC8.
* If not, see http://www.gnu.org/licenses/.
*
* Authors:
* 		 - Michael Sioutis 	   (michael.sioutis@oru.se)
* 		 - Jean Marie Lagniez  (lagniez@cril.fr)
* 		 - Valentin Montmirail (montmirail@cril.fr)
*
* Contributors:
* 		 - Valentin Montmirail (montmirail@cril.fr)
***************************************************************************************************/

#define protected public

bool triangulation = false;

#include "Main.h"

using namespace std;

inline int myLit(Lit l)
{
    return sign(l) ? -(var(l)) : (var(l));
}


int main(int argc, char** argv)
{
	auto begin = chrono::high_resolution_clock::now();
 	cout << "c +=======================================================================================================+" << endl;
	cout << "c | Graph Coloring: An Incremental SAT-Based Approach                                                     |" << endl;
	cout << "c |                                                                                                       |" << endl;
	cout << "c | Based on Glucose 4.1 (core version)                                                                   |" << endl;
	cout << "c | Version:      March 2019 -- Release: 0.1                                                              |" << endl;
	cout << "c | Authors:      Valentin Montmirail                                                                     |" << endl;
	cout << "c | Contributors: Valentin Montmirail                                                                     |" << endl;
	cout << "c | Contact:      valentin.montmirail@univ-cotedazur.fr                                                   |" << endl;
	cout << "c |-------------------------------------------------------------------------------------------------------|" << endl;

	setbuf( stdout , NULL );
	bool res = true;
	bool toPrintModel = false;
	double elaspedTimeMs = 0.0f;
	Glucose::SimpSolver glucose;

	vector<bool> copy_model;

	if ( (argc > 1 && !strcmp("-t", argv[1])) || (argc > 2 && !strcmp("-t", argv[2])) )
	{
		triangulation = true;
	}

	if ( (argc > 1 && !strcmp("-print", argv[1])) || (argc > 2 && !strcmp("-print", argv[2])) )
	{
		toPrintModel = true;
	}	
	
	Graph graph(triangulation);

	if ( (argc > 1 && !strcmp("-dot", argv[1])) || (argc > 2 && !strcmp("-dot", argv[2])) )
	{
		graph.toDOT(stderr);
		cerr << endl;
		exit(0);
	}	

	glucose.setIncrementalMode();
	glucose.verbosity = 0;
	glucose.adaptStrategies = false;
	
	SAT_Encoding sat_encoding(&graph,&glucose);	

	cout << "c | #Variables:  " << glucose.nVars() << endl;
	cout << "c | #Clauses:    " << glucose.nClauses() << endl;

	cout << "c |-------------------------------------------------------------------------------------------------------|" << endl;

	cout << "c | Start solving.                                                                                        | " << endl;

	unsigned int k = sat_encoding.getNbNodes();

	if ( (argc > 1 && !strcmp("-maxsat", argv[1])) || (argc > 2 && !strcmp("-maxsat", argv[2])) )
	{
		FILE* file = stdout;		
		
		unsigned int size_clauses = glucose.clauses.size();
		unsigned int size_trail = glucose.trail.size();

		fprintf(file,"p wcnf %d %d %d\n", glucose.nVars(),size_clauses+size_trail+k,k);
		
		for (unsigned int i = 0 ; i < size_clauses; ++i)
		{
			Glucose::Clause &c = glucose.ca[glucose.clauses[i]];
		
			fprintf(file,"%d ", k);

			unsigned int c_size = c.size();

			for(unsigned int j = 0 ; j < c_size; ++j) 
				fprintf(file,"%d ", myLit(c[j]));

			fprintf(file,"0\n"); 
		}

		for(unsigned int i = 0; i < size_trail; ++i) 
			fprintf(file,"%d %d 0\n",k, myLit(glucose.trail[i]));

		/* Need to maximize the number of n_i to false :) thus minimize the number of colors. */
		for(unsigned int i = 1; i < k; i++) 
			fprintf(file,"1 -%d 0\n",sat_encoding.n_i[i]);
	
		exit(0);
	}

	ModelChecker model(&glucose,&graph,&sat_encoding,k);
	openwbo::Adder encoder;	

	vec<Lit> n_is;
	vec<uint64_t> coeffs;
	for(unsigned int i = 0; i < sat_encoding.n_i.size(); i++) 
	{
		n_is.push(Glucose::mkLit(sat_encoding.n_i[i],true));	
		coeffs.push(1);
	}


	try {

		vec<Lit> assumptions;		
		encoder.encodeInc(&glucose, n_is, coeffs, k, assumptions);

		while(res && k > 0)
		{	
			auto t_start = chrono::high_resolution_clock::now();

			res = glucose.solve(assumptions);

			auto t_end = chrono::high_resolution_clock::now();		
			elaspedTimeMs = std::chrono::duration<double, std::milli>(t_end-t_start).count();			   
			printf("c | Solving for k = %5u : %20.5f ms | p cnf %10d %10d | Assumptions : %d \n",k,elaspedTimeMs,glucose.nVars(),glucose.nClauses(),assumptions.size());

			if(res)
			{											
				copy_model.resize(glucose.model.size());
				for(int i = 1; i < glucose.model.size(); i++)
				{
					copy_model[i] = (glucose.model[i] == l_True);
				}				

				model.setSolver(copy_model);

				unsigned int new_k = model.obtainNbColors();

				if(new_k < k) k = new_k;
				else k--;

				encoder.updateInc(&glucose,k,assumptions);				
				
				// model.toDOT_color(stderr);		
			} 		
		}
	} 
	catch (const Glucose::OutOfMemoryException & ex) 
    {

      std::cout << "s UNKNOWN" << std::endl;
      std::cout << "c Glucose run out of memory during the solving phase..." << std::endl;
    }

	glucose.printIncrementalStats();

	cout << "s SATISFIABLE" << endl;
	model.setSolver(copy_model);
	k = model.obtainNbColors();			
	cout << "o " << k << " colors" << endl;
	if(toPrintModel)
	{
		model.toDOT_color(stderr);
		cerr << endl;
	}
	
	// cout << "c v ";
 	// for(unsigned int i = 1; i < copy_model.size(); i++) printf("%s%u ",(copy_model[i] == true) ? "" : "-", (i));    
 	// cout << endl;
	
	cout << "c | End solving.                                                                                          |" << endl;
	
	auto end = chrono::high_resolution_clock::now();
	elaspedTimeMs = std::chrono::duration<double, std::milli>(end-begin).count();			   
	printf("c | Total: %20.5f ms\n",elaspedTimeMs);

	cout << "c +=======================================================================================================+" << endl;	

	
	exit(0);
}