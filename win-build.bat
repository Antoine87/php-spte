@mkdir build
@cd build
@cmake ..
@cmake --build . --config Release
@cd ..
@rmdir /S /Q build
@move bin\Release\* bin\
@rmdir bin\Release
@pause
