//Elsa Gonsiorowski
//October 7, 2011
//Rensselaer Polytechnic Institute

//General structs, defs, and vars for the model

#ifndef _gates_model_h
#define _gates_model_h

#include "ross.h"

//define logic type and value
typedef int BOOL;
typedef int LOGIC;
#define TRUE (1)
#define FALSE (0)
#define X (-1)
#define Z (2)

#define LOGIC_NOT(x) ((x) + 1) % 2

//message types
#define INIT_MSG (0)
#define SOURCE_MSG (1)
#define SETUP_MSG (2)
#define LOGIC_MSG (3)
#define SINK_MSG (4)
#define WAVE_MSG (5)

//sim settings
#define MESSAGE_PAD (0.4)

//Model structs
typedef struct {
    int type;
    int id;
    int value;
    // used for reverse computation of ff/latch
    int internal_pin0;
    int internal_pin1;
    long rng_count;
} message;

typedef struct {

    //gate specific information
    unsigned int gate_type;

    //variables for linking gates
    int* inputs;
    int* internals;
    int output_size;
    int* output_gid;
    int* output_pin;
    int* output_val;

    //lp stats
    int received_events;
    int roll_backs;

    //wave stuffs
    BOOL wave_print;
    char wave_id;

} gate_state;

// Command Line Options
extern unsigned int module_index;
extern unsigned int lp_offset;
extern unsigned int md_offset;
extern unsigned int cp_file_num;
extern unsigned int file_offset;
extern unsigned int part_offset;
extern unsigned int use_unique_name_flag;

// Global vars accessed by main and driver
extern tw_lptype gates_lps[];

// RIO functions
void gate_serialize (gate_state *s, void *buffer, tw_lp *lp);
void gate_deserialize (gate_state *s, void * buffer, tw_lp *lp);
size_t gate_size (gate_state *s, tw_lp *lp);

// Mapping functions
void module_loader_mapping_setup(void);
tw_lp * module_loader_mapping_to_local(tw_lpid lpid);
tw_peid module_loader_mapping_to_pe(tw_lpid gid);

// content generated by gate-parser
typedef void (*gate_func)(int input[], int internal[], int output[]);
extern gate_func function_array[];

typedef float (*delay_func)(int in_pin, int out_pin, BOOL rising);
extern delay_func delay_array[];

typedef void (*reverse_func)(int input[], int internal[], int output[]);
extern reverse_func reverse_array[];

extern int gate_input_size[];
extern int gate_internal_size[];
extern int gate_output_size[];

extern int routing_table[];

#endif
