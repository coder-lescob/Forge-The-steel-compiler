# *Forge* - The steel compiler

This is the current version (0.2.0) of **forge - the steel compiler**. Not finished yet.

## what's **Steel**

### overview

Steel is my programming language that I created in 2025. Steel uses the C abi which means that it uses the same way than C to communicates between functions and store data, so it is compatible with any software that is compatible with C, which is most of the software of the world. Even if I'm not a big fan of Rust I borrowed from it its major pro which is that it is safe in term of memory, but unlike Rust, instead of using references or wierd living time variables that I don't understand Steel uses pointers that are made safes by a lot of compiler checks (not garenty yet that it was correctly implemented).

### why I created it

I created the Steel programming language because I find that Rust syntax is too rusty. So I want to create an option for the C dinosaurs like me, that is safe in terms of memory. If you like Rust no problem use it, if you love C you can use Steel to prevent memory vulnerabilities. Steel is not created to replace C but to provide a memory safe alternative to Rust for those that love the C way of programming.

### Safty features

In Steel a function can be flaged to return a pointer that should be freed and the compiler will detect when it is the best moment to free it (the sooner after beeing used last) which avoid memory leaks, checks are also performs to avoid buffer overflows at run time if the size or the index cannot be known at compile time and at compile time when it can, if some programmer tries to put a value bigger than the variable used for storage the compiler will truncanate to the right size and throw a warning to this programmer, if a negative value is trying to be put in an unsigned variable the compiler will throw an error to the not careful programmer and finally the usage of multiple sized variable in a calcul is totally valid as long as the size of the bigger variable is less than or equal to the size of the variable used for storage.

## why Steel ?

In metallurgy Steel is an alloy of iron and carbon. The carbon is really lite like C is for programmers while iron rusts in contact with oxygene. So Steel is an alloy, a mix, between C and Rust. Even if its name should be inoxydable steel to be accurate.

## My old C++ version

Steel is a language that I created in 2025, with my first version in C++, but the project came to a dead end when I wasn't to implement nicely comments, the other reson of why I have abandoned the C++ version is that I wanted to do it in C to challenge myself, also the parser algorithm wasn't optimal and I wanted to invent a new one.