#ifndef FILEIO_H
#define FILEIO_H

int load_archive(const char *filename, const char *entry,
		char *buffer, int size);
int save_archive(const char *filename, const char *entry,
		const char *buffer, int size);

#endif
