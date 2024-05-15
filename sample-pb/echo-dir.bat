@echo off
for /f "delims=" %%a in ('dir  /b /ad /s ^|sort /r' ) do (echo "%%a")


PAUSE