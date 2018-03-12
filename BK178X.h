/*
  BK178X.h - Arduino library for interfacing with BK178X power supply.

  Copyright (c) 2018 Hunter Circuit Works.  All right reserved.
*/

#ifndef BK178X_h
#define BK178X_h

#define BK178X_DEBUG 0

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif



class BK178X
{

  public:
	BK178X();
    BK178X(uint8_t);
	uint8_t	open();
	uint8_t	open(uint16_t);
	uint8_t	remote_mode(uint8_t);
	uint8_t	output_control(uint8_t);
	uint8_t set_voltage(uint32_t);
	uint8_t set_current(uint16_t);
    uint8_t enable_local_key(uint8_t);
	

  private:
	uint8_t addr;
    uint8_t buf[26];

    uint8_t clear_buffer();
	uint8_t checksum();
	uint8_t read_status();
	
};

#endif

