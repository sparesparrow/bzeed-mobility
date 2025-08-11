@ECHO OFF

SET DIR=%~dp0
SET APP_BASE_NAME=%~n0
SET APP_HOME=%DIR%

SET DEFAULT_JVM_OPTS="-Xmx64m" "-Xms64m"

IF NOT DEFINED JAVA_HOME (
  ECHO ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
  EXIT /B 1
)

SET JAVA_EXE=%JAVA_HOME%\bin\java.exe
IF NOT EXIST "%JAVA_EXE%" (
  ECHO ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
  EXIT /B 1
)

SET CLASSPATH=%APP_HOME%\gradle\wrapper\gradle-wrapper.jar

"%JAVA_EXE%" %DEFAULT_JVM_OPTS% -cp "%CLASSPATH%" org.gradle.wrapper.GradleWrapperMain %*