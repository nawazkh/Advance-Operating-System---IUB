#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
 *   printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
 *     printf("inn*sizeof(struct inode): %d\n", inode_off);
 *       */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
 *   printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
 *     */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
 * block is indicated in the high-order bit.  Shift the byte by j
 * positions to make the match in bit7 (the 8th bit) and then shift
 * that value 7 times to the low-order bit to print.  Yes, it could be
 * the other way...  
 */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}


int fs_open(char *filename, int flags) {
	struct filetable *file_desc;
	struct directory *root_dir;
	struct dirent *entry;
	struct inode *i;
	int j;
	
	if(filename == NULL){
		fprintf(stderr, "file name is null\n");
		return SYSERR;
	}
	if(strlen(filename) == 0){
                fprintf(stderr, "file name is correct\n");
                return SYSERR;
        }
	root_dir = &(fsd.root_dir);
	for(j = 0; j < root_dir->numentries;i++){
		entry = &(root_dir->entry[j]);
		if(!strcmp(entry->name, filename)){
			i = (struct inode*)getmem(sizeof(struct inode));
			if(fs_get_inode_by_num(0,entry->inode_num, i) == SYSERR){
				fprintf(stderr,"could not get inode for %s\n",filename );
				return SYSERR;
			}
			file_desc = &(oft[next_open_fd]);
			next_open_fd++;
			file_desc->in = *i;
			file_desc->de = entry;
			file_desc->fileptr = 0;
			file_desc->state = flags;
			break;
		}	
	}
	if(entry == NULL){
		fprintf(stderr,"file not found\n");
		return SYSERR;
	}
	return (next_open_fd - 1);
}

int fs_close(int fd) {
	struct filetable *file_desc;
	struct inode *i;
	if(fd < 0 || fd >= next_open_fd){
                fprintf(stderr, "Bad file descriptor \n");
                return SYSERR;
        }
	file_desc = &(oft[fd]);
	i = &(file_desc->in);
	fs_put_inode_by_num(0, i->id, i);
	file_desc->fileptr = 0;
	freemem(&(file_desc->in), sizeof(struct inode));
	file_desc->state = FSTATE_CLOSED;
	return OK;
		
}

int fs_create(char *filename, int mode) {
	struct filetable *file_desc;
	struct dirent *entry;
        struct inode *i;
	struct directory *root = &(fsd.root_dir);
	int j, fd, nIn;
	fd = fs_open(filename, O_RDWR);
	if(fd != SYSERR){
		fprintf(stderr,"file will open in read/write mode \n");
		return fd;
	}
	
	nIn = fsd.inodes_used;
	fsd.inodes_used++;
	i = (struct inode*)getmem(sizeof(struct inode));
	fs_get_inode_by_num(0, nIn, i);
	i->id = nIn;
	i->type = INODE_TYPE_FILE;
	i->device = 0;
	i->size = 0;
	
	fs_put_inode_by_num(0,nIn,i);
	entry = &(root->entry[root->numentries]);
	entry->inode_num = nIn;
	strcpy(entry->name, filename);
	(root->numentries)++;
	
	if(next_open_fd >= NUM_FD){
		fprintf(stderr, " max number of files have been opened\n");
		return SYSERR;
	}
	file_desc = &(oft[next_open_fd]);
	file_desc->fileptr = 0;
	file_desc->state = mode;
	file_desc->de = &entry;
	file_desc->in = *i;
	next_open_fd++;
	
	return (next_open_fd - 1);
}

int fs_seek(int fd, int offset) {
	struct inode *i;
        struct filetable *file_desc;
	
	if(fd >= next_open_fd){
                fprintf(stderr, "Bad file descriptor \n");
                return SYSERR;
        }
	
	file_desc = &(oft[fd]);
        i = &(file_desc->in);
	
	if(offset > i->size){
		fprintf(stderr, "error on seeking beyond End of file\n");
		return SYSERR;
	}
	file_desc->fileptr += offset;
	return OK;
}

int fs_read(int fd, void *buf, int nbytes) {
	int blck, blck_offset, bts_read = 0, remaining;
	struct inode *i;
	struct filetable *file_desc;
	char* current;
	
	if(fd < 0 || fd >= next_open_fd){
		fprintf(stderr, "Bad file descriptor \n");
		return SYSERR;
	}
	
	file_desc = &(oft[fd]);
	i = &(file_desc->in);
	current = buf;
	blck = file_desc->fileptr/MDEV_BLOCK_SIZE;
	blck_offset = file_desc->fileptr%MDEV_BLOCK_SIZE;
	if(blck_offset > 0){ // extra space has to be copied 
		bts_read = (MDEV_BLOCK_SIZE - blck_offset + 1);
		bs_bread(0, i->blocks[blck], blck_offset, block_cache, remaining);
		memcpy(current, block_cache, remaining);
	}
	
	while(bts_read < nbytes){
		blck = file_desc->fileptr/MDEV_BLOCK_SIZE;
		if((nbytes - bts_read)>=MDEV_BLOCK_SIZE){// read remaining
			remaining = MDEV_BLOCK_SIZE;
		}else{
			remaining = (nbytes - bts_read);
		}
		bs_bread(0, i->blocks[blck],0,block_cache, remaining);
		memcpy(current, block_cache, remaining);
		current += remaining;

		bts_read += remaining;
		file_desc->fileptr += remaining;
		fprintf(stderr, "output read :  %d, with file pointer as%d \n",bts_read,file_desc->fileptr);
	}
	
	return bts_read;
}

int fs_write(int fd, void *buf, int nbytes) {
	struct filetable *file_desc;
	struct inode *i;
	char *current;
	int j,ndbs, data_block, blck, blck_offset, nWritten = 0, remaining = 0;
	
	if(fd < 0 || fd >= next_open_fd){
                fprintf(stderr, "Bad file descriptor \n");
                return SYSERR;
        }
	
	file_desc = &oft[fd];
	if(file_desc->state == O_RDONLY){
		fprintf(stderr,"cannot open file in read only mode\n");
		return SYSERR;
	}
	
	current = buf;
	i = &(file_desc->in);
	blck = file_desc->fileptr/MDEV_BLOCK_SIZE;
	blck_offset = file_desc->fileptr%MDEV_BLOCK_SIZE;
	if(blck_offset > 0){
		memcpy(block_cache, current,(MDEV_BLOCK_SIZE-blck_offset+1));
		bs_bwrite(0, i->blocks[blck], 0, (void*)block_cache, (MDEV_BLOCK_SIZE-blck_offset+1));
		nWritten += (MDEV_BLOCK_SIZE-blck_offset+1);
		i->size += nWritten;
		file_desc->fileptr += nWritten;
		current += nWritten;
	}
	while(nWritten < nbytes){
		blck = file_desc->fileptr/MDEV_BLOCK_SIZE;
		data_block = -1;
		for(j = 15; j <= 512;j++){
			if(!fs_getmaskbit(j)){
				data_block = j;
				break;
			}
		}
		//data_block = get_next_free_db();
		fs_setmaskbit(data_block);
		i->blocks[blck] = data_block;
		if((nbytes - nWritten) > MDEV_BLOCK_SIZE){
			remaining = MDEV_BLOCK_SIZE;
		}else{
			remaining =(nbytes - nWritten);
		}
	
		memcpy(block_cache, current, remaining);
		bs_bwrite(0,i->blocks[blck], 0, (void*)block_cache, remaining);
		file_desc->fileptr += remaining;
		nWritten += remaining;
		i->size += remaining;
		current += remaining;
	}
	return nWritten;
}

#endif /* FS */
