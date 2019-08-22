package com.sxr.sdk.ble.keepfit.client;

import java.util.Comparator;

public class ComparatorBleDeviceItem implements Comparator<BleDeviceItem>{

	@Override
	public int compare(BleDeviceItem arg0, BleDeviceItem arg1) {
		int rssi0 = arg0.getRssi();
		int rssi1 = arg1.getRssi();
        int result = 0;
        if(rssi0 < rssi1)
        {
            result=1;
        }
        if(rssi0 > rssi1)
        {
            result=-1;
        }
        
        return result;
	}

}
