#include <openpgpsdk/create.h>
#include <openpgpsdk/crypto.h>
#include <openpgpsdk/keyring.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc,char **argv)
    {
    const char *keyfile;
    const char *user_id;
    ops_keyring_t keyring;
    const ops_key_data_t *key;
    ops_create_info_t *info;

    if(argc != 3)
	{
	fprintf(stderr, "%s <keyfile> <user_id>\n", argv[0]);
	exit(1);
	}

    keyfile = argv[1];
    user_id = argv[2];

    ops_init();
    ops_keyring_read(&keyring, keyfile);

    key = ops_keyring_find_key_by_userid(&keyring, user_id);
    assert(key);

    info = ops_create_info_new();
    ops_writer_set_fd(info, 1);  // stdout for now
    ops_writer_push_encrypt(info, key);

    for( ; ; )
	{
	unsigned char buf[8192];
	int n;
	
	n=read(0,buf,sizeof buf);
	if(!n)
	    break;
	if(n < 0)
	    {
	    perror("stdin");
	    exit(4);
	    }
	ops_write(buf,n,info);
	}

    return 0;
    }
