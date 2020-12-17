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

#ifndef MODEL_CHECKER_H
#define MODEL_CHECKER_H

#include "Graph.h"
#include "SimpSolver.h"
#include "SolverTypes.h"
#include "Encoder.h"
#include "Enc_Totalizer.h"
#include "SAT_Encoding.h"

using namespace std;
using namespace msp;

class ModelChecker {

private:

	Solver* solver;
	Graph* graph;
	SAT_Encoding* encoding;
	unsigned int k;

	vector<bool> model;

	vector<string> colors = {
	  "red", 
	  "blue",
	  "green",
	  "purple",
	  "orange",
	  "yellow",
	  "salmon",
	  "paleturquoise",
	  "lightsteelblue",
	  "palegreen",
	  "palegoldenrod",
	  "aliceblue",
	  "crimson",
	  "darksalmon",
	  "pink",
	  "deeppink",
	  "gold",
	  "firebrick",
	  "dimgrey",
	  "darkorange",
	  "cyan",
	  "beige",
	  "darkgreen",
	  "orchid",
	  "turquoise",
	  "tomato",
	  "tan",
	  "royalblue"
	};

public:

	ModelChecker(Solver * s, Graph* g, SAT_Encoding* e, unsigned int _k) { solver = s; graph = g; encoding = e; k = _k; }

	void setSolver(vector<bool> & copy) { model = copy; }

	void toDOT_color(FILE* file=stdout);

	unsigned int obtainNbColors();

};

#endif