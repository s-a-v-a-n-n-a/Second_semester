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
###***MATERIALS***

In this particular case hashmap will contain key words and meanings so it seems to be a dictionary.
The program was written with using fast [list structure](https://github.com/s-a-v-a-n-n-a/My-List) that was written by the author earlier.
For measuring duration of the work of function profiler callgrind was used with the application kcachegrind.
Work was done on the Ubuntu 20.04.2 with the processor intel core i7.

###***RESTRICTIONS***

For easier work with hashtable it was decided to use ony insert at back of the list function as it provides direct access to tha data. Thus in addition function of deleting elements was replaced.
Lately fixing the size of the key was used.
Measurint the time of program working also included the fact that the major of time user finds words and the most of the inserts are made at the beginning of the program, when the dictionary is loaded.

###***FORMULAS FOR CALCULATING THE BOOST***

Using profiler and comparing two results the coefficient of the boost we will calculate is (second result - first result)/(first result). (1)
To calculate the total boost the formula we wil use (the coefficient of the boost/amount of the lines on assembly) * 1000. (2)


Writing the hashmap interface. The functions were hash_get(getting the hash of the key, that is variation of polinomial hash), hash_find_element(returns the index of the element or -1 if it is absent), hash_add_element(inserts the element if it is absent) and hash_resize_and_rehash(increases the size of the hashmap due to the load factor, which is 75%).

Profiling by the category "self" showed that the most used functions were strcmp (its part by avx) and hash_find_element.

![Picture 1]()
Picture 1

###***THE SECOND STEP***

As strcmp function uses avx registers, we needed to make the comparing much easier. That is why it was decided to fix the size of key word as 32, as 32 bytes is the size of the avx register. However the author decided not to change the type of the list data into char[64], limited herself to check the minimum size and leaving it to the user due to the meaning can be longer that 32 letters. Even if we do the change compiler would not know about this feature and modify string comparing. The ***Picture 2.1*** shows the rewritten on assembly (nasm) function of comparing two strings.

![Picture 2.1]()
Picture 2.1

Usage of assembly lines is 7.

At the second part it was decided to rewrite finding element in the list cycle by extended assembly. Using flag -masm=intel the author provided herself an opportunity to write it using intel syntax. The idea of boosting is in using calee-saving registers to avoid the excessive amount of pushes into the stack. Looking ahead we can observe that rewritten string comparing function does not need any pushes. Thus we work only with registers that even a little boost the program.

![Picture 2.2]()
Picture 2.2

Usage of assembly lines is 22.
Total usage of assembly lines is 29. 

###***THE THIRD STEP***
Comparing of two programs. We launch kcachegrind with two generated callgrind files and see the results shown in the ***Picture 2.3***. 

![Picture 2.3]()
Picture 2.3

Putting the data in the formula (1) we gain the acceleration factor 1.25.
Calculating with formula (2) the total result is 43.

The calculations for optimization flag -O1 are 1.27 and 44. They are shown in the ***Picture 2.4***

![Picture 2.4]()
Picture 2.4

###***THE FORTH STEP: ADDITIONAL***

The author could not refuse the opportunity to boost the program by using better hashing algorithm. Crc32 with polinom 0x04C11DB7 was used. Counted with intrinsic crc32_u32 it showed boost as ? (formule(1)). ***Picture 4.1*** shows duration of hash_get function working and ***Picture 4.2*** shows the duration of hash_crc_intrinsic. 

![Picture 4.1]()
Picture 4.1
![Picture 4.2]() 
Picture 4.2

Comparison with last fast version is shown in the ***Picture 4.3***

![Picture 4.3]()
Picture 4.3

Comparison with slow version (***Picture 4.4***):

![Picture 4.4]()
Picture 4.4

Final calculations for optimization flag -O1 are 1.41 and 49.

**RESULTS**
-----------
The counted acceleration factor is 44, therefore the total coefficient is 1.27. Boost after using crc-32 hash is 49. 

**CONCLUSION**
--------------
After this investigation we can infer that extended assembly and calling assembly function bossts the program due to direct access to registers and own understanding of guaranteed values.

**LITERATURE AND LINKS**
------------------------
<li> 
	* [Author's github page](https://github.com/s-a-v-a-n-n-a/Second_semester/tree/main/Hash_map)
	* ["Computer systems" by Bryant O'Hallaron](http://guanzhou.pub/files/Computer%20System_EN.pdf)
	* [Documantation for using extended assembly](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
	* [Intel website with intrinsics](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=1606,3853,1606,3990,3990,5117,5495,5480,5421,5417,5477,5507,3853,5653,3343,3828,3828,627,1781,1606,5706,4913,3343,257,3343,3853,1606,5706,5507,4903,3295,3416,606,3344,3859,5200,5144,5153,5153,4173,5663,3853,1606,3863,3993,5653,5655,5156,3859,3840,3853,1609,5156,1523,2457,4118,2940,2961,3536,571,3344,3296,4914,4907,774,782,774,790,892,3344,3418,5663,6190,790,766,790,766,339,3898,1609,1523,5945,5945,4950,4969,339,88,115,339,5939,5909,5945,5909,3418,3557,3547,3418,5909,790,916,3865,3418,6171,766,766,5023,5023,782,5023,6171,301,339,339,766,766,774,790,3865,301,339,301,3865,3344,3418,5597,5655,3418,3323,3865,1287,3418,782,3865&techs=SSE4_1,SSE4_2,AVX,AVX2)
</li>
