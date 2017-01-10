/*
 * Function definitions for server.c
 * @Author Adam Ho
 */

int start_server_thread();

void agent_thread_init(int connfd);

static void usage();

void load_commandline_arg(int argc, char** argv);

void SIGINT_Handler(int sig);
