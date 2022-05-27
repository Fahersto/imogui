![example workflow](https://github.com/fahersto/imogui/actions/workflows/cmake.yml/badge.svg)
# imogui - draw on overlays using imgui
A GUI library which hooks existing overlays to draw on them using imgui (https://github.com/ocornut/imgui). The project includes hookFTW (a hooking library) as a submodule which in turn includes zydis (a disassembler) as a submodule.

## Setting up
1. Clone including submodules:
```
git clone --recursive https://github.com/Fahersto/imogui.git
```
2. Build the library using CMAKE.

## Using imogui
You can create any imgui element or draw primitives using the passed imogui::Renderer inside the provided OnDraw callback:
```cpp
void OnDraw(imogui::Renderer* renderer)
```
Example implementations are provided in src/Examples. 

## Currently supported overlays
### 32 Bit
Overlay | 				OpenGL		|	 DirectX 9 					| DirectX 11 				| DirectX 12
--------| 				--------	| ---------- 					| -------------				| -------------
Steam   				| 	 :heavy_check_mark:    |		:heavy_check_mark:		|		:x:		 			|		:x:		 
Discord   				| 	 :heavy_check_mark:    |		:heavy_check_mark:		|		:x:		 			|		:x:		
Origin   				| 	 :x:    |		:x:	  					|		:x:		 			|		:x:		 
MSI Afterburner			| 	 :heavy_check_mark:    |		:x:	   					|		:x:		 			|		:x:		 
Overwolf   				| 	 :x:    |		:x:	   					|		:x:		 			|		:x:		 
GeForce Experience		| 	 :x:    |		:x:	   					|		:x:		 			|		:x:		
OBS						| 	 :heavy_check_mark:    |		:heavy_check_mark:	   	|		:x:		 			|		:x:		

### 64 Bit
Overlay | 				OpenGL		|	 DirectX 9 					| DirectX 11				| DirectX 12
--------| 				--------	| ---------- 					| -------------				| -------------
Steam   				| 	 :heavy_check_mark:    |		:x:	   |				:heavy_check_mark:			|		:x:			 
Discord   				| 	 :heavy_check_mark:    |		:x:	   |				:heavy_check_mark:			|		:x:	
Origin   				| 	 :x:    |		:x:	   |				:heavy_check_mark:			|		:x:			 
MSI Afterburner			| 	 :heavy_check_mark:    |		:x:	   |				:heavy_check_mark:			|		:x:			 	 
Overwolf   				| 	 :x:    |		:x:	   |				:x:							|		:x:		
GeForce Experience		| 	 :x:    |		:x:	   |				:x:		 					|		:x:		
OBS						| 	 :heavy_check_mark:    |		:x:	   |		:heavy_check_mark:		 			|		:x:		

*MSI Afterburner includes the Rivatuner Statistics Server which is the overlay hooked.

## Roadmap
- Add support for DirectX 12
- Add more overlays
