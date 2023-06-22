#ifndef __JK_BMS_HELPER__

#define __JK_BMS_HELPER__



#include "esphome.h"
#include <stdio.h>
#include <string.h>



//namespace JkBmsHelper {

    float chargeCurrent = 0;
    float dischargeCurrent = 0;

    float limitedChargeCurrent = 0;
    float limitedDischargeCurrent = 0;

    char limitedChargeCurrentReason[200];
    char limitedDischargeCurrentReason[200];
    
    float rampUpLimitedChargeCurrent = -1;
    float rampUpLimitedDischargeCurrent = -1;

    bool chargeIsLimited = false;
    bool dischargeIsLimited = false;

    float minCharge;
    float minDischarge;

    void initValues(float initChargeCurrent, float initDischargeCurrent, float initMinCharge, float initMinDischarge ) {
        chargeCurrent = initChargeCurrent;
        dischargeCurrent = initDischargeCurrent;
        
        limitedChargeCurrent = chargeCurrent;
        limitedDischargeCurrent = dischargeCurrent;
        
        strcpy(limitedChargeCurrentReason,"");
        strcpy(limitedDischargeCurrentReason,"");
        
        if (rampUpLimitedChargeCurrent == -1) {
          rampUpLimitedChargeCurrent = chargeCurrent;
        } 
        
        if (rampUpLimitedDischargeCurrent == -1) {
          rampUpLimitedDischargeCurrent = dischargeCurrent;
        }

        minCharge = initMinCharge;
        minDischarge = initMinDischarge;
    }
    
    void limitAbsolutChargeCurrent(float maximum) {
        if (limitedChargeCurrent > maximum) {
            limitedChargeCurrent = maximum;  
        }
    }

    void limitAbsolutDischargeCurrent(float maximum) {
        if (limitedDischargeCurrent > maximum) {
            limitedDischargeCurrent = maximum;  
        }
    }
    
    void limitChargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText, bool byPowerOf) {
        if ((maximum > minimum && val > minimum)
            || (maximum < minimum && val < minimum)) {
            
            if (maximum > minimum) {
                maximum -= (maximum - minimum) * bufferFromMax;
            } else if (maximum < minimum) {
                maximum += (minimum - maximum) * bufferFromMax;
            }
            
            float limitFactor = 1;
            
            if (byPowerOf) {
                limitFactor = 1 - pow(((val - minimum) / (maximum - minimum)), 2);
            } else {
                limitFactor = 1 - ((val - minimum) / (maximum - minimum));
            }
            
            float chargeLimit = fmax(0.0f, chargeCurrent * limitFactor);

            if (chargeLimit < minCharge) {
                chargeLimit = 0.0f;
            }

            if (limitedChargeCurrent > chargeLimit) {
                limitedChargeCurrent = chargeLimit;
                strcpy(limitedChargeCurrentReason, "charge limited by ");
                strcat(limitedChargeCurrentReason, limitText);
                ESP_LOGI("main", "charge limited by %s to %.1f", limitText, uint16_t(limitedChargeCurrent * 10) / 10.0f);
                chargeIsLimited = true;
            }
        }
    }
    
    void limitChargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText) {
        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);
    }

    void limitDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText, bool byPowerOf) {
        if ((maximum > minimum && val > minimum)
            || (maximum < minimum && val < minimum)) {
            
            if (maximum > minimum) {
                maximum -= (maximum - minimum) * bufferFromMax;
            } else if (maximum < minimum) {
                maximum += (minimum - maximum) * bufferFromMax;
            }
            
            float limitFactor = 1;
            
            if (byPowerOf) {
                limitFactor = 1 - pow(((val - minimum) / (maximum - minimum)), 2);
            } else {
                limitFactor = 1 - ((val - minimum) / (maximum - minimum));
            }
            
            float dischargeLimit = fmax(0.0f, dischargeCurrent * limitFactor);

            if (dischargeLimit < minDischarge) {
                dischargeLimit = 0.0f;
            }
            
            if (limitedDischargeCurrent > dischargeLimit) {
                limitedDischargeCurrent = dischargeLimit;
                strcpy(limitedDischargeCurrentReason, "discharge limited by ");
                strcat(limitedDischargeCurrentReason, limitText);
                ESP_LOGI("main", "discharge limited by %s to %.1f", limitText, uint16_t(limitedDischargeCurrent * 10) / 10.0f);
                dischargeIsLimited = true;
            } 
        }
    }
    
    void limitDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText) {
        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);
    }

    void limitChargeAndDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText, bool byPowerOf) {
        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, byPowerOf);
        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, byPowerOf);
    }
    
    void limitChargeAndDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, const char* limitText) {
        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);
        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);
    }
    
    void setRampups(float step) {
        // CHARGING
        if (chargeIsLimited) {
            rampUpLimitedChargeCurrent = fmax(minCharge, rampUpLimitedChargeCurrent);
            if (limitedChargeCurrent > rampUpLimitedChargeCurrent) {
                strcpy(limitedChargeCurrentReason, "charge limited for ramp up by ");
                char str_f[20];
                sprintf(str_f, "%.1f", limitedChargeCurrent - rampUpLimitedChargeCurrent);
                strcat(limitedChargeCurrentReason, str_f);
                limitedChargeCurrent = rampUpLimitedChargeCurrent;
            } else {
                rampUpLimitedChargeCurrent = limitedChargeCurrent;
            }
            
            if (limitedChargeCurrent == chargeCurrent) {
                chargeIsLimited = false;
            }

            rampUpLimitedChargeCurrent += step;
        } else {
            rampUpLimitedChargeCurrent = limitedChargeCurrent;
        }

        // DISCHARGING
        if (dischargeIsLimited) {
            rampUpLimitedDischargeCurrent = fmax(minDischarge, rampUpLimitedDischargeCurrent);
            if (limitedDischargeCurrent > rampUpLimitedDischargeCurrent) {
                strcpy(limitedDischargeCurrentReason, "discharge limited for ramp up by ");
                char str_f[20];
                sprintf(str_f, "%.1f", limitedDischargeCurrent - rampUpLimitedDischargeCurrent);
                strcat(limitedDischargeCurrentReason, str_f);
                limitedDischargeCurrent = rampUpLimitedDischargeCurrent;
            } else {
                rampUpLimitedDischargeCurrent = limitedDischargeCurrent;
            }
            
            if (limitedDischargeCurrent == dischargeCurrent) {
                dischargeIsLimited = false;
            }

            rampUpLimitedDischargeCurrent += step;
        } else {
            rampUpLimitedDischargeCurrent = limitedDischargeCurrent;
        }
    }

//}
#endif	//__JK_BMS_HELPER__