/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>

int do_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char * const *ap;
	int left, adv, expr, last_expr, neg, last_cmp;

	/* args? */
	if (argc < 3)
		return 1;

#if 0
	{
		printf("test:");
		left = 1;
		while (argv[left])
			printf(" %s", argv[left++]);
	}
#endif

	last_expr = 0;
	left = argc - 1; ap = argv + 1;
	if (left > 0 && strcmp(ap[0], "!") == 0) {
		neg = 1;
		ap++;
		left--;
	} else
		neg = 0;

	expr = -1;
	last_cmp = -1;
	last_expr = -1;
	while (left > 0) {

		if (strcmp(ap[0], "-o") == 0 || strcmp(ap[0], "-a") == 0)
			adv = 1;
		else if (strcmp(ap[0], "-z") == 0 || strcmp(ap[0], "-n") == 0)
			adv = 2;
		else
			adv = 3;

		if (left < adv) {
			expr = 1;
			break;
		}

		if (adv == 1) {
			if (strcmp(ap[0], "-o") == 0) {
				last_expr = expr;
				last_cmp = 0;
			} else if (strcmp(ap[0], "-a") == 0) {
				last_expr = expr;
				last_cmp = 1;
			} else {
				expr = 1;
				break;
			}
		}

		if (adv == 2) {
			if (strcmp(ap[0], "-z") == 0)
				expr = strlen(ap[1]) == 0 ? 1 : 0;
			else if (strcmp(ap[0], "-n") == 0)
				expr = strlen(ap[1]) == 0 ? 0 : 1;
			else {
				expr = 1;
				break;
			}

			if (last_cmp == 0)
				expr = last_expr || expr;
			else if (last_cmp == 1)
				expr = last_expr && expr;
			last_cmp = -1;
		}

		if (adv == 3) {
			if (strcmp(ap[1], "=") == 0)
				expr = strcmp(ap[0], ap[2]) == 0;
			else if (strcmp(ap[1], "!=") == 0)
				expr = strcmp(ap[0], ap[2]) != 0;
			else if (strcmp(ap[1], ">") == 0)
				expr = strcmp(ap[0], ap[2]) > 0;
			else if (strcmp(ap[1], "<") == 0)
				expr = strcmp(ap[0], ap[2]) < 0;
			else if (strcmp(ap[1], "-eq") == 0)
				expr = simple_strtol(ap[0], NULL, 10) == simple_strtol(ap[2], NULL, 10);
			else if (strcmp(ap[1], "-ne") == 0)
				expr = simple_strtol(ap[0], NULL, 10) != simple_strtol(ap[2], NULL, 10);
			else if (strcmp(ap[1], "-lt") == 0)
				expr = simple_strtol(ap[0], NULL, 10) < simple_strtol(ap[2], NULL, 10);
			else if (strcmp(ap[1], "-le") == 0)
				expr = simple_strtol(ap[0], NULL, 10) <= simple_strtol(ap[2], NULL, 10);
			else if (strcmp(ap[1], "-gt") == 0)
				expr = simple_strtol(ap[0], NULL, 10) > simple_strtol(ap[2], NULL, 10);
			else if (strcmp(ap[1], "-ge") == 0)
				expr = simple_strtol(ap[0], NULL, 10) >= simple_strtol(ap[2], NULL, 10);
			else {
				expr = 1;
				break;
			}

			if (last_cmp == 0)
				expr = last_expr || expr;
			else if (last_cmp == 1)
				expr = last_expr && expr;
			last_cmp = -1;
		}

		ap += adv; left -= adv;
	}

	if (neg)
		expr = !expr;

	expr = !expr;

	debug (": returns %d\n", expr);

	return expr;
}

/**
*0 sucess. 1 failed
*/
#define CONFIG_SCRIPT_ADDR 0x84000084
int do_checkout_firmware(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	volatile unsigned char *fw = (volatile unsigned char *)CONFIG_SCRIPT_ADDR;

	unsigned char str[6] = {0};
	strncpy(str, fw, 6);
	if ( !strcmp(str, "script") ) {
		return 0;
		
	} else {
		return 1;
	}
	
	return 1;
}

int do_burning_qsdk(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char cmd[128] = {0};
	
	printf("do_burning_qsdk\n");
	sprintf(cmd, "sf probe && imgaddr=0x84000000 && source $imgaddr:script");

	return run_command(cmd, 0);
}

int do_burning_lede(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char cmd[128] = {0};
	
	printf("do_burning_lede\n");
	sprintf(cmd, "sf probe && sf erase 0x%x 0x%x && sf write 0x84000000 0x%x $filesize",
		CONFIG_FIRMWARE_START, CONFIG_FIRMWARE_SIZE, CONFIG_FIRMWARE_START);

	return run_command(cmd, 0);
}


U_BOOT_CMD(
	test,	CONFIG_SYS_MAXARGS,	1,	do_test,
	"minimal test like /bin/sh",
	"[args..]"
);

int do_false(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return 1;
}

U_BOOT_CMD(
	false,	CONFIG_SYS_MAXARGS,	1,	do_false,
	"do nothing, unsuccessfully",
	NULL
);

int do_true(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return 0;
}

U_BOOT_CMD(
	true,	CONFIG_SYS_MAXARGS,	1,	do_true,
	"do nothing, successfully",
	NULL
);

U_BOOT_CMD(
	checkfw,	CONFIG_SYS_MAXARGS,	1,	do_checkout_firmware,
	"minimal test like /bin/sh",
	"[args..]"
);

U_BOOT_CMD(
	burning_qsdk,	CONFIG_SYS_MAXARGS,	1,	do_burning_qsdk,
	"minimal test like /bin/sh",
	"[args..]"
);

U_BOOT_CMD(
	burning_lede,	CONFIG_SYS_MAXARGS,	1,	do_burning_lede,
	"minimal test like /bin/sh",
	"[args..]"
);


