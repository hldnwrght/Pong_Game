//This holds the header file for the client portion

#ifndef client_head_H_
#define client_head_H_

void checkinput(int signum);
void init_paddle(int lpaddle, int rpaddle);
void del_paddle(int lpaddle, int rpaddle);
void quit(int signum);
void term_Set(int input);

#endif
