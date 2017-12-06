#ifndef  __FW_KM_H__
#define __FW_KM_H__

#include <stdbool.h>
#include <stdint.h>

/** 0: KM INITIALISE
    #BB00
    Initialize the Key Manager
    Action:
    Full initialization of the Key Manager (as during EMS). All Key Manager variables,
    buffers and indirections are initialized. The previous state of the Key Manager is lost.
    Entry conditions:
    No conditions.
    Exit conditions:
    AF,BC,DE and HL corrupt. All other registers preserved.
    Notes:
    The Key Manager indirection (KM TEST KEY) is set to its default routine.
    The key buffer is set up (to be empty).
    The expansion buffer is set up and the expansions are set to their default strings.
    The translation table are initialized to their default translations.
    The repeating key map is initialized to its default state.
    The repeat speeds are set to their default values.
    Shift and caps lock are turned off.
    The break event is disarmed.
    See Appendices II, III and IV for the default translation tables, repeating key table
    and expansion strings.
    This routine enables interrupts.
    Related entries:
    KM RESET
*/
void fw_km_initialise(void);

/** 1: KM RESET
    #BB03
    Reset the Key Manager.
    Action:
    Reinitializes the Key Manager indirections and buffers.
    Entry conditions:
    No conditions.
    Exit conditions:
    AF,BC,DE and HL corrupt. All other registers preserved.
    Notes:
    The Key Manager indirection (KM TEST KEY) is set to its default routine.
    The key buffer is set up (to be empty).
    The expansion buffer is set up and the expansions are set to their default strings (see
    Appendix IV).
    The break event is disarmed.
    All pending keys and characters are discarded.
    This routine enables interrupts.
    Related entries:
    KM DISARM BREAK
    KM EXP BUFFER
    KM INITIALISE
*/
void fw_km_reset(void);

/** 2: KM WAIT CHAR
    #BB06
    Wait for the next character from the keyboard.
    Action:
    Try to get a character from the key buffer or the current expansion string. This routine
    waits until a character is available if no character is immediately available.
    Entry conditions:
    No conditions.
    Exit conditions:
    Carry true. A contains the character. Other flags corrupt. All other registers preserved.
    Notes:
    The possible sources for generating the next character are, in the order that they are
    tested:
    The 'put back' character. The next character of an expansion string. The first character
    of an expansion string. A character from a key translation table.
    Expansion tokens found in the key translation table are expanded to their associated
    strings. Expansion tokens found in expansion strings are not expanded but are treated
    as characters.
    Related entries:
    KM CHAR RETURN
    KM READ CHAR
    KM WAIT KEY
*/
unsigned char fw_km_wait_char (void);

/** 3: KM READ CHAR
    #BB09
    Test if a character is available from the keyboard.
    Action: Try to get a character from the key buffer or the current expansion string.
    This routine does not wait for a character to become available if there is no character
    available immediately.
    Entry conditions:
    No conditions.
    Exit conditions:
    If there was a character available:
    Carry true. A contains the character.
    If there was no character available.
    Carry false. A corrupt.
    Always:
    Other flags corrupt. All other registers preserved.
    Notes:
    The possible sources for generating the next character are, in the order that they are
    tested:
    The 'put back' character. The next character of an expansion string. The first character
    of an expansion string. A character from a key translation table.
    Expansion tokens in the key translation table will be expanded to their associated
    strings. Expansion tokens found in expansion strings are not expanded but are treated
    as characters.
    This routine will always return a character if one is available. It is therefore possible
    to flush out the Key Manager buffers by calling KM READ CHAR repeatedly until it
    reports that no character is available.
    Related entries:
    KM CHAR RETURN
    KM FLUSH
    KM READ KEY
    KM WAIT CHAR
*/
unsigned char fw_km_read_char (void);

/** 4: KM CHAR RETURN #BB0C
    Return a single character to the keyboard for next time.
    Action:
    Save a character for the next call of KM READ CHAR or KM WAIT CHAR.
    Entry conditions:
    A contains the character to put back.
    Exit conditions:
    All registers and flags preserved.
    Notes:
    The 'put back' character will be returned before any other character is generated by
    the keyboard. It will not be expanded (or otherwise dealt with) but will be returned as
    it is. The 'put back' character need not have been read from the keyboard, it could be
    inserted by the user for some purpose.
    It is only possible to have one 'put back' character. If this routine is called twice
    without reading a character between these then the first (put back' will be lost.
    Furthermore, it is not possible to return character 255 (because this is used as the
    marker for no 'put back' character).
    Related entries:
    KM READ CHAR
    KM WAIT CHAR
*/
void fw_km_char_return (unsigned char c) __z88dk_fastcall;

/** 
    #### CFWI-specific information: ####

    since C cannot handle carry flag, this routine returns zero if
    expansion is okay, non-zero if expansion failed.
    
    5: KM SET EXPAND
    #BB0F
    Set an expansion string.
    Action:
    Set the expansion string associated with an expansion token.
    Entry conditions:
    B contains the expansion token for the expansion to set. C contains the length of the
    string. HL contains the address of the string.
    Exit conditions:
    If the expansion is OK:
    Carry true.
    If the string was too long or the token was invalid:
    Carry false.
    Always:
    A,BC,DE,HL and other flags corrupt. All other registers preserved.
    Notes:
    The string to be set may lie anywhere in RAM. Expansion strings cannot be set
    directly from ROM.
    The characters in the string are not expanded (or otherwise dealt with). It is therefore
    possible to put any character into an expansion string.
    If there is insufficient room in the expansion buffer for the new string then no change
    is made to the expansions.
    If the string set is currently being used to generate characters (by KM READ CHAR
    or KM WAIT CHAR) then the unread portion of the string is discarded. The next
    character will be read from the key buffer.
    This routine enables interrupts.
    Related entries:
    KM GET EXPAND
    KM READ CHAR
    KM WAIT CHAR

*/
uint8_t fw_km_set_expand(uint8_t token, uint8_t string_length, unsigned char* string);

/** 

    #### CFWI-specific information: ####
    
    since C cannot handle carry flag, this routine returns a byte
    value if a character was returned, and any value outside range
    0-255 if not.
    
    6: KM GET EXPAND
    #BB12
    Get a character from an expansion string.
    Action:
    Read a character from an expansion string. The characters in the string are numbered
    starting from 0.
    Entry conditions:
    A contains an expansion token.
    L contains the character number.
    Exit conditions:
    If the character was found:
    Carry true. A contains the character.
    If the token was invalid or the string was not long enough:
    Carry false. A corrupt.
    Always:
    DE and other flags corrupt. All other registers preserved.
    Notes:
    The characters in the expansion string are not expanded (or otherwise dealt with). It is
    therefore possible to put any character into an expansion string.
    Related entries:
    KM READ CHAR
    KM SET EXPAND
*/
uint16_t fw_km_get_expand(uint8_t token, uint8_t char_number);


/** WARNING DONE BUT UNTESTED, MIGHT NOT WORK

    #### CFWI-specific information: ####

    since C cannot handle carry flag, this routine returns zero if
    operation went okay, non-zeroon failure.

    7: KM EXP BUFFER
    #BB15
    Allocate a buffer for expansion strings.
    Action:
    Set the address and length of the expansion buffer. Initialize the buffer with the
    default expansion strings.
    Entry conditions:
    DE contains the address of the buffer. HL contains the length of the buffer.
    Exit conditions:
    If the buffer is OK:
    Carry true.
    If the buffer is too short.
    Carry false.
    Always:
    A,BC,DE,HL and other flags corrupt. All other registers preserved.
    Notes:
    The buffer must not be located underneath a ROM and it must be at least 49 bytes
    long (i.e. have sufficient space for the default expansion strings). If the new buffer is
    too short then the old buffer is left unchanged.
    The default expansion strings are given in Appendix IV.
    Any expansion string currently being read is discarded.
    This routine enables interrupts.
    Related entries:
    KM GET EXPAND
    KM SET EXPAND
*/
uint8_t fw_km_exp_buffer(unsigned char *buffer, uint16_t buffer_bytecount);

/** 8: KM WAIT KEY
    #BB18
    Wait for next key from the keyboard.
    Action:
    Try to get a key from the key buffer. This routine waits until a key is found if no key
    is immediately available.
    Entry conditions:
    No conditions.
    Exit conditions:
    Carry true. A contains the character or expansion token.
    Other flags corrupt. All registers preserved.
    Notes:
    The next key is read from the key buffer and translated using the appropriate key
    translation table. Expansion tokens are not expanded but are passed out for the user to
    deal with, as are normal characters. Other Key Manager tokens (shift lock, caps lock
    and ignore) are obeyed but are not passed out.
    Related entries:
    KM READ KEY
    KM WAIT CHAR
*/
unsigned char fw_km_wait_key (void);

/** 
    #### CFWI-specific information: ####
    
    since C cannot handle carry flag, this routine returns a byte
    value if a character was returned, and any value outside range
    0-255 if not.

    9: KM READ KEY
    #BB1B
    Test if a key is available from the keyboard.
    Action:
    Try to get a key from the key buffer. This routine does not wait if no key is available
    immediately.
    Entry conditions:
    No conditions.
    Exit conditions.
    If a key was available:
    Carry true.
    A contains the character or expansion token.
    If no key was available:
    Carry false. A corrupt.
    Always:
    Other flags corrupt. All other registers preserved.
    Notes:
    The next key is read from the key buffer and translated using the appropriate key
    translation table. Expansion tokens are not expanded but are passed out for the user to
    deal with, as are normal characters. Other Key Manager tokens (shift lock, caps lock
    and ignore) are obeyed but are not passed out.
    This routine will always return a key if one is available. It is therefore possible to
    flush out the key buffer by calling KM READ KEY repeatedly until it claim no key is
    available. Note, however, that the 'put back' character or a partially read expansion
    string is ignored. It is advisable to use KM READ CHAR to flush these out when
    emptying the Key Manager buffers, or, in V1.1 firmware, to call KM FLUSH.
    Related entries:
    KM FLUSH
    KM READ CHAR
    KM WAIT KEY
*/
uint16_t fw_km_read_key (void);

/** WARNING DONE BUT UNTESTED, MIGHT NOT WORK
 
    #### CFWI-specific information: ####
    
    since C cannot handle zero flag, value is returned like this:
    
    uint16_t returned_value = fw_km_test_key(mykey);
    if (returned_value & 0ff)
    {
    // key pressed
    bool modifier_control = (returned_value & 0x80);
    bool modifier_shift = (returned_value & 0x20);
    }
    else
    {
    // key not pressed
    }


    10: KM TEST KEY
    #BB1E
    Test if a key is pressed.
    Action:
    Test if a particular key or joystick button is pressed. This is done using the key state
    map rather then by accessing the keyboard hardware.
    Entry conditions:
    A contains the key number.
    Exit conditions:
    If the key is pressed:
    Zero false.
    If the key is not pressed:
    Zero true.
    Always:
    Carry false. C contains the current shift and control state.
    A,HL and other flags corrupt. All other registers preserved.
    Notes:
    The shift and control states are automatically read when a key is scanned. If bit 7 is
    set then the control key is pressed and if bit 5 is set then one of the shift keys is
    pressed.
    The key number is not checked. An invalid key number will generate the correct shift
    and control states but the state of the key tested will be meaningless.
    The key state map which this routine tests is updated by the keyboard scanning
    routine. Normally this run is every fiftieth of a second and so the state may be out of
    date by that much. The key debouncing requires that a key should be released for two
    scans of the keyboard before it is marked as released in the key state map; the
    pressing of a key is detected immediately.
    Related entries:
    KM GET JOYSTICK
    KM GET STATE
    KM READ KEY
*/
uint16_t fw_km_test_key(uint8_t key_number) __z88dk_fastcall;

void fw_km_disarm_break(void);
void fw_km_break_event(void);
void fw_km_flush(void);

#endif /* __FW_KM_H__ */
