@echo off
setlocal enabledelayedexpansion
echo ��ʼ����...

if exist *.sdf (
	del *.sdf
	echo "ɾ���ļ�" *.sdf
)

if exist *.v12.suo (
	del *.v12.suo /AH
	echo "ɾ���ļ�" *.v12.suo
)

if exist Bin (
	rd Bin /s/q
	echo "ɾ��Ŀ¼" Bin
)

if exist ipch (
	rd ipch /s/q
	echo "ɾ��Ŀ¼" ipch
)

for /r . %%a in (ipch) do (    
  if exist %%a (  
  echo "ɾ��" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (*.sdf) do (    
  if exist %%a (  
  echo "ɾ��" %%a  
  del "%%a"   
 )  
)

for /r . %%a in (Release) do (    
  if exist %%a (  
  echo "ɾ��" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (Debug) do (    
  if exist %%a (  
  echo "ɾ��" %%a  
  rd /s /q "%%a"   
 )  
)

for /r . %%a in (obj) do (    
  if exist %%a (  
  echo "ɾ��" %%a  
  rd /s /q "%%a"   
 )  
)

pause
