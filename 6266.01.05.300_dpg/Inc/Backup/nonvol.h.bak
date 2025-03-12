// $Id$
 
// nonvol.h        Copyright (C) 2010, Real-Time Systems Inc.
//-------------------------------------------- All Rights Reserved ----------
//
//  Nonvolatile Variable Support
//
//---------------------------------------------------------------------------
 
#ifndef NONVOL_H
#define NONVOL_H
 
#include "eeprom_.h"
 
 
// Nonvolatile storage layout manager for byte-eraseable devices.
 
class NvStore
{
  Eeprom& eeprom;       // EEPROM driver
  EeAddr base;          // start address of first record in EEPROM
  EeAddr end;           // end of available EEPROM
 // static const /*size_t*/uint32_t maxNameLen = 16;  // maximum record name length
 
 
public:
  NvStore(Eeprom& ee, EeAddr b, EeAddr e)
  : eeprom(ee), base(b), end(e) { }
 
  // Look up variable.  If found, read into buffer.  If not found, create
  // record and initialize from buffer.  If neither is possible, return an
  // invalid address so update() can ignore the record.
 
  EeAddr open(const char* name, void* buf, size_t len);
 
  // Write out buffer to record payload at 'addr'.
 
  void update(EeAddr addr, const void* buf, size_t len);
};
 
 
// Nonvolatile variable.
 
template <typename T>
class Nv
{
  T t;                  // RAM image of the nonvolatile variable
  NvStore& store;       // EEPROM storage layout manager
  EeAddr addr;          // EEPROM address of the record payload
 
public:
  Nv(const T& _t, NvStore& s, const char* name)
  : t(_t), store(s), addr(store.open(name, &t, sizeof(t)))
  { }
 
  // Read variable from RAM image.
 
  operator const T& () const { return t; }
 
  // Write variable to RAM and through to backing store.
 
  const T& operator = (const T& _t)
  { 
    t = _t;
    store.update(addr, &t, sizeof(t));
    return t; 
  }
};  
 
 
#endif // NONVOL_H
