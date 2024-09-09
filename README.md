# OpenGD77
Firmware for DMR transceivers using the STM32F405VGT MCU, AT1846S RF chip and HR-C6000 DMR chipset.  
Including the Radioddiy TYT MD-380UV / Retevis RT-3S and Baofeng DM-1701 / Retevis RT-84

# Project status

The firmware is relatively stable and provides DMR and FM audio transmission and reception, as well as a DMR hotspot mode.  
However it does not support some core functionality that the official firmware supports, including sending and receiving of text / SMS messages

The firmware source code does not contain a AMBE codec required for DMR operation.  
This functionality is provided by the official firmware which is merged with the OpenGD77 by the OpenGD77CPS or firmware loader


# User guide

See https://github.com/LibreDMR/OpenGD77_UserGuide


# Credits
Originally conceived by Kai DG4KLU.  
Further development by Roger VK3KYY, latterly assisted by Daniel F1RMB, Alex DL4LEX, Colin G4EML and others.

Current lead developer and source code gatekeeper is Roger VK3KYY


# Copyright

 The firmware is copyright of the OpenGD77 developers. See individual source files for copyright information.

## MCU SDK and API code:   
   See license files in sub-folders
	
## FreeRTOS
   Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  
   All Rights Reserved.


# License

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

4. Use of this source code or binary releases for commercial purposes is strictly forbidden. This includes, without limitation,
   incorporation in a commercial product or incorporation into a product or project which allows commercial use.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# Special thanks

Thanks to those who have assisted the project including :

BD4VOW
CT1HSN
CT4TX 
DG3GSP
DG4KLU
DJ0HF
DL4LEX
EA3BIL
EA3IGM
EA5SW
EB3AM
EW1ADG
F1CXG
F1RMB
G4ELM
IK0NWG
IU4LEG
IZ2EIB
JE4SMQ
JG1UAA
OH1E
OK2HAD
ON1HK
ON7LDS
OZ1MAX
PU4RON
SQ6SFO
SQ7PTE
TA5AYX
VK3KYY
VK4JWT
VK7JS
VK7ZCR
VK7ZJA
ZL1XE
