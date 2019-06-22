# Animator
This is an hand-drawn animation program made as personal project. The main technologies are C++, Qt5 and OpenGL 4.5, no other libraries have been used.

### Features
- **Basic animation:** A scrub-capable timeline, a play/pause button for reproduction @ 24fps and onion skinning allow for basic animation crafting.  
![Basic animation](https://raw.githubusercontent.com/giuliom95/animator/master/media/onion.gif)
- **Tablet support:** Drawing tablets with pen pressure sensitivity are fully supported.  
![Tablet support](https://raw.githubusercontent.com/giuliom95/animator/master/media/tablet.png)
- **Smooth strokes:** Each stroke is smoothened through 3rd grade hermite interpolation using tesselation on the GPU.  
![Smooth strokes](https://raw.githubusercontent.com/giuliom95/animator/master/media/smooth.png)

### Issues
This is a personal and educational project. I will probably neither implement nor fix what is listed here.
- **Basic functionalities are lacking:** No save capabilities, upper and lower bounds of the animation are fixed, no undo and the only way to create a new animation is to restart the program.
- **Not tested:** It works fine on my system (Linux, Readeon GPU) but I know for sure it has problems on non Radeon GPUs. Also it has been tested on Windows but the tablet may act strangely sometimes due to Windows Ink Workspace.
