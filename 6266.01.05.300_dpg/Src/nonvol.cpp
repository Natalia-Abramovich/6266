// $Id$
 
// nonvol.cpp        Copyright (C) 2010, Real-Time Systems Inc.
//-------------------------------------------- All Rights Reserved ----------
//
//  Nonvolatile Variable Support
//
//---------------------------------------------------------------------------
 
//#include "types.h"
#include "string.h"
#include "minmax.h"
#include <stdint.h> 
#include "nonvol.h"
#include "main.h" 
 
// An NvField object supports reading or writing a header or payload field
// incrementally, while simultaneously updating the checksum and advancing
// the EEPROM address on each operation.
 
typedef uint8_t NvSum;    // ones-complement checksum
 
class NvField
{
  Eeprom& eeprom; // the backing EEPROM device
  EeAddr addr;    // current read/write address in device
  NvSum sum;    // checksum accumulator
 
  // Update ones-complement checksum with contents of buffer and advance
  // address with its length.
 
  void note(const void* buf, size_t len)
  {
    const uint8_t* p = static_cast<const uint8_t*>(buf);
 
    for (size_t i = 0; i < len; i++)
    {
      NvSum t = sum;
 
      if ((sum += *p++) < t)
        sum += 1;
    }
 
    addr += len;
  }
 
public:
  NvField(Eeprom& e, EeAddr a) : eeprom(e), addr(a), sum(0) { }
 
  // Write buffer to device; update checksum and address.
 
  void write(const void* buf, size_t len)
  {
    eeprom.write(addr, buf, len);
    note(buf, len);
  }
 
  // Read from device into buffer; update checksum and address.
 
  bool read(void* buf, size_t len)
  {
    bool retval = eeprom.read(addr, buf, len);
    note(buf, len);
    return retval;   
  }
 
  // Write or read arbitrary types.
 
  template <typename T> void write(const T& t) { write(&t, sizeof(t)); }
  template <typename T> bool read(T& t) { return read(&t, sizeof(t)); } 
 
  // Write out checksum to device.
 
  void writeSum() { write(NvSum(~sum)); }  
 
  // Read checksum from device and test it.
 
  bool testSum() { NvSum s;  return read(s) && NvSum(~sum) == 0; }
 
  // Get address of field following this one.
 
  EeAddr next() const { return addr; }
};
 
 
// Look up variable.  If found, read into buffer.  If not found, create
// record and initialize from buffer.  If neither is possible, return an
// invalid address so update() can ignore the record.

EeAddr NvStore::open(const char* name, void* buf, size_t len)
{
  EeAddr addr = base;
  static const size_t maxNameLen = 16;  // maximum record name length

  while (addr < end)
  {
    NvField hdr(eeprom, addr);
 
    // Get length of the data field.  Abort if read error.
 
    size_t dataLen;
    if (!hdr.read(dataLen))
      return 0;
 
    // Has the data length ever been written?  If so, we should be able to
    // find a record here.
 
    if (dataLen != ~0u)
    {
      // Get record name and compare with the desired name.
 
      bool match = true;
      char c;
      const char* n = name;
      for (size_t i = 0;
           i < maxNameLen && hdr.read(c) && c != '\0';
           i++)
      {
        if (c != *n++)
          match = false;
      }
 
      // Header checksum valid?
 
      if (hdr.testSum())
      {
        // Is this the desired record?  Read the data field once to test its
        // checksum.
 
        if (match)
        {
          NvField pay(eeprom, hdr.next());
          uint8_t t;
          for (size_t i = 0; i < len; i++)
            pay.read(t);
 
          // If data field checksum is good, copy out data field to variable,
          // else fall back to supplied default values.
 
          if (pay.testSum())
            NvField(eeprom, hdr.next()).read(buf, len);
          else
            update(hdr.next(), buf, len);
 
          // In either case, we're done.
 
          return hdr.next();
        }
 
        // Not the desired record; continue searching.
 
        else
        {   
          addr = hdr.next() + dataLen + sizeof(NvSum);
          continue;
        }
      }  
    }
 
    // No record ever written here, or can't read header -- stop searching.
    __no_operation();
    break;
  }
 
  // Check that a new record will fit within our part of the device.
 
  size_t recLen = sizeof(size_t)                    // payload length
                + min(strlen(name) + 1, maxNameLen) // record name
                + sizeof(NvSum)                     // header checksum
                + len                               // payload length
                + sizeof(NvSum);                    // payload checksum
 
  if (addr >= end - recLen)
    return 0;
 
  // Create a new record and initialize its data field to the supplied
  // default values.
 
  NvField nhdr(eeprom, addr);
  nhdr.write(len);
  nhdr.write(name, min(strlen(name), maxNameLen - 1));
  nhdr.write('\0');
  nhdr.writeSum(); 
 
  update(nhdr.next(), buf, len);
  return nhdr.next();
}
 
// Write contents of data field and set its checksum.
 
void NvStore::update(EeAddr addr, const void* buf, size_t len)
{
  if (addr)
  {
    NvField payload(eeprom, addr);
    payload.write(buf, len);
    payload.writeSum();
  }
}  
