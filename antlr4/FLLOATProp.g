grammar FLLOATProp;

statement : '~' statement                         #negation
          | ID                                    #atom
          | '(' statement ')'                     #paren
          | <assoc=right> statement '&' statement #and
          | <assoc=right> statement '|' statement #or
          | 'True'                                #top
          | 'False'                               #bot
          ;

/** "a numeral [-]?(.[0-9]+ | [0-9]+(.[0-9]*)? )" */ NUMBER
   : '-'? ( '.' DIGIT+ | DIGIT+ ( '.' DIGIT* )? )
   ;


fragment DIGIT
   : [0-9]
   ;

/** "Any string of alphabetic ([a-zA-Z\200-\377]) characters, underscores
 *  ('_') or digits ([0-9]), not beginning with a digit"
 */ ID
   : LETTER ( LETTER | DIGIT )*
   ;


fragment LETTER
   : [a-zA-Z\u0080-\u00FF_]
   ;

WS
   : [ \t\n\r]+ -> skip
   ;
