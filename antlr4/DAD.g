grammar DAD;

data_aware_declare: (declare)*;

declare: name=LABEL '(' (fields)+ ')'            #nary_prop
       | name=LABEL '(' fields ',' INTNUMBER ')' #unary_prop
       ;

fields: label=LABEL ',' prop;

prop  : VAR rel (NUMBER | STRING)    #atom
      | 'true'                       #top
      | 'false'                      #bot
      |<assoc=right> prop '&&' prop  #and
      |<assoc=right> prop '||' prop  #or
      | '~' prop                     #not
      ;

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