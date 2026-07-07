# ANÁLISIS PROFUNDO DE ARCHIVOS .EXE - HUAWEI ONT TOOLS

## RESUMEN EJECUTIVO

Se analizaron 3 archivos .exe relacionados con herramientas para ONTs Huawei:
1. **PANDA-FLASHER.exe** (3.5 MB) - Herramienta nativa PE32 para flash/reparación de ONT
2. **Huawei-PON-Tools-HG8145V5-20260414.exe** (2.8 MB) - Assembly .NET para configuración PON
3. **Huawei-Downgrade-Tools.exe** (2.4 MB) - Assembly .NET para downgrade de firmware

## HERRAMIENTAS INSTALADAS

- 7zip (p7zip-full) - Extracción de archivos
- binwalk - Análisis de firmware
- capstone - Framework de desensamblado
- mono-utils (monodis) - Descompilador .NET
- radare2 - Framework de ingeniería inversa
- strings - Extracción de cadenas
- file - Identificación de tipos de archivo

## ANÁLISIS DETALLADO

### 1. PANDA-FLASHER.exe

**Tipo:** PE32 executable (GUI) Intel 80386, nativo
**Propósito:** Herramienta de reparación/flash para ONT Huawei
**Ruta de compilación:** F:\code\pandora\src\RepairRelease\ONT

#### Archivos y Scripts Embebidos:

**Script de Habilitación de Telnet (shell script):**
```bash
#!/bin/sh
var_file_telnetenable="/mnt/jffs2/TelnetEnable"
var_jffs2_current_ctree_file="/mnt/jffs2/hw_ctree.xml"
var_current_ctree_bak_file="/var/hw_ctree_equipbak.xml"
var_current_ctree_file_tmp="/var/hw_ctree.xml.tmp"
var_pack_temp_dir="/bin/"

HW_Open_Telnet_Ctree_Node()
    var_node_telnet=InternetGatewayDevice.X_HW_Security.AclServices
    varIsXmlEncrypted=0
    EnableLanTelnetValue="1"
    
    cp -f $var_jffs2_current_ctree_file $var_current_ctree_bak_file
    $var_pack_temp_dir/aescrypt2 1 $var_current_ctree_bak_file $var_current_ctree_file_tmp
    
    if [ 0 -eq $? ]
        varIsXmlEncrypted=1
        mv $var_current_ctree_bak_file $var_current_ctree_bak_file".gz"
        gunzip -f $var_current_ctree_bak_file".gz"
    
    cfgtool set $var_current_ctree_bak_file $var_node_telnet TELNETLanEnable $EnableLanTelnetValue
    
    if [ $varIsXmlEncrypted -eq 1 ]
        gzip -f $var_current_ctree_bak_file
        mv $var_current_ctree_bak_file".gz" $var_current_ctree_bak_file
        $var_pack_temp_dir/aescrypt2 0 $var_current_ctree_bak_file $var_current_ctree_file_tmp
    
    rm -f $var_jffs2_current_ctree_file
    cp -f $var_current_ctree_bak_file $var_jffs2_current_ctree_file

HW_Open_Telnet_Ctree_Node
echo "feature.name = \"HW_SSMP_FEATURE_CLI_CHINA_MODE\" feature.enable=\"1\" feature.attribute=\"1\"" > /mnt/jffs2/hw_hardinfo_feature
```

**Archivos XML de Configuración Referenciados:**
- `/var/UpgradeCheck.xml` (SHA256: 72bb964d55d36eeca81b401ea94686a222ac2f9df0cab6797b19a2787e7db129)
- `/mnt/jffs2/hw_ctree.xml` - Archivo de configuración principal
- `/var/hw_ctree_equipbak.xml` - Backup de configuración
- `/var/hw_ctree.xml.tmp` - Archivo temporal

**Scripts Adicionales:**
- `/tmp/dealosgfile.sh` (SHA256: c714159ef4c00435adf378d5cba0a4544430c54dcde8e8ae430e0384bf34076e)
- `/tmp/duit9rr.sh` (SHA256: d3ff2e59070360fa2e2e841ac2d951a01cddc2e663e2895fcc490e8a37fcb2d9)

**Herramientas Referenciadas:**
- `aescrypt2` - Herramienta de encriptación/desencriptación AES
- `cfgtool` - Herramienta de configuración de ONT
- `gunzip`/`gzip` - Compresión/descompresión

**DLLs Utilizadas:**
KERNEL32.dll, USER32.dll, GDI32.dll, MSIMG32.dll, COMDLG32.dll, ADVAPI32.dll, SHELL32.dll, COMCTL32.dll, SHLWAPI.dll, ole32.dll, OLEAUT32.dll, oledlg.dll, WS2_32.dll, gdiplus.dll, IPHLPAPI.DLL, OLEACC.dll, IMM32.dll, WINMM.dll

**Manifest XML:**
- Requiere privilegios de administrador (requireAdministrator)
- Usa Microsoft.Windows.Common-Controls v6.0.0.0

---

### 2. Huawei-PON-Tools-HG8145V5-20260414.exe

**Tipo:** PE32 Mono/.Net assembly
**Versión:** 26.4.1.4
**Descripción:** Huawei PON Tools EG & HG 8145V5
**Compañía:** @DKIJAYA
**Copyright:** 2026

#### Análisis .NET:

**Clase Principal:** `PowerShellHostForm` (PSExecutor_1831f0c7)

**Funcionalidad:**
- Ejecutor de PowerShell embebido
- Busca `pwsh.exe` o `powershell.exe` en el PATH
- Ejecuta scripts con privilegios de administrador (runas)
- Manejo de timeout y monitoreo de procesos

**Métodos Principales:**
- `StartPowerShellProcess()` - Inicia proceso PowerShell
- `MonitorProcess()` - Monitorea ejecución y maneja timeout
- `FindPowerShellExecutable()` - Busca ejecutable PowerShell
- `get_ExitCode()` - Obtiene código de salida

**Recursos Extraídos:**
- 8 iconos (ICO) de diferentes tamaños (16x16, 32x32, 48x48)
- Manifest XML (application manifest)
- Version info

**Manifest XML:**
```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <assemblyIdentity version="1.0.0.0" name="MyApplication.app"/>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
    <security>
      <requestedPrivileges xmlns="urn:schemas-microsoft-com:asm.v3">
        <requestedExecutionLevel level="asInvoker" uiAccess="false"/>
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
```

---

### 3. Huawei-Downgrade-Tools.exe

**Tipo:** PE32 Mono/.Net assembly
**Versión:** 26.3.12.0
**Descripción:** Huawei Downgrade Tools
**Comentarios:** Telnet & SSH script for ONT Huawei
**Copyright:** 2025

#### Análisis .NET:

**Clase Principal:** `PowerShellHostForm` (PSExecutor_994ad26b)

**Funcionalidad:**
- Ejecutor de PowerShell embebido
- Similar al PON Tools pero para downgrade de firmware
- Manejo de scripts Telnet & SSH para ONT Huawei

**Métodos Principales:**
- `StartPowerShellProcess()` - Inicia proceso PowerShell
- `MonitorProcess()` - Monitorea ejecución
- `FindPowerShellExecutable()` - Busca ejecutable PowerShell

**Recursos Extraídos:**
- 1 icono (ICO)
- Manifest XML
- Version info

---

## ARCHIVOS EXTRAÍDOS

### Directorio: extracted/

**Huawei-PON-Tools:**
- .text (código)
- .reloc (tabla de reubicación)
- .rsrc/ICON/ (8 iconos)
- .rsrc/MANIFEST/1 (XML)
- .rsrc/version.txt
- .rsrc/GROUP_ICON/

**Huawei-Downgrade-Tools:**
- .text (código)
- .reloc (tabla de reubicación)
- .rsrc/ICON/2.ico
- .rsrc/MANIFEST/1 (XML)
- .rsrc/version.txt
- .rsrc/GROUP_ICON/

### Directorio: analysis/

- PANDA-FLASHER-strings.txt (17,144 cadenas)
- Huawei-PON-Tools-strings.txt (288 cadenas)
- Huawei-Downgrade-Tools-strings.txt (225 cadenas)
- Huawei-PON-Tools.il (código IL descompilado, 25KB)
- Huawei-Downgrade-Tools.il (código IL descompilado, 25KB)
- PANDA-FLASHER-script1.sh (script de habilitación de telnet)
- PANDA-FLASHER-tools-refs.txt (referencias a herramientas)
- PANDA-FLASHER-additional-scripts.txt (scripts adicionales)

---

## HALLAZGOS IMPORTANTES

### 1. Mecanismo de Habilitación de Telnet

PANDA-FLASHER.exe contiene un script de shell que:
- Habilita el servicio Telnet en ONTs Huawei
- Modifica el archivo de configuración hw_ctree.xml
- Usa encriptación AES (aescrypt2) para proteger/modificar configuración
- Establece el parámetro TELNETLanEnable=1
- Activa modo CLI China (HW_SSMP_FEATURE_CLI_CHINA_MODE)

### 2. Ejecutores de PowerShell

Ambas herramientas .NET son ejecutores de PowerShell que:
- Buscan pwsh.exe o powershell.exe
- Se ejecutan con privilegios de administrador
- Tienen mecanismos de timeout y monitoreo
- Están diseñadas para ejecutar scripts de configuración/downgrade

### 3. Archivos de Configuración Críticos

- **hw_ctree.xml**: Archivo de configuración principal del ONT
- **UpgradeCheck.xml**: Verificación de actualizaciones
- **TelnetEnable**: Archivo que habilita Telnet

### 4. Herramientas del Sistema

- **aescrypt2**: Encriptación/desencriptación AES
- **cfgtool**: Configuración del ONT
- **configtool**: Configuración adicional

---

## SEGURIDAD

### Privilegios Requeridos:
- PANDA-FLASHER.exe: Requiere privilegios de administrador
- Huawei-PON-Tools: asInvoker (mismos privilegios)
- Huawei-Downgrade-Tools: asInvoker (mismos privilegios)

### Consideraciones:
- Las herramientas modifican configuración crítica del ONT
- Habilitan acceso remoto (Telnet)
- Pueden realizar downgrade de firmware
- Usan encriptación para proteger archivos de configuración

---

## CONCLUSIÓN

Las tres herramientas analizadas son utilitarios legítimos para administración de ONTs Huawei:

1. **PANDA-FLASHER**: Herramienta avanzada de reparación/flash con capacidad de habilitar Telnet
2. **Huawei-PON-Tools**: Herramienta de configuración PON para modelos HG8145V5
3. **Huawei-Downgrade-Tools**: Herramienta para downgrade de firmware via Telnet/SSH

Todas las herramientas están diseñadas para ejecutarse en Windows y requieren acceso al ONT Huawei para realizar sus funciones.

---

**Fecha de Análisis:** 2026-07-07
**Analizado por:** opencode
