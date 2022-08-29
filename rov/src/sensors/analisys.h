#ifndef ANALISYS_H
#define ANALISYS_H

struct TAnalisysConfig {
    long int nextevent;
};
typedef struct TAnalisysConfig AnalisysConfig;

void analisys_setup(long int now);
void analisys_loop(long int now);

#endif
