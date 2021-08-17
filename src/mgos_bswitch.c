#include "mgos.h"
#include "mgos_bthing.h"
#include "mg_bswitch_sdk.h"
#include "mgos_bswitch.h"

#ifdef MGOS_HAVE_MJS
#include "mjs.h"
#endif

mgos_bthing_t MGOS_BSWITCH_THINGCAST(mgos_bswitch_t sw) {
  return MG_BTHING_SENS_CAST4(MG_BSWITCH_CAST1(sw));
}

mgos_bsensor_t MGOS_BSWITCH_SENSCAST(mgos_bswitch_t sw) {
  return (mgos_bsensor_t)MG_BSWITCH_CAST1(sw);
}

mgos_bbactuator_t MGOS_BSWITCH_DOWNCAST(mgos_bswitch_t sw) {
  return (mgos_bbactuator_t)sw;
}

mgos_bswitch_t mgos_bswitch_create(const char *id, int group_id, int switching_time) {
  mgos_bswitch_t MG_BSWITCH_NEW(sw);
  if (mg_bthing_init(MG_BTHING_ACTU_CAST4(sw), id, MGOS_BSWITCH_TYPE)) {
    struct mg_bswitch_cfg *sw_cfg = calloc(1, sizeof(struct mg_bswitch_cfg));
    struct mg_bbsensor_cfg *sens_cfg = calloc(1, sizeof(struct mg_bbsensor_cfg));
    if (sens_cfg && sw_cfg) {
      if (mg_bswitch_init(sw, group_id, switching_time, sw_cfg, sens_cfg) &&
          mg_bthing_register(MGOS_BSWITCH_THINGCAST(sw))) {
        LOG(LL_INFO, ("bSwitch '%s' successfully created.", id));
        return sw;
      }
    } else {
      LOG(LL_ERROR, ("Unable to allocate memory for 'mg_bbsensor_cfg' and/or 'mg_bswitch_cfg'"));
    }
    free(sens_cfg);
    free(sw_cfg);
    mg_bthing_reset(MG_BTHING_ACTU_CAST4(sw));
  }
  free(sw);
  LOG(LL_ERROR, ("Error creating bSwitch '%s'. See above error message for more details.'", (id ? id : "")));
  return NULL; 
}

bool mgos_bswitch_set_inching(mgos_bswitch_t sw, int timeout, bool lock) {
  if (sw && (timeout > 0 || timeout == MGOS_BSWITCH_NO_INCHING)) {
    struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG(sw);
    cfg->inching_timeout = timeout;
    cfg->inching_lock = (timeout == MGOS_BSWITCH_NO_INCHING ? false : lock);
    return true;
  }
  return false;
}

static void mg_bswitch_inching_cb(void *arg) {
  int64_t now = mgos_uptime_micros();

  mgos_bthing_t thing;
  mgos_bthing_enum_t things = mgos_bthing_get_all();
  while (mgos_bthing_filter_get_next(&things, &thing, MGOS_BTHING_FILTER_BY_TYPE, MGOS_BSWITCH_TYPE)) {
    struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG((mgos_bswitch_t)thing);
    if (cfg->inching_timeout != MGOS_BSWITCH_NO_INCHING && cfg->inching_start > 0) {
      if ((now - cfg->inching_start) > (cfg->inching_timeout * 1000)) {
        // stop inching
        cfg->inching_start = 0;
        // switch OFF the switch
        mgos_bbactuator_set_state(MGOS_BSWITCH_DOWNCAST((mgos_bswitch_t)thing), false);
      }
    }
  }
  (void) arg;
}

bool mgos_bswitch_init() {
  // initialize the polling global timer
  if (mgos_set_timer(10, MGOS_TIMER_REPEAT, mg_bswitch_inching_cb, NULL) == MGOS_INVALID_TIMER_ID) {
    LOG(LL_ERROR, ("Unable to start the internal inching timer for bSwitches.'"));
    return false;
  }
  return true;
}