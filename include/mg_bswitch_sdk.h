/*
 * Copyright (c) 2021 DIY356
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MG_BSWITCH_SDK_H_
#define MG_BSWITCH_SDK_H_

#include <stdbool.h>
#include "mg_bthing_sdk.h"
#include "mg_bbactuator_sdk.h"
#include "mgos_bswitch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MG_BSWITCH_NEW(sw) MG_BTHING_ACTU_NEW(sw);

/*****************************************
 * Cast Functions
 */

// Convert (mgos_bswitch_t) into (struct mg_bthing_sens *)
struct mg_bthing_sens *MG_BSWITCH_CAST1(mgos_bswitch_t thing);

// Convert (mgos_bswitch_t) into (struct mg_bthing_actu *)
struct mg_bthing_actu *MG_BSWITCH_CAST2(mgos_bswitch_t thing);
/*****************************************/

#define MG_BSWITCH_CFG(sw) ((struct mg_bswitch_cfg *)MG_BSWITCH_CAST2(sw)->cfg)

struct mg_bswitch_overrides {
  mg_bthing_setting_state_handler_t setting_state_cb;
};

struct mg_bswitch_cfg {
  struct mg_bbactuator_cfg base;
  struct mg_bswitch_overrides overrides;
  int group_id;
  int switching_time;
  int inching_timeout;
  bool inching_lock;
  int64_t inching_start;
};

bool mg_bswitch_init(mgos_bswitch_t sw,
                     int group_id, int switching_time,
                     struct mg_bswitch_cfg *sw_cfg,
                     struct mg_bbinsens_cfg *sens_cfg);

void mg_bswitch_reset(mgos_bswitch_t sw);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_BSWITCH_SDK_H_ */