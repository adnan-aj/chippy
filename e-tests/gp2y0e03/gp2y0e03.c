/* -*- c-basic-offset: 8 -*-
 * gp2y0e03.c -- GP2Y0E03 Distance Measuring Sensor Driver
 * Copyright (C) 2014 Hiroshi Takekawa
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * ATTENTION: GPL version 2 only. You cannot apply any later version.
 * This situation may change in the future.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define GP2Y0E03_SLAVE_ADDR		0x40

#define GP2Y0E03_REG_SHIFT_BIT		0x35
#define GP2Y0E03_REG_DISTANCE		0x5e
#define GP2Y0E03_REG_SOFTWARE_RESET	0xee

#define GP2Y0E03_RESET 0x06

#define GP2Y0E03_RETRY_COUNT 10
#define GP2Y0E03_RETRY_INTERVAL 10

static int open_i2c_dev(int adapter_n) {
	char dev_fn[20];

	snprintf(dev_fn, 19, "/dev/i2c-%d", adapter_n);

	return open(dev_fn, O_RDWR);
}

static int set_slave_addr(int fd, int addr) {
	return ioctl(fd, I2C_SLAVE, addr);
}

static int gp2y0e03_read_reg(int fd, int addr) {
	int i, byte;

	for (i = 0; i < GP2Y0E03_RETRY_COUNT; i++) {
		byte = i2c_smbus_read_byte_data(fd, addr);
		if (byte >= 0 || i == GP2Y0E03_RETRY_COUNT - 1)
			break;
		usleep(GP2Y0E03_RETRY_INTERVAL * 1000);
	}

	return byte;
}

static int gp2y0e03_write_reg(int fd, int addr, unsigned char byte) {
	int i, res;

	for (i = 0; i < GP2Y0E03_RETRY_COUNT; i++) {
		res = i2c_smbus_write_byte_data(fd, addr, byte);
		if (res >= 0 || i == GP2Y0E03_RETRY_COUNT - 1)
			break;
		usleep(GP2Y0E03_RETRY_INTERVAL * 1000);
	}

	return res;
}

static int reset_gp2y0e03(int fd) {
	int res;

	res = gp2y0e03_write_reg(fd, GP2Y0E03_REG_SOFTWARE_RESET, GP2Y0E03_RESET);
	if (res < 0)
		return res;

	return 1;
}

static int read_distance(int fd, double *distance_r) {
	int shift, h, l, d;
	double v;

	shift = gp2y0e03_read_reg(fd, GP2Y0E03_REG_SHIFT_BIT);
	if (shift < 0)
		return shift;
	h = gp2y0e03_read_reg(fd, GP2Y0E03_REG_DISTANCE);
	if (h < 0)
		return h;
	l = gp2y0e03_read_reg(fd, GP2Y0E03_REG_DISTANCE + 1);
	if (l < 0)
		return l;
	d = (h << 4) | l;

	v = (d / 16) >> shift;
	*distance_r = v;

	return 1;
}

int main(int argc, char **argv)
{
	int fd, res;
	double distance;

	if ((fd = open_i2c_dev(1)) < 0) {
		fprintf(stderr, "open_i2c_dev() failed: %s\n", strerror(errno));
		return fd;
	}

	if ((res = set_slave_addr(fd, GP2Y0E03_SLAVE_ADDR)) < 0) {
		fprintf(stderr, "set_slave_addr() failed: %s\n", strerror(errno));
		close(fd);
		return res;
	
}
	if ((res = reset_gp2y0e03(fd)) < 0) {
		fprintf(stderr, "enable_gp2y0e03() failed: %s\n", strerror(errno));
		close(fd);
		return res;
	}

	while (1) {
		sleep(1);

		if ((res = read_distance(fd, &distance)) < 0) {
			printf("Cannot get pressure\n");
		} else {
			printf("distance = %g cm\n", distance);
		}
	}

	close(fd);

	return 0;
}
