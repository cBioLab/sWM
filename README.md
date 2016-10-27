# **sWM**

C++ implementation of sWM

# Summary
This library is the implementation of sWM. 

# List of Supported CPUs and Operating Systems
* Intel 64-bit CPUs
* 64-bit Linux (tested on ubuntu 14.04 LTS and ubuntu 16.04 LTS)

# List of Supported Compilers and Prerequisite Tools
* gcc 4.8.4 + OpenSSL 1.0.1f + GMP 5.1.3

# Installation Requirements

Create a working directory (e.g., work) and clone the following repositories.

       mkdir work
       cd work
       git clone git://github.com/cBioLab/sWM.git
       git clone git://github.com/iskana/PBWT-sec.git
       git clone git://github.com/herumi/xbyak.git
       git clone git://github.com/herumi/mcl.git
       git clone git://github.com/herumi/cybozulib.git
       git clone git://github.com/herumi/cybozulib_ext.git

* Xbyak is a prerequisite for optimizing the operations in the finite field on Intel CPUs.
* OpenSSL and libgmp-dev are available via apt-get (or other similar commands).

# Installation
		cd sWM/src
		make

* use tcmalloc (optimal) for Linux; sudo apt-get install libgoogle-perftools-dev

# Prerequisite Files and Libraries for Running Your Application
	* OpenSSL
	* GMP (libgmp-dev)

# Running sWM with a sample data
Server:
		
		cd sWM/bin
		./server -c <num_of_threads> -f ../data/server/sample.wm -p <port　number> -e <epsilon>

Client:

		cd sWM/bin
		./client -c <num_of_threads> -h <hostname> -f ../data/client/sample.que -p <port number>

# Making WM file
		cd sWM/tool/bin
		./strtowav.sh < <sequence file> > <WM file>

# Sequence file format
 	   1st line	 : The length of the sequence data
	   2nd line	 : The alpabet size of this sequence data 
	   3rd line  : Sequence data

# Query file format
	   1st line	 : The length of the query data
	   2nd line	 : The alpabet size of this query data   
	   3rd line  : Query data

# Data format
Both query and sequence data should be described as a series of numbers. A space is used as a delimiter.
e.g., 1 2 15 4 2 1 3


# Copyright Notice
Copyright (C) 2016, cBioLab
All rights reserved.

# License

sWM (files in this repository) is distributed under the [BSD 3-Clause License] (http://opensource.org/licenses/BSD-3-Clause "The BSD 3-Clause License").

# Licenses of External Libraries
Licenses of external libraries are listed as follows.

* PBWT-sec: BSD-3-Clause
* cybozulib: BSD-3-Clause
* mcl: BSD-3-Clause
* Xbyak: BSD-3-Clause
* MPIR: LGPL2
* OpenSSL: Apache License Version 1.0 + SSLeay License

Software including any of those libraries is allowed to be used for commercial purposes without releasing its source code as long as the regarding copyright statements described as follows are included in the software.

* This product includes software that was developed by an OpenSSL project in order to use OpenSSL toolkit.
* This product includes PBWT-sec.
* This product includes mcl, cybozulib, and Xbyak.
* This product includes MPIR.

# Contact Information
* Hiroki Sudo (hsudo108@ruri.waseda.jp)
* Masanobu Jimbo (jimwase@asagi.waseda.jp)

# History

2016/August/12; initial version

