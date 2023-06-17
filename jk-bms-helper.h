#ifndef __JK_BMS_HELPER__

#define __JK_BMS_HELPER__



#include "esphome.h"



//namespace JkBmsHelper {



    float chargeCurrent = 0;

    float dischargeCurrent = 0;

    

    float limitedChargeCurrent = 0;

    float limitedDischargeCurrent = 0;

    String limitedChargeCurrentReason = "";

    String limitedDischargeCurrentReason = "";

    

    void initValues(float initChargeCurrent, float initDischargeCurrent) {

        chargeCurrent = initChargeCurrent;

        dischargeCurrent = initDischargeCurrent;

        

        limitedChargeCurrent = chargeCurrent;

        limitedDischargeCurrent = dischargeCurrent;

        

        limitedChargeCurrentReason = "";

        limitedDischargeCurrentReason = "";

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

    

    void limitChargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText, boolean byPowerOf) {

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



            if (limitedChargeCurrent > chargeLimit) {

                limitedChargeCurrent = chargeLimit;

                limitedChargeCurrentReason = "charge limited by " || limitText;

                ESP_LOGI("main", "charge limited by %s to %.1f", limitText, uint16_t(limitedChargeCurrent * 10) / 10.0f);

                

            }

        }

    }

    

    void limitChargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText) {

        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);

    }



    void limitDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText, boolean byPowerOf) {

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

            

            if (limitedDischargeCurrent > dischargeLimit) {

                limitedDischargeCurrent = dischargeLimit;

                limitedDischargeCurrentReason = "discharge limited by " || limitText;

                ESP_LOGI("main", "discharge limited by %s to %.1f", limitText, uint16_t(limitedDischargeCurrent * 10) / 10.0f);

            } 

        }

    }

    

    void limitDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText) {

        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);

    }



    void limitChargeAndDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText, boolean byPowerOf) {

        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, byPowerOf);

        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, byPowerOf);

    }

    

    void limitChargeAndDischargeCurrent(float val, float minimum, float maximum, float bufferFromMax, String limitText) {

        limitChargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);

        limitDischargeCurrent(val, minimum, maximum, bufferFromMax, limitText, false);

    }

//}



#endif	//__JK_BMS_HELPER__