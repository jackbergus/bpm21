grammar Traces;

log : (trace ';')* trace;

trace: event+;
event : LABEL data_part?;
data_part: '{' field* '}';
field : VAR '=' (NUMBER | STRING) ;

VAR: ('a'..'z')+;
LABEL: ('A'..'Z')[a-zA-Z]*;
INTNUMBER : ('0'..'9')+ ;
NUMBER : INTNUMBER ('.' INTNUMBER)?;
STRING : '"' (~[\\"] | '\\' [\\"])* '"';
SPACE : [ \t\n]+ -> skip;

COMMENT
    : '/*' .*? '*/' -> skip
;

LINE_COMMENT
    : '//' ~[\r\n]* -> skip
;