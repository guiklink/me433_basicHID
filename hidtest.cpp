#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <string.h>

#define MAX_STR 255
#define BUFFER_SIZE 65
#define DATA_DIVIDER '#'
#define FILE_SIZE 100

void load_data_on_buffer(char *str, unsigned char *buffer);
void init_buffer(unsigned char *buf);

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[BUFFER_SIZE];
	//wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	/*// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);*/

	char msg_buffer[64];
	char row_number;

	printf("\nWrite a message for thi PIC32:\n");
	scanf("%s", msg_buffer);
	printf("\nDisplay in row: \n");
	scanf("%d",&row_number);

	init_buffer(buf);

	// Sending text
	buf[0] = 0x0;
	buf[1] = 0x01;

	load_data_on_buffer(msg_buffer, buf);

	// Load row number on buffer

	sprintf(msg_buffer,"%d",row_number);

	load_data_on_buffer(msg_buffer, buf);

	res = hid_write(handle, buf, 65);

	printf("\n>>> SENDING BUFFER...\n%s\n\n\n", &buf[2]);

	/*
	// Request state (cmd 0x81). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	for (i = 0; i < 4; i++)
		printf("buf[%d]: %d\n", i, buf[i]);
	*/

	// Creating file

	FILE *ofp;
	ofp = fopen("accels.txt", "w+");

	fprintf(ofp,"x,y,z\r\n");
	// Sample Accel
	for (i = 0; i < FILE_SIZE; ++i)
	{
		buf[0] = 0x0;
		buf[1] = 0x02;
		res = hid_write(handle, buf, 65);

		// Read requested state
		res = hid_read(handle, buf, 65);

		// Print out the returned buffer.
		//printf("Accel Data = %s\n", &buf[1]);
		fprintf(ofp,"%s\r\n",&buf[1]);
	}
	fclose(ofp);

	return 0;
}


void load_data_on_buffer(char *str, unsigned char *buffer)
{
	int i = 2, start, len;

	//printf("\nMSG : %s\n", str);
	//printf("BUF: %c\n", buffer[2]);

	len = strlen(str);

	while(i < BUFFER_SIZE - 1 && buffer[i] != '\0')
		i++;

	start = i;
	//printf("\WORD %s\n", str);
	//printf("\nSTART AT %d\nAnd LEN = %d\n", start, len);


	while(i < (BUFFER_SIZE - 2) && (i - start) < len)
	{
		//printf("Printing: %c\nIn I = %d\n", str[i-start],i);
		buffer[i] = str[i-start];
		i++;
	}

	buffer[i] = DATA_DIVIDER;

	if(i + 1 < BUFFER_SIZE - 1)
		buffer[i+1] = '\0';
}

void init_buffer(unsigned char *buffer)
{
	int i;

	for (i = 0; i < BUFFER_SIZE; ++i)
	{
		buffer[i] = '\0';
	}
}
