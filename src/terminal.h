#ifndef __TERMINAL_H_
#define __TERMINAL_H_

struct termios orig_term_attr;
struct termios new_term_attr;

void SetTerminal( struct winsize * w );

#endif
