@Echo off
echo set sh=WScript.CreateObject("WScript.Shell") >tmp.vbs
echo WScript.Sleep 1000 >>tmp.vbs
echo sh.SendKeys "open 192.168.100.1{ENTER}" >>tmp.vbs
echo WScript.Sleep 1000 >>tmp.vbs
echo sh.SendKeys "root{ENTER}" >>tmp.vbs
echo WScript.Sleep 1000 >>tmp.vbs
echo sh.SendKeys "admin{ENTER}" >>tmp.vbs
echo WScript.Sleep 1000 >>tmp.vbs
echo sh.SendKeys "su{ENTER}" >>tmp.vbs
echo WScript.Sleep 1000 >>tmp.vbs
echo sh.SendKeys "system rollback{ENTER}" >>tmp.vbs
echo WScript.Sleep 3000 >>tmp.vbs
echo sh.SendKeys "quit{ENTER}" >>tmp.vbs
echo WScript.Sleep 3000 >>tmp.vbs
echo sh.SendKeys "reset{ENTER}" >>tmp.vbs
start telnet
cscript //nologo tmp.vbs
del tmp.vbs
