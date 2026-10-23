# Licensing

This repository mixes several kinds of content, and each kind has its own license.

## 1. Protocol header and example code — MIT

Files: `docs/protocol/bb_protocol.h`, everything under `examples/`.

```
MIT License

Copyright (c) 2026 Darius Vaitiekus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 2. Documentation and mechanical files — CC BY 4.0

Files: all documentation (`*.md`, except this file), `mechanical/` (STEP and STL files). Logos are excluded, see section 4.

Licensed under the [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/).
You may share and adapt them for any purpose, provided you give credit to "VILKAS by Darius Vaitiekus" and link to this repository.

## 3. Official firmware images — proprietary, free redistribution

Files: everything under `firmware/`.

Copyright (c) 2026 Darius Vaitiekus. All rights reserved.

- You may download, flash and use these images on any hardware, free of charge.
- You may redistribute the images **unmodified**, together with their checksums.
- You may not modify, decompile or disassemble the images, or redistribute modified images as official VILKAS firmware.
- The images are provided "as is", without warranty of any kind. Combat robots are dangerous. You're responsible for safe operation, including failsafe testing before every event.

## 4. Logos

Files: `assets/logo/`.

The VILKAS logos and the DV Electronics logo are **not** covered by CC BY or MIT. All rights reserved. You may use them unmodified to refer to VILKAS (e.g. in articles, event pages or shop listings for official hardware), but not as the logo of your own product.

## 5. Name

"VILKAS", "VILKAS-TX" and "VILKAS-RX" identify the official hardware and firmware. Compatible third-party products may say they are "VILKAS-compatible", but they should not use the VILKAS name as their own product name.

## Not covered (closed)

PCB design files (KiCad, Gerbers, BOM, CPL), the enclosure source CAD, the full firmware source code and the programmer unit are not part of this repository and are not licensed.
