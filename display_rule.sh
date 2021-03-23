#!/bin/bash
export CLASSPATH=".:/usr/local/lib/antlr-4.9.1-complete.jar:$CLASSPATH"
alias antlr4='java -jar submodules/antlr4/tool/target/antlr4-4.9.2-SNAPSHOT-complete.jar'
antlr4 antlr4/DAD.g
cd antlr4
export CLASSPATH="../submodules/antlr4/tool/target/antlr4-4.9.2-SNAPSHOT-complete.jar:$CLASSPATH"
alias grun='java org.antlr.v4.gui.TestRig DAD data_aware_declare -gui'
javac DAD*.java
java org.antlr.v4.gui.TestRig DAD data_aware_declare -gui