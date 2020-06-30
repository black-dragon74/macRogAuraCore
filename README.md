# macRogAuraCore - rogauracore ported from Linux.

(c) 2020 black.dragon74 aka Nick

macOS-compatible IOHIDKit implementation similar to the ROG
Aura Core software.  Supports RGB keyboards with IDs
[0b05:1854](https://linux-hardware.org/index.php?id=usb:0b05-1854)
(GL553, GL753),
[0b05:1869](https://linux-hardware.org/index.php?id=usb:0b05-1869)
(GL503, FX503, GL703) and [0b05:1866](https://linux-hardware.org/index.php?id=usb:0b05-1866) (GL504, GL703, GX501, GM501).

## Usage

```
Usage:
   macRogAuraCore COMMAND ARGUMENTS

COMMAND should be one of:
   initialize_keyboard
   brightness
   on
   off
   single_static
   single_breathing
   single_colorcycle
   multi_static
   multi_breathing
   red
   green
   blue
   yellow
   gold
   cyan
   magenta
   white
   black
   rainbow
```

## Building

### From github:

Install Xcode, clone the github repo and enter the top-level directory.  Then:

```sh
xcodebuild -configuration Release
```

## Credits

- Will Roberts for original `rogauracore` for Linux
