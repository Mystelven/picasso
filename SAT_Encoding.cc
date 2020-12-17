/****************************************************************************************
 *
 * This file is part of Graph Coloring
 *
 * Graph Coloring is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * Graph Coloring is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Graph Coloring.
 * If not, see http://www.gnu.org/licenses/.
 *
 * Contributors:
 *       - Valentin Montmirail (valentin.montmirail@univ-cotedazur.fr)
 ***************************************************************************************************/


#include "SAT_Encoding.h"


	SAT_Encoding::SAT_Encoding(Graph* graph, Glucose::Solver* solver)
	{	
		openwbo::Encoder encoder;
		
		openwbo::Totalizer encoding;

		nbNodes = graph->getNbNodes();

		n_i.resize(nbNodes);

		unsigned int index_of_variable = 0;

		for(unsigned int i = 0; i < nbNodes; ++i) n_i[i] = index_of_variable++;

		for(unsigned int i = 0; i < nbNodes; ++i) 
		{	
			s_ij.resize(nbNodes);
			for(unsigned int j = i+1; j < nbNodes; ++j)
			{
				s_ij[i].resize(nbNodes);

				s_ij[i][j] = ++index_of_variable;
			} 
		}

		while(solver->nVars() <= (int)index_of_variable) encoding.newSATVariable(solver);

		/* To assure that two nodes connected are not in the same color. */
		for(unsigned int i = 1; i <= nbNodes; ++i) 
		{	
			for(unsigned int j = i+1; j <= nbNodes; ++j)
			{	
				if(graph->are_nodes_connected(i,j))
				{	
					if(verbose) cout << "c | ~s_"<<(i)<<"," << (j) << " (" << s_ij[i-1][j-1] << ")"  << endl;

					/* if the nodes i and j are connected, they cannot be in the same color. */
					encoding.addUnitClause(solver,Glucose::mkLit(s_ij[i-1][j-1],true));
				}

			}
		}

		if(graph->setOfTriplets.size() == 0)
		{
			/* To assure the transitivity and eucleanity of the colors. */
			for(unsigned int i = 1; i <= nbNodes; ++i) 
			{	
				for(unsigned int j = i+1; j <= nbNodes; ++j)
				{	
					for(unsigned int k = j+1; k <= nbNodes; ++k)
					{
						if(verbose) cout << "c | s_"<<(i)<<"," << (j) << " (" << s_ij[i-1][j-1] << ")" << " & s_" << (j) << "," << (k) << " (" << s_ij[j-1][k-1] << ")" << " --> " << "s_" << (i) << "," << (k) << " (" << s_ij[i-1][k-1] << ")" << endl;
						encoding.addTernaryClause(solver,Glucose::mkLit(s_ij[i-1][j-1],true),Glucose::mkLit(s_ij[j-1][k-1],true),Glucose::mkLit(s_ij[i-1][k-1],false));

						if(verbose) cout << "c | s_"<<(i)<<"," << (j) << " (" << s_ij[i-1][j-1] << ")" << " & s_" << (i) << "," << (k) << " (" << s_ij[i-1][k-1] << ")" << " --> " << "s_" << (j) << "," << (k) << " (" << s_ij[j-1][k-1] << ")" << endl;
						encoding.addTernaryClause(solver,Glucose::mkLit(s_ij[i-1][j-1],true),Glucose::mkLit(s_ij[i-1][k-1],true),Glucose::mkLit(s_ij[j-1][k-1],false));
					}
				}
			}
		} else {

			for(auto triangle : graph->setOfTriplets)
			{
				unsigned int i = triangle[0]-1;
				unsigned int j = triangle[1]-1;
				unsigned int k = triangle[2]-1;

				if(verbose) cout << "c | s_"<<(i+1)<<"," << (j+1) << " (" << s_ij[i][j] << ")" << " & s_" << (j+1) << "," << (k+1) << " (" << s_ij[j][k] << ")" << " --> " << "s_" << (i+1) << "," << (k+1) << " (" << s_ij[i][k] << ")" << endl;
				encoding.addTernaryClause(solver,Glucose::mkLit(s_ij[i][j],true),Glucose::mkLit(s_ij[j][k],true),Glucose::mkLit(s_ij[i][k],false));

				if(verbose) cout << "c | s_"<<(i+1)<<"," << (j+1) << " (" << s_ij[i][j] << ")" << " & s_" << (i+1) << "," << (k+1) << " (" << s_ij[i][k] << ")" << " --> " << "s_" << (j+1) << "," << (k+1) << " (" << s_ij[j][k] << ")" << endl;
				encoding.addTernaryClause(solver,Glucose::mkLit(s_ij[i][j],true),Glucose::mkLit(s_ij[i][k],true),Glucose::mkLit(s_ij[j][k],false));
			}
		}


		/* Now we need to count the number of colors needed */
		for(unsigned int k = 2; k <= nbNodes; ++k)
		{	
			Glucose::vec<Glucose::Lit> clause;
			for(unsigned int i = 1; i <= nbNodes; ++i)
			{	
				if(i < k)
				{
					clause.push(Glucose::mkLit(s_ij[i-1][k-1],false));
					if(verbose) cout << "c | ~n_" << (k) << " (" << n_i[k-1] << ")" << " v " << "~s_" << (i) << "," << (k) << " (" << s_ij[i-1][k-1] << ")" << endl;
					encoding.addBinaryClause(solver,Glucose::mkLit(n_i[k-1],true),Glucose::mkLit(s_ij[i-1][k-1],true));
				}
			}

			if(verbose) 
			{
				cout << "c | n_" << (k) << " (" << n_i[k-1] << ")" << " v ";
				for(unsigned int i = 1; i <= nbNodes; ++i)
				{
					if(i < k) 
					{
						cout << "s_" << (i) << "," << (k) << " (" << s_ij[i-1][k-1] << ")";					
						if(i+1 < nbNodes) cout << " v ";
					}

					
				}
				cout << endl;
			}

			clause.push(Glucose::mkLit(n_i[k-1],false));
			solver->addClause(clause);
		}		
	}
