# 3Axes_Ctrl_USB

This project uses BTT RUMBA32_v1_0E 3D printer board (based around 180MHz STM32F446VET6) as improvised 3-axes Stepper Motor Controller with USB interface.

![BTT Rumba32_v1.0](https://github.com/DZeyfert/3Axes_Ctrl_USB/blob/main/PICs/BTT_Rumba32_v1.0.jpg)

### Limitations:

For stepper motors (especially open loop) speed should be ramped up and down. Here it is left to the concience of a PC software programmer: the "3Axes_Ctrl_USB" just creates certain number of steps per certain amount of time.

### Control protocol (over USB VCP):
```git
[Interval_ms];[Axes_Num];[Axis1_Steps];[Axis2_Steps];[Axis3_Steps];\n
```
|Parameter|Length|Range|
| --- | --- | --- |
| [Interval_ms] | 3 | 20..999 |
| [Axes_Num] | 1 | 1..3 |
| [Axis<N>_Steps] | 4 | -999..+999 |



Example of proper command:

```git
020;3;+000;-013;+060;\n
```

This translates in: "Use only 3 Axes; in the next 20ms put Axis1 on hold, step Axis2 13 times in one direction, step Axis3 60 times the other direction"

When duration of the time interval is over and all requested steps are done, all axes automatically put themselves on hold.
If the next command arrives earlier than the previous was completed, new one overrides the old one. For example, you can implement emergency stop like this:

```git
020;3;+000;+000;+000;\n
```

### Q & A

>Q: Why support only 3 axes if the board in question has capability to drive up to 6 motors?

A: Because Rumba board was repurposed from antoher project, where axes X and Z fought honorably but were in the end defeated by previous user. And I only had 3 motors. If you want more axes, just change define "AXES_NUM" to the new value. Code is written in a way that everything will scale up (or down) automatically (preprocessor magic), but you have to implement corresponding low-level initialization code if you want to use axes X, Z and E1 (for further explanations check "settings.h" in "MCU Firmware/Code").

>Q: Why don't you use Marlin or smth and control it via serial?

A: What's the fun in that? :grin:
