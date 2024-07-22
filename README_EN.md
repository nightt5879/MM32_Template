[中文](README.md) | English
# MM32_Template
A Project Template for MM32 Series

# 0. Introduction
Recently, I've been using MM32 chips and found the chip packages and projects on the official MM32 website to be overly redundant and complex.<br>
I hope to have a more flexible and generic template available on GitHub (I searched but found **no good** template).<br>
For more detailed examples, datasheets, etc., please refer to the [official MM32 website](https://www.mindmotion.com.cn/products/mm32mcu/).

# 1. Usage
## 1.1 Install Chip Packages
First, find the chip model you need in the `MM32_KEIL_Pack_2.22` folder and double-click to install it (otherwise, Keil might report an error due to missing chip packages).<br>

## 1.2 Select Project Template
Note: Here, I only uploaded a project specific to MM32F0141C6P, but the F0141 series should work as well. You can change the model in the settings.<br>
![Change Chip Model through Settings](/imgs/1.png)<br>
Select the different chip corresponding to your model and open the project.<br>

## 1.3 Usage
Directly copy the entire project folder, such as MM32F0140, to your desired location. You can rename it as you wish (or just keep it as project).<br>
The project directory structure is as follows:<br>
- `Application/`
  - `main.c` (main function)
- `Drivers` (folder for hardware-related code)
- `HAL_Lib/` (official HAL library)
  - `Inc` (header files)
  - `Src` (source files)
- `MDK-ARM` (Keil project folder)
- `STARTUP` (startup files)

# 2. How to Contribute to this Project?
We warmly welcome friends who use the MM32 series to help update this project so that more models can use this template.<br>

## 2.1 Format
Refer to the format of the `MM32F0140` folder. You can name the folders according to the chip model.<br>
You can see that there are many different chip models that can be added.<br>
![Different Chip Models](/imgs/2.png)<br>

## 2.2 Adding Steps
First, you need to fork this project, then add your chip model folder in your project, and submit a PR.<br>
Please provide a screenshot of the Build Output when submitting the PR, so everyone can confirm that your project can compile and program correctly.<br>
Please add directly to the **/imgs path** and name it after the chip model (if you know Markdown, you can directly add it to this file as well).<br>
Note: When adding images, please include the path from the root directory MM32_Template (you can hide your path but at least include the path starting from MM32_Template).<br>

# Build Output
MM32F0140:<br>
![MM32F0140](/imgs/MM32F0140.jpg)<br>
