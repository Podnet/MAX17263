#include "MAX17263.h"

#ifdef __cplusplus
extern "C"
{
#endif


bool mgos_max17263_battery_present(); // gives battery present
bool mgos_max17263_powerOnResetEvent();  // reset the soc chip
void mgos_max17263_init();  // intialize
int16_t mgos_max17263_get_current(); // gives current reading
float mgos_max17263_get_Vcell();  /
float mgos_max17263_get_Capacity_mAh();
float mgos_max17263_get_SOC(); // SOC = State of Charge 
float mgos_max17263_get_TimeToEmpty();
float mgos_max17263_get_Temp(); 
float mgos_max17263_get_AvgVCell(); 
void mgos_max17263_read_History(history_t &history);
void mgos_max17263_restore_History(history_t &history);









#ifdef __cplusplus
}
#endif  /* __cplusplus */