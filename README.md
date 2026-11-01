# PID Controller
A cruise-control PID controller and vehicle speed simulation written in C for UCD's COMP10060 module.

## Overview
This program simulates a car's cruise control. The user enters a start speed and a target speed. A PID controller then works out the acceleration needed to reach that speed and hold it, against simulated drag and rolling resistance. The program prints the car's speed and acceleration once per simulated second, so you can see how the controller responds.

## Features
- **PID control with Ziegler–Nichols tuning**: the gains come from the ultimate gain `Ku` and period `L` using the classic Z-N rules.
- **Integral anti-windup**: the integral term stops accumulating while the output is saturated, which prevents overshoot after long periods at full acceleration.
- **Realistic actuator limits**: acceleration is clamped between 0 and 8 km/h/s, so the controller can't brake below zero or accelerate harder than a real car.
- **Vehicle physics model**: speed-dependent aerodynamic drag plus constant rolling resistance.
- **Interactive**: change the target speed, reset the current speed, or exit, all from the terminal.
- **Input validation**: negative speeds are rejected, and targets above 190 km/h are capped (above that, drag is greater than the maximum acceleration).

## How it Works
The controller runs at a fixed time step of `dt = 0.1 s` (100 ms).

**Gains (Ziegler–Nichols PID):**

| Parameter | Formula | Value (`Ku = 20`, `L = 0.2`) |
|---|---|---|
| `Kp` | 0.6 · Ku | 12 |
| `Ti` | 0.5 · L | 0.1 |
| `Td` | 0.125 · L | 0.025 |

**Control law:**

```
error = target − speed
accel = Kp · (error + ∫error dt / Ti + Td · d(error)/dt)
```

The integral only accumulates when the output isn't saturated in the same direction as the error (conditional integration). After that, `accel` is clamped to `[0, 8]`.

**Plant model** (applied every time step):

```
speed = speed / (1 + drag · speed) − resistance + accel · dt
```

with `drag = 0.0002 · dt` and `resistance = 0.8 · dt`.

## Getting Started
Compile with any C compiler (GCC, Clang or MSVC):

```bash
gcc src/pid_controller.c -o pid_controller -lm
```

```bash
./pid_controller
```

## Usage
| Input at the target-speed prompt | Effect |
|---|---|
| Any number from 0 to 190 | Set a new target speed (km/h) |
| A number above 190 | Capped to 190 |
| `-2` | Enter a new current speed |
| `-1` | Exit |

## Example Output
Accelerating from 0 to 60 km/h. The controller holds maximum acceleration, then eases off without overshooting and settles at 60 km/h:

```
Input your start speed: 0
Input your new target speed: 60
 Time | Speed | Accel
 0.00 | 0.00 | 0.00
 1.00 | 7.20 | 8.00
 2.00 | 14.37 | 8.00
 ...
 8.00 | 55.92 | 8.00
 9.00 | 60.23 | 0.09
 10.00 | 59.97 | 1.78
 11.00 | 60.00 | 1.40
 13.00 | 60.00 | 1.54
 ...
 17.00 | 60.00 | 1.52
```

The steady-state acceleration of about 1.52 km/h/s is exactly what's needed to cancel out drag and rolling resistance at 60 km/h.

## Key Challenges
The main challenge was the car overshooting its target massively which was down to a simple logic error within the code, if(!((accel > 8 && error > 0) || (accel < 0 && error < 0))), requiring a change from an OR statement to an AND statement, despite the fact that it was a tiny bug it took the longest time to catch.

## Licence
MIT. See [LICENSE](LICENSE).
