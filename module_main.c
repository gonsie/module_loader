//Elsa Gonsiorowski
//October 7, 2011
//Rensselaer Polytechnic Institute

//The main function and command line arguments

#include <stdio.h>
#include <libgen.h>
#include <assert.h>

#include "ross.h"
#include "gates_model.h"
#include "library.h"
#include "routing.h"

//#define VERIFY_READ 1
#define LPS_PER_KP (18)

unsigned int module_index = 0;

// EMPTY FUNCTIONS
void blank_init(gate_state *s, tw_lp *lp) {
    return;
}

void blank_event_handler(gate_state *s, tw_bf *bf, message *in_msg, tw_lp *lp){
    return;
}

void blank_event_handler_rc(gate_state *s, tw_bf *bf, message *in_msg, tw_lp *lp){
    return;
}

void blank_finish(gate_state *s, tw_lp *lp){
    return;
}

const tw_optdef module_loader_opts[] = {
    TWOPT_GROUP("Module Loader"),
    TWOPT_UINT("index", module_index,"index of submodule to be loaded"),
    TWOPT_END(),
};

tw_peid module_loader_map(tw_lpid gid) {
    return (tw_peid) 0;
}

void module_loader_mapping_setup(void){
    int lpid, kpid;
    int i;
    tw_pe *pe = tw_pe_next(NULL);

    int extra_lp_on_kp = g_tw_nlp - (g_tw_nkp * LPS_PER_KP);

    for (lpid = 0, kpid = 0; kpid < g_tw_nkp; kpid++) {
        tw_kp_onpe(kpid, pe);

        int lps_on_kp = LPS_PER_KP;
        if (kpid < extra_lp_on_kp) {
            lps_on_kp++;
        }

        for (i = 0; i < lps_on_kp; i++, lpid++) {
            tw_lp_onpe(lpid, pe, lpid);
            tw_lp_onkp(g_tw_lp[lpid], g_tw_kp[kpid]);
        }
    }
    return;
}

tw_lp * module_loader_mapping_to_local(tw_lpid lpid){
    return g_tw_lp[lpid];
}

tw_lptype mylps[] = {
    {(init_f) blank_init,
     (pre_run_f) NULL,
     (event_f) blank_event_handler,
     (revent_f) blank_event_handler_rc,
     (final_f) blank_finish,
     (map_f) module_loader_map,
    sizeof(gate_state)},
    {0},
};

io_lptype iolps[] = {
    {(serialize_f) gate_serialize,
     (deserialize_f) gate_deserialize,
     (model_size_f) gate_size},
    {0},
};


#define module_loader_main main

int module_loader_main(int argc, char* argv[]){

    int i, j;

    tw_opt_add(module_loader_opts);
    tw_init(&argc, &argv);

    if (tw_nnodes() != 1) {
        printf("ERROR: Module Loader expects 1 MPI-Rank, found %d\n", tw_nnodes());
        return 1;
    }


    g_tw_mapping = CUSTOM;
    g_tw_custom_initial_mapping = &module_loader_mapping_setup;
    g_tw_custom_lp_global_to_local_map = &module_loader_mapping_to_local;

    printf("Rank %ld loading Module %d\n", g_tw_mynode, module_index);

    g_tw_nlp = routing_table[module_index+1] - routing_table[module_index];
    g_tw_nkp = g_tw_nlp / LPS_PER_KP;

    tw_define_lps(g_tw_nlp, sizeof(message), 0);

    g_tw_lp_types = mylps;
    g_io_lp_types = iolps;
    tw_lp_setup_types();

    g_io_events_buffered_per_rank = 0;
    io_init_local(g_tw_nkp);

    char dataname[100];
    char *datapath = dirname(argv[0]);
    sprintf(dataname, "%s/data-%ld.vbench", datapath, g_tw_mynode);

    // ALWAYS USE MPI FILE I/O
    // each rank reads its own file

    // //MPI_READ on rank 0, scatter around
    // // MAX_BLOCK_SIZE is the max size of the block of text for any single processor
    // // the text has been grouped into blocks for each processor
    // else {
    //     // size of text block to be read
    //     int MAX_BLOCK_SIZE = (LP_COUNT+1) * LINE_LENGTH;
    //     char *block = (char *) malloc(MAX_BLOCK_SIZE);

    //     // all reading happens from task 0
    //     if (g_tw_mynode == 0) {

    //         FILE *f;
    //         f = fopen(datapath, "r");

    //         // get max block size
    //         for (i = 0; i < GLOBAL_NP_COUNT; i++) {
    //             int BLOCK_SIZE = LP_COUNT * LINE_LENGTH;
    //             if (i < EXTRA_LP_COUNT) {
    //                 BLOCK_SIZE += LINE_LENGTH;
    //             }
    //             fread(block, BLOCK_SIZE, 1, f);
    //             if (g_tw_mynode == i) {
    //                 for (j = 0; j < g_tw_nlp; j++) {
    //                     strncpy(global_input[j], block + (j * LINE_LENGTH), LINE_LENGTH);
    //                 }
    //             } else {
    //                 MPI_Send(block, BLOCK_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    //             }

    //             printf("Reading lines for node %d\n", i);
    //         }

    //         fclose(f);
    //         free(block);
    //     } else {
    //         int BLOCK_SIZE = g_tw_nlp * LINE_LENGTH;
    //         MPI_Status req;
    //         MPI_Recv(block, BLOCK_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &req);
    //         for (j = 0; j < g_tw_nlp; j++) {
    //             strncpy(global_input[j], block + (j * LINE_LENGTH), LINE_LENGTH);
    //         }
    //         printf("Rank %d received its block\n", g_tw_mynode);
    //     }
    // }

#if VERIFY_READ
    if (g_tw_mynode == 0) {
        printf("Line 0: %s\n", global_input[0]);
        printf("Line 1: %s\n", global_input[1]);
        printf("Line 2: %s\n", global_input[2]);
        printf("Line 3: %s\n", global_input[3]);
        printf("Line 100: %s\n", global_input[100]);
        printf("Line Last: %s\n", global_input[LP_COUNT-1]);
    }
#endif

    tw_run();

    io_store_multiple_partitions("submodule_checkpoint");

    tw_end();

    return 0;
}

