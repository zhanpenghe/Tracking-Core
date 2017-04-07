/*
 * Function definitions for server.c
 * @Author Adam Ho
 */

int start_server_thread();

void start_output_thread();

void start_calculation_thread();

void agent_thread_init(int connfd);

static void usage();

void load_commandline_arg(int argc, char** argv);

void SIGINT_Handler(int sig);

char *get_beacon_mac_addr(char *id);


//getters for global var in server.c

void *get_pos_list();

void get_parameters();

float get_PN();

float get_PA();

int get_PMA();

void *get_agent_infos();

int get_agent_num();

void *get_agent_list();

void *get_pos_logger();