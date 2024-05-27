# Another GameBoy Emulator
AGBE is a simple work-in-progress emulator for the GameBoy game system. (Most specifically emulating the DMG black and white model).

This is a personal project that I started in the process of learning more about C programming, low level programming, assembly and video game system emulation in general. As such, it is not aimed to be an alternative for already existing GameBoy emulators and is only targeting a moderate level of accuracy (at best M-cycle level).

Currently the following is implemented:
- Opcodes intepreter and full CPU emulation
- PPU using pixel FIFOs
- Basic keyboard input support (B: Z key, A: X key, Space: Select, Enter: Start, Arrow keys for D-Pad)
- Support for no mapper and MBC1 cartridges, including exporting external RAM in a .sav file if the mapper has a battery

## Credits
### Documentation
The following are the documentation I used for this project:
- [Pandocs](https://gbdev.io/pandocs/)
- [The Cycle-Accurate Game Boy Docs](https://github.com/AntonioND/giibiiadvance/blob/master/docs/TCAGBD.pdf)
- [Gekkio's Complete Technical Reference](https://gekkio.fi/files/gb-docs/gbctr.pdf)
- [The Ultimate Game Boy Talk](https://www.youtube.com/watch?v=HyzD8pNlpwI)

### Frameworks
I used the following technologies and frameworks to make AGBE:
- C language
- [SDL2](https://www.libsdl.org/) for rendering, input and audio
