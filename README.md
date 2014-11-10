re4ct
=====

An MS Windows service: monitor a file and react if it changes.

------------------------------------------
Below is an example of service deployment.
This service will keep eye on the local Windows proxy settings file located at 
C:\Users\dummy\AppData\Roaming\DWP_PAC\LocalPac
------------------------------------------
#1. Register a service:

sc.exe create re4ct binPath= C:\tmp\re4ct.exe

#2. Open the service registry ImagePath value:

C:\sysinternals\regjump.exe  "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\re4ct\ImagePath"

#3. Set the init string for the service:

C:\tmp\re4ct.exe C:\Users\dummy\AppData\Roaming\DWP_PAC LocalPac

#4. The service can be started now
