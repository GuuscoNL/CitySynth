# City Synth

City Synth is a tool made in C++ using the raylib library where you can generate procedural cities based on a population heatmap. The generated cities can be exported to a JSON file and imported into Blender, Unity, Unreal Engine, ect.
<br><br>
This project was created for my IPASS (Individual Propedeuse Assessment)

## Usage
The tool is mainly created for game developers or artist that need a quick city layout for there game or scene. However this tool can also be used for other means, for example a piping system with a heatmap that is based on real life data or for irrigation perhaps.

## Installation

To use City Synth you don't need to install anything, you can go to the latest release and get the executable. Be aware that the executable is only for windows and be sure leave the shaders folder in the same directory as the executable.
<br><br>
However if you want to build from source you need to follow these steps first:
1. You need to have VS code installed, you can download it [here](https://code.visualstudio.com/download)
2. Open VS code and download the C/C++ extension.
3. Go to the [raylib library website](https://www.raylib.com/) and click on dowload. 
4. If you don't want to donate to the raylib library you can just click on "No thanks, just take me to the downloads"
5. Be sure to download "raylib 4.5 Windows Installer 64bit (MinGW compiler)"
6. Follow the instructions of the installer.
7. Now you can clone this repo and press F5 to build and run City Synth. The exe will have the same name as the file you have currently open.  When building you can ignore the warnings generated bu rayGUI.
8. I recommend building the release version, since it is more than 4x faster. You can do this by opening the command pallet (ctrl+shift+P) and run the command `Task: Run Task` and select `build release`. This will build the exe, but won't automatically run it.
  
The guide above is based on this [video](https://youtu.be/PaAcVk5jUd8) made by Programming With Nick. You only need to follow the steps before 2:04. The rest is already done.

## How to use

Hold down right mouse button or L-shift to start moving the camera. You can move using WASD and you can zoom in and out by scrolling the mouse wheel.
When you open the tool you will see in the top right corner settings for generating cities. Some are self explanatory, but here are some of the settings with more explanation:
- **Highway**
  - **Max angle:** The maximum angle the Highway can deviate. (0-180)
  - **Sample amount:** The amount of samples the Highway uses to see where the highest pop is. (0-100)
  - **Branch chance:** The probability of the Highway branching at a 90-degree angle to another Highway. (0-100)
  - **SideRoad chance:** The probability of the Highway branching at a 90-degree angle to a SideRoad. (0-100)
- **SideRoad**
  - **Branch chance:** The probability of the SideRoad branching at a 90-degree angle to another SideRoad. (0-100)
  - **Pop threshold:** The minimum amount of population that is needed for a SideRoad to be accepted. (0-1)
- **General**
  - **Seed:** The Seed for the random values. (0-999999)
  - **Segment limit:** The amount of RoadSegments to create. (Recommended below 15000 due to generation time) (0-30000)
  - **Size:** The size of the city, must be even. After unfocused it will resize the city, but it won't regenerate the city. (1-2000)
  - **City name:** The name of the city. This will be used as the name of the JSON file when the city is exported.
- **Heatmap**
  - **Frequency:** How frequent the black patches are. Use small increments. Bigger = smaller and more frequent patches.
  - **Amplitude:** Haven't found out what this exactly does yet. :sweat_smile:
  - **Lucanrity:** How "dense" the patches are? (I recommend experimenting with different values)
  - **Octaves:** 1 means smooth, 2 means rough.
  - **File name:** Input a file name here including the extension to import as the heatmap. The image must be in the same directory as the executable. It will automatically resize the imageA to fit the size of the city. Press Import to apply the image. This will not regenerate the City automatically.
- **Controls**
  - **Reset City:** Will reset the city.
  - **Export City:** Will export a city to JSON format with the city name as the name of the file. The file will be made in the same directory as the executable.
  - **Generate City:** Generates the city based on all the settings.

## Export example
Here is an example of a small city exported to a JSON file:
```json
{
	"highways": [
		{ "from": 0, "to": 1 },
		{ "from": 0, "to": 2 },
		{ "from": 1, "to": 3 },
		{ "from": 2, "to": 5 }
	],
	"nodes": {
		"0": [0.0, 0.0],
		"1": [1.399999976158142, 1.399999976158142],
		"2": [-1.399999976158142, -1.399999976158142],
		"3": [2.792330741882324, 2.792330741882324],
		"4": [1.399999976158142, 1.399999976158142],
		"5": [-2.764118194580078, -2.764118194580078],
		"6": [-1.399999976158142, -1.399999976158142]
	},
	"settings": {
		"highwayHeight": 0.04500000178813934,
		"highwayLength": 1.399999976158142,
		"highwayWidth": 0.25,
		"sideRoadHeight": 0.03999999910593033,
		"sideRoadLength": 0.699999988079071,
		"sideRoadWidth": 0.10000000149011612
	},
	"sideroads": [
		{ "from": 2, "to": 6 },
		{ "from": 1, "to": 4 }
	]
}

```
- `"highways"`: An array of all the Highways. Each Highway object has a "from" property and a "to" property, these are the node ID's of the starting and ending nodes of the Highway. 
- `"nodes"`: key-values pairs. The key is the node ID and the values are the X and Y of the node.
- `"settings"`: Some of the settings used to generate this city, these might come in handy when regenerating the city.
- `"sideroads"`: An array of all the SideRoads. Each SideRoad object has a "from" property and a "to" property, these are the node ID's of the starting and ending nodes of the SideRoad. 

## Documentation
You can find the full documentation after cloning this repo and by opening the [html/index.html](html/index.html) file in the browser.
## Tools used / sources used
- Raylib library used for the rendering of the city and the GUI.
  - [Raylib website](https://www.raylib.com/) - Ramon Santamaria (@raysan5)
  - [Raylib github](https://github.com/raysan5/raylib) - Ramon Santamaria (@raysan5)
  - [Raygui:](https://github.com/raysan5/raygui) The Header file for the GUI. - Ramon Santamaria (@raysan5)
  - [RayGUI maker tool:](https://raylibtech.itch.io/rguilayout) The tool used to generate the GUI. - Ramon Santamaria (@raysan5)
- Github template for raylib in VS code.
  - [Github template:](https://github.com/educ8s/Raylib-CPP-Starter-Template-for-VSCODE-V2) The github template this repo is based on. (Raylib for C++ in VS Code) - educ8s 
  - [How to install video:](https://youtu.be/PaAcVk5jUd8) The how to install video I follow to start this project. - Programming With Nick
- [Lighting shaders](shaders) and [rlight.h](src/include/rlights.h) taken from [Basic lighting example](https://www.raylib.com/examples/shaders/loader.html?name=shaders_basic_lighting) - Chris Camacho (@codifies)
- [Simplex noise C++](https://github.com/SRombauts/SimplexNoise) - Sébastien Rombauts
- [C++ json](https://github.com/nlohmann/json/tree/develop) - Niels Lohmann
- Sources used for the Algorithm:
  - Project inspired by [Fail Masterclass #9 - Subversion](https://youtu.be/1giu6sMnAxY) - Introversion Software
  - Original scientific paper: [Procedural Modeling of Cities](https://cgl.ethz.ch/Downloads/Publications/Papers/2001/p_Par01.pdf) - Parish, Y. I. H., & Müller, P. (2001)
  - [Simplifaction of the L-systems to psuedo code used by the original paper](http://nothings.org/gamedev/l_systems.html) - Sean Barrett
  - [Demonstration of the algorithm and slides with simple explanation](https://phiresky.github.io/procedural-cities/) - phiresky
  - [Demonstration with buildings](https://www.tmwhere.com/city_generation.html) - t-mw
- [Chat GPT](https://chat.openai.com/) for general help, becuase I am still new at C++, but also used for one small algorithm in the code. - OpenAI
- [Doxygen](https://www.doxygen.nl/) used for documenting the code. - Dimitri van Heesch

## License
This piece of software is under the MIT license, more info in [LICENSE](LICENSE).