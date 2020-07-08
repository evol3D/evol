#ifndef EVOL_APP_H
#define EVOL_APP_H

extern struct _ev_app_struct{
    const char *name;
    int (*start)();
    int (*destroy)();
} App;

#endif //EVOL_APP_H
