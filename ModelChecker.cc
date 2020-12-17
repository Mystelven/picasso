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

#include "ModelChecker.h"

	void ModelChecker::toDOT_color(FILE* file)
	{

		fprintf(file,"graph g {\n");

		vector<unsigned int> same_colors;

		unsigned int color_index = -1;

		for(unsigned int i = 0; i < graph->nbVertices; ++i)
		{
			if(std::find(same_colors.begin(), same_colors.end(), i) == same_colors.end()) {
				color_index++;
				fprintf(file,"%d [style=filled, fillcolor=%s]\n",i+1,colors[color_index].c_str());
			}

			for(unsigned int j = i+1; j < graph->nbVertices; ++j)
			{
				if(model[encoding->s_ij[i][j]]) 
				{
					fprintf(file,"%d [style=filled, fillcolor=%s]\n",j+1,colors[color_index].c_str());	
					same_colors.push_back(j);
				}
			}	

		}
    		
		for(Edge* e : graph->edges)  e->toDOT(file); 

    	fprintf(file,"}");
	}


	unsigned int ModelChecker::obtainNbColors()
	{
		vector<unsigned int> same_colors;

		unsigned int nbColors = graph->nbVertices;

		for(unsigned int i = 0; i < graph->nbVertices; ++i)
		{
			if(std::find(same_colors.begin(), same_colors.end(), i) != same_colors.end()) {
				nbColors--;
			}

			for(unsigned int j = i+1; j < graph->nbVertices; ++j)
			{
				if(model[encoding->s_ij[i][j]])  
				{
					same_colors.push_back(j);
				}
			}	
		}
    		
		return nbColors;
	}