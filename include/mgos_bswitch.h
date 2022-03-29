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

#ifndef MGOS_BSWITCH_H_
#define MGOS_BSWITCH_H_

#include <stdbool.h>
#include "mgos_bthing.h"
#include "mgos_bswitch.h"
#include "mgos_bbinactu.h"
#include "mgos_bbinsens.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_BSWITCH_NO_GROUP 0
#define MGOS_BSWITCH_NO_INCHING 0
#define MGOS_BSWITCH_DEFAULT_SWITCHING_TIME 10 //milliseconds

struct mg_bthing_actu;
typedef struct mg_bthing_actu *mgos_bswitch_t;

mgos_bthing_t MGOS_BSWITCH_THINGCAST(mgos_bswitch_t sw);

mgos_bbinsens_t MGOS_BSWITCH_SENSCAST(mgos_bswitch_t sw);

mgos_bbinactu_t MGOS_BSWITCH_DOWNCAST(mgos_bswitch_t sw);

mgos_bswitch_t mgos_bswitch_create(const char *id, int group_id, int switching_time, const char *domain);

bool mgos_bswitch_set_inching(mgos_bswitch_t sw, int timeout, bool lock);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_BSWITCH_H_ */