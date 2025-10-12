HAL 9000 Raspberry Pi Emulator
==============================

This project contains a playful HAL 9000 style emulator tailored for the
Raspberry Pi 3B+ running Raspbian (or any Linux environment with a standard C
compiler).  The program provides an interactive command shell that responds with
familiar HAL phrases, mission status updates, diagnostic information, and
platform-aware telemetry for the Pi-based mission.

Features
--------
* Boot sequence that introduces the HAL 9000 neural heuristics node assigned to
  the Raspberry Pi operations mission.
* Text-driven command interface including help, system status, diagnostics, and
  pod bay door control (with appropriate HAL attitude).
* Rolling event log so you can review recent conversations with the computer.
* Platform command that reports Raspberry Pi hardware, kernel, and OS
  information.
* Natural-language triggers for iconic requests such as "open the pod bay
  doors" and "sing Daisy".

Build Instructions
------------------

Ensure you have a C11-capable compiler available (for example, `gcc` on
Raspbian) and standard build tools such as `make`.

Use `make hal9000` to build the application.

Run Instructions
----------------

The resulting program is a standard Linux binary.  Run it directly from a
terminal session on the Raspberry Pi:

```
./hal9000
```

Once HAL boots, type commands such as `status`, `mission`, `platform`,
`override`, or the infamous `open the pod bay doors` and enjoy the conversation.
