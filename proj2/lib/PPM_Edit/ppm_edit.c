/*
 * PPM_Edit Library (C Version)
 *
 * Version 0.1.0 (Last Updated: 2016-07-20)
 *
 * Description:
 *     Implements a custom data structure made to handle images. This structure
 *     relies on CN_SGrids to operate. As they are practically for this kind of
 *     structure exclusively. You can manipulate colours, import, and export
 *     PPM files flawlessly. It even supports PGM and PBM files.
 * 
 *     Changelog of this library is located at the bottom of "ppm_edit.h".
 * 
 * Author:
 *     Clara Van Nguyen
 * 
 * For documentation and details on every function in this library, visit the following URL:
 * http://web.eecs.utk.edu/~ssmit285/lib/ppm_edit/c/index.html
 */

#include "ppm_edit.h"

//Functions
//Initialiser
PPM ppm_init() {
	return ppm_init_size(0, 0);
}

PPM ppm_init_size(cnpe_uint width, cnpe_uint height) {
	PPM img = (PPM) malloc(sizeof(struct ppm));
	img->width  = width;
	img->height = height;
	img->range  = 0xFF;
	if (width * height != 0)
		img->img = (PPM_RGB *) malloc(sizeof(struct ppm_rgb) * width * height);
	else
		img->img = NULL;
	return img;
}

//Destructor
void ppm_free(PPM obj) {
	if (obj->img != NULL)
		free(obj->img);
	free(obj);
}

//File Management
void ppm_open(PPM obj, const char* fname) {
	FILE* fp = fopen(fname, "rb");
	if (fp == NULL)
		return; //You failed.

	cnpe_byte header[3];
	header[2] = '\0';
	fread(header, 1, 2, fp);
	printf("%s\n", header);

	cnpe_byte _mode = header[1] - 0x30;

	if (header[0] != 'P' || _mode > 6)
		return; //Header Fail

	rewind(fp);
	cnpe_byte _str[64],
	          _wid[64],
	          _hei[64],
			  _ran[64];
	
	fscanf(fp, "%s %s %s %s", _str, _wid, _hei, _ran);

	obj->width  = atoi(_wid);
	obj->height = atoi(_hei);
	obj->range  = atoi(_ran);

	//Force Clear
	if (obj->img != NULL)
		free(obj->img);
	obj->img = (PPM_RGB*) malloc(sizeof(struct ppm_rgb) * obj->width * obj->height);

	cnpe_byte _val;
	//Go to next line
	while (1) {
		_val = fgetc(fp);
		if (_val == 0xD || _val == 0xA)
			continue;
		break;
	}
	fseek(fp, -1, SEEK_CUR);

	switch (_mode) {
		case 0x1: _open_pbm_ascii (obj, fp); break; //PBM ASCII
		case 0x2: _open_pgm_ascii (obj, fp); break; //PGM ASCII
		case 0x3: _open_ppm_ascii (obj, fp); break; //PPM ASCII
		case 0x4: _open_pbm_binary(obj, fp); break; //PBM BINARY
		case 0x5: _open_pgm_binary(obj, fp); break; //PGM BINARY
		case 0x6: _open_ppm_binary(obj, fp); break; //PPM BINARY
	}
	fclose(fp);
}

//Get
cnpe_uint ppm_get_width(PPM obj) {
	return obj->width;
}

cnpe_uint ppm_get_height(PPM obj) {
	return obj->height;
}

cnpe_uint ppm_get_size(PPM obj) {
	return (obj->width * obj->height);
}

PPM_RGB* ppm_at(PPM obj, cnpe_uint x, cnpe_uint y) {
	return (PPM_RGB*) ((char*) obj->img + (sizeof(struct ppm_rgb) * ((y * obj->width) + x)));
}

PPM_RGB* ppm_data(PPM obj) {
	return obj->img;
}

cnpe_byte ppm_empty(PPM obj) {
	return (obj->img == NULL);
}

//Loading Functions
void _open_pbm_ascii(PPM obj, FILE* fp) {
	//P1
	//TODO: Implement this...
}

void _open_pgm_ascii(PPM obj, FILE* fp) {
	//P2
	//TODO: Implement this...
}

void _open_ppm_ascii(PPM obj, FILE* fp) {
	//P3
	//TODO: Implement this...
}

void _open_pbm_binary(PPM obj, FILE* fp) {
	//P4
	cnpe_uint pos = ftell(fp), fsize, i, sread;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	
	sread = fsize - pos;
	cnpe_byte* bytes = (cnpe_byte *) malloc(sread);
	fread(bytes, 1, sread, fp);

	for (i = 0; i < sread; i++)
		memset(obj->img + 1, bytes[i] == 0x00 ? 0xFF : 0x00, sizeof(struct ppm_rgb));
	free(bytes);
}

void _open_pgm_binary(PPM obj, FILE* fp) {
	//P5
	cnpe_uint pos = ftell(fp), fsize, i, sread;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	sread = fsize - pos;
	cnpe_byte* bytes = (cnpe_byte *) malloc(sread);
	fread(bytes, 1, sread, fp);
	
	for (i = 0; i < sread; i++)
		memset(obj->img + i, bytes[i] * (0xFF / obj->range), sizeof(struct ppm_rgb));
	free(bytes);
}

void _open_ppm_binary(PPM obj, FILE* fp) {
	//P6
	cnpe_uint pos = ftell(fp), fsize, sread;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	
	sread = fsize = pos;
	fread(obj->img, 1, sread, fp);
	printf("Read %d bytes (size: %d)\n", sread, fsize);
	
}
