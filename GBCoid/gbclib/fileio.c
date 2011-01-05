#include "zip.h"
#include "unzip.h"
#include <stdio.h>

int load_archive(const char *filename, const char *entry,
		char *buffer, int size)
{
    unzFile fd = NULL;
    int ret = 0;

	/* Attempt to open the archive */
	fd = unzOpen(filename);
	if(!fd)
	{
		return -1;
	}

	/* Go to first file in archive */
	ret = unzLocateFile(fd, entry, 0);
	if(ret != UNZ_OK)
	{
		unzClose(fd);
		return -1;
	}

	/* Open the file for reading */
	ret = unzOpenCurrentFile(fd);
	if(ret != UNZ_OK)
	{
		unzClose(fd);
		return -1;
	}

	/* Read (decompress) the file */
	ret = unzReadCurrentFile(fd, buffer, size);

	/* Close the current file */
	unzCloseCurrentFile(fd);
	unzClose(fd);

	return ret;
}

int save_archive(const char *filename, const char *entry,
		const char *buffer, int size)
{
    zipFile fd = NULL;
    int ret = 0;
    fd=zipOpen(filename, APPEND_STATUS_ADDINZIP);
    if(!fd)
    {
       return (0);
    }

    ret=zipOpenNewFileInZip(fd, entry,
			    NULL,
				NULL,0,
			    NULL,0,
			    NULL,
			    Z_DEFLATED,
			    Z_DEFAULT_COMPRESSION);
			    
    if(ret != ZIP_OK)
    {
       zipClose(fd,NULL);
       return (0);    
    }

    ret=zipWriteInFileInZip(fd,buffer,size);
    if(ret != ZIP_OK)
    {
      zipCloseFileInZip(fd);
      zipClose(fd,NULL);
      return (0);
    }

    ret=zipCloseFileInZip(fd);
    if(ret != ZIP_OK)
    {
      zipClose(fd,NULL);
      return (0);
    }

    ret=zipClose(fd,NULL);
    if(ret != ZIP_OK)
    {
      return (0);
    }
	
    return(1);
}

