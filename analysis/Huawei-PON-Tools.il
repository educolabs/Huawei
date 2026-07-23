.assembly extern System.Windows.Forms
{
  .ver 4:0:0:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly extern mscorlib
{
  .ver 4:0:0:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly extern System
{
  .ver 4:0:0:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly extern System.Drawing
{
  .ver 4:0:0:0
  .publickeytoken = (B0 3F 5F 7F 11 D5 0A 3A ) // .?_....:
}
.assembly extern System.Core
{
  .ver 4:0:0:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly 'Huawei-PON-Tools-HG8145V5-20260414'
{
  .custom instance void class [mscorlib]System.Reflection.AssemblyCompanyAttribute::'.ctor'(string) =  (01 00 08 40 44 4B 49 4A 41 59 41 00 00 ) // ...@DKIJAYA..

  .custom instance void class [mscorlib]System.Reflection.AssemblyProductAttribute::'.ctor'(string) =  (
		01 00 1F 48 75 61 77 65 69 20 50 4F 4E 20 54 6F   // ...Huawei PON To
		6F 6C 73 20 45 47 20 26 20 48 47 20 38 31 34 35   // ols EG & HG 8145
		56 35 00 00                                     ) // V5..

  .custom instance void class [mscorlib]System.Runtime.CompilerServices.RuntimeCompatibilityAttribute::'.ctor'() =  (
		01 00 01 00 54 02 16 57 72 61 70 4E 6F 6E 45 78   // ....T..WrapNonEx
		63 65 70 74 69 6F 6E 54 68 72 6F 77 73 01       ) // ceptionThrows.

  .custom instance void class [mscorlib]System.Reflection.AssemblyDescriptionAttribute::'.ctor'(string) =  (
		01 00 1F 48 75 61 77 65 69 20 50 4F 4E 20 54 6F   // ...Huawei PON To
		6F 6C 73 20 45 47 20 26 20 48 47 20 38 31 34 35   // ols EG & HG 8145
		56 35 00 00                                     ) // V5..

  .custom instance void class [mscorlib]System.Reflection.AssemblyCopyrightAttribute::'.ctor'(string) =  (
		01 00 13 43 6F 70 79 72 69 67 68 74 20 C3 82 C2   // ...Copyright ...
		A9 20 32 30 32 36 00 00                         ) // . 2026..

  .custom instance void class [mscorlib]System.Runtime.CompilerServices.CompilationRelaxationsAttribute::'.ctor'(int32) =  (01 00 08 00 00 00 00 00 ) // ........

  .custom instance void class [mscorlib]System.Reflection.AssemblyTitleAttribute::'.ctor'(string) =  (
		01 00 1F 48 75 61 77 65 69 20 50 4F 4E 20 54 6F   // ...Huawei PON To
		6F 6C 73 20 45 47 20 26 20 48 47 20 38 31 34 35   // ols EG & HG 8145
		56 35 00 00                                     ) // V5..

  .custom instance void class [mscorlib]System.Reflection.AssemblyFileVersionAttribute::'.ctor'(string) =  (01 00 08 32 36 2E 34 2E 31 2E 34 00 00 ) // ...26.4.1.4..

  .hash algorithm 0x00008004
  .ver  26:4:1:4
}
.module 'Huawei-PON-Tools-HG8145V5-20260414.exe' // GUID = {3D4B0AD5-82A1-4847-9F2C-9C7800B8A022}


.namespace PSExecutor_1831f0c7
{
  .class private auto ansi beforefieldinit PowerShellHostForm
  	extends [System.Windows.Forms]System.Windows.Forms.Form
  {
    .field  private  class [System]System.Diagnostics.Process _process
    .field  private  <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> _monitorTimer
    .field  private  int32 _exitCode
    .field  private  int32 _timeoutMinutes
    .field  private  valuetype [mscorlib]System.DateTime _startTime

    // method line 1
    .method public hidebysig specialname 
           instance default int32 get_ExitCode ()  cil managed 
    {
        // Method begins at RVA 0x2050
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  ldfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_exitCode
	IL_0006:  ret 
    } // end of method PowerShellHostForm::get_ExitCode

    // method line 2
    .method public hidebysig specialname rtspecialname 
           instance default void '.ctor' (int32 timeoutMinutes)  cil managed 
    {
        // Method begins at RVA 0x2058
	// Code size 203 (0xcb)
	.maxstack 3
	.locals init (
		class [mscorlib]System.Reflection.AssemblyTitleAttribute	V_0,
		string	V_1)
	IL_0000:  ldarg.0 
	IL_0001:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::.ctor()
	IL_0006:  ldarg.0 
	IL_0007:  ldarg.1 
	IL_0008:  stfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_timeoutMinutes
	IL_000d:  ldarg.0 
	IL_000e:  call valuetype [mscorlib]System.DateTime valuetype [mscorlib]System.DateTime::get_Now()
	IL_0013:  stfld valuetype [mscorlib]System.DateTime PSExecutor_1831f0c7.PowerShellHostForm::_startTime
	IL_0018:  call class [mscorlib]System.Reflection.Assembly class [mscorlib]System.Reflection.Assembly::GetExecutingAssembly()
	IL_001d:  call !!0 class [mscorlib]System.Reflection.CustomAttributeExtensions::GetCustomAttribute<class [mscorlib]System.Reflection.AssemblyTitleAttribute> (class [mscorlib]System.Reflection.Assembly)
	IL_0022:  stloc.0 
	IL_0023:  ldarg.0 
	IL_0024:  ldloc.0 
	IL_0025:  brtrue.s IL_002e

	IL_0027:  ldstr "PowerShell Application"
	IL_002c:  br.s IL_0034

	IL_002e:  ldloc.0 
	IL_002f:  callvirt instance string class [mscorlib]System.Reflection.AssemblyTitleAttribute::get_Title()
	IL_0034:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Control::set_Text(string)
	IL_0039:  ldarg.0 
	IL_003a:  ldc.i4.1 
	IL_003b:  ldc.i4.1 
	IL_003c:  newobj instance void [System.Drawing]System.Drawing.Size::.ctor(int32, int32)
	IL_0041:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_Size(<BROKEN CLASS token_ 1000013 due to Could not load file or assembly 'System.Drawing, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a' or one of its dependencies.>)
	IL_0046:  ldarg.0 
	IL_0047:  ldc.i4.0 
	IL_0048:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_StartPosition(<BROKEN CLASS token_ 1000014 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.>)
	IL_004d:  ldarg.0 
	IL_004e:  ldc.i4 -10000
	IL_0053:  ldc.i4 -10000
	IL_0058:  newobj instance void [System.Drawing]System.Drawing.Point::.ctor(int32, int32)
	IL_005d:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_Location(<BROKEN CLASS token_ 1000015 due to Could not load file or assembly 'System.Drawing, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a' or one of its dependencies.>)
	IL_0062:  ldarg.0 
	IL_0063:  ldc.i4.1 
	IL_0064:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_ShowInTaskbar(bool)
	IL_0069:  ldarg.0 
	IL_006a:  ldc.i4.1 
	IL_006b:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_WindowState(<BROKEN CLASS token_ 1000016 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.>)
	IL_0070:  ldarg.0 
	IL_0071:  ldc.r8 0.01
	IL_007a:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_Opacity(float64)
	.try { // 0
	  IL_007f:  call class [mscorlib]System.Reflection.Assembly class [mscorlib]System.Reflection.Assembly::GetExecutingAssembly()
	  IL_0084:  callvirt instance string class [mscorlib]System.Reflection.Assembly::get_Location()
	  IL_0089:  stloc.1 
	  IL_008a:  ldarg.0 
	  IL_008b:  ldloc.1 
	  IL_008c:  call <BROKEN CLASS token_ 1000017 due to Could not load file or assembly 'System.Drawing, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a' or one of its dependencies.> [System.Drawing]System.Drawing.Icon::ExtractAssociatedIcon(string)
	  IL_0091:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::set_Icon(<BROKEN CLASS token_ 1000017 due to Could not load file or assembly 'System.Drawing, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a' or one of its dependencies.>)
	  IL_0096:  leave.s IL_009b

	} // end .try 0
	catch class [mscorlib]System.Object { // 0
	  IL_0098:  pop 
	  IL_0099:  leave.s IL_009b

	} // end handler 0
	IL_009b:  ldarg.0 
	IL_009c:  newobj instance void [System.Windows.Forms]System.Windows.Forms.Timer::.ctor()
	IL_00a1:  stfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_00a6:  ldarg.0 
	IL_00a7:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_00ac:  ldc.i4.s 0x64
	IL_00ae:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Timer::set_Interval(int32)
	IL_00b3:  ldarg.0 
	IL_00b4:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_00b9:  ldarg.0 
	IL_00ba:  ldftn instance void(object, class [mscorlib]System.EventArgs)
	IL_00c0:  newobj instance void class [mscorlib]System.EventHandler::'.ctor'(object, native int)
	IL_00c5:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Timer::add_Tick(class [mscorlib]System.EventHandler)
	IL_00ca:  ret 
    } // end of method PowerShellHostForm::.ctor

    // method line 3
    .method public hidebysig 
           instance default void StartPowerShellProcess (class [System]System.Diagnostics.Process process)  cil managed 
    {
        // Method begins at RVA 0x2140
	// Code size 19 (0x13)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  ldarg.1 
	IL_0002:  stfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	IL_0007:  ldarg.0 
	IL_0008:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_000d:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Timer::Start()
	IL_0012:  ret 
    } // end of method PowerShellHostForm::StartPowerShellProcess

    // method line 4
    .method private hidebysig 
           instance default void MonitorProcess (object sender, class [mscorlib]System.EventArgs e)  cil managed 
    {
        // Method begins at RVA 0x2154
	// Code size 155 (0x9b)
	.maxstack 2
	.locals init (
		valuetype [mscorlib]System.TimeSpan	V_0)
	IL_0000:  ldarg.0 
	IL_0001:  ldfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	IL_0006:  brfalse IL_009a

	IL_000b:  ldarg.0 
	IL_000c:  ldfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_timeoutMinutes
	IL_0011:  ldc.i4.0 
	IL_0012:  ble.s IL_006b

	IL_0014:  call valuetype [mscorlib]System.DateTime valuetype [mscorlib]System.DateTime::get_Now()
	IL_0019:  ldarg.0 
	IL_001a:  ldfld valuetype [mscorlib]System.DateTime PSExecutor_1831f0c7.PowerShellHostForm::_startTime
	IL_001f:  call valuetype [mscorlib]System.TimeSpan valuetype [mscorlib]System.DateTime::op_Subtraction(valuetype [mscorlib]System.DateTime, valuetype [mscorlib]System.DateTime)
	IL_0024:  stloc.0 
	IL_0025:  ldloca.s 0
	IL_0027:  call instance float64 valuetype [mscorlib]System.TimeSpan::get_TotalMinutes()
	IL_002c:  ldarg.0 
	IL_002d:  ldfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_timeoutMinutes
	IL_0032:  conv.r8 
	IL_0033:  blt.un.s IL_006b

	.try { // 0
	  IL_0035:  ldarg.0 
	  IL_0036:  ldfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	  IL_003b:  callvirt instance bool class [System]System.Diagnostics.Process::get_HasExited()
	  IL_0040:  brtrue.s IL_0054

	  IL_0042:  ldarg.0 
	  IL_0043:  ldfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	  IL_0048:  callvirt instance void class [System]System.Diagnostics.Process::Kill()
	  IL_004d:  ldarg.0 
	  IL_004e:  ldc.i4.m1 
	  IL_004f:  stfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_exitCode
	  IL_0054:  leave.s IL_0059

	} // end .try 0
	catch class [mscorlib]System.Object { // 0
	  IL_0056:  pop 
	  IL_0057:  leave.s IL_0059

	} // end handler 0
	IL_0059:  ldarg.0 
	IL_005a:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_005f:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Timer::Stop()
	IL_0064:  ldarg.0 
	IL_0065:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::Close()
	IL_006a:  ret 
	IL_006b:  ldarg.0 
	IL_006c:  ldfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	IL_0071:  callvirt instance bool class [System]System.Diagnostics.Process::get_HasExited()
	IL_0076:  brfalse.s IL_009a

	IL_0078:  ldarg.0 
	IL_0079:  ldarg.0 
	IL_007a:  ldfld class [System]System.Diagnostics.Process PSExecutor_1831f0c7.PowerShellHostForm::_process
	IL_007f:  callvirt instance int32 class [System]System.Diagnostics.Process::get_ExitCode()
	IL_0084:  stfld int32 PSExecutor_1831f0c7.PowerShellHostForm::_exitCode
	IL_0089:  ldarg.0 
	IL_008a:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_008f:  callvirt instance void [System.Windows.Forms]System.Windows.Forms.Timer::Stop()
	IL_0094:  ldarg.0 
	IL_0095:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::Close()
	IL_009a:  ret 
    } // end of method PowerShellHostForm::MonitorProcess

    // method line 5
    .method family virtual hidebysig 
           instance default void Dispose (bool disposing)  cil managed 
    {
        // Method begins at RVA 0x220c
	// Code size 30 (0x1e)
	.maxstack 8
	IL_0000:  ldarg.1 
	IL_0001:  brfalse.s IL_0016

	IL_0003:  ldarg.0 
	IL_0004:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_0009:  brfalse.s IL_0016

	IL_000b:  ldarg.0 
	IL_000c:  ldfld <BROKEN CLASS token_ 1000004 due to Could not load file or assembly 'System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089' or one of its dependencies.> PSExecutor_1831f0c7.PowerShellHostForm::_monitorTimer
	IL_0011:  callvirt instance void class [System]System.ComponentModel.Component::Dispose()
	IL_0016:  ldarg.0 
	IL_0017:  ldarg.1 
	IL_0018:  call instance void [System.Windows.Forms]System.Windows.Forms.Form::Dispose(bool)
	IL_001d:  ret 
    } // end of method PowerShellHostForm::Dispose

	.property instance int32 ExitCode ()
	{
		.get instance default int32 PSExecutor_1831f0c7.PowerShellHostForm::get_ExitCode () 
	}
  } // end of class PSExecutor_1831f0c7.PowerShellHostForm
}

.namespace PSExecutor_1831f0c7
{
  .class private auto ansi beforefieldinit Program
  	extends [mscorlib]System.Object
  {
    .field  private static  class [mscorlib]System.Func`2<string,bool> 'CS$<>9__CachedAnonymousMethodDelegate3'
    .custom instance void class [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::'.ctor'() =  (01 00 00 00 ) // ....


    // method line 6
    .method private static hidebysig 
           default void Main (string[] args)  cil managed 
    {
        // Method begins at RVA 0x222c
	.entrypoint
    } // end of method Program::Main

    // method line 7
    .method private static hidebysig 
           default bool IsRunningAsAdmin ()  cil managed 
    {
        // Method begins at RVA 0x2644
	// Code size 34 (0x22)
	.maxstack 2
	.locals init (
		class [mscorlib]System.Security.Principal.WindowsIdentity	V_0,
		class [mscorlib]System.Security.Principal.WindowsPrincipal	V_1,
		bool	V_2)
	.try { // 0
	  IL_0000:  call class [mscorlib]System.Security.Principal.WindowsIdentity class [mscorlib]System.Security.Principal.WindowsIdentity::GetCurrent()
	  IL_0005:  stloc.0 
	  IL_0006:  ldloc.0 
	  IL_0007:  newobj instance void class [mscorlib]System.Security.Principal.WindowsPrincipal::'.ctor'(class [mscorlib]System.Security.Principal.WindowsIdentity)
	  IL_000c:  stloc.1 
	  IL_000d:  ldloc.1 
	  IL_000e:  ldc.i4 544
	  IL_0013:  callvirt instance bool class [mscorlib]System.Security.Principal.WindowsPrincipal::IsInRole(valuetype [mscorlib]System.Security.Principal.WindowsBuiltInRole)
	  IL_0018:  stloc.2 
	  IL_0019:  leave.s IL_0020

	} // end .try 0
	catch class [mscorlib]System.Object { // 0
	  IL_001b:  pop 
	  IL_001c:  ldc.i4.0 
	  IL_001d:  stloc.2 
	  IL_001e:  leave.s IL_0020

	} // end handler 0
	IL_0020:  ldloc.2 
	IL_0021:  ret 
    } // end of method Program::IsRunningAsAdmin

    // method line 8
    .method private static hidebysig 
           default void RestartAsAdmin ()  cil managed 
    {
        // Method begins at RVA 0x2684
	// Code size 76 (0x4c)
	.maxstack 2
	.locals init (
		class [System]System.Diagnostics.ProcessStartInfo	V_0,
		class [System]System.Diagnostics.ProcessStartInfo	V_1,
		class [mscorlib]System.Exception	V_2)
	.try { // 0
	  IL_0000:  newobj instance void class [System]System.Diagnostics.ProcessStartInfo::'.ctor'()
	  IL_0005:  stloc.1 
	  IL_0006:  ldloc.1 
	  IL_0007:  call class [mscorlib]System.Reflection.Assembly class [mscorlib]System.Reflection.Assembly::GetExecutingAssembly()
	  IL_000c:  callvirt instance string class [mscorlib]System.Reflection.Assembly::get_Location()
	  IL_0011:  callvirt instance void class [System]System.Diagnostics.ProcessStartInfo::set_FileName(string)
	  IL_0016:  ldloc.1 
	  IL_0017:  ldc.i4.1 
	  IL_0018:  callvirt instance void class [System]System.Diagnostics.ProcessStartInfo::set_UseShellExecute(bool)
	  IL_001d:  ldloc.1 
	  IL_001e:  ldstr "runas"
	  IL_0023:  callvirt instance void class [System]System.Diagnostics.ProcessStartInfo::set_Verb(string)
	  IL_0028:  ldloc.1 
	  IL_0029:  stloc.0 
	  IL_002a:  ldloc.0 
	  IL_002b:  call class [System]System.Diagnostics.Process class [System]System.Diagnostics.Process::Start(class [System]System.Diagnostics.ProcessStartInfo)
	  IL_0030:  pop 
	  IL_0031:  leave.s IL_004b

	} // end .try 0
	catch class [mscorlib]System.Exception { // 0
	  IL_0033:  stloc.2 
	  IL_0034:  ldstr "Failed to restart with administrator privileges: "
	  IL_0039:  ldloc.2 
	  IL_003a:  callvirt instance string class [mscorlib]System.Exception::get_Message()
	  IL_003f:  call string string::Concat(string, string)
	  IL_0044:  call void class [mscorlib]System.Console::WriteLine(string)
	  IL_0049:  leave.s IL_004b

	} // end handler 0
	IL_004b:  ret 
    } // end of method Program::RestartAsAdmin

    // method line 9
    .method private static hidebysig 
           default string FindPowerShellExecutable ()  cil managed 
    {
        // Method begins at RVA 0x26ec
	// Code size 48 (0x30)
	.maxstack 1
	.locals init (
		string	V_0,
		string	V_1)
	IL_0000:  ldstr "pwsh.exe"
	IL_0005:  call string class PSExecutor_1831f0c7.Program::FindExecutableInPath(string)
	IL_000a:  stloc.0 
	IL_000b:  ldloc.0 
	IL_000c:  call bool string::IsNullOrEmpty(string)
	IL_0011:  brtrue.s IL_0015

	IL_0013:  ldloc.0 
	IL_0014:  ret 
	IL_0015:  ldstr "powershell.exe"
	IL_001a:  call string class PSExecutor_1831f0c7.Program::FindExecutableInPath(string)
	IL_001f:  stloc.1 
	IL_0020:  ldloc.1 
	IL_0021:  call bool string::IsNullOrEmpty(string)
	IL_0026:  brtrue.s IL_002a

	IL_0028:  ldloc.1 
	IL_0029:  ret 
	IL_002a:  ldstr "powershell.exe"
	IL_002f:  ret 
    } // end of method Program::FindPowerShellExecutable

    // method line 10
    .method private static hidebysig 
           default string FindExecutableInPath (string executableName)  cil managed 
    {
        // Method begins at RVA 0x2734
	// Code size 166 (0xa6)
	.maxstack 4
	.locals init (
		string	V_0,
		class [mscorlib]System.Collections.Generic.IEnumerable`1<string>	V_1,
		string	V_2,
		string	V_3,
		string	V_4,
		char[]	V_5,
		class [mscorlib]System.Collections.Generic.IEnumerator`1<string>	V_6)
	.try { // 2
	  IL_0000:  ldstr "PATH"
	  IL_0005:  call string class [mscorlib]System.Environment::GetEnvironmentVariable(string)
	  IL_000a:  stloc.0 
	  IL_000b:  ldloc.0 
	  IL_000c:  call bool string::IsNullOrEmpty(string)
	  IL_0011:  brfalse.s IL_001b

	  IL_0013:  ldnull 
	  IL_0014:  stloc.s 4
	  IL_0016:  leave IL_00a3

	  IL_001b:  ldloc.0 
	  IL_001c:  ldc.i4.1 
	  IL_001d:  newarr [mscorlib]System.Char
	  IL_0022:  stloc.s 5
	  IL_0024:  ldloc.s 5
	  IL_0026:  ldc.i4.0 
	  IL_0027:  ldc.i4.s 0x3b
	  IL_0029:  stelem.i2 
	  IL_002a:  ldloc.s 5
	  IL_002c:  callvirt instance string[] string::Split(char[])
	  IL_0031:  ldsfld class [mscorlib]System.Func`2<string,bool> PSExecutor_1831f0c7.Program::'CS$<>9__CachedAnonymousMethodDelegate3'
	  IL_0036:  brtrue.s IL_0049

	  IL_0038:  ldnull 
	  IL_0039:  ldftn bool class PSExecutor_1831f0c7.Program::'<FindExecutableInPath>b__2'(string)
	  IL_003f:  newobj instance void class [mscorlib]System.Func`2<string, bool>::'.ctor'(object, native int)
	  IL_0044:  stsfld class [mscorlib]System.Func`2<string,bool> PSExecutor_1831f0c7.Program::'CS$<>9__CachedAnonymousMethodDelegate3'
	  IL_0049:  ldsfld class [mscorlib]System.Func`2<string,bool> PSExecutor_1831f0c7.Program::'CS$<>9__CachedAnonymousMethodDelegate3'
	  IL_004e:  call class [mscorlib]System.Collections.Generic.IEnumerable`1<!!0> class [System.Core]System.Linq.Enumerable::Where<string> (class [mscorlib]System.Collections.Generic.IEnumerable`1<!!0>, class [mscorlib]System.Func`2<!!0,bool>)
	  IL_0053:  stloc.1 
	  IL_0054:  ldloc.1 
	  IL_0055:  callvirt instance class [mscorlib]System.Collections.Generic.IEnumerator`1<!0> class [mscorlib]System.Collections.Generic.IEnumerable`1<string>::GetEnumerator()
	  IL_005a:  stloc.s 6
	  .try { // 1
	    IL_005c:  br.s IL_0085

	    IL_005e:  ldloc.s 6
	    IL_0060:  callvirt instance !0 class [mscorlib]System.Collections.Generic.IEnumerator`1<string>::get_Current()
	    IL_0065:  stloc.2 
	    .try { // 0
	      IL_0066:  ldloc.2 
	      IL_0067:  callvirt instance string string::Trim()
	      IL_006c:  ldarg.0 
	      IL_006d:  call string class [mscorlib]System.IO.Path::Combine(string, string)
	      IL_0072:  stloc.3 
	      IL_0073:  ldloc.3 
	      IL_0074:  call bool class [mscorlib]System.IO.File::Exists(string)
	      IL_0079:  brfalse.s IL_0080

	      IL_007b:  ldloc.3 
	      IL_007c:  stloc.s 4
	      IL_007e:  leave.s IL_00a3

	      IL_0080:  leave.s IL_0085

	    } // end .try 0
	    catch class [mscorlib]System.Object { // 0
	      IL_0082:  pop 
	      IL_0083:  leave.s IL_0085

	    } // end handler 0
	    IL_0085:  ldloc.s 6
	    IL_0087:  callvirt instance bool class [mscorlib]System.Collections.IEnumerator::MoveNext()
	    IL_008c:  brtrue.s IL_005e

	    IL_008e:  leave.s IL_009c

	  } // end .try 1
	  finally  { // 1
	    IL_0090:  ldloc.s 6
	    IL_0092:  brfalse.s IL_009b

	    IL_0094:  ldloc.s 6
	    IL_0096:  callvirt instance void class [mscorlib]System.IDisposable::Dispose()
	    IL_009b:  endfinally 
	  } // end handler 1
	  IL_009c:  leave.s IL_00a1

	} // end .try 2
	catch class [mscorlib]System.Object { // 2
	  IL_009e:  pop 
	  IL_009f:  leave.s IL_00a1

	} // end handler 2
	IL_00a1:  ldnull 
	IL_00a2:  ret 
	IL_00a3:  ldloc.s 4
	IL_00a5:  ret 
    } // end of method Program::FindExecutableInPath

    // method line 11
    .method public hidebysig specialname rtspecialname 
           instance default void '.ctor' ()  cil managed 
    {
        // Method begins at RVA 0x2810
	// Code size 7 (0x7)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call instance void object::'.ctor'()
	IL_0006:  ret 
    } // end of method Program::.ctor

    // method line 12
    .method private static hidebysig 
           default bool '<FindExecutableInPath>b__2' (string p)  cil managed 
    {
        .custom instance void class [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::'.ctor'() =  (01 00 00 00 ) // ....

        // Method begins at RVA 0x2728
	// Code size 10 (0xa)
	.maxstack 8
	IL_0000:  ldarg.0 
	IL_0001:  call bool string::IsNullOrWhiteSpace(string)
	IL_0006:  ldc.i4.0 
	IL_0007:  ceq 
	IL_0009:  ret 
    } // end of method Program::<FindExecutableInPath>b__2

  } // end of class PSExecutor_1831f0c7.Program
}

