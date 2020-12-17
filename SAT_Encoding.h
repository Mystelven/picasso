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

#ifndef SAT_ENCODING_H
#define SAT_ENCODING_H

#include "Graph.h"
#include "SimpSolver.h"
#include "Encoder.h"
#include "Enc_Totalizer.h"

using namespace std;
using namespace msp;

class SAT_Encoding {

private:

	bool verbose = false;

	unsigned int nbNodes;

public:

	vector< vector<unsigned int> > s_ij;

	vector<unsigned int> n_i;

	inline unsigned int getNbNodes() { return nbNodes; }

	SAT_Encoding(Graph* g, Glucose::Solver* solver);

	~SAT_Encoding() { }

};

#endif