//Elsa Gonsiorowski
//October 31, 2015
//Rensselaer Polytechnic Institute

#include "gates_model.h"

typedef struct {
	unsigned int type;
	int output_size;
} gate_serial;

size_t gate_size (gate_state *s, tw_lp *lp) {
    return sizeof(gate_serial);
}

void gate_serialize (gate_state *s, void * buffer, tw_lp *lp) {
	gate_serial tmp;
	tmp.type = s->gate_type;
	tmp.output_size = s->output_size;
    memcpy(buffer, &tmp, sizeof(gate_serial));
    return;
}

void gate_deserialize (gate_state *s, void *buffer, tw_lp *lp) {
	gate_serial tmp;
    memcpy(&tmp, buffer, sizeof(gate_serial));
    s->gate_type = tmp.type;
    s->output_size = tmp.output_size;
    return;
}

