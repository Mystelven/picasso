// Copyright (c) 2014 Denis Maua
// All Rights Reserved.
//
// This file is part of MSP library
//
// MSP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MSP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with MSP.  If not, see <http://www.gnu.org/licenses/>.

/** This file implements the Graph class, which contains routines for manipulating domain graphs */

#include "Graph.h"
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <chrono>
#include <set>

using namespace std;

namespace msp {


  Graph::Graph(bool triangulation) 
  {    
    cout << "c | Parsing started                                                                                       |" << endl;
    auto t_start = chrono::high_resolution_clock::now();
    double elaspedTimeMs;

    for (string line; getline(cin, line);) 
    { 
      if(line[0] == 'c') continue;

      istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> tokens(beg, end);

      if(line[0] == 'p')
      {
          nbVertices = stoi(tokens[2]);
          nbEdges    = stoi(tokens[3]);

          printf("c | *  %8d edges                                                                                     | \n",nbEdges);
          printf("c | *  %8d nodes                                                                                     | \n",nbVertices); 

          nbEdges = 0;
          variable.resize(nbVertices+1);

          for(unsigned int i = 0; i <= nbVertices; i++)
          {
            variable[i] = new Variable(i,std::to_string((i)));
          }

      } 
      else if(line[0] == 'e') 
      { 
        add_edge(stoi(tokens[1]), stoi(tokens[2]));
      }
    }
 
    std::vector<Factor > factors;

    toInitialize(factors);    

    auto t_end = chrono::high_resolution_clock::now();
    elaspedTimeMs = std::chrono::duration<double, std::milli>(t_end-t_start).count();        
    printf("c | Parsing done: %4.5f ms\n",elaspedTimeMs);

    if(triangulation)
    {
      t_start = chrono::high_resolution_clock::now();
      triangulate(setOfTriplets);
      t_end = chrono::high_resolution_clock::now();
      std::cout << "c | Number of Triangles: " << setOfTriplets.size() << endl;
      printf("c | Triangulation done: %4.5f ms\n",elaspedTimeMs);
    }

    // cout << *this << endl;

  }

  bool Graph::are_nodes_connected(unsigned int i, unsigned int j)
  {
    for(Edge* e : edges)
    {
      if( (e->getIn() == i && e->getOut() == j) || (e->getIn() == j && e->getOut() == i)) 
        return true;
    }

    return false;
  }

  /** Constructor for given list of factors */
  void Graph::toInitialize( std::vector<Factor >& factors )
  {
    _l_width =  0;
    _l_wwidth = 0;

    _nodes.resize(_g.size());

    unsigned card=0;
    
    // unmark all nodes and find highest size of variable
    for (auto v: _g) {

	     _p[v.first] = false;
    	 if (v.first->size() > card) card = v.first->size();
    }

    _u_width  = _g.size(); // trivial upper bound on treewidth

    _u_wwidth = (_u_width+1)*log2(card); // trivial upper bound on weighted treewidth

    // // run min-degree heuristic to find lower bound on treewidth
    std::pair<Variable*, unsigned> r;

    for (unsigned i=0; i < _nodes.size(); i++) {

     	// select variable
    	r = min_degree();

    	// add it to order
    	_nodes[i] = r.first;

     	// mark variable
    	_p[r.first] = true;

     	// for detecting maximum variable cardinality
    	if (r.first->size() > card) card = r.first->size();

    	double degree = 0; double weight = log2(r.first->size());
    	// comput weigth
    	for (auto x: _g[r.first])
        
        if (!_p[x]) {
    		  degree++; // node's degree in chordalization
    		  weight += log2(x->size()); // node's weight in cordalization
    	  }

    	  if (degree>_l_width) _l_width=degree; // update lower bound on treewidth

    	  if (weight>_l_wwidth) _l_wwidth=weight; // update lower bound on weighted treewidth
    }

  }

  /** Constructor for given vector of variable cliques */
  Graph::Graph( std::vector<std::vector<Variable* > >& cliques ) : _l_width(0), _l_wwidth(0) // trivial lower bounds on treewidth
  {
    // turn factor scopes into cliques
    for (auto clique: cliques)
      for (auto u: clique)
	for (auto v: clique)
	  if (u != v) _g[u].insert(v);

    _nodes.resize(_g.size());

    unsigned card=0;
    // unmark all nodes and find highest size of variable
    for (auto v: _g)
      {
	_p[v.first] = false;
    	if (v.first->size() > card) card = v.first->size();
      }
    _u_width  = _g.size(); // trivial upper bound on treewidth
    _u_wwidth = (_u_width+1)*log2(card); // trivial upper bound on weighted treewidth

    // // run min-degree heuristic to find lower bound on treewidth
    std::pair<Variable*, unsigned> r;
    for (unsigned i=0; i < _nodes.size(); i++)
      {
     	// select variable
    	r = min_degree();
    	// add it to order
    	_nodes[i] = r.first;
     	// mark variable
    	_p[r.first] = true;
     	// for detecting maximum variable cardinality
    	if (r.first->size() > card) card = r.first->size();

    	double degree = 0; double weight = log2(r.first->size());
    	// comput weigth
    	  for (auto x: _g[r.first])
    	    if (!_p[x])
    	      {
    		degree++; // node's degree in chordalization
    		weight += log2(x->size()); // node's weight in cordalization
    	      }
    	  if (degree>_l_width) _l_width=degree; // update lower bound on treewidth
    	  if (weight>_l_wwidth) _l_wwidth=weight; // update lower bound on weighted treewidth
    }
  }

  /** Min fill heuristic.
   *
   * Chooses node with least number of unconnected neighbors and returns a pair (node, degree*), where degree* is the number of higher-ordered neighbors.
   *
   */
  std::pair<Variable*, unsigned> Graph::min_fill()
  {
    
    std::pair<Variable* , unsigned> res(0, _g.size());
    for (auto var: _p)
      if (!var.second)
	{ 
	  unsigned this_score = 0; // score for this variable
	  // count the number of fill-in edges for each variable
	  for (auto x: _g[var.first])
	    if (!_p[x])
	      for (auto y: _g[var.first])
		if (*x < *y && !_p[y] && !_g[x].count(y)) // y is not a adjacent to x -- fill-in edge found!
		      this_score++;
	  if (this_score < res.second)
	    { // update incubent solution
	      res.second = this_score; res.first=var.first;
	    }
	}
    return res;
  }

  /** Min degree heuristic.
   *
   * Chooses node with least number of unconnected neighbors, and  returns a pair (node, degree*), where degree* is the number of higher-ordered neighbors.
   *
   */
  std::pair<Variable*, unsigned> Graph::min_degree()
  {
    
    std::pair<Variable* , unsigned> res(NULL, _g.size());
    for (auto var: _p)
      if (!var.second)
	{ 
	  unsigned this_score = 0; // score for this variable
	  for (auto x: _g[var.first])
	    if (!_p[x]) 	  // count the number of higher-order neighbors
	      this_score++;
	  if (this_score < res.second)
	    { // update incubent solution
	      res.second = this_score; res.first=var.first;
	    }
	}
    return res;
  }

  /** Triangulates the graph and finds a suitable variable elimination sequence.
   *
   *  Uses a heuristic to triangulate the graph (i.e., make it chordal) and then
   *  finds a perfect elimination sequence for the resulting chordal graph.
   *
   */
  void Graph::triangulate(std::vector<std::vector<unsigned int>> & setOfTriplets)
  {
    std::pair<Variable*, unsigned> r;

    // unmark all nodes
    for (auto v: _p) _p[v.first] = false;

    unsigned int i = 0, degree = 0;

    while (i <= _nodes.size()+1)
    {
      // select variable to eliminate
      r = min_fill();

      // add it to order
      _nodes[i++] = r.first;

      // mark variable
      _p[r.first] = true;

      degree = 0; 
        
      // connect neighbors
      for (auto x: _g[r.first])
      {
      	if (!_p[x])
        {
          degree++; // node's degree in chordalization            
            	
          for (auto y: _g[r.first])
          {              
            if (x != NULL && y != NULL && !_p[y])
            { 
              unsigned int i = r.first->id();
              unsigned int j = x->id();
              unsigned int k = y->id();              

              if( i != j && j != k && i != k)
              {
                std::vector<unsigned int> triplet;              
                triplet.push_back(i);
                triplet.push_back(j);
                triplet.push_back(k);
                sort(triplet.begin(),triplet.end());
                
                bool toAdd = true;

                for(auto t : setOfTriplets)
                {
                  if(t == triplet) 
                  {
                    toAdd = false;
                    break;
                  }
                }

                if(toAdd)
                { 
                  // cout << "c | ";
                  // for (auto t: triplet) cout << t << " - ";
                  // cout << endl;
                  setOfTriplets.push_back(triplet);
                }
              }
            }
          }
      	}   
      }   	 
    }   

    // exit(0);
  }

  /** Returns a simplicial node.
   *
   * @return a pointer to a simplicial node or a NULL if no simplicial exists.
   */
  Variable* Graph::find_simplicial()
  {
    for (auto v: _nodes)
      {
	bool simplicial = true;
	for (auto x: _g[v])
	  if (!_p[x])
	    for (auto y: _g[v])
	      if (*x < *y && !_p[y] && !_g[x].count(y))
		{
		  simplicial = false; break;
		}
	if (simplicial) return v;
      }
    return NULL;
  }


  /** Returns an ordered vector of Variables 
   *
   * @return a vector of variable objects ordered accordingly
   */
  std::vector<Variable > Graph::ordering()
  {
    std::vector<Variable > order(_nodes.size());
    for (unsigned i=0; i < _nodes.size(); i++)
      order[i] = *(_nodes[i]);
    return order;
  }

  /** Auxiliar function for printing out the edges. */
  std::ostream& Graph::print_edges( std::ostream &o )
  {
    for (auto u: _g)
      {	
	o << "c | " << (_p[u.first]?"*":" ") << u.first->name() << ": ";
	for (Variable* v: _g[u.first])
	  o << v->name() << " ";
	o << "," << std::endl;
      }
    return o;
  }

 /**
  * @brief this methods displays the whole problem in the DOT format.
  * @param[in] file the file in which we display the problem.
  * by default: stdout
  */
  void Graph::toDOT(FILE* file)
  {
    fprintf(file,"graph g {\n");

    for(Edge* e : edges)  e->toDOT(file); 

    fprintf(file,"}");

  }

  /** Default printing. */
  std::ostream& operator<<(std::ostream &o, Graph &g) 
  {
    std::pair<unsigned,unsigned> tw = g.treewidth();
    std::pair<double,double> wtw = g.w_treewidth();
    o << "c | Graph(treewidth:[" << tw.first << "," << tw.second << "], w_treewidth:[" << wtw.first << "," << wtw.second << "], edges:{" << std::endl;
    g.print_edges(o);
    o << "c | })";
    return o;
  }

}
