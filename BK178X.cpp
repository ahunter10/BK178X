/*
  BK178X.h - Arduino library for interfacing with BK178X power supply.

  Copyright (c) 2018 Hunter Circuit Works.  All right reserved.
*/



#include "BK178X.h"


BK178X::BK178X(void)
{
  addr = 0;
}


BK178X::BK178X(uint8_t a)
{
  addr = a;
}


uint8_t BK178X::open(void)
{

  Serial3.begin(4800);
  if (BK178X_DEBUG) Serial.begin(19200);
  return true;
}


uint8_t BK178X::open(uint16_t baud)
{

  Serial3.begin(baud);
  if (BK178X_DEBUG) Serial.begin(19200);
  return true;
}


//0 for front panel mode
//1 for remote mode
uint8_t BK178X::remote_mode(uint8_t mode)
{
  clear_buffer();

  buf[0] = 0xAA;
  buf[1] = addr;
  buf[2] = 0x20;
  buf[3] = mode;
  buf[25] = checksum();

  Serial3.write(buf, 26);
  return read_status();
}


//0 for output off
//1 for output on
uint8_t BK178X::output_control(uint8_t mode)
{

  clear_buffer();

  buf[0] = 0xAA;
  buf[1] = addr;
  buf[2] = 0x21;
  buf[3] = mode;
  buf[25] = checksum();

  Serial3.write(buf, 26);
  return read_status();
}


//setpoint in mV
uint8_t BK178X::set_voltage(uint32_t mv)
{

  clear_buffer();

  buf[0] = 0xAA;
  buf[1] = addr;
  buf[2] = 0x23;
  buf[3] = (mv & 0x000000FF);
  buf[4] = (mv & 0x0000FF00) >> 8;
  buf[5] = (mv & 0x00FF0000) >> 16;
  buf[6] = (mv & 0xFF000000) >> 24;
  buf[25] = checksum();

  Serial3.write(buf, 26);
  return read_status();
}



//setpoint in mA
uint8_t BK178X::set_current(uint16_t ma)
{

  clear_buffer();

  buf[0] = 0xAA;
  buf[1] = addr;
  buf[2] = 0x24;
  buf[3] = (ma & 0x00FF);
  buf[4] = (ma & 0xFF00) >> 8;
  buf[25] = checksum();

  Serial3.write(buf, 26);
  return read_status();
}


//enable key 7 to switch back to local mode
uint8_t BK178X::enable_local_key(uint8_t mode)
{

  clear_buffer();

  buf[0] = 0xAA;
  buf[1] = addr;
  buf[2] = 0x37;
  buf[3] = mode;
  buf[25] = checksum();

  Serial3.write(buf, 26);
  return read_status();
}


uint8_t BK178X::clear_buffer(void)
{
  int i;
  for (i = 0; i < 26; i++) {
    buf[i] = 0;
  }
}


uint8_t BK178X::checksum(void)
{
  uint8_t c = 0;
  uint8_t i;
  for (i = 0; i < 25; i++) {
    c += buf[i];
  }
  return c;
}


uint8_t BK178X::read_status()
{
  uint32_t t = millis();
  uint8_t i = 0;

  clear_buffer();
  while ((millis() - t) < 1000) {

    if (Serial3.available()) {
      buf[i] = Serial3.read();
      i++;
      if (i == 26) {
        break;
      }
    }
  }

  if (i == 26) {
    if (checksum() != buf[25]) {
      //CHECKSUM ERROR
      if (BK178X_DEBUG) Serial.println("Return packet checksum error");
      return false;
    }
    else if (buf[2] != 0x12) {
      //NOT STATUS PACKET
      if (BK178X_DEBUG) Serial.println("error: did not receive status packet");
      return false;
    }
    else if (buf[3] == 0x90) {
      if (BK178X_DEBUG) Serial.println("error: remote checksum error");
      return false;
    }
    else if (buf[3] == 0xA0) {
      //COMMAND NOT ACCEPTED
      if (BK178X_DEBUG) Serial.println("error: incorrect parameter");
      return false;
    }
    else if (buf[3] == 0xB0) {
      //COMMAND NOT ACCEPTED
      if (BK178X_DEBUG) Serial.println("error: unrecognized command");
      return false;
    }
    else if (buf[3] == 0xC0) {
      //COMMAND NOT ACCEPTED
      if (BK178X_DEBUG) Serial.println("error: invalid command");
      return false;
    }
    else if (buf[3] == 0x80) {
	  //SUCCESS
      if (BK178X_DEBUG) Serial.println("success: command accepted");
      return true;
    }
    else {
	  //OTHER ERROR
      if (BK178X_DEBUG) Serial.println("error: unknown status");
      return false;
    }
  }
  else {
    //TIMEOUT
    if (BK178X_DEBUG) Serial.println("Timeout error");
    return false;
  }
}






