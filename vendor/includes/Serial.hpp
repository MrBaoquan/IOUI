/** Serial.h
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * License: This source code can be used and/or modified without restrictions.
 * It is provided as is and the author disclaims all warranties, expressed 
 * or implied, including, without limitation, the warranties of
 * merchantability and of fitness for any purpose. The user must assume the
 * entire risk of using the Software.
 *
 * @author Hans de Ruiter
 *
 * @version 0.1 -- 28 October 2008
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <string>
#include <windows.h>

typedef std::basic_string<TCHAR> tstring;

class Serial
{
private:
	HANDLE commHandle;

public:
	Serial(const std::string &commPortName, int bitRate = 115200)
	{
		commHandle = CreateFileA(("\\\\.\\" + commPortName).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			0, NULL);

		if (commHandle == INVALID_HANDLE_VALUE)
		{
			throw("ERROR: Could not open com port");
		}
		else
		{
			// set timeouts
			COMMTIMEOUTS cto = { MAXDWORD, 0, 0, 0, 0 };
			DCB dcb;
			if (!SetCommTimeouts(commHandle, &cto))
			{
				Serial::~Serial();
				throw("ERROR: Could not set com port time-outs");
			}

			// set DCB
			memset(&dcb, 0, sizeof(dcb));
			dcb.DCBlength = sizeof(dcb);
			dcb.BaudRate = bitRate;
			dcb.fBinary = 1;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
			dcb.fRtsControl = RTS_CONTROL_ENABLE;

			dcb.Parity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;
			dcb.ByteSize = 8;

			if (!SetCommState(commHandle, &dcb))
			{
				Serial::~Serial();
				throw("ERROR: Could not set com port parameters");
			}
		}
	}

	virtual ~Serial()
	{
		CloseHandle(commHandle);
	}
	/** Writes a NULL terminated string.
	 *
	 * @param buffer the string to send
	 *
	 * @return int the number of characters written
	 */
	int write(const char buffer[])
	{
		DWORD numWritten;
		WriteFile(commHandle, buffer, strlen(buffer), &numWritten, NULL);

		return numWritten;
	}

	/** Writes a string of bytes to the serial port.
	 *
	 * @param buffer pointer to the buffer containing the bytes
	 * @param buffLen the number of bytes in the buffer
	 *
	 * @return int the number of bytes written
	 */
	int write(const char* buffer, int buffLen)
	{
		DWORD numWritten;
		WriteFile(commHandle, buffer, buffLen, &numWritten, NULL);

		return numWritten;
	}
	/** Reads a string of bytes from the serial port.
	 *
	 * @param buffer pointer to the buffer to be written to
	 * @param buffLen the size of the buffer
	 * @param nullTerminate if set to true it will null terminate the string
	 *
	 * @return int the number of bytes read
	 */
	int read(char* buffer, int buffLen, bool nullTerminate = true)
	{
		DWORD numRead;
		if (nullTerminate)
		{
			--buffLen;
		}

		BOOL ret = ReadFile(commHandle, buffer, buffLen, &numRead, NULL);

		if (!ret)
		{
			return 0;
		}

		if (nullTerminate)
		{
			buffer[numRead] = '\0';
		}

		return numRead;
	}

#define FLUSH_BUFFSIZE 10
	/** Flushes everything from the serial port's read buffer
	 */
	void flush()
	{
		char buffer[FLUSH_BUFFSIZE];
		int numBytes = read(buffer, FLUSH_BUFFSIZE, false);
		while (numBytes != 0)
		{
			numBytes = read(buffer, FLUSH_BUFFSIZE, false);
		}
	}
};

#endif
