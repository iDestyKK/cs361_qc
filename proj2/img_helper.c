/*
 * COSC 361 - Project 2: Image Helper
 *
 * Description:
 *     This is a helper file for all three parts which implements functions
 *     that allow the user to export image files of their data generated
 *     using First Come, First Serve, Round Robin, or Shortest Job First
 *     algorithms.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#include "img_helper.h"
#include "lib/PPM_Edit/ppm_edit.h"

//Structs
/*typedef struct cnb_char {
	cn_byte *pixel; //[w][h]
} CNB_CHAR;

typedef struct font_set {
	CNB_CHAR chars[256];
} FONT_SET;*/

void font_set_load_font(FONT_SET* fs, char* fname) {
	FILE* fp = fopen(fname, "rb");

	if (fp == NULL)
		return; //You failed
	
	//Read the width and height
	cn_uint char_num,
			a, b,
	        i = 0;
	cn_byte char_id;
	fread(&fs->width , sizeof(cn_uint), 1, fp);
	fread(&fs->height, sizeof(cn_uint), 1, fp);
	fread(&char_num  , sizeof(cn_uint), 1, fp);
	
	for (; i < 256; i++) {
		fs->valid[i] = CN_FALSE;
	}
	
	for (i = 0; i < char_num; i++) {
		fread(&char_id, sizeof(char), 1, fp);
		CNB_CHAR* cur_char = &fs->chars[char_id];
		cur_char->pixel = (cn_byte *) malloc(sizeof(cn_byte) * fs->width * fs->height);
		for (b = 0; b < fs->height; b++) {
			for (a = 0; a < fs->width; a++) {
				fread(&cur_char->pixel[a + (b * fs->width)], sizeof(cn_byte), 1, fp);
			}
		}
		fs->valid[char_id] = CN_TRUE;
	}

	fclose(fp);
}

CNB_CHAR* font_set_get_char(FONT_SET* fs, char index) {
	return &fs->chars[index];
}

void print_char_to_ppm(cnpe_uint x, cnpe_uint y, PPM img, FONT_SET* fs, char index) {
	cnpe_uint a, b;
	CNB_CHAR* bitmap = &fs->chars[index];
	if (fs->valid[index] == CN_TRUE) {
		for (b = 0; b < fs->height; b++) {
			for (a = 0; a < fs->width; a++) {
				PPM_RGB* pixel = ppm_at(img, x + a, y + b);
				if (bitmap->pixel[a + (b * fs->width)] == 1) {
					pixel->R = 0;
					pixel->G = 0;
					pixel->B = 0;
				}
			}
		}
	}
}

void font_write_on_ppm(cnpe_uint x, cnpe_uint y, FONT_SET* fs, PPM img, char* str) {
	cn_uint len = strlen(str),
	        i = 0;
	for (; i < len; i++) {
		if (str[i] != ' ')
			print_char_to_ppm(x + (fs->width + 1) * i, y, img, fs, str[i]);
	}
}

void font_write_on_ppm_centered(cnpe_uint x, cnpe_uint y, FONT_SET* fs, PPM img, char* str) {
	cn_uint len = strlen(str);
	font_write_on_ppm(x - ((fs->width + 1) * len * 0.5), y, fs, img, str);
}

void draw_rectangle_colour(PPM img, cnpe_uint x1, cnpe_uint y1, cnpe_uint x2, cnpe_uint y2, PPM_RGB* colour) {
	cn_uint a, b;
	PPM_RGB* pixel;
	for (b = y1; b < y2; b++) {
		for (a = x1; a < x2; a++) {
			pixel = ppm_at(img, a, b);
			pixel->R = colour->R;
			pixel->G = colour->G;
			pixel->B = colour->B;
		}
	}
}

void whiteout(PPM img) {
	memset(img->img, 0xFF, img->width * img->height * 3);
	/*cn_uint a, b;
	PPM_RGB* pixel;
	for (b = 0; b < img->height; b++) {
		for (a = 0; a < img->width; a++) {
			pixel = ppm_at(img, a, b);
			pixel->R = 0xFF;
			pixel->G = 0xFF;
			pixel->B = 0xFF;
		}
	}*/
}

void ppm_write_to_file(PPM img, char* fname) {
	//Write the PPM data to a PPM file
	FILE* fp = fopen(fname, "wb");
	if (fp == NULL)
		return; //You failed (and wasted the processing above...)
						
	//Header
	fprintf(fp, "P6\n%d %d\n255\n", img->width, img->height);
									
	//Actual bytes
	fwrite(ppm_data(img), sizeof(struct ppm_rgb), ppm_get_size(img), fp);
	
	fclose(fp);
}
