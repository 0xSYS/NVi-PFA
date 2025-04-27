@echo off
"C:\\Program Files\\Java\\jdk-21\\bin\\java" ^
  --class-path ^
  "C:\\Users\\Hexagon\\.gradle\\caches\\modules-2\\files-2.1\\com.google.prefab\\cli\\2.1.0\\aa32fec809c44fa531f01dcfb739b5b3304d3050\\cli-2.1.0-all.jar" ^
  com.google.prefab.cli.AppKt ^
  --build-system ^
  cmake ^
  --platform ^
  android ^
  --abi ^
  arm64-v8a ^
  --os-version ^
  21 ^
  --stl ^
  c++_static ^
  --ndk-version ^
  27 ^
  --output ^
  "C:\\Users\\Hexagon\\AppData\\Local\\Temp\\agp-prefab-staging14154890040898373318\\staged-cli-output" ^
  "C:\\Users\\Hexagon\\.gradle\\caches\\8.12\\transforms\\911fc0ec81bf1bedb9f080a8bd8c9ca9\\transformed\\SDL3-3.2.10\\prefab"
