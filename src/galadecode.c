/** decode gala signal - quick & dirty hack ahead! 
 * 
 * @todo support different waveform formats
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "config.h"



/** global variables to make local later, side effects y'know */
struct
{
	/** array with filenames to process */
	char **files;
	/** bytes per sample (1, 2, 3, 4, 8)*/
	int bytes_per_sample;
	/** samples per second */
	int samplerate;
	/** current size of buffer */
	ssize_t bufsize;
	/** interval in ms to output speed */
	int output_interval;
}_g;


/** print commandline help */
static void _print_help(char *name)
{
	printf("calculate and display current speed from raw GALA waveform data - %s\n"
	       "Usage: %s [options] <stream>\n\n"
	       "Choose \"-\" as <file> to read from stdin\n\n"
	       "Valid options:\n"
	       "\t--help\t\t\t-h\t\tThis help text\n"
	       "\t--interval <ms>\t\t-i <ms>\t\tMaximum interval to show speed [500]\n\n",	       
	       PACKAGE_URL, name);
}


/** parse commandline arguments */
static bool _parse_args(int argc, char *argv[])
{
	int index, argument;

	static struct option loptions[] =
	{
		{"help", 0, 0, 'h'},
		{"interval", required_argument, 0, 'i'},
		{0,0,0,0}
	};

	
	const char arglist[] = "hi:";
	while((argument = getopt_long(argc, argv, arglist, loptions, &index)) >= 0)
	{
		
		switch(argument)
		{			
			/* --help */
			case 'h':
			{
				_print_help(argv[0]);
				return false;
			}

			/* --interval */
			case 'i':
			{
				if(sscanf(optarg, "%d", (int*) &_g.output_interval) != 1)
				{
					fprintf(stderr, "Invalid interval \"%s\" (Use an integer to define interval in milliseconds)", optarg);
					return false;
				}
				break;
			}
				
			/* invalid argument */
			case '?':
			{
				fprintf(stderr, "argument %d is invalid\n", index);
				_print_help(argv[0]);
				return false;
			}

			/* unhandled arguments */
			default:
			{
				fprintf(stderr, "argument %d is invalid\n", index);
				break;
			}
		}
	}


	_g.files = &argv[optind];
	
	
	return true;
}


/** filter noise from signal, we only want peaks above a certain threshold */
static void filter_noise(short *signal, ssize_t samples, short threshold)
{
	ssize_t samples_left;
	
	for(samples_left = samples;
	    samples_left-1 > 0; samples_left--)
	{
		/* filter negative noise */
		if(*signal < 0 && *signal > -threshold)
		{
			*signal++ = 0;
			continue;
		}

		/* filter positive noise */
		if(*signal > 0 && *signal < threshold)
		{
			*signal++ = 0;
			continue;
		}

		signal++;
	}
}


/** find zerocrossing and output frequency */
double zerocrossing(short *signal, ssize_t samples)
{
	static ssize_t samples_since_last_zerocross;
	static bool sampled_one_time;
	
	double frequency = 0;
	ssize_t samples_left;
	
	for(samples_left = samples;
	    samples_left-1 > 0; 
	    samples_left--)
	{
		/* negative -> positive zero crossing ? */
		if(signal[0] < 0 && signal[1] >= 0)
		{
			if(samples_since_last_zerocross != 0 &&
			   sampled_one_time)
			{
				frequency = (double) _g.samplerate/(double) samples_since_last_zerocross;
			}

			sampled_one_time = true;
			samples_since_last_zerocross = 0;
		}

		samples_since_last_zerocross++;
		
		/* move buffer further */
		signal++;
	}

	return frequency;
}

/** sample current time */
bool time_sample(struct timeval *t)
{
	if(gettimeofday(t, NULL) != 0)
        {
                fprintf(stderr, "Failed to sample time: %s", strerror(errno));
                return false;
        }

        return true;
}

/** check if interval passed since last sample */
bool time_interval_passed(struct timeval *last, int interval)
{
	/** hold the last time we printed the speed */
	struct timeval current;

	/* sample current time */
	if(!time_sample(&current))
		return false;
	
	/* calculate delay in ms */
	int delay = (current.tv_sec - last->tv_sec)*1000;
	delay += (current.tv_usec - last->tv_usec)/1000;

	/* if delay is bigger or equal to requested interval, return true
	   and sample time again */ 
	if(delay >= interval)
	{
		if(!time_sample(last))
			return false;
		
		return true;
	}
	
	/* else we return false and check again in next call */
	return false;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int main(int argc, char *argv[])
{
	

	/* initialize variables */
	int result = -1;
	/* buffer for waveform data */
	char *buffer = NULL;
	/** hold current time */
	struct timeval current;
	/** current frequency (equals km/h) */
	double freq = 0;
	
	_g.output_interval = 500;
	_g.bytes_per_sample = 2;
	_g.samplerate = 48000;
	_g.bufsize = 4096*_g.bytes_per_sample;
	
	/* parse commandline arguments */
	if(!_parse_args(argc, argv))
		return result;

	/* do we have at least one filename? */
	if(!_g.files[0])
	{
		fprintf(stderr, "No input file(s) given\n");
		goto m_deinit;
	}

	/* allocate buffer */
	if(!(buffer = calloc(1, _g.bufsize)))
	{
		fprintf(stderr, "Unable to allocate memory: %s\n", strerror(errno));
		goto m_deinit;
	}

	/* sample current time */
	if(!time_sample(&current))
		goto m_deinit;
	
	
	/* walk all files (supplied as commandline arguments) and output them */
	int filecount;
	for(filecount = 0; _g.files[filecount]; filecount++)
	{
		
		/* open file */
		int fd;
		if(_g.files[filecount][0] == '-' && strlen(_g.files[filecount]) == 1)
		{
			fd = STDIN_FILENO;
		}
		else
		{
			if((fd = open(_g.files[filecount], O_RDONLY)) < 0)
			{
				fprintf(stderr, "Failed to open \"%s\": %s\n", 
					_g.files[filecount], strerror(errno));
				continue;
			}
		}


		/* read a bufferful */
		ssize_t bytes_read = _g.bufsize;

		/* read complete file */
		while(bytes_read != 0)
		{
			if((bytes_read = read(fd, buffer, _g.bufsize)) == -1)
			{
				fprintf(stderr, "Error while reading: %s\n", strerror(errno));
				close(fd);
				goto m_deinit;
			}

			/* filter noise */
			filter_noise((short *) buffer, bytes_read/_g.bytes_per_sample, 5000);
			
			
			/* find zero crossing */
			double f = zerocrossing((short *) buffer, 
			             bytes_read/_g.bytes_per_sample);

			if(f > 0)
				freq = f;
			
			/* output frequency */
			if(time_interval_passed(&current, _g.output_interval))	
				printf("%f\n", freq);
			
		}
		
		/* close file */
		close(fd);
	}

	
	/* success */
	result = 0;
	
m_deinit:
	free(buffer);
	
	return result;
}
