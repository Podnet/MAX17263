#include "mgos.h"
#include "mgos_max17263.h"



bool mgos_max17263_battery_present(MAX17263 *mx){
    return mx->batteryPresent();
} // gives battery present
bool mgos_max17263_powerOnResetEvent(MAX17263 *mx){
    return mx->powerOnResetEvent();
}  // reset the soc chip
void mgos_max17263_init(MAX17263 *mx){
    return mx->init();
}  // intialize
int16_t mgos_max17263_get_current(MAX17263 *mx){
    return mx->getCurrent();
} // gives current reading
float mgos_max17263_get_Vcell(MAX17263 *mx){
    return mx->getVcell();
}  
float mgos_max17263_get_Capacity_mAh(MAX17263 *mx){
    return mx->getCapacity_mAh();
}
float mgos_max17263_get_SOC(MAX17263 *mx){
    return mx->getSOC();
} // SOC = State of Charge 
float mgos_max17263_get_TimeToEmpty(MAX17263 *mx){
    return mx->getTimeToEmpty();
}
float mgos_max17263_get_Temp(MAX17263 *mx){
    return mx->getTemp();
}
float mgos_max17263_get_AvgVCell(MAX17263 *mx){
    return mx->getAvgVCell();
}
void mgos_max17263_read_History(MAX17263 *mx, history_t &history){
    return mx->readHistory(history);
}
void mgos_max17263_restore_History(MAX17263 *mx, history_t &history){
    return mx->restoreHistory(history); 
}