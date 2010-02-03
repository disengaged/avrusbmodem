#include "modem.h"

static volatile unsigned char MODEM_TxBuf[MODEM_TX_BUFFER_SIZE];
static volatile unsigned char MODEM_RxBuf[MODEM_RX_BUFFER_SIZE];
static volatile unsigned char MODEM_TxHead;
static volatile unsigned char MODEM_TxTail;
static volatile unsigned char MODEM_RxHead;
static volatile unsigned char MODEM_RxTail;
static volatile unsigned char MODEM_LastRxError;

void modem_init(void)
{
	MODEM_TxHead = 0;
    MODEM_TxTail = 0;
    MODEM_RxHead = 0;
    MODEM_RxTail = 0;
}

unsigned int modem_getc(void)
{    
    unsigned char tmptail;
    unsigned char data;

    if (MODEM_RxHead == MODEM_RxTail)
	{
        return MODEM_NO_DATA;   // no data available 
    }
    
    // Calculate/store buffer index 
    tmptail = (MODEM_RxTail + 1) & MODEM_RX_BUFFER_MASK;
    MODEM_RxTail = tmptail; 
    
    // Get data from receive buffer
    data = MODEM_RxBuf[tmptail];
    
    return (MODEM_LastRxError << 8) + data;
}

void modem_putc(unsigned char data)
{
    unsigned char tmphead;
    
    tmphead  = (MODEM_TxHead + 1) & MODEM_TX_BUFFER_MASK;
    
    if (tmphead == MODEM_TxTail) 	// Error - transmit buffer overflow
	{
        MODEM_LastRxError = MODEM_BUFFER_OVERFLOW >> 8;
		Debug("Modem Buffer Overflow\r\n");
		return;
    }
    
    MODEM_TxBuf[tmphead] = data;
    MODEM_TxHead = tmphead;
}

void modem_puts(const char *s)
{
    while (*s) 
      modem_putc(*s++);
}

bool modem_TxBufferEmpty()
{
	if (MODEM_TxHead == MODEM_TxTail)
		return true;
	else
		return false;
}

bool modem_RxBufferEmpty()
{
	if (MODEM_RxHead == MODEM_RxTail)
		return true;
	else
		return false;
}

unsigned int modem_getTxBuffer(unsigned char *s, int maxSize)
{
	unsigned char tmptail;
	unsigned int i = 0;
		
	while (MODEM_TxHead != MODEM_TxTail && maxSize < MODEM_TX_BUFFER_SIZE)
	{
		// Calculate and store new buffer index 
		tmptail = (MODEM_TxTail + 1) & MODEM_TX_BUFFER_MASK;
	    MODEM_TxTail = tmptail;

	    // Get one byte from buffer and write it to the buffer
	    s[i++] = MODEM_TxBuf[tmptail];
	}

	return i;
}

unsigned int modem_putRxBuffer(unsigned char *s, int size)
{
	unsigned char tmphead;
	unsigned int lastRxError = 0;
	
	for (unsigned int BufferByte = 0; BufferByte < size; BufferByte++)
	{
		tmphead = (MODEM_RxHead + 1) & MODEM_RX_BUFFER_MASK;
		
		if (tmphead == MODEM_RxTail)							// Error - receive buffer overflow
		{	        
	        lastRxError = MODEM_BUFFER_OVERFLOW >> 8;
			break;
	    }
		else
		{
			MODEM_RxHead = tmphead;								// Store new index
			MODEM_RxBuf[tmphead] = s[BufferByte];			// Store received data in buffer
	    }
	}

	return lastRxError;
}

