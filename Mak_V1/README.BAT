Echo off
CLS
Echo If the Program Disk is not in Drive A, type the following
Echo command to view the README file on your screen:
Echo   "TYPE D:README.TXT |MORE"
Echo where D is the drive designator.
Pause 
CLS 
A:
TYPE README.TXT |MORE
