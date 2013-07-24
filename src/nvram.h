#ifndef _nvram_h_
#define _nvram_h_
#include <linux/autoconf.h>

#define NVRAM_MTDNAME "nvram"
#define NVRAM_DEVNAME "nvram"

#define RANV_PRINT(x, ...) do { if (nvram_debug) printk("\n%s %d: " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)
#define RANV_ERROR(x, ...) do { printk("%s %d: ERROR! " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)

//x is the value returned if the check failed
#define RANV_CHECK_INDEX(x) do { \
        if (index < 0 || index >= FLASH_BLOCK_NUM) { \
                RANV_PRINT("index(%d) is out of range\n", index); \
		return x; \
        } \
} while (0)

#define RANV_CHECK_VALID() do { \
        if (!fb[index].valid) { \
                RANV_PRINT("fb[%d] invalid, init again\n", index); \
		init_nvram_block(); \
        } \
} while (0)

typedef struct environment_s {
	unsigned long crc;		//CRC32 over data bytes
	char *data;
} env_t;

typedef struct cache_environment_s {
	char *name;
	char *value;
} cache_t;

#define MAX_CACHE_ENTRY 500
typedef struct block_s {
	char *name;
	env_t env;			//env block
	cache_t	cache[MAX_CACHE_ENTRY];	//env cache entry by entry
	unsigned long flash_offset;
	unsigned long flash_max_len;	//ENV_BLK_SIZE
	char valid;
	char dirty;
} block_t;

#define MAX_NAME_LEN 128
#define MAX_VALUE_LEN (ENV_BLK_SIZE * 5)
typedef struct nvram_ioctl_s {
	int index;
	int ret;
	char *name;
	char *value;
} nvram_ioctl_t;


#define NVRAM_IOCTL_GET		0x01
#define NVRAM_IOCTL_GETALL	0x02
#define NVRAM_IOCTL_SET		0x03
#define NVRAM_IOCTL_COMMIT	0x04
#define NVRAM_IOCTL_CLEAR	0x05

/*************************************************************/
#if 0
#define EZP_PROD_VERSION "0.0.0_ALPHA"
#define EZP_PROD_CAT "0"
#define EZP_PROD_SUBCAT "0"
#define EZP_PROD_SUBSUBCAT "0"
#define EZP_PROD_FW_VERSION "V1.00(BWQ.1)C0"

/* rule buf len for NVRAM */
#define NVRAM_TMP_LEN 256

/* Staging file for NVRAM */
#define NVRAM_MTD_NAME		"nvram"
#define NVRAM_STAGING		"/tmp/.nvram"
#define NVRAM_RO			1
#define NVRAM_RW			0

/* Helper macros */
#define NVRAM_ARRAYSIZE(a)	sizeof(a)/sizeof(a[0])
#define	NVRAM_ROUNDUP(x, y)	((((x)+((y)-1))/(y))*(y))

/* NVRAM constants */
#define NVRAM_OFFSET		0
#define NVRAM_SPACE			0x8000
#define NVRAM_MAGIC			0x48534C46	/* 'FLSH' */
#define NVRAM_VERSION		1

/* magic, len, crc8 to be skipped */
#define NVRAM_CRC_START_POSITION	9 

/* None of specific NVRAM options. */
#define NVRAM_NONE 0x00
/* NOT allowed to be disclosed (e.g. a config file). */
#define NVRAM_PRIVATE 0x01
/* NOT allowed to be overwritten (e.g. a license key). If the value is
 * empty, it could be written for once.*/
#define NVRAM_PROTECTED 0x02
/* For temporary use. */
#define NVRAM_TEMP 0x04
/* Customized by the authorized program. */
#define NVRAM_CUSTOMIZED 0x08
/* NVRAM could be empty. */
#define NVRAM_EMPTY 0x10
/* NVRAM value should be set by *_default if this value is empty . */
#define NVRAM_DEFAULT 0x20
/* NVRAM is undefined. */
#define NVRAM_UNDEFINED 0x80000000

struct nvram_header {
	unsigned int magic;
	unsigned int  len;
	unsigned int crc_ver_init;	/* 0:7 crc, 8:15 ver, 16:31 sdram_init */
	unsigned int config_refresh;	/* 0:15 sdram_config, 16:31 sdram_refresh */
	unsigned int config_ncdl;	/* ncdl values for memc */
} __attribute__((__packed__));

struct nvram_tuple {
	char *name;
	char *value;
	unsigned int option;
	struct nvram_tuple *next;
};

struct nvram_handle {
	int fd;
	char *mmap;
	int access;
	unsigned int length;
	unsigned int offset;
	struct nvram_tuple *nvram_hash[257];
	struct nvram_tuple *nvram_dead;
};

typedef struct nvram_handle nvram_handle_t;
typedef struct nvram_header nvram_header_t;
typedef struct nvram_tuple  nvram_tuple_t;


/* **************** Helper functions *****************/
/* String hash */
unsigned int hash(const char *s);
/* Free all tuples. */
void _nvram_free(nvram_handle_t *h);
/* (Re)allocate NVRAM tuples. */
nvram_tuple_t * _nvram_realloc( nvram_handle_t *h, nvram_tuple_t *t,
	const char *name, const char *value );
/* (Re)initialize the hash table. */
int _nvram_rehash(nvram_handle_t *h);
/* Computes a crc8 over the input data. */
unsigned char hndcrc8 (unsigned char * pdata, unsigned int nbytes, unsigned char crc);

/** \defgroup inner_func Inner Functions 
 *@{*/ 
/* Get nvram header. */
nvram_header_t * _nvram_header(nvram_handle_t *h);

/* Copy NVRAM contents to staging file. */
int nvram_to_staging(void);
/* Copy staging file to NVRAM device. */
int staging_to_nvram(void);

/* Determine NVRAM device node. */
char * nvram_find_mtd(void);
/* Check NVRAM staging file. */
char * nvram_find_staging(void);
/* Open NVRAM and obtain a handle. */
nvram_handle_t * _nvram_open(const char *file, int access);
/* Invoke a NVRAM handle for get, getall. */
nvram_handle_t * _nvram_open_rdonly(void);
/* Invoke a NVRAM handle for set, unset & commit. */
nvram_handle_t * _nvram_open_staging(void);
/* Close NVRAM handle and free memory. */
int _nvram_close(nvram_handle_t *h);

/* Get the value of an NVRAM variable. */
char * _nvram_get(nvram_handle_t *h, const char *name);
/* Get all NVRAM variables. */
nvram_tuple_t * _nvram_getall(nvram_handle_t *h);

/* Set the value of an NVRAM variable. */
int _nvram_set(nvram_handle_t *h, const char *name, const char *value);
/* Unset the value of an NVRAM variable. */
int _nvram_unset(nvram_handle_t *h, const char *name);

/* Regenerate NVRAM. */
int _nvram_commit(nvram_handle_t *h);
/**@}*/

/** \defgroup pub_func Public Functions 
 * @{ */
/* init NVRAM flash block */
void *nvram_init(void);
const nvram_handle_t * get_nvram_handle(void);
nvram_header_t * nvram_header(void);

/* wrapper of above common functions */
char * nvram_get(const char *name);
#define nvram_safe_get(name) (nvram_get(name) ? : "")
int nvram_get_option(const char *name);
int nvram_set(const char *name, const char *value);
int nvram_fset(const char *name, const char *value);
int nvram_unset(const char *name);
int nvram_reset(const char *name);
nvram_tuple_t * nvram_getall(void);
int nvram_commit(void);

int nvram_match(char *name, char *match);
int nvram_invmatch(char *name, char *invmatch);

/* restore from the whole factory default */
int nvram_default(void);
/* restore one specific rule from factory default */
int nvram_default_rule(const char *name);
/* commit  after restore the whole factory default */
int nvram_factory(void);
void nvram_boot(void);

int nvram_export(const char *filename);
int nvram_import(const char *filename);

//TODO
int nvram_upgrade(const char *source);
int nvram_downgrade(const char *target);

//helper function
int nvram_dump(void);
/**@}*/

#endif
#endif /* _nvram_h_ */
