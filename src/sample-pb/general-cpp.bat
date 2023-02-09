::由于PROTOBUF_GENERATE_CPP只能编译一个目录下的proto，尽量选择手动生成pb.cc，以下为脚本示例

SET pbExe=%~dp0\protoc-win64\protoc.exe
SET pbDir=%~dp0\pb
SET cppDir=%~dp0\pb_cpp

if exist %cppDir% ( rd /s /Q %cppDir% )
md %cppDir%

%pbExe% ^
--proto_path=%pbDir% ^
--cpp_out=%cppDir% ^
%pbDir%\common\*.proto ^
%pbDir%\notify\*.proto ^
%pbDir%\req\*.proto ^
%pbDir%\res\*.proto ^
%pbDir%\*.proto


PAUSE