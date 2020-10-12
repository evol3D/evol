#ifndef EVOL_RENDERER_H
#define EVOL_RENDERER_H

extern struct ev_Renderer {
    int (*init)();
    int (*deinit)();
} Renderer;
;
#endif //EVOL_RENDERER_H 
