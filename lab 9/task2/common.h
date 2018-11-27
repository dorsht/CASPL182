#define DIR_MAX_SIZE 2048
#define LS_RESP_SIZE 2048
#define FILE_BUFF_SIZE 1024

typedef enum {
	IDLE,
	CONNECTING,
	CONNECTED,
	DOWNLOADING,
	SIZE
} c_state;
	
typedef struct {
	char* server_addr;	// Address of the server as given in the [connect] command. "nil" if not connected to any server
	c_state conn_state;	// Current state of the client. Initially set to IDLE
	char* client_id;	// Client identification given by the server. NULL if not connected to a server.
	int sock_fd;
} client_state;

long file_size(char * filename);
char* list_dir();
client_state* create_client();
void set_initial_state (client_state * cs);
void free_client_state (client_state * cs);
void debug_print (int debug_flag, char * msg, char * info);