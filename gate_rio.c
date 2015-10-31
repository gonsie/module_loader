//Elsa Gonsiorowski
//October 31, 2015
//Rensselaer Polytechnic Institute

#include "gates_model.h"

size_t gate_size (gate_state *s, tw_lp *lp) {
    memcpy(buffer, s, sizeof(gate_state));
    return;
}

void gate_deserialize (gate_state *s, void * buffer, tw_lp *lp) {
    memcpy(s, buffer, sizeof(gate_state));
    return;
}

void gate_serialize (gate_state *s, void *buffer, tw_lp *lp) {
    return sizeof(gate_state);
}

