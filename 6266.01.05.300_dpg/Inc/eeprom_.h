// $Id$
 
// eeprom.h        Copyright (C) 2010, Real-Time Systems Inc.
//-------------------------------------------- All Rights Reserved ----------
//
//  Abstract Base Class for all EEPROM Drivers
//
//---------------------------------------------------------------------------
 
#ifndef EEPROM__H
#define EEPROM__H
 
#include <stddef.h>
 

// EEPROM address type.
 
typedef unsigned int EeAddr;
 
 
// Base class for all EEPROM drivers.
 
class Eeprom
{
public:
  virtual ~Eeprom() { }
  virtual bool erase() { return false; }
 
  // Operations on raw buffers.
 
  virtual bool write(EeAddr addr, const void* buf, size_t len) = 0;
  virtual bool read(EeAddr addr, void* buf, size_t len) = 0;
 
  // Operations on known data types.
 
  template <typename T>
  bool write(EeAddr addr, const T& t) { return write(addr, &t, sizeof(t)); }
 
  template <typename T>
  bool read(EeAddr addr, T& t) { return read(addr, &t, sizeof(t)); }
};
 
 
#endif // EEPROM_H
