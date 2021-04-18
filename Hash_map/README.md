**INVESTIGATION OF BOOSTING HASHMAP WITH CHAINING**
===================================================
**AUTHORS**
-----------
Research was conducted by a first year student at MIPT Anna Savchuk in 2021.

**INTRODUCTION**
------------------------
In field of working with data using of hashmaps is widely spread. Hashmap is a structure thst provides an opportunity of inserting, finding and deleting information for the O(1) of time. One of the types of hashamps is the one with chaining. It is considered that data with the same hash are storing in the same chain.
However such hashmap is not fast enough. Even optimized by compiler, we do not gain expected result. That happens due to parts of the algorithm that are not clear for optimizer. Contrariwise this knowledge is available for a programmer, who can achieve the boost by different restrictions.  
Here we come up with the idea to boost the work of program by rewriting two functions on assembler in two different ways on flag -O0 and -O1.
These optimization can be useful in the field of working with data as there is a dependence of the correct operation of databases on the speed of work with them.

**WORK PROGRESS**
-----------------
### ***MATERIALS***

In this particular case hashmap will contain key words and meanings so it seems to be a dictionary.
The program was written with using fast [list structure](https://github.com/s-a-v-a-n-n-a/My-List) that was written by the author earlier.
For measuring duration of the work of function profiler callgrind was used with the application kcachegrind.
Work was done on the Ubuntu 20.04.2 with the processor intel core i7.

### ***RESTRICTIONS***

For easier work with hashtable it was decided to use ony insert at back of the list function as it provides direct access to tha data. Thus in addition function of deleting elements was replaced.
Lately fixing the size of the key was used.
Measurint the time of program working also included the fact that the major of time user finds words and the most of the inserts are made at the beginning of the program, when the dictionary is loaded.

### ***FORMULAS FOR CALCULATING THE BOOST***

Using profiler and comparing two results the coefficient of the boost we will calculate is (second result - first result)/(first result). (1)
To calculate the total boost the formula we wil use (the coefficient of the boost/amount of the lines on assembly) * 1000. (2)


Writing the hashmap interface. The functions were hash_get(getting the hash of the key, that is variation of polinomial hash), hash_find_element(returns the index of the element or -1 if it is absent), hash_add_element(inserts the element if it is absent) and hash_resize_and_rehash(increases the size of the hashmap due to the load factor, which is 75%).

Profiling by the category "self" showed that the most used functions were strcmp (its part by avx) and hash_find_element.

<img src="Investigation/Picture%201.jpg" alt="Picture 1" width="600">
***Picture 1***

### ***THE SECOND STEP***

As strcmp function uses avx registers, we needed to make the comparing much easier. That is why it was decided to fix the size of key word as 32, as 32 bytes is the size of the avx register. However the author decided not to change the type of the list data into char[64], limited herself to check the minimum size and leaving it to the user due to the meaning can be longer that 32 letters. Even if we do the change compiler would not know about this feature and modify string comparing. The ***Picture 2.1*** shows the rewritten on assembly (nasm) function of comparing two strings.

<img src="Investigation/Picture%202.1.jpg" alt="Picture 2.1" width="600">
***Picture 2.1***

Usage of assembly lines is 7.

At the second part it was decided to rewrite finding element in the list cycle by extended assembly. Using flag -masm=intel the author provided herself an opportunity to write it using intel syntax. The idea of boosting is in using calee-saving registers to avoid the excessive amount of pushes into the stack. Looking ahead we can observe that rewritten string comparing function does not need any pushes. Thus we work only with registers that even a little boost the program.

<img src="Investigation/Picture%202.2.jpg" alt="Picture 2.2" width="600">
***Picture 2.2***

Usage of assembly lines is 22.
Total usage of assembly lines is 29. 

### ***THE THIRD STEP***
Comparing of two programs. We launch kcachegrind with two generated callgrind files and see the results shown in the ***Picture 2.3***. 

<img src="Investigation/Picture%202.3.png" alt="Picture 2.3" width="600">
***Picture 2.3***

Putting the data in the formula (1) we gain the acceleration factor 1.25.
Calculating with formula (2) the total result is 43.

The calculations for optimization flag -O1 are 1.27 and 44. They are shown in the ***Picture 2.4***

<img src="Investigation/Picture%202.4.png" alt="Picture 2.4" width="600">
***Picture 2.4***

### ***THE FORTH STEP: ADDITIONAL***

The author could not refuse the opportunity to boost the program by using better hashing algorithm. Crc32 with polinom 0x04C11DB7 was used. Counted with intrinsic crc32_u32 it showed boost as ? (formule(1)). ***Picture 4.1*** shows duration of hash_get function working and ***Picture 4.2*** shows the duration of hash_crc_intrinsic. 

<img src="Investigation/Picture%204.1.png" alt="Picture 4.1" width="600">
***Picture 4.1***
<img src="Investigation/Picture%204.2.png" alt="Picture 4.2" width="600">
***Picture 4.2***

Comparison with last fast version is shown in the ***Picture 4.3***

<img src="Investigation/Picture%204.3.png" alt="Picture 4.3" width="600">
***Picture 4.3***

Comparison with slow version (***Picture 4.4***):

<img src="Investigation/Picture%204.4.png" alt="Picture 4.4" width="600">
***Picture 4.4***

Final calculations for optimization flag -O1 are 1.41 and 49.

**RESULTS**
-----------
The counted acceleration factor is 44, therefore the total coefficient is 1.27. Boost after using crc-32 hash is 49. 

The acceleration of the functions is clearly shown on the bar graph (***Picture 5***).

<img src="Investigation/Picture%205.jpg" alt="Picture 5" width="600">
***Picture 5***

**CONCLUSION**
--------------
After this investigation we can infer that extended assembly and calling assembly function bossts the program due to direct access to registers and own understanding of guaranteed values.

**LITERATURE AND LINKS**
------------------------
<li> 
[Author's github page](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Hash_map)
</li>
<li>
["Computer systems" by Bryant O'Hallaron](http://guanzhou.pub/files/Computer%20System_EN.pdf)
</li>
<li>
[Documantation for using extended assembly](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
</li>
<li>
[Intel website with intrinsics](https://software.intel.com/sites/landingpage/IntrinsicsGuide)
</li>
