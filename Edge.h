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

#ifndef Edge_h
#define Edge_h

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <assert.h>

/**
 * @brief Represent an Edge between two variables and contain the set of AbstractRelation.
 *
 * It contains also the set of Relations that the edge is allowed to have.
 *
 * @see AbstractRelation
 */
class Edge {

private:

	/** @brief the ID of the input variable */
	unsigned int input;
	
	/** @brief the ID of the output variable */
	unsigned int output;

public:

	inline unsigned int getIn() const { return input; }

	inline unsigned int getOut() const { return output; }


	Edge(unsigned int in, unsigned int out)
	{
		input = in;
		output = out;
	}

	unsigned int getMax() 
	{ 
		if(input > output) return input; 
		else return output; 
	}


	/** 
	 * @brief this function will translate the Edge into a DOT.
	 * 
	 * @param[in] file the file where we want to display DOT, by default: STDOUT.
	 */
	void toDOT(FILE* file)
	{
		std::string label = "";
		
		fprintf(file,"%d -- %d [label=\"%s\"];\n",input,output,label.c_str());	
	}

friend bool operator== (const Edge &c1, const Edge &c2) { return (c1.getIn() == c2.getIn()) && (c1.getOut() == c2.getOut()); }

friend bool operator!= (const Edge &c1, const Edge &c2) { return !(c1 == c2); }

friend bool operator<  (const Edge &c1, const Edge &c2) { return c1.getIn() < c2.getIn(); }

friend bool operator<= (const Edge &c1, const Edge &c2) { return ((c1 < c2) || (c1 == c2)); }

friend bool operator>  (const Edge &c1, const Edge &c2) { return (c1.getIn() > c2.getIn()); }

friend bool operator>= (const Edge &c1, const Edge &c2) { return ((c1 > c2) || (c1 == c2)); }

};

/**
 * @brief Functor to be used in std::sort of Edge*.
 */
struct less_than_inputNodeId { 
	inline bool operator() (Edge* edge1,  Edge* edge2) { return (edge1->getIn() < edge2->getIn()); } 
};

#endif