@echo off
title Copiador de Core Arduino para Projeto Local

:: ====================================================================================
:: CONFIGURACAO - TUDO PRONTO PARA O ARDUINO UNO
:: ====================================================================================

rem ** PASSO 1: Caminho completo para a pasta da versao do core do Arduino **
set ARDUINO_CORE_SOURCE=C:\Users\afonsomiguel\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6

rem ** PASSO 2: Definicao da 'variant' da placa **
rem 'standard' eh a correta para o Arduino UNO, Nano e Mega.
set VARIANT_NAME=standard


:: ====================================================================================
:: LOGICA DO SCRIPT - NAO EDITE ABAIXO DESTA LINHA
:: ====================================================================================
echo.
echo ===========================================
echo  Iniciando copia do Core Arduino...
echo ===========================================
echo.

rem Define os diretorios de trabalho com base na localizacao do script
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
set DESTINATION_DIR=%PROJECT_ROOT%\lib\arduino_core

echo Verificando diretorio de origem...
if not exist "%ARDUINO_CORE_SOURCE%" (
    echo.
    echo ERRO: O diretorio de origem nao foi encontrado!
    echo Verifique o caminho em ARDUINO_CORE_SOURCE:
    echo "%ARDUINO_CORE_SOURCE%"
    echo.
    goto :erro
)
echo Diretorio de origem encontrado.
echo.

echo Criando estrutura de diretorios de destino...
if not exist "%DESTINATION_DIR%\cores" mkdir "%DESTINATION_DIR%\cores"
if not exist "%DESTINATION_DIR%\variants" mkdir "%DESTINATION_DIR%\variants"
echo Estrutura criada em: "%DESTINATION_DIR%"
echo.

echo Copiando a pasta 'cores/arduino'...
xcopy "%ARDUINO_CORE_SOURCE%\cores\arduino" "%DESTINATION_DIR%\cores\arduino\" /E /I /Y /Q
if %errorlevel% neq 0 goto :erro

echo.
echo Copiando a pasta 'variants/%VARIANT_NAME%'...
xcopy "%ARDUINO_CORE_SOURCE%\variants\%VARIANT_NAME%" "%DESTINATION_DIR%\variants\%VARIANT_NAME%\" /E /I /Y /Q
if %errorlevel% neq 0 goto :erro

echo.
echo ===========================================
echo  COPIA CONCLUIDA COM SUCESSO!
echo ===========================================
echo.
goto :fim

:erro
echo.
echo #######################################################
echo # OCORREU UM ERRO DURANTE A EXECUCAO DO SCRIPT!      #
echo #######################################################
echo.

:fim
pause