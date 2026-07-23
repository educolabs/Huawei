#! /bin/sh
var_file_telnetenable="/mnt/jffs2/TelnetEnable"
var_jffs2_current_ctree_file="/mnt/jffs2/hw_ctree.xml"
var_current_ctree_bak_file="/var/hw_ctree_equipbak.xml"
var_current_ctree_file_tmp="/var/hw_ctree.xml.tmp"
var_pack_temp_dir="/bin/"
echo "  "  >>  $var_file_telnetenable
HW_Open_Telnet_Ctree_Node()
	var_node_telnet=InternetGatewayDevice.X_HW_Security.AclServices
	varIsXmlEncrypted=0
	#set telnet
	EnableLanTelnetValue="1"                                                                                                   
	cp -f $var_jffs2_current_ctree_file $var_current_ctree_bak_file
	$var_pack_temp_dir/aescrypt2 1 $var_current_ctree_bak_file $var_current_ctree_file_tmp
	if [ 0 -eq $? ]
		varIsXmlEncrypted=1
		mv $var_current_ctree_bak_file $var_current_ctree_bak_file".gz"
		gunzip -f $var_current_ctree_bak_file".gz"
	#set TELNETLanEnable
	cfgtool set $var_current_ctree_bak_file $var_node_telnet TELNETLanEnable $EnableLanTelnetValue
	if [ 0 -ne $? ]
		echo "ERROR::Failed to set TELNETLanEnable!"
	#encrypt var_default_ctree
	if [ $varIsXmlEncrypted -eq 1 ]
		gzip -f $var_current_ctree_bak_file
		mv $var_current_ctree_bak_file".gz" $var_current_ctree_bak_file
		$var_pack_temp_dir/aescrypt2 0 $var_current_ctree_bak_file $var_current_ctree_file_tmp
	rm -f $var_jffs2_current_ctree_file
	cp -f $var_current_ctree_bak_file $var_jffs2_current_ctree_file
HW_Open_Telnet_Ctree_Node
echo "feature.name = \"HW_SSMP_FEATURE_CLI_CHINA_MODE\" feature.enable=\"1\" feature.attribute=\"1\"" > /mnt/jffs2/hw_hardinfo_feature
echo "success!"
8881DDDDDDDD9991
MMM*hhhT{{{q
{{{riiiTNNN++++
777+777;888=8880777
8880888=777;777*777
)3<</FYY-Rss+V||(Y
+W~~.Rrr/Iaa+9EE %))
   >888o;;;{;;;};;;};;;};;;};;;};;;};;;};;;};;;};;;};;;};;;};;;};;;z555e
),)),)),)),)
),))())())()
.44444444.
4444444444
V5444444444V
VVVVVVVVVVVVVV
./555WWWW5555/
s "dff";;[\PA
(//Gyy9jjj
ZhB%*ikkCCC
*ACDhmmESS
MEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE%
TTT>qqql~~~
rrrlUUU?'''
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<dependency>
    <dependentAssembly>
      <assemblyIdentity
        type="win32"
        name="Microsoft.Windows.Common-Controls"
        version="6.0.0.0"
        processorArchitecture="*"
        publicKeyToken="6595b64144ccf1df"
        language="*"
        />
    </dependentAssembly>
</dependency>
<v3:trustInfo xmlns:v3="urn:schemas-microsoft-com:asm.v3">
    <v3:security>
      <v3:requestedPrivileges>
        <!-- level can be "asInvoker", "highestAvailable", or "requireAdministrator" -->
        <v3:requestedExecutionLevel level="requireAdministrator" />
      </v3:requestedPrivileges>
    </v3:security>
</v3:trustInfo>
</assembly>
5+606R6b6q6
;#<*<<<@<D<H<
:/:E:Q:`:l:{:
;);8;D;S;_;n;z;
<+<7<F<R<a<m<|<
=*=9=E=T=`=o={=
>,>8>G>S>b>n>}>
?+?:?F?U?a?p?|?
929J9c9i9|9
0P1T1X1\1`1d1h1l1p1t1x1|1
>;>E>O>Y>`>
0	1H1S1c1s1
3!4(484H4u4
4+525=5D5O5V5e5l5}5
6#60676>6I6P6[6b6|7
8%848O8\8e8u8
9 9-9<9U9e9w9
: :=:M:`:r:y:
;)<0<=<L<o<v<
=,=<=H=X=w=
?"?/?<?I?[?m?
6F7K7k7w7|7
;M<^<h<;=L=V=m=w=|=
5$60676B6Q6
