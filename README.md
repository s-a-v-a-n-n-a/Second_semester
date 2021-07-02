**DESCRIPTION OF REPOSITORY**
=============================
This repository is created for educational programs. The aim of the semester was to learn how code on machine layers works, work with assembler and use it to speed up the programs.

**WORKING IN DOS**
------------------

**[Drawing frame](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Drawing%20frame)**
Assembler program that makes an animation of growing frame with some text in it. 
The aim was to:
* work with registers and stack
* minimize stack usages

**[Work with interruptions](https://github.com/s-a-v-a-n-n-a/Second_semester/blob/main/Work%20with%20interruptions)**
Assembler program that intercepts DOS interruptions.

**[Printf on assembly](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Printf_on_assembly)**
Assembler program that prints the text due to the format string.
It is connected to C program so thay can call each other.

**[Mandelbrot](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Mandelbrot)**
--------------
Program that draws Mandelbrot set. It was firstly written on C without modifications and then smoothly was rewritten on intrinsics. 
The result was that it became faster more than 2 times.

To draw the picture functions from the [TXLib](https://github.com/ded32/TXLib/tree/0739ed7c54cb6e0d4c8302f08a0936986cfb57b2) were used.

<img src="Mandelbrot/Zoom.jpg" alt="Mandelbrot example" width="600">
Beutiful zoom of Mandelbrot set

**[Cat on the table](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Cat_on_table)**
--------------------
The aim was to merge two pictures - the picture of the table and the picture of the cat. It was done by working with ARGB pixel components. To do it SFML was used.
The function of merging was also accelerated by intrinsics.

**[Hashmap](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Hash_map)**
------------
The realisation of hashmap based on [My_List](https://github.com/s-a-v-a-n-n-a/My_List). It already has its own speed up investigation so I [link](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Hash_map#readme) it here.
Briefly, hashmap was accelerated by rewritting one function on assembler and with extended assembly in the other one. The accelaration only with these two modificatrions is 43 times (that was calculated by special formula). 

**[Binary translator](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Binary_translator)**
---------------------
Binary translator is the largest project of the second semester. It translates my small assembler (AASM) used in [My_CPU](https://github.com/s-a-v-a-n-n-a/My_CPU) into x86-64. To help myself write big code on AASM I used my funny language from [My_compiler](https://github.com/s-a-v-a-n-n-a/My_compiler) and [shellcode](https://github.com/s-a-v-a-n-n-a/Second_semester/blob/main/Binary_translator/automatic.sh).
There is a separated repository for this program so more detailed documentation is [here](https://github.com/s-a-v-a-n-n-a/Binary_translator/blob/main/README.md).

**SUMMARY**
-----------
During this semester, some work was done and some of the material on understanding the inner work of the processor was studied. This study is going to be continued.

