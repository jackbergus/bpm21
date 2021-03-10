grammar DAD;

data_aware_declare: (declare)*;

declare: name=LABEL '(' (fields)+ ')'            #nary_prop
       | name=LABEL '(' fields ',' INTNUMBER ')' #unary_prop
       ;

fields: label=LABEL ',' prop;

prop  : prop_within_dijunction '||' prop  #disj
      | prop_within_dijunction            #conj_or_atom
      | 'true'                            #top
      ;

prop_within_dijunction : atom                              #in_atom
                       | atom '&&' prop_within_dijunction  #atom_conj
                       ;

atom : (isnegated='~')? VAR rel (NUMBER | STRING) ;

rel   : '<' #lt
      | '<=' #leq
      | '>' #gt
      | '>=' #geq
      | '=' #eq
      | '!=' #neq
      ;

VAR: ('a'..'z')+;
LABEL: ('A'..'Z')[a-zA-Z]+;
INTNUMBER : ('0'..'9')+ ;
NUMBER : INTNUMBER ('.' INTNUMBER)?;
STRING : '"' (~[\\"] | '\\' [\\"])* '"';
SPACE : [ \t\n]+ -> skip;