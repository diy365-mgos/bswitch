#include "mgos.h"
#include "mg_bswitch_sdk.h"


/*****************************************
 * Cast Functions
 */

// Convert (mgos_bswitch_t) into (struct mg_bthing_sens *)
struct mg_bthing_sens *MG_BSWITCH_CAST1(mgos_bswitch_t thing) {
  return (struct mg_bthing_sens *)thing;
}

// Convert (mgos_bswitch_t) into (struct mg_bthing_actu *)
struct mg_bthing_actu *MG_BSWITCH_CAST2(mgos_bswitch_t thing) {
  return (struct mg_bthing_actu *)thing;
}
/*****************************************/

bool mg_bswitch_del_inching(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg) {
  if (cfg->inching_start > 0 && cfg->inching_lock == true) {
    LOG(LL_ERROR, ("Inching of '%s' is locked and cannot be turned off.", mgos_bthing_get_id(MGOS_BSWITCH_THINGCAST(sw))));
    return false;
  }
  cfg->inching_start = 0;
  return true;
}

bool mg_bswitch_pre_set_state_on(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg) {
  /* check if some other switch having inching_lock is still ON */
  mgos_bthing_t thing;
  mgos_bthing_enum_t things = mgos_bthing_get_all();
  while (mgos_bthing_typeof_get_next(&things, &thing, MGOS_BSWITCH_TYPE)) {
    struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG((mgos_bswitch_t)thing);
    if (cfg->inching_lock == true && cfg->inching_start > 0) {
      LOG(LL_ERROR, ("Error switching '%s' ON. '%s' has inching-lock and it is still ON.",
        mgos_bthing_get_id(MGOS_BSWITCH_THINGCAST(sw)), mgos_bthing_get_id(thing)));
      return false;
    }
  }

  /* remove inching if configured and in progress */
  if (!mg_bswitch_del_inching(sw, cfg)) {
    LOG(LL_ERROR, ("Error switching '%s' ON becuase failure on turning off inching.",
      mgos_bthing_get_id(MGOS_BSWITCH_THINGCAST(sw))));
    return false;
  }

  return true;
}

bool mg_bswitch_pre_set_state_off(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg) {
   /* remove inching if configured and in progress */
  return mg_bswitch_del_inching(sw, cfg);
}

bool mg_bswitch_pre_set_state(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg, bool state) {
  if (state) {
    return mg_bswitch_pre_set_state_on(sw, cfg);
  } else {
    return mg_bswitch_pre_set_state_off(sw, cfg);
  }
}

void mg_bswitch_post_set_state_on(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg) {
  /* start inching count-down */
  if (cfg->inching_timeout != MGOS_BSWITCH_NO_INCHING) {
    cfg->inching_start = mgos_uptime_micros();
  }
}

void mg_bswitch_post_set_state_off(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg) {
  // nothing to do
}

void mg_bswitch_post_set_state(mgos_bswitch_t sw, struct mg_bswitch_cfg *cfg, bool state) {
  /* wait for switching-time if needed */
  if (cfg->switching_time > 0) {
    mgos_msleep(cfg->switching_time);
  }

  if (state) {
    mg_bswitch_post_set_state_on(sw, cfg);
  } else {
    mg_bswitch_post_set_state_off(sw, cfg);
  }
}

enum MG_BTHING_STATE_RESULT mg_bswitch_setting_state_cb(struct mg_bthing_actu *sw,
                                                        mgos_bvarc_t state,
                                                        void *userdata) {
  if (sw && state) {
    bool bool_state;
    if (mg_bbsensor_state_to_bool(MGOS_BBACTUATOR_SENSCAST(MGOS_BSWITCH_DOWNCAST(sw)), state, &bool_state)) {
      struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG(sw);
      if (mg_bswitch_pre_set_state((mgos_bswitch_t )sw, cfg, bool_state)) {

        enum MG_BTHING_STATE_RESULT ret = cfg->overrides.setting_state_cb(sw, state, userdata);
        if (ret != MG_BTHING_STATE_RESULT_ERROR) {
          mg_bswitch_post_set_state((mgos_bswitch_t )sw, cfg, bool_state);
          return ret;
        }
      }
    }
  }
  return MG_BTHING_STATE_RESULT_ERROR;
}

bool mg_bswitch_init(mgos_bswitch_t sw,
                     int group_id, int switching_time,
                     struct mg_bswitch_cfg *sw_cfg,
                     struct mg_bbsensor_cfg *sens_cfg) {
  if (sw_cfg) {
    // init actuator-base obj
    if (mg_bbactuator_init(sw, &sw_cfg->base, sens_cfg)) {
      /* initalize general settings */
      sw_cfg->group_id = (group_id == -1 ? MGOS_BSWITCH_NO_GROUP : group_id);
      sw_cfg->switching_time = (switching_time == -1 ? MGOS_BSWITCH_DEFAULT_SWITCHING_TIME : switching_time);
      /* initalize inching settings */
      sw_cfg->inching_timeout = MGOS_BSWITCH_NO_INCHING;
      sw_cfg->inching_lock = false;
      sw_cfg->inching_start = 0;
      /* initalize overrides cfg */
      sw_cfg->overrides.setting_state_cb = mg_bthing_on_setting_state(MG_BSWITCH_CAST2(sw), mg_bswitch_setting_state_cb);
     
      return true; // initialization successfully completed
    }
    mg_bswitch_reset(sw);
  } else {
    LOG(LL_ERROR, ("Invalid NULL 'sw_cfg' parameter."));
  }

  LOG(LL_ERROR, ("Error initializing bSwitch '%s'. See above error message for more details.",
    mgos_bthing_get_id(MGOS_BSWITCH_THINGCAST(sw))));
  return false; 
}

void mg_bswitch_reset(mgos_bswitch_t sw) {
  struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG(sw);

  /* clear overrides cfg */
  if (cfg->overrides.setting_state_cb) {
    mg_bthing_on_setting_state(sw, cfg->overrides.setting_state_cb);
    cfg->overrides.setting_state_cb = NULL;
  }
  /* clear general settings */
  cfg->group_id =  MGOS_BSWITCH_NO_GROUP;
  cfg->switching_time = MGOS_BSWITCH_DEFAULT_SWITCHING_TIME;
  /* clear inching settings */
  cfg->inching_timeout = MGOS_BSWITCH_NO_INCHING;
  cfg->inching_lock = false;
  cfg->inching_start = 0;

  // reset actuator-base obj
  mg_bbactuator_reset(sw);
}
