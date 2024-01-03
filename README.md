# Another GameBoy Emulator
AGBE is a simple work-in-progress emulator for the GameBoy game system. (Most specifically emulating the DMG black and white model).\
This is a personal project that I started in the process of learning more about C programming, low level programming, assembly and video game system emulation in general. As such, it is not aimed to be an alternative of already existing GameBoy emulators and is only targeting a moderate level of accuracy (at best M-cycle level).

The emulator is currently not fonctionnal in most games but it has the building blocks for the CPU, PPU and input implementation.

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
- [SDL2](https://www.libsdl.org/) for rendering
