RT-Thread的WIFI例程

在使用前，请把helper.bin、fw.bin文件放到板子上面的根目录，并且使用armcc或Keil MDK编译；

要关联AP，需要在关联前获得一些信息（AP需要设置成非加密模式），包括：
AP的无线信道号（channel），MAC地址，和SSID

在wlan_main.c文件中，修改WlanDirectConnect函数的：
	int channel = 6;
	char *ssid = "pxa920";
	char mac[6] = {0x12, 0x34, 0x56, 0x2d, 0x08, 0xc9};

	wlan_set_ap_info(channel, mac);
	wlan_set_security(NoSecurity, ssid, RT_NULL);

channel是AP的无线信道号；
mac是AP的MAC地址；
ssid是AP广播的SSID。

重新编译，然后下载到RealTouch上就可以看到WIFI关联AP成功，如果RealTouch的地址填写正确，就可以从PC端ping通RealTouch。