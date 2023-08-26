/** parser.h

Module definition for the parser.

*/

#ifndef PARSER_H
#define PARSER_H

#include "control.h"
#include "nfa.h"

NFA *parse(CmpCtrl *cc);
NFA *regex(CmpCtrl *cc);
NFA *group(CmpCtrl *cc);
NFA *gprime(CmpCtrl *cc, NFA *local);
NFA *quantifier(CmpCtrl *cc, NFA *nfa);
NFA *pattern(CmpCtrl *cc);

#endif