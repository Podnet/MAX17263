#include "MAX17263.h"
#include <Wire.h> // 105 byte RAM

// Public Methods

bool MAX17263::batteryPresent()
{ status._word=getStatus(); // without battery, status reading is 1111111111111111, so wait for Bst flag 1111111111110111 
  bool bst = status.bitfield.bst; // check battery status Bst flag = 0, UG6597 page 33 
  return(!bst); 
}

bool MAX17263::powerOnResetEvent()
{ status._word=getStatus();
  bool por = status.bitfield.por; // check POR bit
  return(por);  
}

void MAX17263::init()
{ 
  if(!waitForDNRdataNotReady()) return; // Step 1
  readAndstoreHibernateCFG();
  exitHibernate(); // Step 2: Initialize Configuration 
  setEZconfig(); // Step 2.1 EZ Config (No INI file is needed):
  restoreHibernateCFG();
  clearPORpowerOnReset(); // Step 3: Initialization Complete 
  setLEDCfg1();
  setLEDCfg2();
}

void MAX17263::readHistory(history_t &history) 
{ history.RCOMP0 = read(regRCOMP0); // all copied from UG6595 page 11/15
  history.TempCo = read(regTempCo); 
  history.FullCapRep = read(regFullCapRep); 
  history.Cycles = read(regCycles); 
  history.FullCapNom = read(regFullCapNom);  
}

void MAX17263::restoreHistory(history_t &history) 
{ writeAndVerify(regRCOMP0, history.RCOMP0); // all copied from UG6595 page 11/15
  writeAndVerify(regTempCo, history.TempCo); 
  writeAndVerify(regFullCapRep, history.FullCapRep);   
  // Write dQacc to 200% of Capacity and dPacc to 200%
  // dQAcc: Capacity, tracks change in battery charge between relaxation points, for debug purposes
  // dPAcc: 1/16% per LSB, Initial Value: 0x0190 (25%) tracks change in battery state of charge between relaxation points, for debug purposes
  uint16_t dPacc = 0x0C80; // ?
  uint16_t dQacc = history.FullCapNom / 2; // ?
  writeAndVerify (regdPacc, dPacc); // ?
  writeAndVerify (regdQacc, dQacc); // ?
  writeAndVerify(regCycles, history.Cycles); 
}

float MAX17263::getCapacity_mAh()
{ return read(regRepCap);
}

int16_t MAX17263::getCurrent()
{ return read(regCurrent);
}

float MAX17263::getVcell()
{ return read(regVCell) * voltage_multiplier_V;
}

float MAX17263::getAvgVCell() // todo always 11V
{ return read(regAvgVCell) * voltage_multiplier_V;
}

float MAX17263::getSOC() // at new battery: SOC = 0%, after 10min. 256.0% than 99% 
{ uint16_t SOC_raw = read(regRepSOC);
	return SOC_raw * SOC_multiplier;
}

float MAX17263::getTimeToEmpty()
{ uint16_t TTE_raw = read(regTimeToEmpty);
	return TTE_raw * time_multiplier_Hours;
}

float MAX17263::getTemp()
{ return read(regTemp)/256.0; 
}

// Private Methods

uint16_t MAX17263::getStatus() // MAX1726x-ModelGauge-m5-EZ-user-guide
{ return read(regStatus); 
}

void MAX17263::write(uint8_t reg, uint16_t value)
{ Wire.beginTransmission(I2Caddress); // write LSB first 
  Wire.write(reg);
  twoBytes._word = value;
  Wire.write(twoBytes.bitfield.lowByte); 
  Wire.write(twoBytes.bitfield.highByte); 
  uint8_t last_status = Wire.endTransmission();
}

void MAX17263::writeAndVerify(uint8_t reg, uint16_t value) 
{ uint16_t valueRead, attempt=0;
  do 
  { write(reg, value);
    delay(1); // 1ms
    valueRead = read(reg);
  }
  while (value != valueRead && attempt++<3);
}

uint16_t MAX17263::read(uint8_t reg)
{ Wire.beginTransmission(I2Caddress); 
  Wire.write(reg);
  Wire.endTransmission(false);  
  Wire.requestFrom(I2Caddress, (uint8_t)2); 
  twoBytes.bitfield.lowByte = Wire.read();
  twoBytes.bitfield.highByte = Wire.read(); 
  return twoBytes._word;
}

bool MAX17263::waitForDNRdataNotReady()
{ bool returnval = 0;
  const uint16_t DNRmask = 0x1;
  const unsigned long DNRwait_ms = 100; // time is not tested
  unsigned long start_ms = millis();
  while(1) // wait until until FSTAT.DNR==0
  { 
    if(!(read(regFStat) & DNRmask)) // FSTAT.DNR==0
    { returnval = 1;
      break;
    }
    if((millis()-start_ms) > DNRwait_ms)
    {  
      break;
    }
    delay(10); // according Maxim
  }  
  return returnval;
}

bool MAX17263::waitforRefreshReady()
{ bool returnval = 0;
  const uint16_t refreshMask = 0x8000;
  const unsigned long refreshWait_ms = 800; // > 400
  unsigned long start_ms = millis();
  while(1) 
  { 
    if(!(read(regModelCfg) & refreshMask)) // after ModelCFG reload the MAX1726x clears Refresh bit to 0 
    { returnval = 1;
      break;
    }
    if((millis()-start_ms) > refreshWait_ms)
    { 
      break;
    }
    delay(100); 
  }  
  return returnval;
}

void MAX17263::setEZconfig()
{ setDesignCap_mAh(designCap_mAh); // max 163000mAh // todo 
  setModelCFG(r100, vChg, modelID); 
  waitforRefreshReady();
}

void MAX17263::setDesignCap_mAh(long c) // max 65,535*2,5 = 163000mAh
{
  write(regDesignCap, c); 
  
}  
  
void MAX17263::setModelCFG(bool r100, bool vChg, uint8_t modelID)
{ 
  modelCFG._word = 0;
  modelCFG.bitfield.modelID = modelID;
  modelCFG.bitfield.vChg = vChg; 
  modelCFG.bitfield.r100 = r100;
  modelCFG.bitfield.refresh = 1; // set refresh bit to 1 for model refresh
  write(regModelCfg, modelCFG._word); 
}

void MAX17263::exitHibernate() // UG6595 page 7
{ write(0x60, 0x90); // Exit Hibernate Mode step 1 
  write(regHibCfg, 0x0); // Exit Hibernate Mode step 2
  write(0x60, 0x0); // Exit Hibernate Mode step 3
}

void MAX17263::readAndstoreHibernateCFG()
{ originalHibernateCFG = read(regHibCfg); // store original HibCFG value
} 

void MAX17263::restoreHibernateCFG()
{ write(regHibCfg, originalHibernateCFG); // Restore original HibCFG value
} 

void MAX17263::clearPORpowerOnReset()
{ const uint16_t PORmask = 0xFFFD; // 1111111111111101 
  uint16_t val = read(regStatus); // read status 
  write(regStatus, val & PORmask); // clear POR bit 1111111111111101   
}

void MAX17263::setLEDCfg1() 
{ LEDCfg1._word = read(regLedCfg1); // default 0x6070 page 29/37
  LEDCfg1.bitfield.LEDTimer = 0; // 1.3s -> 0.1s because of external regulator overheath
  LEDCfg1.bitfield.LChg = 0; // default=1, no LEDs on while charging because of T1 overheath
  LEDCfg1.bitfield.NBARS = 10; // using larger LED resistors may not achieve correct auto-count upon start up
  write(regLedCfg1, LEDCfg1._word);   
}
 
void MAX17263::setLEDCfg2() 
{ LEDCfg2._word = read(regLedCfg2); // default 0x011f page 30/37
  LEDCfg2.bitfield.EnAutoLEDCnt = 0; // 1->0 using larger LED resistors may not achieve correct auto-count upon start up
  LEDCfg2.bitfield.EBlink = 1; // 0->1 blink lowest LED when empty is detected
  LEDCfg2.bitfield.Brightness = 31; // max 31
  write(regLedCfg2, LEDCfg2._word);   
}