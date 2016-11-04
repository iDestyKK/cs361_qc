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

#include <stdio.h>
#include <stdlib.h>

//The Secret Weapon(s)
#include "lib/handy/types.h"
#include "lib/PPM_Edit/ppm_edit.h"

//Structs
typedef struct cnb_char {
	cn_byte *pixel; //[w][h]
} CNB_CHAR;

typedef struct font_set {
	cn_uint width, height;
	CNB_CHAR chars[256];
	cn_bool  valid[256];
} FONT_SET;

void      font_set_load_font(FONT_SET*, char*);
CNB_CHAR* font_set_get_char (FONT_SET*, char);
void      print_char_to_ppm (cnpe_uint, cnpe_uint, PPM, FONT_SET*, char);
void      font_write_on_ppm (cnpe_uint, cnpe_uint, FONT_SET*, PPM, char*);
void      whiteout          (PPM);
void      ppm_write_to_file (PPM, char*);
