@echo off
REM Generate the script. Will overwrite any existing temp.txt
if "%1"=="" GOTO NecesarioIP
set IP_CPU=%1
echo La direccion IP CPU es: "%IP_CPU%"

echo user root> temp.txt
echo.>> temp.txt
echo bin>> temp.txt
echo prompt>> temp.txt
echo cd /home/serv>> temp.txt 
echo put uvki-gwser>> temp.txt

echo cd /home/serv/appweb/>> temp.txt 
echo mput ./appweb/*.ico>> temp.txt
echo mput ./appweb/*.html>> temp.txt

echo cd /home/serv/appweb/images>> temp.txt 
echo mput ./appweb/images/*.jpg>> temp.txt

echo cd /home/serv/appweb/languages/>> temp.txt 
echo mput ./appweb/languages/*.json>> temp.txt

echo cd /home/serv/appweb/scripts/>> temp.txt 
echo mput ./appweb/scripts/*.js>> temp.txt
echo cd /home/serv/appweb/scripts/angular/>> temp.txt 
echo mput ./appweb/scripts/angular/*.js>> temp.txt
echo cd /home/serv/appweb/scripts/bootstrap/>> temp.txt 
echo mput ./appweb/scripts/bootstrap/*.js>> temp.txt
echo cd /home/serv/appweb/scripts/jquery/>> temp.txt 
echo mput ./appweb/scripts/jquery/*.js>> temp.txt

echo cd /home/serv/appweb/styles/>> temp.txt 
echo mput ./appweb/styles/*.css>> temp.txt
echo cd /home/serv/appweb/styles/bootstrap/>> temp.txt 
echo mput ./appweb/styles/bootstrap/*.css>> temp.txt
echo cd /home/serv/appweb/styles/fonts/>> temp.txt 
echo mput ./appweb/styles/fonts/*.*>> temp.txt

echo cd /home/serv/appweb/app/controllers/>> temp.txt 
echo mput ./appweb/app/controllers/*.js>> temp.txt

echo cd /home/serv/appweb/app/modules/>> temp.txt 
echo mput ./appweb/app/modules/*.js>> temp.txt

echo cd /home/serv/appweb/app/routes/>> temp.txt 
echo mput ./appweb/app/routes/*.js>> temp.txt

echo cd /home/serv/appweb/app/services/>> temp.txt 
echo mput ./appweb/app/services/*.js>> temp.txt

echo cd /home/serv/appweb/app/templates/>> temp.txt 
echo mput ./appweb/app/templates/*.html>> temp.txt

echo cd /home/serv/appweb/app/views/>> temp.txt 
echo mput ./appweb/app/views/*.html>> temp.txt

echo quit>> temp.txt
REM Launch FTP and pass it the script
ftp -n -s:temp.txt %IP_CPU%
REM 10.249.1.31
REM Clean up.
REM del temp.txt

:NecesarioIP
echo ### Necesario añadir parametro IP CPU. "LoadCfg 10.249.1.XXX"
pause