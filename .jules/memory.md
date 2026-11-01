## Archived: 2026-10-31
- Investigated the NASA-MCOG repository, a control system made for NASA to drive a Ceramic Oxygen Generation technology.
- The project includes PlatformIO configuration for firmware, a state machine, a superloop architecture, and a hardware abstraction layer.
- Root directory contains basic documentation (README.md, RibbonFish.md) as well as subdirectories for firmware, web, docs, electronics (elec), and more.
- The serial port supports a custom command system based on JSON objects for parameter setting and event-like commands (e.g. warmup, cool down, emergency shutdown, acknowledge, idle, operate).
- Explored firmware platform setup instructions in README for both Linux and Windows.
