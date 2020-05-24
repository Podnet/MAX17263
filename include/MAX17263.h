
#ifndef Arduino-MAX17263_Driver.h_h
#define Arduino-MAX17263_Driver.h_h

#include <Arduino.h>

struct history_t
{ uint16_t RCOMP0; 
  uint16_t TempCo;
  uint16_t FullCapRep; 
  uint16_t Cycles; 
  uint16_t FullCapNom; 
};

class MAX17263
{
public:  
  bool batteryPresent();
  bool powerOnResetEvent();
  void init();
  int16_t getCurrent();
  float getVcell();
  float getCapacity_mAh();
  float getSOC(); // SOC = State of Charge 
  float getTimeToEmpty();
  float getTemp(); 
  float getAvgVCell(); 
  void readHistory(history_t &history);
  void restoreHistory(history_t &history);

  uint8_t modelID; 
  bool r100, vChg; 
  long designCap_mAh;
  
private:
  const uint8_t regStatus      = 0x00; // UG6597 page 32 Flags related to alert thresholds and battery insertion or removal
  const uint8_t regModelCfg    = 0xdb; // UG6597 page 29 Basic options of the EZ algorithm.
  const uint8_t regVCell       = 0x09; // VCell reports the voltage measured between BATT and CSP
  const uint8_t regAvgVCell    = 0x19; // The AvgVCell register reports an average of the VCell register readings. 
  const uint8_t regCurrent     = 0x0A; // Voltage between the CSP and CSN pins, and would need to convert to current
  const uint8_t regAvgCurrent  = 0x0B; // The AvgCurrent register reports an average of Current register readings
  const uint8_t regRepSOC      = 0x06; // The Reported State of Charge of connected battery. 
  const uint8_t regTimeToEmpty = 0x11; // How long before battery is empty (in ms).  
  const uint8_t regRepCap      = 0x05; // Reported Capacity. 
  const uint8_t regDesignCap   = 0x18; // Capacity of battery inserted, not typically used for user requested capacity
  const uint8_t regTemp        = 0x08; // Temperature
  const uint8_t regFStat       = 0x3D; // Status of the ModelGauge m5 algorithm
  const uint8_t regHibCfg      = 0xBA; // hibernate mode functionality UG6597 page 41
  const uint8_t regLedCfg1     = 0x40;
  const uint8_t regLedCfg2     = 0x4B;
  const uint8_t regCycles      = 0x17; // history 0 to 655.35 cycles with a 1% LSb  
  const uint8_t regRCOMP0      = 0x38; // history
  const uint8_t regTempCo      = 0x39; // history
  const uint8_t regFullCapRep  = 0x10; // history
  const uint8_t regFullCapNom  = 0x23; // history
  const uint8_t regdPacc       = 0x46; // history
  const uint8_t regdQacc       = 0x45; // history
 
  const uint8_t I2Caddress = 0x36;
  uint16_t originalHibernateCFG;  
  const float voltage_multiplier_V = 78.125e-6; // UG6595 page 4
  const float time_multiplier_Hours = 5.625/3600.0; // UG6595 page 10, lsb = 5.625 seconds
  const float SOC_multiplier = 1.0/256.0; // UG6595 page 4

  uint16_t getStatus(); 
  bool waitForDNRdataNotReady();
  void clearPORpowerOnReset();
  void setDesignCap_mAh(long c);
  void setEZconfig(); 
  void setLEDCfg1();
  void setLEDCfg2();
  void setModelCFG(bool r100, bool vChg, uint8_t modelID);
  bool waitforRefreshReady();
  void exitHibernate();
  void readAndstoreHibernateCFG();
  void restoreHibernateCFG();

  uint16_t read(uint8_t reg);
  void write(uint8_t reg, uint16_t value);
  void writeAndVerify(uint8_t reg, uint16_t value); 

  union 
  { uint16_t _word;
    struct 
    { uint8_t lowByte:8, highByte:8; 
    } bitfield;  
  } twoBytes;
  
  union 
  { uint16_t _word;
    struct 
    { bool :1, por:1, :1, bst:1; // lsb ... msb, por = bit1
    } bitfield;  
  } status;

  union 
  { uint16_t _word;
    struct 
    { bool dnr:1; // lsb ... msb, dnr = bit0
    } bitfield;  
  } fStat;
  
  union 
  { uint16_t _word;
    struct 
    { uint8_t :4, modelID:4, :2, vChg:1, :2, r100:1, :1, refresh:1; // lsb ... msb, page 17/37
    } bitfield;  
  } modelCFG;

  union 
  { uint16_t _word;
    struct 
    { uint8_t NBARS:4, :1, LChg:1, LEDMd:2, :5, LEDTimer:3; // lsb ... msb
    } bitfield;  
  } LEDCfg1;

  union 
  { uint16_t _word;
    struct 
    { uint8_t Brightness:5, :1, EBlink:1, :1, EnAutoLEDCnt:1; // lsb ... msb
    } bitfield;  
  } LEDCfg2;
};

#endif