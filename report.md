# Report for Assignment 6
Collaborated with Ranjana Sinha and Satendra Verma to discuss and understand the assignment objectives. Independently completed the code.

## Implementation:
	1. fs_open. :
		Input parameters:
			filename: The name of the file to be opened
			flags: The mode the file has to be opened in Read/ Read-Write/Write only
		Output parameter:
			fd: an index to the opened file table.
		Have Implemented sanity checks to check for presence of the file and filename.
		Once the sanity checks are performed, this function will create an entry in the Open File Table array using the next_open_fd variable,.
		Sets inode entry to the inode of the file taken. Sets the fileptr to beginning of the file. Returns the index of the oft table.
		Throws error if the next_open_fd crosses the max opened file limit.

	2. fs_close:
		Input parameters:
			fd: index to the opened file table.
		Output parameter:
			status: SYSERR or OK
		Closes the file. Frees up the oft table for future use. The inode entry in the file table entry is saved into the corresponding inode block before freeing the resource. The state of the file table entry is set to unused, for the next file open system calls.

	3. fs_create:
		Input parameters:
			filename: string of the file to be opened
			mode: mode in which the file has to be opened
		Output parameter:
			fd: index of open file table where the file contents are cached
		For a new file name, a new inode is created and input into the root directory. 
		If the file exists, then the fd of that existing file is returned.

	4. fs_seek:
		Input parameters:
			fd: index to the opened file table.
			offset: offset to which file pointer is to be set
		Output parameter:
			status: SYSERR or OK
		Performed sanity check to avoid looking beyond the file.
		This function increments the pointer to the required pointer in the data block and returns OK on success.

	5. fs_read:
		Input parameter:
			fd: The oft table index
			buf:  buffer that will store the read data.
			nbytes: number of bytes to be read
		Output parameter:
			Number of bytes read
		Performed sanity checks to check for validity of fd provided. On valid fd, the inode is retrieved and each data block is read one by one. 
		Also implemented reading of partial data blocks, start and end blocks.

	6. fs_write:
		Input parameter:
			fd: The oft table index
			buf:  buffer which contains the data tot be stored.
			nbytes: number of bytes to be written
		Output parameter:
			Number of bytes written
		Performed sanity checks to check for invalid fd. Also checked for write mode for the file. 
		This function appends the given buffer content to the existing files. Also implemented writing of partial data blocks, start and end blocks.

## Learnings:
	1. How Inode works in the actual file system.
	2. How block-live-data operations are used to slice and fit the larger block into the device blocks.  
