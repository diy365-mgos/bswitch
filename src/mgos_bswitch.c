#include "mgos.h"
#include "mgos_bthing.h"
#include "mg_bswitch_sdk.h"
#include "mgos_bswitch.h"

#ifdef MGOS_HAVE_MJS
#include "mjs.h"
#endif

static int s_inching_timer_id = MGOS_INVALID_TIMER_ID;
static int s_inching_count = 0;

mgos_bthing_t MGOS_BSWITCH_THINGCAST(mgos_bswitch_t sw) {
  return MG_BTHING_SENS_CAST4(MG_BSWITCH_CAST1(sw));
}

mgos_bsensor_t MGOS_BSWITCH_SENSCAST(mgos_bswitch_t sw) {
  return (mgos_bsensor_t)MG_BSWITCH_CAST1(sw);
}

mgos_bbactuator_t MGOS_BSWITCH_DOWNCAST(mgos_bswitch_t sw) {
  return (mgos_bbactuator_t)sw;
}

mgos_bswitch_t mgos_bswitch_create(const char *id, int group_id, int switching_time, const char *domain) {
  mgos_bswitch_t MG_BSWITCH_NEW(sw);
  if (mg_bthing_init(MG_BTHING_ACTU_CAST4(sw), id, MGOS_BSWITCH_TYPE, domain)) {
    struct mg_bswitch_cfg *sw_cfg = calloc(1, sizeof(struct mg_bswitch_cfg));
    struct mg_bbsensor_cfg *sens_cfg = calloc(1, sizeof(struct mg_bbsensor_cfg));
    if (sens_cfg && sw_cfg) {
      mgos_bthing_t thing = MGOS_BSWITCH_THINGCAST(sw);
      if (mg_bswitch_init(sw, group_id, switching_time, sw_cfg, sens_cfg) && mg_bthing_register(thing)) {
        LOG(LL_INFO, ("bSwitch '%s' successfully created.", mgos_bthing_get_uid(thing)));
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

static void mg_bswitch_inching_cb(void *arg) {
  int64_t now = mgos_uptime_micros();

  mgos_bthing_t thing;
  mgos_bthing_enum_t things = mgos_bthing_get_all();
  while (mgos_bthing_filter_get_next(&things, &thing, MGOS_BTHING_FILTER_BY_TYPE, MGOS_BSWITCH_TYPE)) {
    
    LOG(LL_INFO, ("POLLING: '%s'", mgos_bthing_get_uid(thing))); // CANCEL
    struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG((mgos_bswitch_t)thing);
    LOG(LL_INFO, ("inching_timeout=%d / inching_start=%ld)", cfg->inching_timeout, (long)cfg->inching_start)); // CANCEL
    if (cfg->inching_timeout != MGOS_BSWITCH_NO_INCHING && cfg->inching_start > 0) {
      if ((now - cfg->inching_start) > (cfg->inching_timeout * 1000)) {
        // stop inching
        cfg->inching_start = 0;
        // switch OFF the switch
        LOG(LL_INFO, ("AUTO SWITCH OFF (by inching)")); // CANCEL
        mgos_bbactuator_set_state(MGOS_BSWITCH_DOWNCAST((mgos_bswitch_t)thing), false);
      } else {
        LOG(LL_INFO, ("NOT EXPIRED")); // CANCEL
      }
    }
  }
  (void) arg;
}

bool mgos_bswitch_set_inching(mgos_bswitch_t sw, int timeout, bool lock) {
  if (sw && (timeout > 0 || timeout == MGOS_BSWITCH_NO_INCHING)) {

    if (timeout > 0) {
      if (s_inching_timer_id == MGOS_INVALID_TIMER_ID) {
        // initialize the inching global timer
        s_inching_timer_id = mgos_set_timer(500, MGOS_TIMER_REPEAT, mg_bswitch_inching_cb, NULL);
        if (s_inching_timer_id == MGOS_INVALID_TIMER_ID) {
          LOG(LL_ERROR, ("Unable to start the internal inching timer for bSwitches.'"));
          return false;
        }
      }
      ++s_inching_count;
    } else {
      // timeout == MGOS_BSWITCH_NO_INCHING
      --s_inching_count;
      if (s_inching_count == 0 && s_inching_timer_id != MGOS_INVALID_TIMER_ID) {
        mgos_clear_timer(s_inching_timer_id);
      }
    }

    struct mg_bswitch_cfg *cfg = MG_BSWITCH_CFG(sw);
    cfg->inching_timeout = timeout;
    cfg->inching_lock = (timeout == MGOS_BSWITCH_NO_INCHING ? false : lock);
    LOG(LL_INFO, ("inching_timeout = %d", cfg->inching_timeout)); // CANCEL
    return true;
  }
  return false;
}


bool mgos_bswitch_init() {
  return true;
}