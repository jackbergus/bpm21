#!/usr/bin/env python3
#
# ltlf_to_graph_dot_files.py
# This file is part of bpm21
#
# Copyright (C) 2021 - Giacomo Bergami
#
# bpm21 is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# bpm21 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with bpm21. If not, see <http://www.gnu.org/licenses/>.
#
from flloat.parser.ltlf import LTLfParser
from flloat.ltlf import LTLfFormula
import sys
from pathlib import Path

if __name__ == "__main__":
    filepath = sys.argv[1]
    path = str(Path(filepath).parent)
    file = filepath[len(path)+1:]
    with open(filepath) as f:
        i = 1
        for line in [line.rstrip() for line in f]:
            parser = LTLfParser()
            auto = parser(line)
            #print(line)
            assert (isinstance(auto, LTLfFormula))
            g3 = auto.to_automaton().to_graphviz()
            g3.save(filename=file+"_graph_"+str(i)+".dot", directory=path)
            i = i+1
        #print("Done!")

