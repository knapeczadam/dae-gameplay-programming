start GPP_EXAM_RELEASE.exe -x 30 -y 30 -s %RANDOM%

start GPP_EXAM_RELEASE.exe -x 1000 -y 30 -s %RANDOM%

start GPP_EXAM_RELEASE.exe -x 30 -y 600 -s %RANDOM%

start GPP_EXAM_RELEASE.exe -x 1000 -y 600 -s %RANDOM%
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js