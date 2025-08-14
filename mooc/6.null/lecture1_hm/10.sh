#!/bin/bash
#https://www.baeldung.com/linux/cpu-temperature
paste <(cat /sys/class/thermal/thermal_zone*/type) <(cat /sys/class/thermal/thermal_zone*/temp) | column -s
