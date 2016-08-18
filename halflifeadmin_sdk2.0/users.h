/***
*
*	Copyright (c) 2000, Alfred Reynolds
****/

/*

===== users.h ========================================================
 
defines for the various access levels in the admin_ commands

*/

/*

1 - vote map, vote kick,
2, - map, timelimit, fraglimit. (if they can change the map.. it
doesn't make sense to lock these out)
4 - prematch, reload
8 - pause, unpause
16 - pass, no_pass
32 - teamplay, Friendly-Fire
64 - admin_change other server variables.. (gravity, fall damage,
etc.. )
128 - admin_say, admin_kick
256 - admin_ban, admin_unban
leaves some space for other stuff.

32768 - reserved slot
*/


#define ALL 0
#define LEV1 1
#define LEV2 1<<1
#define LEV4  1<<2
#define LEV8 1<<3
#define LEV16 1<<4
#define LEV32 1<<5 //32
#define LEV64 1<<6 // 64
#define LEV128 1<<7
#define LEV256 1<<8
#define LEV512 1<<9
#define LEV1024 1<<10
#define LEV2048  1<< 11
#define LEV4096  1<<12
#define LEV8192 1<<13
#define LEV16384 1<<14
#define LEV32768 1<<15


#define USERS_SIZE 128 //max number of users
#define NICKS_SIZE 1024 // max number of nick names
#define MAX_IPS 1024 // max number of reserved ips
#define MODELS_SIZE 128 // max number of models we password

// char field sizes
#define USERNAME_SIZE 64 // max number of username chars
#define PASSWORD_SIZE 64 // max size of password (big cause of win32)
#define IP_SIZE 16 // max lenght of an ip line


typedef struct {
	char username[USERNAME_SIZE],pass[PASSWORD_SIZE];
	int access;
} user_struct;


typedef struct {
	char username[USERNAME_SIZE],pass[PASSWORD_SIZE];
	int access;
	int vote;
} player_struct;

#define ulong unsigned long

typedef struct {
	char  str[IP_SIZE+1];
	char  nstr[IP_SIZE+1];
	ulong addr;
	ulong mask;
} ip_struct;


#define MAP_VOTE 0
#define KICK_VOTE 1 

#define VOTE_LENGTH 2 //number of vote data types to store
typedef struct {
	int enabled; // whether the vote is enabled
	int time; // the time it started
	int num_yes,num_no; // number voted yes, number no
	int total_num; // total number of possible votes
	char name[USERNAME_SIZE];  // pointer to string containing kick data
} vote_struct;

