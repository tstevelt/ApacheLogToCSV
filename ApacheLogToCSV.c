/*----------------------------------------------------------------------------
	Program : ApacheLogToCSV.c
	Author  : Silver Hammer Software LLC
	Author  : Tom Stevelt
	Date    : Jun 2021
	Synopsis: Parse Apache access_log into pipe (|) delimited file. 

	MIT License
	-----------

	Copyright (c) 2021 Tom Stevelt (https://www.silverhammersoftware.com)
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		06/13/2021	* Removed check for existing output file
	tms		06/13/2021	* Added starting line option.

----------------------------------------------------------------------------*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>

typedef struct
{
	short	month;
	short	day;
	short	year;
	short	hour;
	short	minute;
	short	second;
} DATETIMEVAL;

int ApacheMonth ( char strMonth[] )
{
	if ( strncmp ( strMonth, "Jan", 3 ) == 0 )
	{
		return ( 1 );
	}
	else if ( strncmp ( strMonth, "Feb", 3 ) == 0 )
	{
		return ( 2 );
	}
	else if ( strncmp ( strMonth, "Mar", 3 ) == 0 )
	{
		return ( 3 );
	}
	else if ( strncmp ( strMonth, "Apr", 3 ) == 0 )
	{
		return ( 4 );
	}
	else if ( strncmp ( strMonth, "May", 3 ) == 0 )
	{
		return ( 5 );
	}
	else if ( strncmp ( strMonth, "Jun", 3 ) == 0 )
	{
		return ( 6 );
	}
	else if ( strncmp ( strMonth, "Jul", 3 ) == 0 )
	{
		return ( 7 );
	}
	else if ( strncmp ( strMonth, "Aug", 3 ) == 0 )
	{
		return ( 8 );
	}
	else if ( strncmp ( strMonth, "Sep", 3 ) == 0 )
	{
		return ( 9 );
	}
	else if ( strncmp ( strMonth, "Oct", 3 ) == 0 )
	{
		return ( 10 );
	}
	else if ( strncmp ( strMonth, "Nov", 3 ) == 0 )
	{
		return ( 11 );
	}
	else if ( strncmp ( strMonth, "Dec", 3 ) == 0 )
	{
		return ( 12 );
	}

	return ( 0 );
}

DATETIMEVAL	*ParseApacheDate ( char dbuf[] )
{
	static	DATETIMEVAL	dtValue;

	/*----------------------------------------------------------
		 0123456789-123456789-123456789-123456789-
		[01/Jun/2021:03:47:12 -0400]
	----------------------------------------------------------*/
	dtValue.month = ApacheMonth ( &dbuf[3] ); 
	dtValue.day = atoi ( &dbuf[0] );
	dtValue.year =  atoi ( &dbuf[7] );
	dtValue.hour =  atoi ( &dbuf[12] );
	dtValue.minute =  atoi ( &dbuf[15] );
	dtValue.second =  atoi ( &dbuf[18] );

	return ( &dtValue );
}


int main (int argc, char ** argv)
{
	FILE	*ifp, *ofp;
	char	xbuffer[10240];
	int		lineno;
	int		StartLine = 0;
//	int		Debug = 0;
	int		len, ndx, InQuote, InBracket;
	char		workbuf[24];
	DATETIMEVAL	*dtVal;

	if ( argc < 3 )
	{
		printf ( "USAGE: ApacheLogToCSV infile outfile [startline]\n" );
		exit ( 1 );
	}

	if ( access ( argv[1], F_OK ) != 0 )
	{
		printf ( "Cannot access input file %s\n", argv[1] );
		exit ( 1 );
	}

//	if ( access ( argv[2], F_OK ) == 0 )
//	{
//		printf ( "Output file %s exists! Program canceled!\n", argv[2] );
//		exit ( 1 );
//	}

	if (( ifp = fopen ( argv[1], "r" )) == (FILE *)0 )
	{
		printf ( "Cannot open input file %s\n", argv[1] );
		exit ( 1 );
	}

	if (( ofp = fopen ( argv[2], "w" )) == (FILE *)0 )
	{
		printf ( "Cannot created output file %s\n", argv[2] );
		exit ( 1 );
	}

	if ( argc > 3 )
	{
		StartLine = atoi ( argv[3] );
	}

	lineno = 0;

	fprintf ( ofp, "ADDR|" );
	fprintf ( ofp, "ID|" );
	fprintf ( ofp, "USER|" );
	fprintf ( ofp, "DATE|" );
	fprintf ( ofp, "TIME|" );
	fprintf ( ofp, "COMMAND|" );
	fprintf ( ofp, "STATUS|" );
	fprintf ( ofp, "BYTES|" );
	fprintf ( ofp, "REFER|" );
	fprintf ( ofp, "AGENT\n" );

	while ( fgets ( xbuffer, sizeof(xbuffer), ifp ) != (char *)0 )
	{
		lineno++;

		if ( lineno < StartLine )
		{
			continue;
		}

		len = strlen ( xbuffer );

		InQuote = InBracket = 0;

		for ( ndx = 0; ndx < len; ndx++ )
		{
			if ( xbuffer[ndx] == '"' )
			{
				if ( InQuote )
				{
					InQuote = 0;
				}
				else
				{
					InQuote = 1;
				}
			}
			else if ( InQuote == 0 && xbuffer[ndx] == '[' )
			{
				InBracket = 1;

				/*----------------------------------------------------------
					 123456789-123456789-123456789-123456789-
					[01/Jun/2021:03:47:12 -0400]
				----------------------------------------------------------*/
				sprintf ( workbuf, "%20.20s", &xbuffer[ndx+1] );
				dtVal = ParseApacheDate ( workbuf );

				fprintf ( ofp, "%02d/%02d/%02d|%02d:%02d:%02d",
					dtVal->month,
					dtVal->day,
					dtVal->year % 100,
					dtVal->hour,
					dtVal->minute,
					dtVal->second );

				ndx += 26;

			}
			else if ( InQuote == 0 && xbuffer[ndx] == ']' )
			{
				InBracket = 0;
			}
			else if ( xbuffer[ndx] == 92 && xbuffer[ndx+1] == '"' )
			{
				 xbuffer[ndx+1] = '.';
			}
			else if ( xbuffer[ndx] == ' ' )
			{
				if ( InQuote || InBracket )
				{
					fprintf ( ofp, " " );
				}
				else
				{
					fprintf ( ofp, "|" );
				}
			}
			else if ( xbuffer[ndx] == '|' )
			{
				fprintf ( ofp, "**PIPE**" );
			}
			else
			{
				fprintf ( ofp, "%c", xbuffer[ndx] );
			}
		}
	}

	fclose ( ifp );
	fclose ( ofp );

    return 0;
}
