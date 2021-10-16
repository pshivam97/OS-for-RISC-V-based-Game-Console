OS Project Final Phase
Shivam Pandey (918972220)

"Behavior of my OS (Firmware) and Application (Cartridge)" :-

1. Application program is run when the cartridge is loaded into the console. 
2. Multiple APIs have been implemented and demonstrated as “system calls” in the firmware (OS kernel codespace). 
3. System call APIs will be used to map user input with the game process, and these APIs will provide abstractions to the game developers to interface with the video graphics. Please note that these APIs are implemented as Syscalls. 
4. A specific number is assigned for a specific type of system call and is passed as the first element in the parameter array. 
5. Note that the Cartridge/Application program DOES NOT DIRECTLY INTERACT with the Memory/Registers at all. All the interactions happen via the API Syscalls which are implemented in the firmware (OS). 
6. When the program is initially run, a message “Hello World!” is displayed along with the game pointer “X”, which can be moved using the multi-button controller. 
7. When ‘u’ is pressed, the mode switches from TEXT to GRAPHICS, and a background is set along with two small sprites.

8. When ‘i’ is pressed, the mode switches from GRAPHICS to TEXT, and a message “Welcome Back!” is displayed along with the game pointer “X”.

9. When the CMD button is pressed, a Command interrupt is raised due to which a “PAUSED” message is displayed.

10. Whenever the Video Interrupt occurs, a blinking (Colour changing) Large Sprite is displayed in the Graphics Mode. 