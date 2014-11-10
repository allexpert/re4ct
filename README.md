re4ct
=====

An MS Windows service: monitor a file and react if it changes.

REM ------------------------------------------
REM Below is an example of service deployment.
REM This service will keep eye on the local Windows proxy settings file located at 
REM C:\Users\dummy\AppData\Roaming\DWP_PAC\LocalPac
REM ------------------------------------------
REM 1. Register a service:

sc.exe create re4ct binPath= C:\tmp\re4ct.exe

REM 2. Open the service registry ImagePath value:

C:\sysinternals\regjump.exe  "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\services\re4ct\ImagePath"

REM 3. Set the init string for the service:

C:\tmp\re4ct.exe C:\Users\dummy\AppData\Roaming\DWP_PAC LocalPac

REM 4. The service can be started now
