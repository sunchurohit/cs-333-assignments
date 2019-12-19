#include "emufs.h"

/**************************** File system helper functions ****************************/

//	Function to encrypt a block of data 
int encrypt(char* input, char* encrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		encrypted[i] = ~input[i];
	}
}

//	Function to decrypt a block of data 
int decrypt(char* input, char* decrypted)
{
	for(int i=0; i<BLOCKSIZE; i++)
	{
		decrypted[i] = ~input[i];
	}
}

//	The following helper functions are used to read and write 
//	superblock and metadata block. 
//	Change the function definitions with required arguments
struct superblock_t* readSuperblock(struct mount_t* mount_point)
{
	/*
		* Read 0th block from the device into a blocksize buffer
		* Create superblock_t variable and fill it using reader buffer
		* Return the superblock_t variable
	*/
	struct superblock_t* sb = (struct superblock_t*) malloc(BLOCKSIZE);
	char* buf = (char*) malloc(BLOCKSIZE) ;
	if(mount_point->device_fd <= 0)
	{
		printf("error , device_fd <= 0\n");
	}
	int ret = readblock(mount_point->device_fd,0,buf);

	if(ret == -1)
	{
		printf("error in readblock\n");
	}

	sb = (struct superblock_t*) buf;
	return sb;
}

int writeSuperblock(struct mount_t* mount_point,struct superblock* sb)
{
	/*
		* Read the 0th block from device into a buffer
		* Write the superblock into the buffer
		* Write back the buffer into block 0
	*/
	char* buf = (char*) malloc(BLOCKSIZE);
	buf = (char *) sb;

	if(mount_point->device_fd <= 0)
	{
		printf("error , device_fd <= 0\n");
	}
	int ret = writeblock(mount_point->device_fd,0,buf);

	return ret;
}

struct metadata_t* readMetadata(struct mount_t* mount_point)
{
	// Same as readSuperBlock(), but it is stored on block 1
	// Need to decrypt if emufs-encrypted is used 
	struct metadata_t* md = (struct metadata_t*) malloc(BLOCKSIZE);
	char* buf = (char*) malloc(BLOCKSIZE) ;

	if(mount_point->device_fd <= 0)
	{
		printf("error , device_fd <= 0\n");
	}

	int ret = readblock(mount_point->device_fd,1,buf);
	if(ret == -1)
	{
		printf("error in readblock\n");
	}
	if(mount_point->fs_number == 1)
	{
		char* dec = (char*) malloc(BLOCKSIZE);
		decrypt(buf,dec);
		md = (struct metadata_t*) dec ;
	}
	else
	{
		md = (struct metadata_t*) buf ;
	}

	return md;
}

int writeMetadata(struct mount_t* mount_point,struct metadata_t* md)
{
	// Same as writeSuperblock(), but it is stored on block 1
	// Need to decrypt/encrypt if emufs-encrypted is used  
	char* buf = (char*) malloc(BLOCKSIZE);
	buf = (char *) md;
	if(mount_point->device_fd <= 0)
	{
		printf("error , device_fd <= 0\n");
	}
	if(mount_point->fs_number == 1)
	{
		char* enc = (char*) malloc(BLOCKSIZE);
		encrypt(buf,enc);
		return writeblock(mount_point->device_fd,1,enc);
	}
	else
	{
		return writeblock(mount_point->device_fd,1,buf);
	}
}

/**************************** File system API ****************************/

int create_file_system(struct mount_t *mount_point, int fs_number)
{
	/*
	   	* Read the superblock.
	    * Set file system number on superblock
		* Clear the bitmaps.  values on the bitmap will be either '0', or '1', or'x'. 
		* Create metadata block in disk
		* Write superblock and metadata block back to disk.

		* Return value: -1,		error
						 1, 	success
	*/
	struct superblock_t *sb = readSuperblock(mount_point);
	sb->fs_number = fs_number;
	mount_point->fs_number = fs_number;
	int i;
	sb->bitmap[0] = '1'; sb->bitmap[1] = '1';
	for(i=2;i < sb->disk_size;i++)
	{
		sb->bitmap[i] = '0';
	}
	for(int j = i; j < MAX_BLOCKS; j++)
	{
		sb->bitmap[j] = 'x';
	}
	struct metadata_t *md = (struct metadata_t*) malloc(BLOCKSIZE);
	for(i=0;i <MAX_FILES;i++)
	{
		md->inodes[i].status = 0;
	}

	if(writeSuperblock(mount_point,sb) == -1 || writeMetadata(mount_point,md) == -1)
	{
		return -1;
	}

	printf("[%s] File system created.\n", mount_point->device_name);

	return 1;
}


struct file_t* eopen(struct mount_t* mount_point, char* filename)
{
	/* 
		* If file exist, get the inode number. inode number is the index of inode in the metadata.
		* If file does not exist, 
			* find free inode.
			* allocate the free inode as USED
			* if free id not found, print the error and return -1
		* Create the file hander (struct file_t)
		* Initialize offset in the file hander
		* Return file handler.

		* Return NULL on error.
	*/
	struct metadata_t *md = readMetadata(mount_point); int index = -1;
	//printf("working fine 1!\n");
	for(int i=0;i < MAX_FILES;i++)
	{
		if(md->inodes[i].status == 1)
		{
			if(strcmp(md->inodes[i].name,filename) == 0 && md->inodes[i].status == 1)
			{	
				index = i; break;
			}
		}
	}
	// printf("working fine 2!\n");
	// printf("%d\n", index);
	if(index == -1)
	{
		for(int i=0;i<MAX_FILES;i++)
		{
			if(md->inodes[i].status == 0)
			{
				//printf("eyy\n");
				index = i;
				md->inodes[i].status = 1;
				md->inodes[i].file_size = 0;
				for(int j=0;j<4;j++)
				{
					md->inodes[i].blocks[j] = -1;
				}
				//printf("hello baby!!\n");
				time(&md->inodes[i].modtime);
				strcpy(md->inodes[i].name,filename);
				break;
			}
		}
	}
	//printf("%d\n", index);
	if(index == -1)
	{
		printf("error, not inode id found\n") ; return NULL;
	}

	struct file_t *ft = (struct file_t*) malloc(sizeof(struct file_t));
	ft->offset = 0;
	ft->inode_number = index;
	ft->mount_point = mount_point;

	if(writeMetadata(mount_point,md) == -1)
	{
		printf("error in writing md\n"); return NULL;
	}

	printf("File '%s' Opened \n",filename);

	return ft;
}

int ewrite(struct file_t* file, char* data, int size)
{
	// You do not need to implement partial writes in case file exceeds 4 blocks
	// or no free block is available in the disk. 

	// Return value: 	-1,  error
	//					Number of bytes written
	struct mount_t *mount_point = file->mount_point ;
	struct superblock_t *sb = readSuperblock(mount_point);
	struct metadata_t *md = readMetadata(mount_point);
	struct inode_t *id = &md->inodes[file->inode_number];
	id->status = 1;

	

	if(file->offset + size > 4*BLOCKSIZE)
	{
		printf("Writing too much data from current postion\n");
		return -1;
	}

	int no_of_blocks_to_write = size / BLOCKSIZE ;
	int block_number_to_start = file->offset / BLOCKSIZE ;
	int can_write = 0; int new_blocks_required = 0;

	char* enc = (char*) malloc(BLOCKSIZE); // use only if fs_number is 1.

	for(int i=block_number_to_start; i < block_number_to_start + no_of_blocks_to_write ;i++)
	{
		if(id->blocks[i] == -1)
		{
			new_blocks_required++;
		}
	}

	int no_of_blocks_available = 0;

	for(int i=2; i < sb->disk_size; i++)
	{
		if(sb->bitmap[i] == '0')
		{
			no_of_blocks_available++;
		}
	}

	if(new_blocks_required > no_of_blocks_available)
	{
		printf("No blocks to write data!!\n");
		return -1;
	}

	//printf("hellooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");

	for(int i=block_number_to_start;i < block_number_to_start + no_of_blocks_to_write ;i++)
	{
		//printf("%d\n",id->blocks[i]);
		int t = id->blocks[i];	
		//printf("%d\n\n\n\n\n\n\n\n\n", t);
		if(t == -1)
		{
			for(int j=2; j<sb->disk_size; j++)
			{
				if(sb->bitmap[j] == '0')
				{
					sb->bitmap[j] ='1';
					//printf("%d\n\n\n\n\n\n\n\n\n\n\n\n\n",j);
					id->blocks[i] = j;
					break;
				}
			}
		}

		if(mount_point->fs_number == 1)
		{
			
			encrypt(data + 512*(i-block_number_to_start),enc); // use only if fs_number is 1.

			if(writeblock(mount_point->device_fd,id->blocks[i],enc) == -1)
			{
				printf("error in writing \n"); return -1;
			}
		}
		else
		{
			if(writeblock(mount_point->device_fd,id->blocks[i],data+ 512*(i-block_number_to_start)) == -1)
			{
				printf("error in writing \n"); return -1;
			}
		}

		
	}

	id->file_size = file->offset + size ;
	time(&id->modtime);
	eseek(file,file->offset + size) ;

	writeMetadata(mount_point,md);
	writeSuperblock(mount_point,sb);

	return size;
}

int eread(struct file_t* file, char* data, int size)
{
	// NO partial READS.

	// Return value: 	-1,  error
	//					Number of bytes read

	struct mount_t *mount_point = file->mount_point ;
	struct superblock_t *sb = readSuperblock(mount_point);
	struct metadata_t *md = readMetadata(mount_point);
	struct inode_t *id = &md->inodes[file->inode_number];

	int block_to_start = file->offset / BLOCKSIZE ;
	int no_of_blocks_to_read = size / BLOCKSIZE ;

	char* dec = (char*) malloc(size);
	// decrypt(data,dec); // use only if fs_number is 1.

	if(block_to_start + no_of_blocks_to_read > 4)
	{
		printf("Reading more blocks!!\n"); return -1;
	}

	for(int i=block_to_start;i < block_to_start + no_of_blocks_to_read; i++)
	{
		if(id->blocks[i] == -1)
		{
			printf("Cannot read some blocks (Invalid offset) !!\n"); return -1;
		}
	}

	for(int i=block_to_start;i < block_to_start + no_of_blocks_to_read; i++)
	{
		if(readblock(mount_point->device_fd,id->blocks[i],dec + 512*(i-block_to_start)) == -1)
		{
			printf("Some error in reading!!\n"); return -1;
		}
	}

	if(mount_point->fs_number == 1)
	{
		for(int i = 0 ; i < size/BLOCKSIZE; i++)
		{
			decrypt(dec + 512*i,data + 512*i) ;
		}
	}
	else
	{
		strcpy(data,dec);	
	}

	eseek(file,file->offset + size);
	writeMetadata(mount_point,md);
	writeSuperblock(mount_point,sb);
	return size;
}

void eclose(struct file_t* file)
{
	// free the memory allocated for the file handler 'file'
	free(file);
	printf("File Closed\n");
}

int eseek(struct file_t *file, int offset)
{
	// Change the offset in file hanlder 'file'
	file->offset = offset;
	return 1;
}

void fsdump(struct mount_t* mount_point)
{

	printf("\n[%s] fsdump \n", mount_point->device_name);
	printf("%-10s %6s \t[%s] \t%s\n", "  NAME", "SIZE", "BLOCKS", "LAST MODIFIED");

	struct metadata_t *md = readMetadata(mount_point);

	for(int i = 0 ; i < MAX_FILES; i++)
	{
		if(md->inodes[i].status == 1)
		{
			printf("%-10s %6d \t[%d %d %d %d] \t%s", 
				md->inodes[i].name, 
				md->inodes[i].file_size,
				md->inodes[i].blocks[0],
				md->inodes[i].blocks[1],
				md->inodes[i].blocks[2],
				md->inodes[i].blocks[3],
				asctime(localtime(&(md->inodes[i].modtime))));
		}
	}
}
