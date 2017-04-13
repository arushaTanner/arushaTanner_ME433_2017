#ifndef _IO_EXPANDER_H    /* Guard against multiple inclusion */
#define _IO_EXPANDER_H

#define expander_addr 0b0100000
#define EXtrisbits 0x00
#define EXportbits 0x09
#define EXlatbits 0x10

void io_expander_init(void);

void io_expander_set(unsigned char pin,unsigned char level);

void io_expander_sset(unsigned char pin,unsigned char level);

void in_expander_get(void);

#endif /* _IO_EXPANDER_H */
