# bSwitchs Library
## Overview
A bSwitch is a specilized version of [bActuator](https://github.com/diy365-mgos/bactuator). It allows you to easily manage bActuators having just binary (true/false) state.
## Features
- **Verbose state** - Optionally, you can configure verbose states instead of default booleans (e.g: 'ON'/'OFF'). This feature is inherited form [bBinarySensors](https://github.com/diy365-mgos/bbsensor#features).
- **GPIO as actuators** - You can easily manage GPIO as actuators. Just include the [bThings GPIO library](https://github.com/diy365-mgos/bthing-gpio) in your project.
## Get Started in C/C++
Include the library in your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/diy365-mgos/bswitch
```
#### Example 1 - Toggle GPIO state
Create a bActuator for toggling the GPIO #2 state.

In addition, include this library in your mos.yml file.
```yaml
libs:
  - origin: https://github.com/diy365-mgos/bthing-gpio
```
```c
```
## C/C++ APIs Reference
### Inherited APIs
A bSwitch inherits inherits APIs from:
- [bThing](https://github.com/diy365-mgos/bthing)
- [bBinarySensor](https://github.com/diy365-mgos/bbsensor)
- [bABinaryctuator](https://github.com/diy365-mgos/bbactuator)
#### Remarks on (*mgos_bthing_set_state_handler_t)
The input state of the [*set-state* handler](https://github.com/diy365-mgos/bthing#mgos_bthing_set_state_handler_t) of a bSwitch is a boolean [bVariant](https://github.com/diy365-mgos/bvar).

Example
```c
static bool actu_set_state_handler(mgos_bthing_t thing, mgos_bvarc_t state, void *userdata) {
  bool is_true_or_false = mgos_bvar_get_bool(state);
  // ... update physical sw state
  return true;
}
```
### MGOS_BSWITCH_TYPE
```c
#define MGOS_BSWITCH_TYPE 1024 
```
The bSwitch type ID returned by [mgos_bthing_get_type()](https://github.com/diy365-mgos/bthing#mgos_bthing_get_type). It can be used with [mgos_bthing_is_typeof()](https://github.com/diy365-mgos/bthing#mgos_bthing_is_typeof).

Example:
```c
mgos_bactuator_t sw = mgos_bactuator_create(...);
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BSWITCH_TYPE))
  LOG(LL_INFO, ("I'm a bSwitch."));
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BACTUATOR_TYPE))
  LOG(LL_INFO, ("I'm a bActuator."));
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BTHING_TYPE_ACTUATOR))
  LOG(LL_INFO, ("I'm a bThing sw."));
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BBSENSOR_TYPE))
  LOG(LL_INFO, ("I'm a bBinarySensor."));
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BSENSOR_TYPE))
  LOG(LL_INFO, ("I'm a bSensor."));
if (mgos_bthing_is_typeof(MGOS_BACTUATOR_THINGCAST(sw), MGOS_BTHING_TYPE_SENSOR))
  LOG(LL_INFO, ("I'm a bThing sensor."));
```
Output console:
```bash
I'm a bSwitch.
I'm a bActuator.
I'm a bThing sw.
I'm a bBinarySensor.
I'm a bSensor.
I'm a bThing sensor.
```
### MGOS_BSWITCH_THINGCAST
```c
mgos_bthing_t MGOS_BSWITCH_THINGCAST(mgos_bswitch_t sw);
```
Casts a bSwitch to a generic bThing to be used with [inherited bThing APIs](https://github.com/diy365-mgos/bthing).

|Parameter||
|--|--|
|sw|A bSwitch.|

Example:
```c
mgos_bswitch_t sw = mgos_bswitch_create(...);
LOG(LL_INFO, ("%s successfully created.", mgos_bthing_get_id(MGOS_BSWITCH_THINGCAST(sw))));
```
### MGOS_BSWITCH_SENSCAST
```c
mgos_bbsensor_t MGOS_BSWITCH_SENSCAST(mgos_bswitch_t sw);
```
Casts a bSwitch to a bBinarySensor to be used with [inherited bBinarySensor APIs](https://github.com/diy365-mgos/bbsensor).

|Parameter||
|--|--|
|sw|A bSwitch.|

Example:
```c
mgos_bswitch_t sw = mgos_bswitch_create(...);
bool state;
if (mgos_bbsensor_get_state(MGOS_BSWITCH_SENSCAST(sw), &state))
  LOG(LL_INFO, ("The state is %s.", (state ? "TRUE" : "FALSE")));
```
### MGOS_BSWITCH_DOWNCAST
```c
mgos_bbactuator_t MGOS_BSWITCH_DOWNCAST(mgos_bswitch_t sw);
```
Casts a bSwitch to a bBinaryActuator to be used with [inherited bBinaryActuator APIs](https://github.com/diy365-mgos/bbactuator).

|Parameter||
|--|--|
|sw|A bSwitch.|
### mgos_bswitch_create
```c
mgos_bswitch_t mgos_bswitch_create(const char *id);
```
Creates a bSwitch. Returns `NULL` on error.

|Parameter||
|--|--|
|id|The bSwitch ID.|
### mgos_bswitch_set_state
```c
bool mgos_bswitch_set_state(mgos_bswitch_t sw, bool state);
```
Sets the binary state of a bSwitch. Returns `true` on success, or `false` otherwise. This is an helper function for [mgos_bthing_set_state()](https://github.com/diy365-mgos/bthing#mgos_bthing_set_state).

|Parameter||
|--|--|
|sw|A bSwitch.|
|state|The binary state to set.|
### mgos_bswitch_toggle_state
```c
bool mgos_bswitch_toggle_state(mgos_bswitch_t sw);
```
Toggles the binary state of a bSwitch. Returns `true` on success, or `false` otherwise.

|Parameter||
|--|--|
|sw|A bSwitch.|
## To Do
- Implement javascript APIs for [Mongoose OS MJS](https://github.com/mongoose-os-libs/mjs).