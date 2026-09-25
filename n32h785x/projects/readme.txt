1、功能说明
    此例程展示了GPIO的基本配置方法。

2、使用环境

    软件开发环境：VSCODE + GCC

    芯片支持：
        N32H785
        N32H785xxxEC
        N32H787


3、使用说明

    系统配置
        1、时钟源：HSI+PLL
        2、系统时钟频率：
            M7核 600MHz  M4核 300MHz
        3、GPIO：
            N32H787系列: 
            PA1(D1)为推挽输出功能
            PA2(D2)为推挽输出功能

    使用方法：
        1、编译后将CM7和CM4的工程程序分别下载到开发板并复位运行;
        2、可观察到D1间隔1s循环点亮;
        3、同时D2闪烁指示M4核正在运行。
        
4、注意事项
    无
    
1. Function description

    This example shows the basic configuration of GPIO.

2. Use environment

    Software development environment: VSCODE + GCC

    MCU support:
        N32H785
        N32H785xxxEC
        N32H787

3. Instructions for use

    System Configuration:
        1. Clock source: HSI+PLL
        2. System Clock frequency: 
            M7 Core：600MHz  M4 Core：300MHz
            
        3.GPIO:
            N32H760 Serie:
            PA1 (D1) is selected as push-pull output function
            PA2 (D2) is selected as push-pull output function

     Instructions:
        1. After compiling, download the program of CM7 and CM4 projects respectively and reset, the program start running；
        2. D1 can be observed to light up cyclically at 1s intervals；
        3. At the same time, D2 flashes to indicate that the M4 core is running; 
4. Attention
    None

