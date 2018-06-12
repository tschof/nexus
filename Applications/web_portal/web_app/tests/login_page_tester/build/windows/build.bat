SETLOCAL
IF NOT "%1" == "Debug" (
  SET PROD_ENV=1
)
PUSHD %~dp0..\..\..\..\library\build\windows
CALL build.bat
POPD
PUSHD %~dp0..\..\
IF exist application (
  rm -rf application
)
mkdir .\application
cp -r ..\..\resources .\application
cp -r source\index.html .\application
PUSHD node_modules
IF exist beam (
  rm -rf beam
)
cp -r ..\..\..\..\..\..\..\Beam\web_api\library\* .
IF exist @types\beam (
  rm -rf @types\beam
)
mkdir @types\beam
cp -r ..\..\..\..\..\..\..\Beam\web_api\library\beam\library\beam\* @types\beam
IF exist nexus (
  rm -rf nexus
)
cp -r ..\..\..\..\..\..\web_api\library\* .
IF exist @types\nexus (
  rm -rf @types\nexus
)
mkdir @types\nexus
cp -r ..\..\..\..\..\..\web_api\library\nexus\library\nexus\* @types\nexus
POPD
node .\node_modules\webpack\bin\webpack.js
POPD
ENDLOCAL