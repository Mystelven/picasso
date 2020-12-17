#/***************************************************************************************************
#* Copyright (c) 2018 Artois University
#* Copyright (c) 2018 Côte d'Azur University
#*
#* This file is part of Tree-Width Abstractions
#*
#* Tree-Width Abstractions is free software: you can redistribute it and/or modify it 
#* under the terms of the GNU General Public License as published by the Free Software Foundation, 
#* either version 3 of the License, or (at your option) any later version.
#*
#* Tree-Width Abstractions is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
#* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#* See the GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License along with Tree-Width Abstractions.
#* If not, see http://www.gnu.org/licenses/.
#*
#* Authors:
#*       - Valentin Montmirail (montmirail@cril.fr)
#*		 - Marie Pelleau	   (marie.pelleau@unice.fr)
#* 		 - Jean-Marie Lagniez  (lagniez@cril.fr)
#* Contributors:
#*       - Valentin Montmirail (montmirail@cril.fr)
#***************************************************************************************************/

# compiler :
COMPILER = g++

LPROFILAGE = -fprofile-arcs -ftest-coverage -fPIC -O0

COPTIONS = -O3 -Wall -Wextra -Wno-unused-parameter -std=c++11 
COPTIONS_DEBUG = -pg -g -Wall -Wextra -Wno-unused-parameter -std=c++11 $(LPROFILAGE)

LOPTIONS += -static -lboost_system

UNAME_S := $(shell uname -s)

EXECUTABLE = graphcoloring

# -------------------------------------------------------------------
#  Emplacement des sources et des objets, des includes et de l'executable
# -------------------------------------------------------------------

# path:
CODE = $(shell pwd)

# folder for the executable 
EXEDIR = $(CODE)/bin
DEBUGDIR = $(CODE)/debug

# adress of the source code, objects and include files:
SRCDIR = $(CODE)
OBJDIR = $(CODE)/obj

# create the list of file sources:
SRC = $(wildcard $(SRCDIR)/*.cc)

# list the files:
NOM = $(basename $(notdir $(SRC)))
OBJ = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NOM)))
OBJ_DEBUG = $(addprefix $(OBJDIR)/, $(addsuffix .od, $(NOM)))

.PHONY: help

release: faire_dossier $(OBJ)		
	$(COMPILER) -o $(EXEDIR)/$(EXECUTABLE) $(OBJ)

debug: faire_dossier $(OBJ_DEBUG)
	$(COMPILER) -o $(DEBUGDIR)/$(EXECUTABLE) $(LPROFILAGE) $(OBJ_DEBUG) 

# link edition
install: faire_dossier release
	make release

# rules to do the folders
faire_dossier: makedir
	@echo "folders created"

# compilation rules
$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	$(COMPILER) -c $(COPTIONS) -o $@ $< 
	
$(OBJDIR)/%.od: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	$(COMPILER) -c $(COPTIONS_DEBUG) -o $@ $< 

all: release
	make release

# -------------------------------------------------------------------
#  to create the objects folder
# -------------------------------------------------------------------

# creation of the folder $(OBJDIR) if it is necessary:
makedir: 
	mkdir -p $(OBJDIR)
	mkdir -p $(EXEDIR)
	mkdir -p $(DEBUGDIR)

# -------------------------------------------------------------------
#  cleaning rules
# -------------------------------------------------------------------

.PHONY: clean purge

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(EXEDIR)
	@rm -rf $(DEBUGDIR)
	@rm -rf *~


help:
	@echo "--------------------------------------------------------------------------"
	@echo " Disponible rules"
	@echo
	@echo " install : compilation and executable creation"
	@echo " clean   : remove the object files"
	@echo " purge   : remove the object files and the executable"
	@echo " help    : print this comments (defautl)"
	@echo "--------------------------------------------------------------------------"
	@echo
