/*
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
 * Copyright (C) 2012 by Steve Markgraf <steve@steve-m.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "getopt.h"
#endif

#include "rtl-sdr.h"
#include "convenience.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_BUF_LENGTH		(16 * 16384)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

struct CallbackContext {
	int is_exit;
	uint32_t bytes_to_read;
	rtlsdr_dev_t *dev;
	FILE* fp_in;
};

static struct CallbackContext* global_ctx = NULL;

#ifdef _WIN32
BOOL WINAPI sighandler(int signum) {
	if (CTRL_C_EVENT == signum) {
		fprintf(stderr, "Signal caught, exiting!\n");
		global_ctx->is_exit = 1;
		rtlsdr_cancel_async(global_ctx->dev);
		return TRUE;
	}
	return FALSE;
}
#else
static void sighandler(int signum) {
	fprintf(stderr, "Signal caught, exiting!\n");
	global_ctx->is_exit = 1;
	rtlsdr_cancel_async(global_ctx->dev);
}
#endif

static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
{
	if (ctx == NULL) {
		return;
	}

	struct CallbackContext* cfg = ctx;

	if (cfg->is_exit) {
		return;
	}

	if ((cfg->bytes_to_read > 0) && (cfg->bytes_to_read < len)) {
		len = cfg->bytes_to_read;
		cfg->is_exit = 1;
		rtlsdr_cancel_async(cfg->dev);
	}

	if (fwrite(buf, 1, len, cfg->fp_in) != len) {
		fprintf(stderr, "Short write, samples lost, exiting!\n");
		cfg->is_exit = 1;
		rtlsdr_cancel_async(cfg->dev);
	}

	if (cfg->bytes_to_read > 0) {
		cfg->bytes_to_read -= len;
	}
}

void usage(void) {
	fprintf(stderr,
		"rtl_sdr, an I/Q recorder for RTL2832 based DVB-T receivers\n\n"
		"Usage:\t -f frequency_to_tune_to [Hz]\n"
		"\t[-o file to write to (default: stdout)]\n"
		"\t[-s samplerate (default: 2048000 Hz)]\n"
		"\t[-d device_index (default: 0)]\n"
		"\t[-g gain (default: 0 for auto)]\n"
		"\t[-p ppm_error (default: 0)]\n"
		"\t[-b output_block_size (default: 16 * 16384)]\n"
		"\t[-n number of samples to read (default: 0, infinite)]\n"
		"\t[-E enable_option (default: none)]\n"
		"\t    use multiple -E to enable multiple options\n"
		"\t    direct:  enable direct sampling 1 (usually I)\n"
		"\t    direct2: enable direct sampling 2 (usually Q)\n"
		"\t    offset:  enable offset tuning\n"	
		"\t[-T enable bias-T on GPIO PIN 0 (works for rtl-sdr.com v3 dongles)]\n"
		"\t[-h shows help]\n"
	);
	exit(1);
}

int main(int argc, char **argv)
{
#ifndef _WIN32
	struct sigaction sigact;
#endif
	int r, opt;

	uint32_t frequency = 100000000;
	char *filename = NULL;
	uint32_t samp_rate = DEFAULT_SAMPLE_RATE;
	int dev_index = 0;
	int dev_given = 0;
	int gain = 0;
	int ppm_error = 0;
	uint32_t out_block_size = DEFAULT_BUF_LENGTH;
	uint32_t bytes_to_read = 0;
	int direct_sampling = 0;
	int offset_tuning = 0;
	int enable_bias_T = 0;

	while ((opt = getopt(argc, argv, "f:o:s:d:g:p:b:n:E:Th")) != -1) {
		switch (opt) {
		case 'f':
			frequency = (uint32_t)atofs(optarg);
			break;
        case 'o':
            filename = optarg;
            break;
		case 's':
			samp_rate = (uint32_t)atofs(optarg);
			break;
		case 'd':
			dev_index = verbose_device_search(optarg);
			dev_given = 1;
			break;
		case 'g':
			gain = (int)(atof(optarg) * 10); /* tenths of a dB */
			break;
		case 'p':
			ppm_error = atoi(optarg);
			break;
		case 'b':
			out_block_size = (uint32_t)atof(optarg);
			break;
		case 'n':
			bytes_to_read = (uint32_t)atof(optarg) * 2;
			break;
		case 'E':
			if (strncmp("direct", optarg, 7) == 0) {
				direct_sampling = 1;
			} else if (strncmp("direct2", optarg, 8) == 0) {
				direct_sampling = 2;
			} else if (strncmp("offset", optarg, 7) == 0) {
				offset_tuning = 1;
			}
			break;
		case 'T':
			enable_bias_T = 1;
			break;
		case 'h':
		default:
			usage();
			break;
		}
	}

	if(out_block_size < MINIMAL_BUF_LENGTH ||
	   out_block_size > MAXIMAL_BUF_LENGTH ){
		fprintf(stderr, "Output block size wrong value, falling back to default\n");
		fprintf(stderr, "Minimal length: %u\n", MINIMAL_BUF_LENGTH);
		fprintf(stderr, "Maximal length: %u\n", MAXIMAL_BUF_LENGTH);
		out_block_size = DEFAULT_BUF_LENGTH;
	}

	FILE *file = stdout;
	if(filename != NULL) {
		file = fopen(filename, "wb+");
		if (file == NULL) {
			fprintf(stderr, "Failed to open %s\n", filename);
			return 1;
		}
	}

#ifdef _WIN32
	_setmode(_fileno(file), _O_BINARY);
#endif

	if (!dev_given) {
		dev_index = verbose_device_search("0");
	}

	if (dev_index < 0) {
		fprintf(stderr, "Failed to find rtlsdr device %d < 0\n", dev_index);
		exit(1);
	}

	struct CallbackContext ctx;
	ctx.dev = NULL;
	ctx.is_exit = 0;
	ctx.bytes_to_read = bytes_to_read;
	ctx.fp_in = file;
	global_ctx = &ctx;

	r = rtlsdr_open(&ctx.dev, (uint32_t)dev_index);
	if (r < 0) {
		fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
		exit(1);
	}

#ifndef _WIN32
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);
#else
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)sighandler, TRUE );
#endif

	verbose_set_sample_rate(ctx.dev, samp_rate);
	verbose_set_frequency(ctx.dev, frequency);
	if (gain == 0) {
		verbose_auto_gain(ctx.dev);
	} else {
		gain = nearest_gain(ctx.dev, gain);
		verbose_gain_set(ctx.dev, gain);
	}
	verbose_ppm_set(ctx.dev, ppm_error);

	if (enable_bias_T) {
		rtlsdr_set_bias_tee(ctx.dev, enable_bias_T);
		fprintf(stderr, "activated bias-T on GPIO PIN 0\n");
	}
	if (direct_sampling) {
		verbose_direct_sampling(ctx.dev, direct_sampling);
	}
	if (offset_tuning) {
		verbose_offset_tuning(ctx.dev);
	}

	/* Reset endpoint before we start reading from it (mandatory) */
	verbose_reset_buffer(ctx.dev);
    fprintf(stderr, "Reading samples in async mode...\n");
    r = rtlsdr_read_async(ctx.dev, rtlsdr_callback, (void*)(&ctx), 0, out_block_size);

	if (ctx.is_exit) {
		fprintf(stderr, "\nUser cancel, exiting...\n");
    } else {
		fprintf(stderr, "\nLibrary error %d, exiting...\n", r);
    }

	if (file != stdout) {
		fclose(file);
    }

	rtlsdr_close(ctx.dev);
	return r >= 0 ? r : -r;
}
