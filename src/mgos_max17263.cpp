#include "mgos.h"
#include "mgos_max17263.h"



bool mgos_max17263_battery_present(){
    return batteryPresent();
} // gives battery present
bool mgos_max17263_powerOnResetEvent(){
    return powerOnResetEvent();
}  // reset the soc chip
void mgos_max17263_init();{
    return init()
}  // intialize
int16_t mgos_max17263_get_current(){
    return getCurrent();
} // gives current reading
float mgos_max17263_get_Vcell(){
    return getVcell();
}  
float mgos_max17263_get_Capacity_mAh(){
    return getCapacity_mAh();
}
float mgos_max17263_get_SOC(){
    return getSOC();
} // SOC = State of Charge 
float mgos_max17263_get_TimeToEmpty(){
    return getTimeToEmpty();
}
float mgos_max17263_get_Temp(){
    return getTemp();
}
float mgos_max17263_get_AvgVCell(){
    return getAvgVCell();
}
void mgos_max17263_read_History(history_t &history){
    return readHistory(history);
}
void mgos_max17263_restore_History(history_t &history){
    return restoreHistory(history); 
}